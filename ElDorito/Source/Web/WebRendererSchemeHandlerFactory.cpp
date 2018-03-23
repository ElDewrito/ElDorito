#include <winsock2.h>
#include "WebRendererSchemeHandlerFactory.hpp"
#include <boost/network/uri.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include "include/cef_stream.h"
#include "include/wrapper/cef_stream_resource_handler.h"

using namespace Anvil::Client::Rendering;

namespace
{

	CefRefPtr<CefResourceHandler> CreateError(int statusCode, const std::string &statusText);
	CefRefPtr<CefStreamResourceHandler> GetStreamResourceHandler(CefRefPtr<CefRequest> request, const std::string &mimeType,
		CefResponse::HeaderMap &headers, CefRefPtr<CefStreamReader> stream);

	std::string GetMimeType(std::string extension);
	bool ParseRequestUri(const CefRefPtr<CefRequest> &request, std::string &scheme, std::string &host, std::string &path);
	void BuildResponseHeaders(const CefRefPtr<CefRequest> &request, const std::string &contentType,
		const CefRefPtr<CefStreamReader> &stream, CefResponse::HeaderMap &headers);

}

WebRendererSchemeHandlerFactory::WebRendererSchemeHandlerFactory(const std::string &p_Scheme, const std::string &p_Directory)
	: m_Scheme(p_Scheme), m_Directory(p_Directory)
{

}

CefRefPtr<CefResourceHandler> WebRendererSchemeHandlerFactory::Create(CefRefPtr<CefBrowser> p_Browser, CefRefPtr<CefFrame> p_Frame, const CefString& p_SchemeName, CefRefPtr<CefRequest> p_Request)
{
	std::string scheme, host, path;
	if (!ParseRequestUri(p_Request, scheme, host, path) || scheme != m_Scheme)
		return CreateError(400, "Bad Request");

	// Deny access to the internal pages from other frames?
	if (!p_Frame->IsMain() && m_Scheme == "dew" && host == "ui")
		return CreateError(401, "Not Authorized");

	// Get the filename.
	boost::filesystem::path fpath(path);
	auto filePath = boost::filesystem::canonical(m_Directory) / host / fpath;
	if (!boost::filesystem::exists(filePath))
		return CreateError(404, "File Not Found");

	// Set the content type
	auto extension = fpath.has_extension() ? fpath.extension().string() : "";
	boost::to_lower(extension);
	auto contentType = GetMimeType(extension);

	auto stream = CefStreamReader::CreateForFile(filePath.string());

	CefResponse::HeaderMap headers;
	BuildResponseHeaders(p_Request, contentType, stream, headers);

	return GetStreamResourceHandler(p_Request, contentType, headers, stream);
}

namespace
{
	const auto kDefaultMimetype = "application/octet-stream";
	const std::unordered_map<std::string, std::string> kExtensionMimeTypeMap =
	{
		{ ".html", "text/html" },
		{ ".htm", "text/html" },
		{ ".png", "image/png" },
		{ ".jpg", "image/jpeg" },
		{ ".jpeg", "image/jpeg" },
		{ ".jpe", "image/jpeg" },
		{ ".gif", "image/gif" },
		{ ".bmp", "image/bmp" },
		{ ".ico", "image/x-icon" },
		{ ".css", "text/css" },
		{ ".scss", "text/css" },
		{ ".sass", "text/css" },
		{ ".less", "text/css" },
		{ ".js", "application/javascript" },
		{ ".txt", "text/plain" },
		{ ".obj", "text/plain" },
		{ ".svg", "image/svg+xml" },
		{ ".ttf", "application/font-sfnt" },
		{ ".otf", "application/font-sfnt" },
		{ ".woff", "application/x-font-wof" },
		{ ".ogv", "video/ogg" },
		{ ".webm", "video/webm" },
		{ ".ogg", "audio/ogg" }
	};

	std::string GetMimeType(std::string extension)
	{
		if (extension.empty())
			return kDefaultMimetype;

		auto it = kExtensionMimeTypeMap.find(extension);
		if (it == kExtensionMimeTypeMap.end())
			return kDefaultMimetype;

		return it->second;
	}

	CefRefPtr<CefResourceHandler> CreateError(int statusCode, const std::string &statusText)
	{
		return new CefStreamResourceHandler(statusCode, statusText, "text/html", {}, CefStreamReader::CreateForData(new char[1], 1));
	}

