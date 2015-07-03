// CClipboard Class
// ----------------
// Written by David Terracino <davet@lycosemail.com>
//
// http://www.codeguru.com/clipboard/simple_clipboard.shtml
//
// This code is released into the public domain, because
// it's silly easy. If you want to use and expand it, go
// ahead; fine by me. If you do expand this class's
// functionality, please consider sending it back up to
// the MFC Programmer's Sourcebook at www.codeguru.com.
//
// And as always, please give credit where credit is
// due. Don't remove my name from the source.
///////////////////////////////////////////////////////////////////////////////
// Modified by Hans Dietrich  hdietrich@gmail.com
///////////////////////////////////////////////////////////////////////////////

#ifndef CCLIPBOARD_H
#define CCLIPBOARD_H

class CClipboard
{
public:
	static CString GetText();
	static BOOL GetText(LPSTR lpszBuffer, int nBufSize);
	static int GetTextLength();
	static BOOL SetText(LPCTSTR lpszBuffer);
};

#endif