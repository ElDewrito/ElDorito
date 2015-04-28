/*
 *  libzplay - windows ( WIN32 ) multimedia library
 *  TOols
 *
 *  Copyright (C) 2003-2009 Zoran Cindori ( zcindori@inet.hr )
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 * ver: 1.15
 * date: 12. April, 2010.
 *
*/


#ifndef _W_TOOLS_H_
#define _W_TOOLS_H_


#define DecodeSyncSafeInteger4(value1, value2, value3, value4) (((value1 & 0x7F) << 21) | ((value2 & 0x7F) << 14) | ((value3 & 0x7F) << 7) | (value4 & 0x7F))

#define GetBit(value, pos) ( (value >> pos) & 0x01)
#define DecodeInteger4(value1, value2, value3, value4) ((value1 << 24) | (value2 << 16) | ( value3 << 8 ) | value4 )
#define DecodeInteger3(value1, value2, value3) ((value1 << 16) | (value2 << 8) |  value3 )
#define DecodeSyncSafeInteger3(value1, value2, value3) (((value1 & 0x7F) << 14) | ((value2 & 0x7F) << 7) | (value3 & 0x7F))
#define PACK_ID3_ID(a, b, c, d)  a << 0 | b << 8 | c << 16 | d << 24



char *UTF16ToANSI(wchar_t *src, int nCharacterNumber);
wchar_t *ANSIToUTF16(char *src, int nCharacterNumber);
wchar_t *UTF8ToUTF16(char *src, int nCharacterNumber);


/* This uses that the expression (n+(k-1))/k means the smallest
   integer >= n/k, i.e., the ceiling of n/k.  */
# define BASE64_LENGTH(inlen) ((((inlen) + 2) / 3) * 4)

extern bool isbase64 (char ch);

extern void base64_encode (const char *in, size_t inlen,
			   char *out, size_t outlen);

extern size_t base64_encode_alloc (const char *in, size_t inlen, char **out);

extern bool base64_decode (const char *in, size_t inlen,
			   char *out, size_t *outlen);

extern bool base64_decode_alloc (const char *in, size_t inlen,
				 char **out, size_t *outlen);








#endif
