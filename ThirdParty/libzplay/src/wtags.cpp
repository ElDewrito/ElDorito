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
 * date: 22. April, 2010.
 *
*/



#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>


#include "wdecoder.h"
#include "debug.h"
#include "wtools.h"
#include "wtags.h"





typedef struct {
	unsigned char *stream_start;
	unsigned char *stream_end;
	unsigned char *pos;
	unsigned int stream_size;
	unsigned char *unsync_buf;
	unsigned int MajorVersion;
	unsigned int RevisionNumber;
	unsigned int nFlags;
    struct {
    	BOOL Unsync;
        BOOL Extended;
        BOOL Experimental;
        BOOL Footer;
    } Flags;

	unsigned int TagSize;
	unsigned int TotalSize;
} ID3_Header;


void RemoveEndingSpaces(char* buff);
void RemoveEndingSpacesEx(char* buff, int size);
unsigned int id3_deunsynchronise(unsigned char *data, unsigned int length);





// get ID3v1 fields
//  PARAMETERS:
//    header	- ID3 header
//    fields	- fields array
//
//  RETURN VALUES:
//	  1 - all OK
//    0 - error
int GetID3v1(ID3_Header *header, wchar_t **fields);


// get ID3v2 fields
//  PARAMETERS:
//    header	- ID3 header
//    fields	- fields array
//
//  RETURN VALUES:
//	  1 - all OK
//    0 - error
int GetID3v2(ID3_Header *header, wchar_t **fields);

// decode ID3v2 header
//  PARAMETERS:
//    header	- ID3 header
//    fields	- fields array
//
//  RETURN VALUES:
//	  1 - all OK
//    0 - error
int DecodeID3v2Header(ID3_Header *header, wchar_t **fields);

// decode ID3v2.3 header
//  PARAMETERS:
//    header	- ID3 header
//    fields	- fields array
//
//  RETURN VALUES:
//	  1 - all OK
//    0 - error
int decode_id3v2_3(ID3_Header *header, wchar_t **fields);


int decode_id3v2_4(ID3_Header *header, wchar_t **fields);


int decode_id3v2_2(ID3_Header *header, wchar_t **fields);



// get unicode string from ID3 field
//  PARAMETERS:
//    buf - pointer to ID3 data
//   len  - pointer to variable specifying buffer size and receiving returned size in wchar_t
//
//  RETURN VALUES:
//    pointer to allocated memory with null terminated unicode string
//    NULL - error
wchar_t *id3_get_unicode_str(unsigned char *buf, unsigned int *len);


// decode unicode string from ID3 field
//  PARAMETERS:
//    src      - pointer to ID3 data, this pointer will move to next data in text field
//    in_size  - pointer to variable specifying buffer size and receiving size of src buffer when function returns
//    encoding - encoding 
//    out_size - size of returned unicode string in wchar_t
//
//  RETURN VALUES:
//    pointer to allocated memory with null terminated unicode string.
//    NULL - error
wchar_t *id3_decode_unicode_str(unsigned char **src, unsigned int *in_size, unsigned int encoding, unsigned int *out_size);

// get comment unicode string from ID3 field
//  PARAMETERS:
//    buf - pointer to ID3 data
//   len  - pointer to variable specifying buffer size and receiving returned size in wchar_t
//
//  RETURN VALUES:
//    pointer to allocated memory with null terminated unicode string
//    NULL - error
wchar_t *id3_get_comment_unicode_str(unsigned char *buf, unsigned int *len);


// get genre if genre is specified as index (index)
wchar_t *get_genre_from_index_unicode(wchar_t *ptr, unsigned int size);

// decode url string
wchar_t *id3_get_url_unicode_str(unsigned char *buf, unsigned int *len);

// decode picture data
int id3_get_picture_unicode(unsigned char *buf, unsigned int len, wchar_t **fields);