	bool HandleContentRange(const std::string& range_query,
		CefRefPtr<CefStreamReader> stream,
		CefResponse::HeaderMap& header_map,
		int64& content_length) {
		int64 total_bytes = 0;
		std::stringstream content_range_ss;
		std::stringstream content_length_ss;
		int64 range_begin = 0;
		int64 range_end = 0;

		if (range_query.find("bytes=") != 0 ||
			range_query.find(",") != std::string::npos) {
			// Multiple range headers are not supported.
			return false;
		}
		else {
			// Further parse |range_query| and find hyphen.
			std::size_t hyphen_pos = range_query.find("-");
			if (hyphen_pos != std::string::npos) {
				// Skip "bytes=" prefix (offset = 6).
				range_begin = atoll(range_query.substr(6, hyphen_pos - 6).c_str());
				range_end = atoll(range_query.substr(hyphen_pos + 1).c_str());
			}
			else {
				return false;
			}
		}

		if (stream->Seek(0, SEEK_END) == 0) {
			total_bytes = stream->Tell();
			if (total_bytes > 0) {
				if (range_end == 0) {
					range_end = (total_bytes - 1);
				}
				if (range_begin > range_end) {
					return false;
				}
				else
				{
					content_range_ss << "bytes "
						<< range_begin << "-" << range_end << "/"
						<< total_bytes;
					content_length = total_bytes - range_begin;
					content_length_ss << content_length;
					header_map.insert(std::pair<CefString, CefString>(
						"Content-Range", content_range_ss.str()));
					header_map.insert(std::pair<CefString, CefString>(
						"Content-Length", content_length_ss.str()));
				}
			}

			// Set stream cursor to range begin.
			stream->Seek(range_begin, SEEK_SET);
		}

		return true;
	}

	bool ParseRequestUri(const CefRefPtr<CefRequest> &request, std::string &scheme, std::string &host, std::string &path)
	{
		auto url = request->GetURL().ToString();
		boost::replace_all(url, " ", "%20");

		// [scheme:][//authority][path][?query][#fragment]
		boost::network::uri::uri requestUri(url);
		if (!requestUri.is_valid())
			return false;

		auto finalPath = boost::network::uri::decoded(requestUri.path());

		if (finalPath.size() == 0)
			finalPath = "/";

		if (finalPath.substr(finalPath.size() - 1) == "/")
			finalPath += "index.html";

		scheme = requestUri.scheme();
		path = std::move(finalPath);
		host = requestUri.host();
		return true;
	}

	void BuildResponseHeaders(const CefRefPtr<CefRequest> &request, const std::string &contentType,
		const CefRefPtr<CefStreamReader> &stream, CefResponse::HeaderMap &headers)
	{
		CefResponse::HeaderMap requestHeaders;
		request->GetHeaderMap(requestHeaders);

		CefString origin = "";
		auto it = requestHeaders.find("Origin");
		if (it != requestHeaders.end())
			origin = it->second;

		headers.insert(std::make_pair("Access-Control-Allow-Origin", origin));
		headers.insert(std::make_pair("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept"));
	}

	bool ParseRequestRange(CefRefPtr<CefRequest> request,
		std::string& range) {
		CefRequest::HeaderMap headers;
		request->GetHeaderMap(headers);

		auto findRange = headers.find("Range");
		if (findRange != headers.end()) {
			range = findRange->second.ToString();
			return true;
		}

		return false;
	}

	CefRefPtr<CefStreamResourceHandler> GetStreamResourceHandler(
		CefRefPtr<CefRequest> request,
		const std::string &mimeType,
		CefResponse::HeaderMap &headers,
		CefRefPtr<CefStreamReader> stream)
	{
		DCHECK(request);
		DCHECK(stream);

		CefRefPtr<CefStreamResourceHandler> handler;

		std::string range;
		if (ParseRequestRange(request, range)) {

			int64 content_length = -1;
			if (HandleContentRange(range, stream, headers, content_length)) {
				handler = new CefStreamResourceHandler(
					206,
					"Partial Content",
					mimeType,
					headers,
					stream);
			}
			else {
				handler = new CefStreamResourceHandler(
					416,
					"Requested Range Not Satisfiable",
					mimeType,
					headers,
					stream);
			}
		}
		else {
			handler = new CefStreamResourceHandler(200, "OK", mimeType, headers, stream);
		}

		return handler;
	}
}
