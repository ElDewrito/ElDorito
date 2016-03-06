#pragma once
#define WIN32_LEAN_AND_MEAN

#include <boost/thread.hpp>
#include <boost/thread/lockable_adapter.hpp>
#include <include/cef_scheme.h>
#include <unordered_map>
#include <mutex>

namespace Anvil
{
	namespace Client
	{
		namespace Rendering
		{
			class WebRendererSchemeHandler : 
				public CefResourceHandler,
				public boost::basic_lockable_adapter<boost::mutex>
			{
			public:
				WebRendererSchemeHandler(bool p_Main, CefRefPtr<CefFrame> p_Frame);
				~WebRendererSchemeHandler();

			protected:
				virtual bool ProcessRequest(CefRefPtr<CefRequest> p_Request, CefRefPtr<CefCallback> p_Callback) override;
				virtual void GetResponseHeaders(CefRefPtr<CefResponse> p_Response, int64& p_ResponseLength, CefString& p_RedirectURL) override;
				virtual bool ReadResponse(void* p_DataOut, int p_BytesToRead, int& p_BytesRead, CefRefPtr<CefCallback> p_Callback) override;
				virtual bool CanGetCookie(const CefCookie& p_Cookie) override;
				virtual bool CanSetCookie(const CefCookie& p_Cookie) override;
				virtual void Cancel() override;

				void ProcessRequestInternal(CefRefPtr<CefRequest> p_Request, CefRefPtr<CefCallback> p_Callback);
				bool ReadLocalFile(std::string p_Host, std::string p_Path, std::string& p_OutString);

			public:
				static void ExpireCache(int64_t p_Frame);

			protected:
				std::string m_TempData;
				std::string m_TempFileName;
				std::string m_ContentType;
				std::string m_RequestedRange;
				int32_t m_RequestedLength;
				bool m_Partial;
				int32_t m_Offset;
				bool m_ShouldCache;

				bool m_Main;

				CefRefPtr<CefFrame> m_Frame;

				static std::unordered_map<int64_t, std::unordered_map<std::string, std::string>> m_CachedData;
				static std::mutex m_CachedDataMutex;

				IMPLEMENT_REFCOUNTING(WebRendererSchemeHandler);
			};
		}
	}
}