#define GENRES_NUMBER 149
const char *g_sGenres[GENRES_NUMBER] =
	{
		"Blues",
		"Classic Rock",
		"Country",
		"Dance",
		"Disco",
		"Funk",
		"Grunge",
		"Hip-Hop",
		"Jazz",
		"Metal",
		"New Age",
		"Oldies",
		"Other",
		"Pop",
		"R&B",
		"Rap",
		"Reggae",
		"Rock",
		"Techno",
		"Industrial",
		"Alternative",
		"Ska",
		"Death Metal",
		"Pranks",
		"Soundtrack",
		"Euro-Techno",
		"Ambient",
		"Trip Hop",
		"Vocal",
		"Jazz+Funk",
		"Fusion",
		"Trance",
		"Classical",
		"Instrumental",
		"Acid",
		"House",
		"Game",
		"Sound Clip",
		"Gospel",
		"Noise",
		"Alternative Rock",
		"Bass",
		"Soul",
		"Punk",
		"Space",
		"Meditative",
		"Instrumental Pop",
		"Instrumental Rock",
		"Ethnic",
		"Gothic",
		"Darkwave",
		"Techno-Industrial",
		"Electronic",
		"Pop-Folk",
		"Eurodance",
		"Dream",
		"Southern Rock",
		"Comedy",
		"Cult",
		"Gangsta Rap",
		"Top 40",
		"Christian Rap",
		"Pop/Punk",
		"Jungle",
		"Native American",
		"Cabaret",
		"New Wave",
		"Phychedelic",
		"Rave",
		"Showtunes",
		"Trailer",
		"Lo-Fi",
		"Tribal",
		"Acid Punk",
		"Acid Jazz",
		"Polka",
		"Retro",
		"Musical",
		"Rock & Roll",
		"Hard Rock",
		"Folk",
		"Folk/Rock",
		"National Folk",
		"Swing",
		"Fast-Fusion",
		"Bebob",
		"Latin",
		"Revival",
		"Celtic",
		"Blue Grass",
		"Avantegarde",
		"Gothic Rock",
		"Progressive Rock",
		"Psychedelic Rock",
		"Symphonic Rock",
		"Slow Rock",
		"Big Band",
		"Chorus",
		"Easy Listening",
		"Acoustic",
		"Humour",
		"Speech",
		"Chanson",
		"Opera",
		"Chamber Music",
		"Sonata",
		"Symphony",
		"Booty Bass",
		"Primus",
		"Porn Groove",
		"Satire",
		"Slow Jam",
		"Club",
		"Tango",
		"Samba",
		"Folklore",
		"Ballad",
		"power Ballad",
		"Rhythmic Soul",
		"Freestyle",
		"Duet",
		"Punk Rock",
		"Drum Solo",
		"A Capella",
		"Euro-House",
		"Dance Hall",
		"Goa",
		"Drum & Bass",
		"Club-House",
		"Hardcore",
		"Terror",
		"indie",
		"Brit Pop",
		"Negerpunk",
		"Polsk Punk",
		"Beat",
		"Christian Gangsta Rap",
		"Heavy Metal",
		"Black Metal",
		"Crossover",
		"Comteporary Christian",
		"Christian Rock",
		"Merengue",
		"Salsa",
		"Trash Metal",
		"Anime",
		"JPop",
		"Synth Pop",
		"Unknown"
	};




int AllocateID3Fields(wchar_t **fields, unsigned int number_of_fields, unsigned int field_size_in_wchar)
{
	ASSERT_W(fields);

	// temporary array
	wchar_t **buff = (wchar_t**) malloc(number_of_fields * sizeof(wchar_t*));
	if(buff == NULL)
		return 0;


	// allocate memory for each fiels
	unsigned int i;
	for(i = 0; i < number_of_fields; i++)
	{	
		buff[i] = (wchar_t*) malloc(field_size_in_wchar * sizeof(wchar_t));
		if(buff[i] == NULL)
		{
			// allocation error, free previous allocated fields
			unsigned int j;
			for(j = 0; j < i; j++)
				free(buff[j]);

			free(buff);
			return 0;	
		}
		// null terminate field
		buff[i][0] = 0;
	}


	// assing tmp fields to fields
	for(i = 0; i < number_of_fields; i++)	
		fields[i] = buff[i];

	
	
	free(buff);
	
	return 1;
}


void FreeID3Fields(wchar_t **fields, unsigned int number_of_fields)
{
	ASSERT_W(fields);

	unsigned int i;
	// free all ID3 fields
	for(i = 0; i < number_of_fields; i++)
	{	
		if(fields[i] != NULL)
			free(fields[i]);

		fields[i] = NULL;
	}
}


int GetID3v1(ID3_Header *header, wchar_t **fields)
{
	// check if there is ID3v1 tag
	if(header->stream_size < 128 || memcmp(header->stream_end - 127, "TAG", 3) != 0)
		return 0;


	// free previous fields
	FreeID3Fields(fields, ID3_FIELD_NUMBER_EX);

	// allocate and initialize new fields
	if(AllocateID3Fields(fields, ID3_FIELD_NUMBER_EX, 62) == 0)
		return 0;

	// get fields and convert to UNICODE
	char buff[31];

	memcpy(buff, header->stream_end - 124 , 30);
	buff[30] = 0;
	RemoveEndingSpacesEx(buff, 30);
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, buff, -1, fields[ID3_INFO_TITLE], 62);

	memcpy(buff, header->stream_end - 94 , 30);
	buff[30] = 0;
	RemoveEndingSpacesEx(buff, 30);
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, buff, -1, fields[ID3_INFO_ARTIST], 62);

	memcpy(buff, header->stream_end - 64 , 30);
	buff[30] = 0;
	RemoveEndingSpacesEx(buff, 30);
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, buff, -1, fields[ID3_INFO_ALBUM], 62);

	memcpy(buff, header->stream_end - 34 , 4);
	buff[4] = 0;
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, buff, -1, fields[ID3_INFO_YEAR], 62);

	memcpy(buff, header->stream_end - 30 , 30);
	buff[30] = 0;
	if(buff[28] == 0)
	{


		RemoveEndingSpacesEx(buff, 27);
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, buff, -1, fields[ID3_INFO_COMMENT], 62);

		sprintf(buff, "%u", buff[29]);
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, buff, -1, fields[ID3_INFO_TRACK], 62);

	}
	else
	{
		RemoveEndingSpacesEx(buff, 30);
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, buff, -1, fields[ID3_INFO_COMMENT], 62);
	}


	unsigned char genre = (unsigned char) *header->stream_end;
	if(genre >= GENRES_NUMBER)
		genre = GENRES_NUMBER - 1;

	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, g_sGenres[genre], -1, fields[ID3_INFO_GENRE], 62);

	return 1;
}


