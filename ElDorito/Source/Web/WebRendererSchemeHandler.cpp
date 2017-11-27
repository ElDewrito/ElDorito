/*
Code was used from NoFaTe (http://nofate.me)
*/

// Disable warnings about using "safe" C functions
#pragma warning (disable : 4996)

// Disable warnings from boost
#pragma warning (disable : 4348)

#include "WebRendererSchemeHandler.hpp"
#include "Logger.hpp"

#define _WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN

#pragma warning (push)

#include <boost/network/uri.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include <boost/thread.hpp>

#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/filesystem.hpp>

#pragma warning (pop)

#pragma comment(lib, "cppnetlib-uri")
#include "WebRenderer.hpp"
using namespace Anvil::Client::Rendering;

std::unordered_map<int64_t, std::unordered_map<std::string, std::string>> WebRendererSchemeHandler::m_CachedData;
std::mutex WebRendererSchemeHandler::m_CachedDataMutex;

WebRendererSchemeHandler::WebRendererSchemeHandler(const std::string &p_Scheme, const boost::filesystem::path &p_Directory, bool p_Main, CefRefPtr<CefFrame> p_Frame)
	: m_Scheme(p_Scheme), m_Main(p_Main), m_Frame(p_Frame)
{
	m_Directory = boost::filesystem::canonical(p_Directory);
	m_RequestedLength = 0;
	m_Offset = 0;
	m_Partial = false;
	m_ShouldCache = false;
}

WebRendererSchemeHandler::~WebRendererSchemeHandler()
{
	m_TempData.clear();
	m_TempFileName.clear();
	m_ContentType.clear();
	m_RequestedLength = 0;
	m_Offset = 0;
	m_Partial = false;
	m_ShouldCache = false;
}

