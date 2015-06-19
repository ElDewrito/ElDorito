#pragma once

// Used to remove warnings for unused variables
#define Unused(x) ((void)(x))

// Ending assert, disable constant conditional warning
#define Assert_Macro_End  \
__pragma(warning(push)) \
__pragma(warning(disable : 4127)) \
} while (0) \
__pragma(warning(pop))

// Assert Debug Feature
#ifdef _DEBUG

namespace Utils {
	enum class FailBehavior : int
	{
		Halt,
		Continue,
	};

	FailBehavior ReportFailure(
		const char* condition,
		const char *msg,
		const char* file, const char *fnc,
		long line);
}

#define Assert_Break() __debugbreak()

// Assert w/ message
//////////////////////////////////////////////////////////////////////////
#define Assert_Msg(Condition, Message) \
do \
{ \
if (!(Condition)) \
{ \
  Assert::FailBehavior result = Assert::ReportFailure(#Condition, Message, __FILE__, __FUNCTION__, __LINE__); \
if (result == Assert::FailBehavior::Halt) \
  Assert_Break(); \
} \
Assert_Macro_End
//////////////////////////////////////////////////////////////////////////

// Assert w/ no message
//////////////////////////////////////////////////////////////////////////
#define Utils(Condition) \
do \
{ \
if (!(Condition)) \
{ \
  Assert::FailBehavior result = Assert::ReportFailure(#Condition, "Assertion Failed", __FILE__, __FUNCTION__, __LINE__); \
if (result == Assert::FailBehavior::Halt) \
  Assert_Break(); \
} \
  Assert_Macro_End
//////////////////////////////////////////////////////////////////////////

#else

#define Assert_Msg(ignore, ignore_msg) do { (void)0; Assert_Macro_End
#define Assert_Break() ((void)0)
#define Utils(ignore) do { (void)0; Assert_Macro_End

#endif