int GetID3v2(ID3_Header *header, wchar_t **fields)
{
	// free previous fields
	FreeID3Fields(fields, ID3_FIELD_NUMBER_EX);

	// allocate and initialize new fields
	if(AllocateID3Fields(fields, ID3_FIELD_NUMBER_EX, 1) == 0)
		return 0;


	if(DecodeID3v2Header(header, fields) == 0)
	{
		FreeID3Fields(fields, ID3_FIELD_NUMBER_EX);
		return 0;
	}
	

	return 1;


}

int DecodeID3v2Header(ID3_Header *header, wchar_t **fields)
{
	unsigned char *buf = header->stream_start;
	unsigned int buf_size = header->stream_size;

	// if buffer too small, there is no space for ID3 tag header
	if(buf == 0 || buf_size < 10)
		return 0;

	// there is no ID3 identification
	if(memcmp(buf, "ID3", 3) != 0)
		return 0;


	// invalid major version and revision number
	if(buf[3] == 0xFF || buf[4] == 0xFF)
		return 0;

	// invalid tag size
	if(buf[6] >= 0x80 || buf[7] >= 0x80 || buf[8] >= 0x80 || buf[9] >= 0x80)
		return 0;

	// get tag size
	unsigned int tag_size = 0;
	tag_size = DecodeSyncSafeInteger4(buf[6], buf[7], buf[8], buf[9]);

	// check tag size
	if(tag_size + 10 > buf_size)
		return 0;


	// free unsync buffer
	if(header->unsync_buf)
		free(header->unsync_buf);

	header->unsync_buf = 0;


	// get major version, revision number and flags
	header->MajorVersion = buf[3];
	header->RevisionNumber = buf[4];
	header->nFlags = buf[5];

	header->TagSize = tag_size;
	header->TotalSize = header->TagSize + 10;
	header->Flags.Unsync = GetBit(buf[5], 7);
	header->Flags.Extended = GetBit(buf[5], 6);
	header->Flags.Experimental = GetBit(buf[5], 5);
	header->Flags.Footer = GetBit(buf[5], 4);


	// set stream position
	header->stream_start = buf;
	header->stream_size = tag_size;
	header->stream_end = buf + tag_size - 1;
	header->pos = buf + 10;


	switch(header->MajorVersion)
	{
	
		case 2:
		return decode_id3v2_2(header, fields);

		case 3:
		return decode_id3v2_3(header, fields);


		case 4:	
		return decode_id3v2_4(header, fields);
		
	}

	
	return 0;
}


int decode_id3v2_3(ID3_Header *header, wchar_t **fields)
{
	int ret = 0;

	unsigned char *ptr;
	unsigned int size;
	if(header->Flags.Unsync) // deunsync
	{
		header->unsync_buf = (unsigned char*) malloc(header->stream_size);
		if(header->unsync_buf == 0)
			return 0;

		memcpy(header->unsync_buf, header->pos, header->stream_size); 
		header->stream_size = id3_deunsynchronise(header->unsync_buf, header->stream_size);
		
		ptr = header->unsync_buf;
		size = header->stream_size;	
	}
	else
	{
		ptr = header->pos;
		size = header->stream_size;
	}

	
	if(header->Flags.Extended)
	{
		unsigned int pading = (ptr[6] << 24) + (ptr[7] << 16) +(ptr[8] << 8) + (ptr[9]);
		size = size - 10 - ptr[3];
		ptr = ptr + 10 + ptr[3];
	}


	unsigned int len;
	wchar_t **field;
	while(size > 10 && *ptr != 0)
	{
		field = 0;

		len = DecodeInteger4(ptr[4], ptr[5], ptr[6], ptr[7]);

		if(len + 10  > size)
			break;
	

		// check Grouping identity flag
		unsigned int nExtraSize = 0;

		if(GetBit(ptr[9], 5))
			nExtraSize++;
		

		int fExit = 0;
		// check compression flag
		if(GetBit(ptr[9], 7))
		{
			nExtraSize += 4;
			fExit = 1;
		}
		
		// check encryption flag
		if(GetBit(ptr[9], 6))
		{
			nExtraSize++;
			fExit = 1;
		}	
			

		if(nExtraSize > size)
			break;
	

		// check if we have unsupported flags
		if(fExit)
		{
			
			ptr += 10;
			size -= 10;
			ptr += nExtraSize;
			size -= nExtraSize;

			ptr += len;
			size -= len;
			
			continue;	// compression and encryption not supported
		}

		unsigned int data_length = len;


		unsigned int id = *((unsigned int*) ptr);

		switch(id)
		{

			case PACK_ID3_ID('A', 'P', 'I', 'C'): 
			{
				ptr += 10;
				size -= 10;
				ptr += nExtraSize;
				size -= nExtraSize;

				id3_get_picture_unicode(ptr, data_length, fields);
			
				ptr += len;
				size -= len;

			}
			continue;


			case PACK_ID3_ID('T', 'B', 'P', 'M'): 
				field = &fields[ID3_INFO_BPM];
			break;

			case PACK_ID3_ID('T', 'C', 'O', 'M'): 
				field = &fields[ID3_INFO_COMPOSER];
			break;

			case PACK_ID3_ID('T', 'E', 'N', 'C'): 
				field = &fields[ID3_INFO_ENCODER];
			break;

			case PACK_ID3_ID('T', 'P', 'U', 'B'): 
				field = &fields[ID3_INFO_PUBLISHER];
			break;

			case PACK_ID3_ID('T', 'C', 'O', 'P'): 
				field = &fields[ID3_INFO_COPYRIGHT];
			break;

			case PACK_ID3_ID('T', 'O', 'P', 'E'): 
				field = &fields[ID3_INFO_ORIGINAL_ARTIST];
			break;



			case PACK_ID3_ID('W', 'X', 'X', 'X'):
			{
				field = &fields[ID3_INFO_URL];
				ptr += 10;
				size -= 10;
				ptr += nExtraSize;
				size -= nExtraSize;

				wchar_t *str = id3_get_url_unicode_str(ptr, &data_length);
				if(str)
				{
					if(*field)
						free(*field);

					*field =  str;

					ret = 1;
				}
				else
					(*field)[0] = 0;	
			

				ptr += len;
				size -= len;
			}
			continue;


			case PACK_ID3_ID('T', 'P', 'E', '2'): 
				field = &fields[ID3_INFO_ALBUM_ARTIST];
			break;


			case PACK_ID3_ID('T', 'P', 'E', '1'): // TPE1
				field = &fields[ID3_INFO_ARTIST];
			break;

			case PACK_ID3_ID('T', 'I', 'T', '2'): // TIT2
				field = &fields[ID3_INFO_TITLE];
			break;

			case PACK_ID3_ID('T', 'A', 'L', 'B'): // TALB
				field = &fields[ID3_INFO_ALBUM];
			break;

			case PACK_ID3_ID('T', 'R', 'C', 'K'): // TRCK
				field = &fields[ID3_INFO_TRACK];
			break;

			case PACK_ID3_ID('T', 'Y', 'E', 'R'): // TYER
				field = &fields[ID3_INFO_YEAR];
			break;



			case PACK_ID3_ID('T', 'C', 'O', 'N'): // TCON
			{
				field = &fields[ID3_INFO_GENRE];
				ptr += 10;
				size -= 10;
				ptr += nExtraSize;
				size -= nExtraSize;
			
				wchar_t *str = id3_get_unicode_str(ptr, &data_length);

				if(str)
				{
					if(*field)
						free(*field);

					*field = get_genre_from_index_unicode(str, data_length);
					
					ret = 1;	
						
				}
				else
					(*field)[0] = 0;
			
				ptr += len;
				size -= len;

			}
			continue;
			


			case PACK_ID3_ID('C', 'O', 'M', 'M'): // COMM
			{
				field = &fields[ID3_INFO_COMMENT];
				ptr += 10;
				size -= 10;
				ptr += nExtraSize;
				size -= nExtraSize;

				wchar_t *str = id3_get_comment_unicode_str(ptr, &data_length);
				if(str)
				{
					if(*field)
						free(*field);

					*field = str;

					ret = 1;
				}
				else
					(*field)[0] = 0;	
			

				ptr += len;
				size -= len;
			}
			continue;
			

		}
		

		ptr += 10;
		size -= 10;
		ptr += nExtraSize;
		size -= nExtraSize;

		if(field)
		{
			wchar_t *str = id3_get_unicode_str(ptr, &data_length);
			if(str)
			{
				if(*field)
					free(*field);

				*field =  str;
				ret = 1;
			}
			else
				(*field)[0] = 0;

		}

		ptr += len;
		size -= len;

	}

	return ret;
}



