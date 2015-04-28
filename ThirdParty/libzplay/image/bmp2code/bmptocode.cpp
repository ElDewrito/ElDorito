#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <dos.h>
#include <conio.h>

long filesize(FILE *stream);


void info()
{
	printf("BMP2CODE v1.0 - convert bitmap to C++ code\n\n"
			"usage: BMP2CODE [options] infile [outfile]\n\n"
			"OPTIONS:\n"
			"   -t\tcreate only tables, don't write source code for reading function\n" 
			"   -h\tdisplay help\n"
			
			);
			
}


void help()
{
	printf("BMP2CODE v1.0 - convert bitmap to C++ code\n\n"
			"usage: BMP2CODE [options] infile [outfile]\n\n"
			"OPTIONS:\n"
			"   -t\tcreate only tables, don't write source code for reading function\n" 
			"   -h\tdisplay help\n"
			"\n\n"
			"This function converts uncompressed bitmap into C++ source code.\n"
			"Function generates color table and bit table. Use this tables to\n"
			"create bitmap stored in C++ source code.\n"
			"\r\n\r\n"
			"Example: bitmap 2x2 pixels, 24 bit, 16 bytes in array, no color table.\r\n\r\n"
			"unsigned char bitMap[] = {\r\n"
			"   0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0xFF, 0xFF,\r\n"
			"   0xFF, 0x0, 0x0, 0x0, 0x0, 0x0\r\n"
			"};\r\n\r\n"
			"HBITMAP myBitmap=CreateBitmapFromMemory(myDC, 2, 2, 24, 0, 0, bitMap, 16);\r\n\r\n"

			);
			
}

