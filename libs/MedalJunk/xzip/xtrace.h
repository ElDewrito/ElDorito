// XTrace.h  Version 1.1
//
// Author:       Paul Mclachlan
//
// Modified by:  Hans Dietrich
//               hdietrich2@hotmail.com
//
// Version 1.1:  added Unicode support
//               added optional thread id to output string
//               added option to enable/disable full path
//               added TRACERECT macro
//               changed name to avoid conflicts with Paul's class.
//
// This code was taken from article by Paul Mclachlan, "Getting around 
// the need for a vararg #define just to automatically use __FILE__ and 
// __LINE__ in a TRACE macro".  For original article, see
//     http://www.codeproject.com/useritems/location_trace.asp
//
// XTrace.h is a drop-in replacement for MFC's TRACE facility.  It has no
// dependency on MFC.  It is thread-safe and uses no globals or statics.
//
// It optionally adds source module/line number and thread id to each line 
// of TRACE output.  To control these features, use the following defines:
//
//     XTRACE_SHOW_FULLPATH
//     XTRACE_SHOW_THREAD_ID
//
// XTrace.h also provides the TRACERECT macro, which outputs the contents
// of a RECT struct.  In Release builds, no output will be produced.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef XTRACE_H
#define XTRACE_H

#include <stdarg.h>
#include <stdio.h>
#include <windows.h>
#include <tchar.h>

#pragma warning(push)
#pragma warning(disable : 4127)		// conditional expression is constant
#pragma warning(disable : 4996)		// disable bogus deprecation warning

#ifndef INVALID_SET_FILE_POINTER
#define INVALID_SET_FILE_POINTER ((DWORD)-1)
#endif

#define XTRACE_SHOW_FULLPATH	FALSE	// FALSE = only show base name of file
#define XTRACE_SHOW_THREAD_ID	TRUE	// TRUE = include thread id in output
#define XTRACE_FILE				FALSE	// TRUE = output to file

class xtracing_output_debug_string
{
public:
	xtracing_output_debug_string(LPCTSTR lpszFile, int line) :
		m_file(lpszFile),
		m_line(line)
	{
	}

	void operator() (LPCTSTR lpszFormat, ...)
	{
		va_list va;
		va_start(va, lpszFormat);

		TCHAR buf1[BUFFER_SIZE];
		TCHAR buf2[BUFFER_SIZE];

		// add the __FILE__ and __LINE__ to the front
		TCHAR *cp = (LPTSTR) m_file;
		
		if (!XTRACE_SHOW_FULLPATH)
		{
			cp = (TCHAR *)_tcsrchr(m_file, _T('\\'));
			if (cp)
				cp++;
		}

		if (XTRACE_SHOW_THREAD_ID)
		{
			if (_sntprintf(buf1, BUFFER_SIZE-1, _T("%s(%d) : [%X] %s"), 
						cp, m_line, GetCurrentThreadId(), lpszFormat) < 0)
				buf1[BUFFER_SIZE-1] = _T('\0');
		}
		else
		{
			if (_sntprintf(buf1, BUFFER_SIZE-1, _T("%s(%d) : %s"), 
						cp, m_line, lpszFormat) < 0)
				buf1[BUFFER_SIZE-1] = _T('\0');
		}

		// format the message as requested
		if (_vsntprintf(buf2, BUFFER_SIZE-1, buf1, va) < 0)
			buf2[BUFFER_SIZE-1] = _T('\0');

		va_end(va);

		if (XTRACE_FILE)
		{
			TCHAR szPathName[MAX_PATH*2] = { 0 };

			::GetModuleFileName(NULL, szPathName, sizeof(szPathName)/sizeof(TCHAR)-2);

			TCHAR *cp = _tcsrchr(szPathName, _T('\\'));
			if (cp != NULL)
				*(cp+1) = _T('\0');
			_tcscat(szPathName, _T("\\_trace.log"));
			HANDLE hFile = ::CreateFile(szPathName, GENERIC_WRITE, FILE_SHARE_WRITE,
								NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				DWORD dwRC = ::SetFilePointer(hFile,		// handle to file
											  0,			// bytes to move pointer
											  NULL,			// bytes to move pointer
											  FILE_END);	// starting point

				if (dwRC != INVALID_SET_FILE_POINTER)
				{
					DWORD dwWritten = 0;
					::WriteFile(hFile,							// handle to file
								buf2,							// data buffer
								(DWORD)_tcslen(buf2)*sizeof(TCHAR),	// number of bytes to write
								&dwWritten,						// number of bytes written
								NULL);							// overlapped buffer
				}

				::CloseHandle(hFile);
			}
		}
		else
		{
			// write it out
			OutputDebugString(buf2);
		}
	}

private:
	LPCTSTR m_file;
	int     m_line;
	enum    { BUFFER_SIZE = 4096 };
};