int decode_id3v2_2(ID3_Header *header, wchar_t **fields)
{
	unsigned char *ptr;
	unsigned int size;

	int ret = 0;

	if(header->Flags.Extended) // compression not supported
		return 0;
	

	if(header->Flags.Unsync)
	{
		header->unsync_buf = (unsigned char*) malloc(header->stream_size);
		if(header->unsync_buf == 0)
			return 0;

		memcpy(header->unsync_buf, header->pos, header->stream_size); 
		header->stream_size = id3_deunsynchronise(header->unsync_buf, header->stream_size);
		
		ptr = header->unsync_buf;
		size = header->stream_size;
	}
	else
	{
		ptr = header->pos;
		size = header->stream_size;
	}


	unsigned int len;
	wchar_t **field;
	while(size > 6 || *ptr != 0)
	{
		field = 0;
		if(ptr[3] > 0x80 || ptr[4] > 0x80 || ptr[5] > 0x80)
			break;

		len = DecodeInteger3(ptr[3], ptr[4], ptr[5]);

		if(len + 6  > size)
			break;

		unsigned int data_length = len;

		unsigned int id = *((unsigned int*) ptr) & 0x00FFFFFF;

		

		switch(id)
		{

			case PACK_ID3_ID('T', 'B', 'P', '\0'): 
				field = &fields[ID3_INFO_BPM];
			break;

			case PACK_ID3_ID('T', 'C', 'M', '\0'): 
				field = &fields[ID3_INFO_COMPOSER];
			break;

			case PACK_ID3_ID('T', 'E', 'N', '\0'): 
				field = &fields[ID3_INFO_ENCODER];
			break;

			case PACK_ID3_ID('T', 'P', 'B', '\0'): 
				field = &fields[ID3_INFO_PUBLISHER];
			break;

			case PACK_ID3_ID('T', 'C', 'R', '\0'): 
				field = &fields[ID3_INFO_COPYRIGHT];
			break;

			case PACK_ID3_ID('T', 'O', 'A', '\0'): 
				field = &fields[ID3_INFO_ORIGINAL_ARTIST];
			break;


			case PACK_ID3_ID('W', 'X', 'X', '\0'):
			{
				field = &fields[ID3_INFO_URL];
				ptr += 6;
				size -= 6;


				wchar_t *str = id3_get_url_unicode_str(ptr, &data_length);
				if(str)
				{
					if(*field)
						free(*field);

					*field =  str;

					ret = 1;
				}
				else
					(*field)[0] = 0;	
			

				ptr += len;
				size -= len;
			}
			continue;


			case PACK_ID3_ID('T', 'P', '2', '\0'): 
				field = &fields[ID3_INFO_ALBUM_ARTIST];
			break;


			case PACK_ID3_ID('T', 'P', '1', '\0'): // TP1
				field = &fields[ID3_INFO_ARTIST];
			break;

			case PACK_ID3_ID('T', 'T', '2', '\0'): // TT2
				field = &fields[ID3_INFO_TITLE];
			break;

			case PACK_ID3_ID('T', 'A', 'L', '\0'): // TAL
				field = &fields[ID3_INFO_ALBUM];
			break;

			case PACK_ID3_ID('T', 'R', 'K', '\0'): // TRK
				field = &fields[ID3_INFO_TRACK];
			break;

			case PACK_ID3_ID('T', 'Y', 'E', '\0'): // TYE
				field = &fields[ID3_INFO_YEAR];
			break;

			case PACK_ID3_ID('C', 'O', 'M', '\0'): // COM
			{
				field = &fields[ID3_INFO_COMMENT];
				ptr += 6;
				size -= 6;

				wchar_t *str = id3_get_comment_unicode_str(ptr, &data_length);
				if(str)
				{

					if(*field)
						free(*field);

					*field =  str;

					ret = 1;

				}
				else
					(*field)[0] = 0;

				ptr += len;
				size -= len;
			}
			continue;

			case PACK_ID3_ID('T', 'C', 'O', '\0'): // TCO
			{
			
				field = &fields[ID3_INFO_GENRE];

				ptr += 6;
				size -= 6;

				wchar_t *str = id3_get_unicode_str(ptr, &data_length);
				if(str)
				{
					if(*field)
						free(*field);

					*field = get_genre_from_index_unicode( str, data_length);

					ret = 1;
					
				}
				else
					(*field)[0] = 0;

				ptr += len;
				size -= len;

			}
			continue;

		}


		ptr += 6;
		size -= 6;

		if(field)
		{

			wchar_t *str = id3_get_unicode_str(ptr, &data_length);
			if(str)
			{

				if(*field)
					free(*field);

				*field =  str;
				
				ret = 1;	

			}
			else
				(*field)[0] = 0;

		}

		ptr += len;
		size -= len;

	
	}

	return ret;
}