int main(int argc, char **argv)
{

	if(argc > 1) 
	{
		char *input_filename = 0;
		char *output_filename = 0;
		int fOnlyTables = 0;
		int fHelpInfo = 0;
		// parse input arguments

		int i;
		for(i = 1; i < argc; i++)
		{
			if(argv[i][0] != '-')
			{
				input_filename = argv[i];
				i++;
				// search for output filename
				for(i; i < argc; i++)
				{
					if(argv[i][0] != '-')
					{
						output_filename = argv[i];
						i = argc;
						break;
					}
				}
			}
			else
			{
				if(stricmp(argv[i], "-t") == 0)
					fOnlyTables = 1;
					
				if(stricmp(argv[i], "-h") == 0)
					fHelpInfo = 1;	

			}
		}

		if(fHelpInfo)
		{
			help();
			return 1;

		}


		if(input_filename == 0)
		{
			info();
			return 0;
		}


		char output_filename_tmp[MAX_PATH];
		if(output_filename == 0)
		{
			char tmp_filename[MAX_PATH];
			// create output filename
			strcpy(tmp_filename, input_filename);
			char *start = strrchr(tmp_filename, '\\');
			if(start)
				start++;
			else
				start = tmp_filename;

			char *end = strrchr(start, '.');
			if(end)
				*end = 0;

			sprintf(output_filename_tmp, "%s.txt", start);
			output_filename = output_filename_tmp;
			 
		}

		// open file
		FILE *in = fopen(input_filename, "rb");
		if(in == NULL)
		{
			printf("Error: Can't load bmp image.\r\nPress key to exit.\r\n");
			getch();
			return 0;
		}

		unsigned int size = filesize(in);
		
		// read file header
		BITMAPFILEHEADER fh;
		if(fread(&fh, 1, sizeof(BITMAPFILEHEADER), in) != sizeof(BITMAPFILEHEADER))
		{
			fclose(in);
			printf("Error: Can't read bmp image.\r\nPress key to exit.\r\n");
			getch();
			return 0;
		}

		// read info header
		BITMAPINFOHEADER ih;
		if(fread(&ih, 1, sizeof(BITMAPINFOHEADER), in) != sizeof(BITMAPINFOHEADER))
		{
			fclose(in);
			printf("Error: Can't read bmp image.\r\nPress key to exit.\r\n");
			getch();
			return 0;
		}

		// check if this is valid bitmap
		if(fh.bfType != 19778 || fh.bfSize != size || fh.bfReserved1 != 0 || fh.bfReserved2 != 0)
		{
			fclose(in);
			printf("Error: This is not valid BMP file.\r\nPress key to exit.\r\n");
			getch();
			return 0;

		}

			// check if this is valid bitmap
		if(ih.biSize != sizeof(BITMAPINFOHEADER) || ih.biPlanes != 1)
		{
			fclose(in);
			printf("Error: This is not valid BMP file.\r\nPress key to exit.\r\n");
			getch();
			return 0;

		}

			// check if this is valid bitmap
		if(ih.biBitCount != 1 && ih.biBitCount != 4 && ih.biBitCount != 8 && ih.biBitCount != 16 && 
			ih.biBitCount != 24 && ih.biBitCount != 32)
		{
			fclose(in);
			printf("Error: This is not valid BMP file.\r\nPress key to exit.\r\n");
			getch();
			return 0;

		}

		if(ih.biCompression != BI_RGB)
		{
			fclose(in);
			printf("Error: Comprssed bitmaps isn't supported.\r\nPress key to exit.\r\n");
			getch();
			return 1;
		}

 
		FILE *out = fopen(output_filename, "wt");
		if(out == NULL)
		{
			fclose(in);
			printf("Error: Can't create optput file.\r\nPress key to exit.\r\n");
			getch();
			return 1;

		}

		if(fOnlyTables == 0)
		{
		// print data to output
			fprintf(out, "/* use this function to create bitmap from memory\n */"
						"HBITMAP CreateBitmapFromMemory(HDC hdc,								// handle to device context, can be NULL\n"
						"									int nWidth,							// Specifies the width of the bitmap, in pixels.\n"
						"									int nHeight,						// Specifies the height of the bitmap, in pixels.\n"
						"									unsigned int nBitPerPixel,			// Specifies the number of bits per pixel. This value must be 1, 4, 8, 16, 24, or 32.\n" 
						"									RGBQUAD *rgbColorTable,				// pSpecifies an array of RGBQUAD or doubleword data types that define the colors in the bitmap.\n" 
						"									unsigned int nColorUsed,			// Specifies the number of color indices in the color table ( number of elements in rgbColorTable array)\n"
						"									VOID *pBitmapBitsArray,				// pointer to array containing bitmap data\n"
						"									unsigned int nSizeOfBitmapArray);	// size of pBitmapBitsArray array in bytes.\n"
						"\n\n\n");
		}

		fprintf(out,"/* ========== bitmap stored into color table and bits table ======== */\n\n");
		// write color table
		if(ih.biClrUsed)
		{
			fprintf(out,"/* color table, use this with CreateBitmapFromMemory function */\n");
			fprintf(out,"RGBQUAD pColorTable[] = {\n");
			RGBQUAD rgb;
			fread(&rgb, 1, sizeof(RGBQUAD), in);
			fprintf(out, "	{0x%02X, 0x%02X, 0x%02X, 0x%02X}", rgb.rgbBlue, rgb.rgbGreen, rgb.rgbRed, rgb.rgbReserved); 
			unsigned int i;
			for(i = 1; i < ih.biClrUsed; i++)
			{
				fread(&rgb, 1, sizeof(RGBQUAD), in);
				fprintf(out, ",\n	{0x%02X, 0x%02X, 0x%02X, 0x%02X}", rgb.rgbBlue, rgb.rgbGreen, rgb.rgbRed, rgb.rgbReserved); 

			}

			fprintf(out,"\n};\n\n");
		}
		else
		{
			fprintf(out,"/* color table, use this with CreateBitmapFromMemory function */\n");
			fprintf(out,"RGBQUAD *pColorTable = 0;\n\n\n");
		}


		// write data
		fprintf(out,"/* bitmap bits, use this with CreateBitmapFromMemory function */\n");
		fprintf(out,"unsigned char bitMap[] = {\n");
		unsigned char bits;
		unsigned int num = 0;
		fprintf(out, "	"); 
		while(fread(&bits, 1, sizeof(unsigned char), in))
		{
			if(num > 0)
			{
				if(num % 10)
					fprintf(out, ", ");
				else
					fprintf(out, ",\n	");
			}

			fprintf(out, "0x%02X", bits);	
			num++;	
		}

		fprintf(out,"\n};\n\n");

		fprintf(out,"/* informations about bitmap, use this with CreateBitmapFromMemory function */\n\n");

		fprintf(out,"#define BITMAP_WIDTH %i\n", ih.biWidth);
		fprintf(out,"#define BITMAP_HEIGHT %i\n", ih.biHeight);
		fprintf(out,"#define BITMAP_BIT_PER_PIXEL %u\n", ih.biBitCount);
		fprintf(out,"#define BITMAP_COLOR_USED %u\n", ih.biClrUsed);
		fprintf(out,"#define BITMAP_ARRAY_SIZE %u\n\n", num);

		fprintf(out,"/* ========== bitmap stored into color table and bits table END ======== */\n\n");


		if(fOnlyTables == 0)
		{
			fprintf(out, "/* EXAMPLE: create bitmap from memory tables\n\n");
			fprintf(out,"   HBITMAP myBitmap = CreateBitmapFromMemory(0, BITMAP_WIDTH, BITMAP_HEIGHT, BITMAP_BIT_PER_PIXEL, pColorTable, BITMAP_COLOR_USED, bitMap, BITMAP_ARRAY_SIZE);\n*/\n\n");
		}
	
		
		if(fOnlyTables == 0)
		{
			fprintf(out, "\n\n/* definition of CreateBitmapFromMemory function */\n\n");
			fprintf(out, "HBITMAP CreateBitmapFromMemory(HDC hdc, int nWidth, int nHeight, unsigned int nBitPerPixel, RGBQUAD *rgbColorTable, unsigned int nColorUsed, VOID *pBitmapBitsArray, unsigned int nSizeOfBitmapArray)\n"
						"{\n"
						"	unsigned int fDeleteDC = 0;\n"
						"	if(hdc == 0) {\n"
						"		hdc = CreateCompatibleDC(NULL);\n"
						"		if(hdc == 0) return 0;\n"
						"		fDeleteDC = 1;\n"
						"	}			\n"
						"	if(nWidth == 0 ||nHeight == 0 || nBitPerPixel == 0) return 0;\n"
						"	if(nColorUsed != 0 && rgbColorTable == 0) return 0;\n"
						"	// allocate memory for BITMAPINFO + memory for RGBQUAD color table\n"
						"	BITMAPINFO *pBitmapInfo = (BITMAPINFO*) malloc(sizeof(BITMAPINFO) + nColorUsed * sizeof(RGBQUAD));\n"
						"	if(pBitmapInfo == 0) return 0;\n"
						"	// set BITMAPINFO VALUES\n"
						"	pBitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);\n"
						"	pBitmapInfo->bmiHeader.biWidth = nWidth;\n"
						"	pBitmapInfo->bmiHeader.biHeight = nHeight;\n"
						"	pBitmapInfo->bmiHeader.biPlanes = 1;\n"
						"	pBitmapInfo->bmiHeader.biBitCount = nBitPerPixel; \n"
						"	pBitmapInfo->bmiHeader.biCompression = BI_RGB;\n"
						"	pBitmapInfo->bmiHeader.biSizeImage = nSizeOfBitmapArray;\n"
						"	pBitmapInfo->bmiHeader.biXPelsPerMeter = 0;\n"
						"	pBitmapInfo->bmiHeader.biYPelsPerMeter = 0;\n"
						"	pBitmapInfo->bmiHeader.biClrUsed = nColorUsed;\n"
						"	pBitmapInfo->bmiHeader.biClrImportant = 0;\n"
						"	unsigned int i;\n"
						"	for(i = 0; i < nColorUsed; i++) {\n"
						"		pBitmapInfo->bmiColors[i].rgbBlue = rgbColorTable[i].rgbBlue;\n"
						"		pBitmapInfo->bmiColors[i].rgbGreen = rgbColorTable[i].rgbGreen;\n"
						"		pBitmapInfo->bmiColors[i].rgbRed = rgbColorTable[i].rgbRed;\n"
						"		pBitmapInfo->bmiColors[i].rgbReserved = 0;\n"
						"	}\n"
						"	// create bitmap\n"
						"	VOID *pBits;\n"
						"	HBITMAP hbm = CreateDIBSection(hdc, pBitmapInfo, DIB_RGB_COLORS, &pBits, NULL, 0); \n"
						"	if(fDeleteDC)\n"
						"		DeleteDC(hdc);\n"
						"	free(pBitmapInfo);\n"
						"	if(hbm == 0) return 0;\n"
						"	// copy data to bitmap\n"
						"	memcpy(pBits, pBitmapBitsArray, nSizeOfBitmapArray);\n"
						"	return hbm;\n"
						"}\n"
						"\n\n\n");
		}


		fclose(out);
		fclose(in);
		printf("Bitmap successfully converted to C++ code.\r\n\r\n");
		printf("Width: %u\r\n", ih.biWidth);
		printf("Weight: %u\r\n", ih.biHeight);
		printf("Bit per pixel: %u\r\n", ih.biBitCount);
		printf("Color table size: %u\r\n", ih.biClrUsed);
		printf("Bit array size: %u\r\n\r\n", num);
		

		printf("Output: %s\r\n\r\n", output_filename);
		printf("Press key to exit.\r\n");
		getch();
	
	}
	else
	{
		info();
	}

	return 1;
}



long filesize(FILE *stream)
{
   long curpos, length;

   curpos = ftell(stream);
   fseek(stream, 0L, SEEK_END);
   length = ftell(stream);
   fseek(stream, curpos, SEEK_SET);
   return length;

}