void WebRendererSchemeHandler::ProcessRequestInternal(CefRefPtr<CefRequest> p_Request, CefRefPtr<CefCallback> p_Callback)
{
	boost::lock_guard<WebRendererSchemeHandler> s_Guard(*this);

	CefRequest::HeaderMap s_Headers;
	p_Request->GetHeaderMap(s_Headers);

	WriteLog("Processing request: %s", p_Request->GetURL().ToString().c_str());

	//Logger(Util::LogLevel::Debug, "Headers:");
	//for (auto it = s_Headers.begin(); it != s_Headers.end(); ++it)
	//	Logger(Util::LogLevel::Debug, "%ls => %ls", it->first.c_str(), it->second.c_str());

	// HACK: Replace spaces in the URL with %20 (this is the most common reason for a URI to be invalid)
	auto s_Url = p_Request->GetURL().ToString();
	boost::replace_all(s_Url, " ", "%20");

	// [scheme:][//authority][path][?query][#fragment]
	boost::network::uri::uri s_RequestURI(s_Url);

	if (!s_RequestURI.is_valid())
	{
		p_Callback->Cancel();
		return;
	}

	if (s_RequestURI.scheme() != m_Scheme)
	{
		p_Callback->Cancel();
		return;
	}

	auto s_FinalPath = boost::network::uri::decoded(s_RequestURI.path());

	if (s_FinalPath.size() == 0)
		s_FinalPath = "/";

	if (s_FinalPath.substr(s_FinalPath.size() - 1) == "/")
		s_FinalPath += "index.html";

	auto s_Host = s_RequestURI.host();

	// Deny access to the internal pages from other frames?
	if (m_Scheme == "dew" && s_Host == "ui" && !m_Main)
	{
		p_Callback->Cancel();
		return;
	}

	// Get the filename.
	boost::filesystem::path s_Path(s_FinalPath);
	m_TempFileName = s_Path.filename().string();

	auto s_VFSPath = str(boost::format("/%s/%s/%s") % m_Scheme % s_RequestURI.host() % s_FinalPath.substr(1));

	m_TempData.clear();
	m_TempFileName.clear();
	m_RequestedRange.clear();
	m_ContentType = "application/octet-stream";
	m_RequestedLength = 0;
	m_Offset = 0;
	m_Partial = false;
	m_ShouldCache = false;

	bool s_Result;

	m_CachedDataMutex.lock();

	// Do we have the data the user is requesting in our cache?
	auto s_FirstIterator = m_CachedData.find(m_Frame->GetIdentifier());

	if (s_FirstIterator == m_CachedData.end())
	{
		s_Result = ReadLocalFile(s_Host, s_FinalPath, m_TempData);
	}
	else
	{
		auto s_DataIterator = s_FirstIterator->second.find(s_VFSPath);

		if (s_DataIterator == s_FirstIterator->second.end())
		{
			s_Result = ReadLocalFile(s_Host, s_FinalPath, m_TempData);
		}
		else
		{
			m_TempData = s_DataIterator->second;
			s_Result = true;
		}
	}

	m_CachedDataMutex.unlock();

	m_RequestedLength = m_TempData.size();

	// Is this a partial request?
	auto s_RangeIt = s_Headers.find("Range");

	if (s_Result && s_RangeIt != s_Headers.end())
	{
		// Parse the range info.
		std::string s_Range = s_RangeIt->second.ToString();
		auto s_Sep = s_Range.find('=');

		if (s_Sep != std::string::npos)
		{
			s_Range = s_Range.substr(s_Sep + 1);

			std::vector<std::string> s_Ranges;
			boost::split(s_Ranges, s_Range, boost::is_any_of(","));

			// We only handle the first requested range for now.
			// TODO: Change that?
			m_RequestedRange = s_Ranges[0];

			s_Ranges.clear();
			boost::split(s_Ranges, m_RequestedRange, boost::is_any_of("-"));

			if (s_Ranges.size() == 2)
			{
				size_t s_Start = s_Ranges[0].size() == 0 ? 0 : boost::lexical_cast<size_t>(s_Ranges[0]);
				size_t s_End = s_Ranges[1].size() == 0 ? 0 : boost::lexical_cast<size_t>(s_Ranges[1]);

				if (s_Ranges[0].size() == 0)
				{
					s_Start = m_TempData.size() - s_End;
					s_End = m_TempData.size() - 1;
				}
				else if (s_Ranges[1].size() == 0)
				{
					s_End = m_TempData.size() - 1;
				}

				if (s_End > m_TempData.size() - 1)
					s_End = m_TempData.size() - 1;

				if (s_Start > 0 && s_Start <= s_End)
				{
					m_Partial = true;
					m_Offset = s_Start;
					m_RequestedLength = (s_End - s_Start) + 1;
				}
			}
		}
	}

	if (s_Result)
	{
		// Set the content type
		auto s_Extension = s_Path.has_extension() ? s_Path.extension().string() : "";
		boost::to_lower(s_Extension);

		// TODO: Determine mimetype based on the first few bytes of the file, 
		// and fall back to the extension-based method if that fails.
		if (s_Extension != "" && s_Extension != ".")
		{
			if (s_Extension == ".html" || s_Extension == ".htm")
				m_ContentType = "text/html";

			if (s_Extension == ".png")
			{
				m_ShouldCache = true;
				m_ContentType = "image/png";
			}

			if (s_Extension == ".jpg" || s_Extension == ".jpeg" || s_Extension == ".jpe")
			{
				m_ShouldCache = true;
				m_ContentType = "image/jpeg";
			}

			if (s_Extension == ".gif")
			{
				m_ShouldCache = true;
				m_ContentType = "image/gif";
			}

			if (s_Extension == ".bmp")
			{
				m_ShouldCache = true;
				m_ContentType = "image/bmp";
			}

			if (s_Extension == ".ico")
			{
				m_ShouldCache = true;
				m_ContentType = "image/x-icon";
			}

			if (s_Extension == ".css" || s_Extension == ".scss" || s_Extension == ".sass" || s_Extension == ".less")
				m_ContentType = "text/css";

			if (s_Extension == ".js")
				m_ContentType = "application/javascript";

			if (s_Extension == ".txt" || s_Extension == ".obj")
				m_ContentType = "text/plain";

			if (s_Extension == ".svg")
			{
				m_ShouldCache = true;
				m_ContentType = "image/svg+xml";
			}

			if (s_Extension == ".ttf" || s_Extension == ".otf")
			{
				m_ShouldCache = true;
				m_ContentType = "application/font-sfnt";
			}

			if (s_Extension == ".woff")
			{
				m_ShouldCache = true;
				m_ContentType = "application/x-font-woff";
			}

			if (s_Extension == ".ogv")
			{
				m_ShouldCache = true;
				m_ContentType = "video/ogg";
			}

			if (s_Extension == ".ogg")
			{
				m_ShouldCache = true;
				m_ContentType = "audio/ogg";
			}
		}

		if (m_ShouldCache)
		{
			m_CachedDataMutex.lock();

			// Cache our data.
			auto s_Iterator = m_CachedData.find(m_Frame->GetIdentifier());

			if (s_Iterator == m_CachedData.end())
				m_CachedData[m_Frame->GetIdentifier()] = std::unordered_map<std::string, std::string>();

			m_CachedData[m_Frame->GetIdentifier()][s_VFSPath] = m_TempData;

			m_CachedDataMutex.unlock();
		}
	}

	if (s_Result)
		p_Callback->Continue();
	else
		p_Callback->Cancel();
}

