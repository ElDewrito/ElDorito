#pragma once

#include <functional>
#include "../../ThirdParty/rapidjson/document.h"

namespace Anvil::Client::Rendering::Bridge
{
	enum QueryError
	{
		QueryError_Ok,
		QueryError_UnknownError,
		QueryError_JsException,
		QueryError_BadQuery,
		QueryError_UnsupportedMethod,
		QueryError_InvalidArgument,
		QueryError_NetworkError,
		QueryError_NotAvailable,
		QueryError_CommandFailed
	};

	// Signature for a function that handles queries.
	// p_Args contains the JSON data passed to the method.
	// On return, p_Result should contain the result string.
	// If QueryError_Ok is not returned, p_Result should contain an error message.
	typedef std::function<QueryError(const rapidjson::Value &p_Args, std::string *p_Result)> WebRendererQuery;
}
