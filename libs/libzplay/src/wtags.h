/*
 *  libzplay - windows ( WIN32 ) multimedia library
 *  ID3 tag
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


#ifndef _W_TAGS_H_
#define _W_TAGS_H_

#include "wtools.h"



// allocate space for each field and initialize to null terminated string
//  PARAMETES:
//     fields	- array of fields
//     number_of_fields	- number of fields in array
//     filed__size_in_wchar	- size of field in UNICODE characters (1 UNICODE char is 2 byes)
//
//  RETURN VALUES:
//     1 - all OK
//     0 - memory allocation errro
int AllocateID3Fields(wchar_t **fields, unsigned int number_of_fields, unsigned int field_size_in_wchar);



// free fields
//  PARAMETES:
//     fields	- array of fields
//     number_of_fields	- number of fields in array
//
//  RETURN VALUES:
//     None.
void FreeID3Fields(wchar_t **fields, unsigned int number_of_fields);


class ID3Tag {
public:
	ID3Tag();
	~ID3Tag();

	wchar_t **LoadID3Info(unsigned char *buffer, unsigned int buffer_size, int version);

private:
	wchar_t *c_fields[ID3_FIELD_NUMBER_EX];

protected:



};


#endif