int decode_id3v2_4(ID3_Header *header, wchar_t **fields)
{

	unsigned char *ptr = header->pos;
	unsigned char *end = header->stream_end;
	unsigned int size = header->stream_size;

	int ret = 1;

	if(ptr + 10 > end)
		return 0;


	if(header->Flags.Extended)
	{
		
		if(ptr[0] > 0x80 || ptr[1] > 0x80 || ptr[2] > 0x80 || ptr[3] > 0x80)
			return 0;

		unsigned int ext_size = DecodeSyncSafeInteger4(ptr[0], ptr[1], ptr[2], ptr[3]);
		if(ext_size < 6 || ext_size > size)
			return 0;

		size -= ext_size;
		ptr += ext_size;
	}


	unsigned int len;
	wchar_t **field;


	while(size > 10 && *ptr != 0) // we need enough data and check for padding 0 bytes
	{
		if(header->unsync_buf)
		{
			free(header->unsync_buf);
			header->unsync_buf = 0;
		}


		field = 0;
		// check size
		if(ptr[4] > 0x80 || ptr[5] > 0x80 || ptr[6] > 0x80 || ptr[7] > 0x80)
			break;

		len = DecodeSyncSafeInteger4(ptr[4], ptr[5], ptr[6], ptr[7]);
		if(len + 10  > size)
			break;


		
		unsigned int nExtraSize = 0;

		if(GetBit(ptr[9], 6)) // check Grouping identity flag
			nExtraSize++;


		if(GetBit(ptr[9], 0))	// data length indicator
			nExtraSize += 4;

		int fExit = 0;
		// check compression flag
		if(GetBit(ptr[9], 3))
		{
			fExit = 1;
		}
		
		// check encryption flag
		if(GetBit(ptr[9], 2))
		{
			nExtraSize += 1;
			fExit = 1;
		}	
			

		if(nExtraSize > size)
			break;

		// check if we have unsupported flags
		if(fExit)
		{
			
			ptr += 10;
			size -= 10;
			ptr += len;
			size -= len;
			
			continue;	// compression and encryption not supported
		}


		unsigned int data_length = len;

		int unsync = GetBit(ptr[9], 1); // unsync flag

		
		if(unsync)
		{
			// allocate memory for deunsync
			header->unsync_buf = (unsigned char*) malloc(len + 10);
			if(header->unsync_buf == 0)
			{
				ptr += 10;
				size -= 10;
				ptr += len;
				size -= len;
				continue;
			}
			// make copy of frame
			memcpy(header->unsync_buf, ptr, len + 10);
			data_length = id3_deunsynchronise(header->unsync_buf + 10, len - 10);
			ptr = header->unsync_buf;
			
		}


		unsigned int id = *((unsigned int*) ptr);

		switch(id)
		{

			case PACK_ID3_ID('A', 'P', 'I', 'C'): 
			{
				ptr += 10;
				size -= 10;
				ptr += nExtraSize;
				size -= nExtraSize;

				id3_get_picture_unicode(ptr, data_length, fields);
			
				ptr += len;
				size -= len;

			}
			continue;
		
			case PACK_ID3_ID('T', 'B', 'P', 'M'): 
				field = &fields[ID3_INFO_BPM];
			break;

			case PACK_ID3_ID('T', 'C', 'O', 'M'): 
				field = &fields[ID3_INFO_COMPOSER];
			break;

			case PACK_ID3_ID('T', 'E', 'N', 'C'): 
				field = &fields[ID3_INFO_ENCODER];
			break;

			case PACK_ID3_ID('T', 'P', 'U', 'B'): 
				field = &fields[ID3_INFO_PUBLISHER];
			break;

			case PACK_ID3_ID('T', 'C', 'O', 'P'): 
				field = &fields[ID3_INFO_COPYRIGHT];
			break;

			case PACK_ID3_ID('T', 'O', 'P', 'E'): 
				field = &fields[ID3_INFO_ORIGINAL_ARTIST];
			break;


			case PACK_ID3_ID('W', 'X', 'X', 'X'):
			{
				field = &fields[ID3_INFO_URL];
				ptr += 10;
				size -= 10;
				ptr += nExtraSize;
				size -= nExtraSize;


				wchar_t *str = id3_get_url_unicode_str(ptr, &data_length);
				if(str)
				{
					if(*field)
						free(*field);

					*field =  str;

					ret = 1;
				}
				else
					(*field)[0] = 0;	
			

				ptr += len;
				size -= len;

				if(header->unsync_buf)
				{
					free(header->unsync_buf);
					header->unsync_buf = 0;
				}
			}
			continue;


			case PACK_ID3_ID('T', 'P', 'E', '2'): 
				field = &fields[ID3_INFO_ALBUM_ARTIST];
			break;


		
			case PACK_ID3_ID('T', 'P', 'E', '1'): // TPE1
				field = &fields[ID3_INFO_ARTIST];
			break;

			case PACK_ID3_ID('T', 'I', 'T', '2'): // TIT2
				field = &fields[ID3_INFO_TITLE];
			break;

			case PACK_ID3_ID('T', 'A', 'L', 'B'): // TALB
				field = &fields[ID3_INFO_ALBUM];
			break;

			case PACK_ID3_ID('T', 'R', 'C', 'K'): // TRCK
				field = &fields[ID3_INFO_TRACK];
			break;

			case PACK_ID3_ID('T', 'D', 'R', 'C'): // TDRC
				field = &fields[ID3_INFO_YEAR];
			break;


			case PACK_ID3_ID('T', 'C', 'O', 'N'): // TCON
			{
				field = &fields[ID3_INFO_GENRE];
		
				ptr += 10;
				size -= 10;
				ptr += nExtraSize;
				size -= nExtraSize;
			
				wchar_t *str = id3_get_unicode_str(ptr, &data_length);

				if(str)
				{
					if(*field)
						free(*field);

					*field = get_genre_from_index_unicode( str, data_length);

					ret = 1;

				}
				else
					(*field)[0] = 0;
		

				ptr += len;
				size -= len;

				if(header->unsync_buf)
				{
					free(header->unsync_buf);
					header->unsync_buf = 0;
				}
			}
			continue;


			case PACK_ID3_ID('C', 'O', 'M', 'M'): // COMM
			{
				field = &fields[ID3_INFO_COMMENT];

				ptr += 10;
				size -= 10;
				ptr += nExtraSize;
				size -= nExtraSize;

				wchar_t *str = id3_get_comment_unicode_str(ptr, &data_length);
				if(str)
				{
					if(*field)
						free(*field);

					*field = str;

					ret = 1;
				}
				else
					(*field)[0] = 0;

				ptr += len;
				size -= len;

				if(header->unsync_buf)
				{
					free(header->unsync_buf);
					header->unsync_buf = 0;
				}

			}
			continue;

		}
		

		ptr += 10;
		size -= 10;
		ptr += nExtraSize;
		size -= nExtraSize;

		if(field)
		{
			wchar_t *str = 0;
			if(unsync && header->unsync_buf)
				str = id3_get_unicode_str(header->unsync_buf, &data_length);
			else
				str = id3_get_unicode_str(ptr, &data_length);	

			if(str)
			{
				if(*field)
					free(*field);

				*field =  str;

				ret = 1;


			}
			else
				(*field)[0] = 0;

			
		}

		ptr += len;
		size -= len;

		if(header->unsync_buf)
		{
			free(header->unsync_buf);
			header->unsync_buf = 0;
		}

	}


	return ret;

}


