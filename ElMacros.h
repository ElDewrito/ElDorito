#pragma once

// Helpful macros
// Inspired/found here: https://github.com/webcoyote/coho/blob/master/Base/Macros.h
//////////////////////////////////////////////////////////////////////////
#define STRINGIZE_AGAIN( MSG ) #MSG
#define STRINGIZE( MSG ) STRINGIZE_AGAIN( MSG )

//Concatenates two identifiers
#define _CONCATIFY(x,y) x##y
#define CONCAT(x,y) _CONCATIFY(x,y)

#define NOTE( MESSAGE ) \
  __pragma(message(__FILE__ "(" STRINGIZE(__LINE__) ") : Note : " MESSAGE " "))

#define WARN( MESSAGE ) \
  __pragma(message(__FILE__ "(" STRINGIZE(__LINE__) ") : Warning : " MESSAGE " "))

#define TODO( MESSAGE ) \
  __pragma(message(__FILE__ "(" STRINGIZE(__LINE__) ") : TODO : " MESSAGE " "))


#define DISABLE_WARNING_START(x) \
__pragma(warning(push)) \
__pragma(warning(disable : x)) \

#define DISABLE_WARNING_END __pragma(warning(pop))
//////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
  #define DEBUG_MODE 1
  #define RELEASE_MODE 0
#else
  #define DEBUG_MODE 0
  #define RELEASE_MODE 1
#endif

#ifdef _ELREDIST
  #define REDIST_MODE 1
#else
  #define REDIST_MODE 0
#endif
