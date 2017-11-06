#pragma once

#include <include/wrapper/cef_message_router.h>
#include <boost/lockfree/queue.hpp>
#include <unordered_map>
#include <string>
#include "../../ThirdParty/rapidjson/document.h"
#include "WebRendererQuery.hpp"

namespace Anvil::Client::Rendering::Bridge
{
	class WebRendererQueryHandler : public CefMessageRouterBrowserSide::Handler
	{
		struct PendingQuery
		{
			rapidjson::GenericDocument<rapidjson::UTF8<>> m_Json;
			WebRendererQuery m_Method;
			const rapidjson::GenericValue<rapidjson::UTF8<>> *m_Args;
			CefRefPtr<CefMessageRouterBrowserSide::Callback> m_Callback;
		};

		// Can't use shared_ptr here because lockfree queue entries must be trivially assignable
		boost::lockfree::queue<PendingQuery*> m_Queries;

		std::unordered_map<std::string, WebRendererQuery> m_Methods;

	public:
		WebRendererQueryHandler();

		bool OnQuery(CefRefPtr<CefBrowser> p_Browser, CefRefPtr<CefFrame> p_Frame, int64 p_QueryId, const CefString &p_Request, bool p_Persistent, CefRefPtr<Callback> p_Callback) override;
		void OnQueryCanceled(CefRefPtr<CefBrowser> p_Browser, CefRefPtr<CefFrame> p_Frame, int64 p_QueryId) override;

		void AddMethod(const std::string &p_Name, const WebRendererQuery &p_Method);
		void Update();
	};
}