int id3_get_picture_unicode(unsigned char *buf, unsigned int len, wchar_t **fields)
{
	if(len < 3)
		return 0;

	unsigned int enc = buf[0];
	unsigned int out_size = 0;

	buf++;
	len--;

	wchar_t *str = id3_decode_unicode_str(&buf, &len, 0, &out_size);
	wchar_t **field = &fields[ID3_INFO_PICTURE_MIME];

	if(*field)
		free(*field);

	*field = str;


	wchar_t *ptype = (wchar_t*) malloc(12 * sizeof(wchar_t));
	if(ptype)
	{	
		field = &fields[ID3_INFO_PICTURE_TYPE];

		unsigned int t = buf[0];
		swprintf(ptype, L"%u", t);

		if(*field)
			free(*field);

		*field =  ptype;
	}


	buf++;
	len--;

	if(len < 1)
		return 1;

	field = &fields[ID3_INFO_PICTURE_DESCRIPTION];
	str = id3_decode_unicode_str(&buf, &len, enc, &out_size);
	if(*field)
		free(*field);

	*field =  str;

	field = &fields[ID3_INFO_PICTURE_DATA];

	wchar_t *size = (wchar_t*) malloc(12 * sizeof(wchar_t));
	if(size == NULL)
		return 1;	

	wchar_t **field1 = &fields[ID3_INFO_PICTURE_DATA_SIZE];
	swprintf(size, L"%u", len);

	if(*field1)
		free(*field1);

	*field1 = size;


	char *tmp = (char*) malloc(len);
	if(tmp == NULL)
	{
		swprintf(*field1, L"0");	
		return 1;
	}

	memcpy(tmp, buf, len);

	if(*field)
		free(*field);

	*field = (wchar_t*) tmp;

	return 1;
}