bool WebRendererSchemeHandler::ReadLocalFile(std::string p_Host, std::string p_Path, std::string& p_OutString)
{
	p_OutString = "";

	try
	{
		auto s_FilePath = m_Directory / p_Host / p_Path;
		if (!boost::filesystem::exists(s_FilePath))
			return false;

		// Make sure the file is inside the directory
		s_FilePath = boost::filesystem::canonical(s_FilePath, m_Directory);
		if (!boost::starts_with(s_FilePath, m_Directory))
			return false;

		// using boost's memory mapped file.
		auto s_FileSize = boost::filesystem::file_size(s_FilePath);
		auto s_FilePathStr = s_FilePath.string();
		boost::interprocess::file_mapping s_File(s_FilePathStr.c_str(), boost::interprocess::read_only);
		
		boost::interprocess::mapped_region s_Region(s_File, boost::interprocess::read_only);

		auto s_Address = s_Region.get_address();
		auto s_Size = s_Region.get_size();

		// Pointer checking
		if (!s_Address)
			return false;

		// Resize the final buffer
		p_OutString.resize(s_Size);

		// Zero out the buffer
		fill(p_OutString.begin(), p_OutString.end(), 0);

		// Copy the contents
		auto s_Ret = memcpy_s((void*)p_OutString.data(), p_OutString.length(), s_Address, s_Size);
		if (s_Ret)
		{
			WriteLog("Failed reading file %s (%x).", p_Path.c_str(), s_Ret);
			return false;
		}

		WriteLog("Successfully read %s from file.", p_Path.c_str());
		return true;
	}
	catch (...)
	{
	}

	return false;
}

bool WebRendererSchemeHandler::ProcessRequest(CefRefPtr<CefRequest> p_Request, CefRefPtr<CefCallback> p_Callback)
{
	CefResponse::HeaderMap s_Headers;
	p_Request->GetHeaderMap(s_Headers);
	m_Origin = "";
	for (CefResponse::HeaderMap::iterator it = s_Headers.begin(); it != s_Headers.end(); it++)
	{
		if (it->first == "Origin")
			m_Origin = it->second;
	}
	
	// TODO: We shouldn't spawn a thread every single time, but do this on a single thread.
	boost::thread s_Thread(&WebRendererSchemeHandler::ProcessRequestInternal, this, p_Request, p_Callback);

	return true;
}

void WebRendererSchemeHandler::GetResponseHeaders(CefRefPtr<CefResponse> p_Response, int64& p_ResponseLength, CefString& p_RedirectURL)
{
	p_ResponseLength = m_RequestedLength;
	p_Response->SetStatus(m_Partial ? 206 : 200);
	p_Response->SetMimeType(m_ContentType.size() == 0 ? "application/octet-stream" : m_ContentType);

	CefResponse::HeaderMap s_Headers;

	if (m_ShouldCache)
		s_Headers.insert(std::make_pair("Cache-Control", "public, max-age=0"));
	else
		s_Headers.insert(std::make_pair("Cache-Control", "private, max-age=0, no-cache"));
	
	s_Headers.insert(std::make_pair("Access-Control-Allow-Origin", m_Origin));

	s_Headers.insert(std::make_pair("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept"));

	s_Headers.insert(std::make_pair("Content-Length", boost::lexical_cast<std::string>(m_RequestedLength).c_str()));

	// Is this a partial response?
	if (m_Partial)
	{
		p_Response->SetStatusText("Partial Content");
		s_Headers.insert(std::make_pair("Content-Range", str(boost::format("bytes %s/%d") % m_RequestedRange.c_str() % m_TempData.size()).c_str()));
	}

	p_Response->SetHeaderMap(s_Headers);
}

bool WebRendererSchemeHandler::ReadResponse(void* p_DataOut, int p_BytesToRead, int& p_BytesRead, CefRefPtr<CefCallback> p_Callback)
{
	//Logger(Util::LogLevel::Debug, "Reading Response (req: %d - total: %d).", p_BytesToRead, m_TempData.size());

	// Do we have enough data for this?
	if ((size_t)m_Offset >= m_TempData.size())
		return false;

	size_t s_BytesToRead = (size_t)p_BytesToRead > (m_TempData.size() - m_Offset) ? m_TempData.size() : p_BytesToRead;

	memcpy(p_DataOut, m_TempData.data() + m_Offset, s_BytesToRead);
	m_Offset += s_BytesToRead;

	p_BytesRead = s_BytesToRead;

	return true;
}

bool WebRendererSchemeHandler::CanGetCookie(const CefCookie& p_Cookie)
{
	return true;
}

bool WebRendererSchemeHandler::CanSetCookie(const CefCookie& p_Cookie)
{
	return true;
}

void WebRendererSchemeHandler::Cancel()
{
}

void WebRendererSchemeHandler::ExpireCache(int64_t p_Frame)
{
	m_CachedDataMutex.lock();

	auto s_Iterator = m_CachedData.find(p_Frame);

	if (s_Iterator != m_CachedData.end())
		m_CachedData.erase(s_Iterator);

	m_CachedDataMutex.unlock();
}

void WebRendererSchemeHandler::ClearCache()
{
	m_CachedDataMutex.lock();
	m_CachedData.clear();
	m_CachedDataMutex.unlock();
}