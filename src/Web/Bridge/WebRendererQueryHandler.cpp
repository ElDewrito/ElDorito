#include "WebRendererQueryHandler.hpp"

using Anvil::Client::Rendering::Bridge::WebRendererQuery;
using Anvil::Client::Rendering::Bridge::WebRendererQueryHandler;

WebRendererQueryHandler::WebRendererQueryHandler() :
	m_Queries(1024)
{
}

bool WebRendererQueryHandler::OnQuery(CefRefPtr<CefBrowser> p_Browser, CefRefPtr<CefFrame> p_Frame, int64 p_QueryId, const CefString &p_Request, bool p_Persistent, CefRefPtr<Callback> p_Callback)
{
	auto s_Query = new PendingQuery();

	// Parse the JSON request
	auto &s_Json = s_Query->m_Json;
	auto s_RequestStr = p_Request.ToString();
	s_Json.Parse(s_RequestStr.c_str());
	if (s_Json.HasParseError())
	{
		p_Callback->Failure(QueryError_BadQuery, "Bad query: Failed to parse JSON");
		delete s_Query;
		return true;
	}

	// Get the method name
	auto s_MethodMember = s_Json.FindMember("method");
	if (s_MethodMember == s_Json.MemberEnd())
	{
		p_Callback->Failure(QueryError_BadQuery, "Bad query: A \"method\" value is required");
		delete s_Query;
		return true;
	}
	if (!s_MethodMember->value.IsString())
	{
		p_Callback->Failure(QueryError_BadQuery, "Bad query: \"method\" must be a string");
		delete s_Query;
		return true;
	}
	auto s_Method = s_MethodMember->value.GetString();

	// Look it up
	auto s_MethodIt = m_Methods.find(s_Method);
	if (s_MethodIt == m_Methods.end())
	{
		p_Callback->Failure(QueryError_UnsupportedMethod, "Unsupported method: \"" + std::string(s_Method) + "\"");
		delete s_Query;
		return true;
	}
	s_Query->m_Method = s_MethodIt->second;

	// Get the method arguments object
	auto s_ArgsMember = s_Json.FindMember("args");
	if (s_ArgsMember == s_Json.MemberEnd())
	{
		p_Callback->Failure(QueryError_BadQuery, "Bad query: An \"args\" value is required");
		delete s_Query;
		return true;
	}
	if (!s_ArgsMember->value.IsObject())
	{
		p_Callback->Failure(QueryError_BadQuery, "Bad query: \"args\" must be an object");
		delete s_Query;
		return true;
	}
	s_Query->m_Args = &s_ArgsMember->value;

	// Push the query onto the command queue
	s_Query->m_Callback = p_Callback;
	m_Queries.push(s_Query);
	return true;
}

void WebRendererQueryHandler::OnQueryCanceled(CefRefPtr<CefBrowser> p_Browser, CefRefPtr<CefFrame> p_Frame, int64 p_QueryId)
{
}

void WebRendererQueryHandler::AddMethod(const std::string &p_Name, const WebRendererQuery &p_Method)
{
	m_Methods[p_Name] = p_Method;
}

void WebRendererQueryHandler::Update()
{
	// Process each pending query
	PendingQuery *s_PendingQuery;
	while (m_Queries.pop(s_PendingQuery))
	{
		std::unique_ptr<PendingQuery> s_Query(s_PendingQuery); // For safety

		std::string s_Result;
		auto s_ErrorCode = s_Query->m_Method(*s_Query->m_Args, &s_Result);
		if (s_ErrorCode == QueryError_Ok)
			s_Query->m_Callback->Success(s_Result);
		else
			s_Query->m_Callback->Failure(s_ErrorCode, s_Result);
	}
}