class xtracing_entry_output_debug_string
{
public:
	xtracing_entry_output_debug_string(LPCTSTR lpszFile, int line) :
		m_file(lpszFile),
		m_line(line)
	{
	}

	~xtracing_entry_output_debug_string()
	{
		TCHAR buf3[BUFFER_SIZE*3];
		_stprintf(buf3, _T("======  exiting scope:  %s"), buf2);
		OutputDebugString(buf3);
	}

	void operator() (LPCTSTR lpszFormat, ...)
	{
		va_list va;
		va_start(va, lpszFormat);

		TCHAR buf1[BUFFER_SIZE];

		// add the __FILE__ and __LINE__ to the front
		TCHAR *cp = (LPTSTR) m_file;
		
		if (!XTRACE_SHOW_FULLPATH)
		{
			cp = (TCHAR *)_tcsrchr(m_file, _T('\\'));
			if (cp)
				cp++;
		}

		if (XTRACE_SHOW_THREAD_ID)
		{
			if (_sntprintf(buf1, BUFFER_SIZE-1, _T("%s(%d) : [%X] ======  %s"), 
						cp, m_line, GetCurrentThreadId(), lpszFormat) < 0)
				buf1[BUFFER_SIZE-1] = _T('\0');
		}
		else
		{
			if (_sntprintf(buf1, BUFFER_SIZE-1, _T("%s(%d) : ======  %s"), 
						cp, m_line, lpszFormat) < 0)
				buf1[BUFFER_SIZE-1] = _T('\0');
		}

		// format the message as requested
		if (_vsntprintf(buf2, BUFFER_SIZE-1, buf1, va) < 0)
			buf2[BUFFER_SIZE-1] = _T('\0');

		va_end(va);

		if (XTRACE_FILE)
		{
			TCHAR szPathName[MAX_PATH*2] = { 0 };

			::GetModuleFileName(NULL, szPathName, sizeof(szPathName)/sizeof(TCHAR)-2);

			TCHAR *cp = _tcsrchr(szPathName, _T('\\'));
			if (cp != NULL)
				*(cp+1) = _T('\0');
			_tcscat(szPathName, _T("\\_trace.log"));
			HANDLE hFile = ::CreateFile(szPathName, GENERIC_WRITE, FILE_SHARE_WRITE,
								NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				DWORD dwRC = ::SetFilePointer(hFile,		// handle to file
											  0,			// bytes to move pointer
											  NULL,			// bytes to move pointer
											  FILE_END);	// starting point

				if (dwRC != INVALID_SET_FILE_POINTER)
				{
					DWORD dwWritten = 0;
					::WriteFile(hFile,							// handle to file
								buf2,							// data buffer
								(DWORD)_tcslen(buf2)*sizeof(TCHAR),	// number of bytes to write
								&dwWritten,						// number of bytes written
								NULL);							// overlapped buffer
				}

				::CloseHandle(hFile);
			}
		}
		else
		{
			// write it out
			OutputDebugString(buf2);
		}
	}

private:
	LPCTSTR m_file;
	int     m_line;
	enum    { BUFFER_SIZE = 4096 };
	TCHAR buf2[BUFFER_SIZE*2];
};

#undef TRACE
#undef TRACE0
#undef TRACE1
#undef TRACE2

#define  _DEBUGnow

#ifdef _DEBUG

#define TRACE (xtracing_output_debug_string(_T(__FILE__), __LINE__ ))
#define TRACEENTRY (xtracing_output_debug_string(_T(__FILE__), __LINE__ ))
#define TRACEERROR (xtracing_output_debug_string(_T(__FILE__), __LINE__ ))
#define TRACE0 TRACE
#define TRACE1 TRACE
#define TRACE2 TRACE

#else

#ifndef __noop
#if _MSC_VER < 1300
#define __noop ((void)0)
#endif
#endif

#define TRACE		__noop
#define TRACEERROR	__noop
#define TRACE0		__noop
#define TRACE1		__noop
#define TRACE2		__noop

#endif	//_DEBUG

#define TRACEHILO(d) \
			WORD ___hi = HIWORD(d); WORD ___lo = LOWORD(d); \
			TRACE(_T(#d) _T(":  HIWORD = %u  LOWORD = %u\n"), ___hi, ___lo)

#define TRACESIZE(s) TRACE(_T(#s) _T(":  cx = %d  cy = %d\n"), \
                           (s).cx, (s).cy)

#define TRACEPOINT(p) TRACE(_T(#p) _T(":  x = %d  y = %d\n"), \
                           (p).x, (p).y)

#define TRACERECT(r) TRACE(_T(#r) _T(":  left = %d  top = %d  right = %d  bottom = %d\n"), \
                           (r).left, (r).top, (r).right, (r).bottom)

#pragma warning(pop)

#endif //XTRACE_H