unsigned int id3_deunsynchronise(unsigned char *data, unsigned int length)
{
  unsigned char  *old;
  unsigned char *end = data + length;
  unsigned char *new_data;

  if (length == 0)
    return 0;

  for (old = new_data = data; old < end - 1; ++old)
  {
    *new_data++ = *old;
    if (old[0] == 0xff && old[1] == 0x00)
      ++old;
  }

  *new_data++ = *old;

  return new_data - data;
}


wchar_t *id3_get_unicode_str(unsigned char *buf, unsigned int *len)
{

	unsigned int length = *len;
	if(length < 2)
		return NULL;

	length--;

	unsigned int enc = buf[0];
	unsigned int out_size;
	unsigned char *ptr = buf + 1;
	wchar_t *str = id3_decode_unicode_str(&ptr, &length, enc, &out_size);
	*len = out_size;
	return str;
	
}


// decode src string, allocate dest memory and copy string into dest memory
wchar_t *id3_decode_unicode_str(unsigned char **src, unsigned int *in_size, unsigned int encoding, unsigned int *out_size)
{

	unsigned char *str = *src;
	unsigned char *dest = NULL;
	wchar_t *out = NULL;
	unsigned int block_length = *in_size;
	*out_size = 0;
	switch(encoding)
	{
		case 0:  // Latin1
		{
			// determine length of string
			unsigned int i;
			unsigned int len = 0;
			for(i = 0; i < block_length; i++)
			{
				if(str[i] == 0)
					break;

				len++;
			}

			dest = (unsigned char*) malloc(len + 1);
			if(dest == NULL)
				return NULL;

			memcpy(dest, str, len);
			dest[len] = 0;

			unsigned int null_terminated = 0;

			if(len < block_length && str[len] == 0) // null terminated
				null_terminated = 1;

			*src = str + len + null_terminated;
			*in_size = block_length - len - null_terminated;

			// convert to unicode

			int size = 	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, (char*) dest, -1, NULL, 0);
			out = (wchar_t*) malloc((size + 1) * sizeof(wchar_t));
			if(out == NULL)
			{
				free(dest);
				return NULL;
			}

			// null terminate out
			out[0] = 0;
			size = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, (char*) dest, -1, out, size);
			out[size] = 0;
			
			free(dest);
			*out_size = size;

		}
		return out;


		case 1: // UTF-16 [UTF-16] encoded Unicode [UNICODE] with BOM
		{
			// make internal copy

			unsigned char *internal_copy = NULL;

			if(block_length < 2)
				return NULL;

			unsigned char *ptr = str;

			// determine byte order

			int byteorder = (( str[0] << 8 ) | str[1] );
			unsigned int header_size = 0;

			if (byteorder == 0xFFFE) // LE
			{
				ptr += 2;
				block_length -= 2;
				header_size = 2;
				
			}
			else if (byteorder == 0xFEFF)  // BE
			{
				unsigned char *internal_copy = (unsigned char*) malloc(block_length);
				if(internal_copy == NULL)
					return NULL;

				memcpy(internal_copy, str, block_length);

				ptr = internal_copy;


				ptr += 2;
				block_length -= 2;
				header_size = 2;

				// determine string size

				unsigned int i;
				unsigned int len = 0;

				unsigned short *ch = (unsigned short*) ptr;
				for(i = 0; i < block_length / 2; i++)
				{
					if(ch[i] == 0)
						break;
						
					len++;	
				}

				for(i = 0; i < len * 2; i += 2) // swap bytes
				{
					unsigned char c;
					c = ptr[i];
					ptr[i] = ptr[i + 1];
					ptr[i + 1] = c;	
				}	
			}


			// determine string length
			unsigned int i;
			unsigned int len = 0;

			unsigned short *ch = (unsigned short*) ptr;
			for(i = 0; i < block_length / 2; i++)
			{
				if(ch[i] == 0)
					break;
						
				len++;	
			}

			out = (wchar_t*) malloc((len + 1) * sizeof(wchar_t));
			if(out == NULL)
			{
				if(internal_copy)
					free(internal_copy);

				return NULL;
			}

			// copy 
			ch = (unsigned short*) ptr;
			for(i = 0; i < len; i++)
			{
				out[i] = ch[i];	
			}

			out[len] = 0;	// null terminate string

			if(internal_copy)
				free(internal_copy);

		
			unsigned int null_terminated = 0;

			if(len < block_length && ptr[len] == 0) // null terminated
				null_terminated = 2;

			*src = str + header_size + len + null_terminated;
			*in_size = block_length - len - null_terminated;

			*out_size = len;


		}
		return out;


		case 2:  // UTF-16 [UTF-16] encoded Unicode [UNICODE] without BOM
		{

			// determine string length
			unsigned int i;
			unsigned int len = 0;

			unsigned short *ch = (unsigned short*) str;
			for(i = 0; i < block_length / 2; i++)
			{
				if(ch[i] == 0)
					break;
						
				len++;	
			}

			out = (wchar_t*) malloc((len + 1) * sizeof(wchar_t));
			if(out == NULL)
				return NULL;
		

			// copy 
			ch = (unsigned short*) str;
			for(i = 0; i < len; i++)
			{
				out[i] = ch[i];	
			}

			out[len] = 0;	// null terminate string

	
			unsigned int null_terminated = 0;

			if(len < block_length && str[len] == 0) // null terminated
				null_terminated = 2;

			*src = str + len + null_terminated;
			*in_size = block_length - len - null_terminated;

			*out_size = len;
		

		}
		return out;


		case 3: // UTF-8 [UTF-8] encoded Unicode [UNICODE]. Terminated with $00.
		{

			// determine length
			unsigned int i;
			unsigned int len = 0;
			for(i = 0; i < block_length; i++)
			{
				if(str[i] == 0)
					break;

				len++;
			}


			int size = MultiByteToWideChar(CP_UTF8,  0, (char*) str, len, NULL, 0);
			out = (wchar_t*) malloc((size + 1) * sizeof(wchar_t));
			if(out == NULL)
				return NULL;

			out[0] = 0;

			size = MultiByteToWideChar(CP_UTF8,  0, (char*) str, len, out, size);
			out[size] = 0;

			unsigned int null_terminated = 0;

			if(len < block_length && str[len] == 0) // null terminated
				null_terminated = 1;

			*src = str + len + null_terminated;
			*in_size = block_length - len - null_terminated;

			*out_size = size;

		}
		return out;


		default:
		return NULL;
	}

}



wchar_t *id3_get_comment_unicode_str(unsigned char *buf, unsigned int *len)
{
	if(*len < 5)
		return NULL;

	unsigned int enc = buf[0];
	unsigned int out_size;
	unsigned char *ptr = buf + 4;
	unsigned int length = *len - 4;
	wchar_t *str = id3_decode_unicode_str(&ptr, &length, enc, &out_size);
	if(str)
		free(str);

	str = id3_decode_unicode_str(&ptr, &length, enc, &out_size);

	*len = out_size;

	return str;

}


wchar_t *get_genre_from_index_unicode(wchar_t *ptr, unsigned int size)
{
	
	if(size >= 3 && ptr[0] == L'(' && ptr[1] != L'(')
	{		
		// search )
		unsigned int i;
		for(i = 1; i < size; i++)
		{
			if(ptr[i] == L')')
				ptr[i] = 0;
		}
		
		wchar_t *genre_index = ptr + 1;


		unsigned int index = GENRES_NUMBER - 1;

		index = _wtoi(genre_index);
		if(index >= GENRES_NUMBER)
			index = GENRES_NUMBER - 1;


		// convert genre to unicode
		unsigned int size = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, g_sGenres[index], -1, NULL, 0);
		if(size == 0)
			return ptr;

		// allocate memory
		wchar_t *tmp = (wchar_t*) malloc(size  * sizeof(wchar_t));
		if(tmp == NULL)
			return ptr;

		size = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, g_sGenres[index], -1, tmp, size);
		if(size == 0)
		{
			free(tmp);
			return ptr;
		}

		free(ptr);
		return tmp;
	}

	return ptr;
	

}



wchar_t *id3_get_url_unicode_str(unsigned char *buf, unsigned int *len)
{

	if(*len < 1)
		return NULL;

	unsigned int enc = buf[0];
	unsigned int out_size;
	unsigned char *ptr = buf + 1;
	unsigned int length = *len - 1;
	wchar_t *str = id3_decode_unicode_str(&ptr, &length, enc, &out_size);
	if(str)
		free(str);

	str = id3_decode_unicode_str(&ptr, &length, 0, &out_size);
	*len = out_size;
	return str;
}




ID3Tag::ID3Tag()
{
	// initialize fields
	unsigned int i;
	for(i = 0; i < ID3_FIELD_NUMBER_EX; i++)
		c_fields[i] = NULL;
}

ID3Tag::~ID3Tag()
{
	// free fields
	FreeID3Fields(c_fields, ID3_FIELD_NUMBER_EX);

}


wchar_t **ID3Tag::LoadID3Info(unsigned char *buffer, unsigned int buffer_size, int version)
{
	if(buffer == NULL || buffer_size == 0)
		return NULL;

	ID3_Header header;
	
	memset(&header, 0, sizeof(ID3_Header));
	header.stream_start = buffer;
	header.stream_size = buffer_size;
	header.stream_end = buffer + buffer_size - 1;

	int ret = 0;

	switch(version)
	{

		case 1:
			ret = GetID3v1(&header, c_fields);
			if(header.unsync_buf)
				free(header.unsync_buf);

			header.unsync_buf = 0;
			if(ret)
				return c_fields;
		return NULL;

		case 2:
			ret = GetID3v2(&header, c_fields);
			if(header.unsync_buf)
				free(header.unsync_buf);

			header.unsync_buf = 0;
			if(ret)
				return c_fields;
		return NULL;

		default:
		return NULL;
	}

	return NULL;
}


