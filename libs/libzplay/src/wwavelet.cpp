/*
 *  libzplay - windows ( WIN32 ) multimedia library
 *  Wavelet packed transformation
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
 * ver: 1.14
 * date: 15. April, 2010.
 *
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>


#include "wwavelet.h"





// D4 coefficients
WAVELET_REAL D4_H[4] = { 0.6830127, 1.1830127, 0.3169873, -0.1830127};
WAVELET_REAL D4_G[4] = { -0.1830127, -0.3169873, 1.1830127, -0.6830127};

WWavelet::WWavelet(unsigned int WaveletType)
{
	c_buff = 0;
	c_nBuffSize = 0;


	switch(WaveletType)
	{

		case D4_WAVELET:
			forward_step = daubechies4_forward_step;
			inverse_step = daubechies4_inverse_step;
		break;


		case HAAR_WAVELET:
		default:
			forward_step = haar_forward_step;
			inverse_step = haar_inverse_step;
		break;

	}
	
    const double sqrt_3 = sqrt( (double) 3 );
    const double denom = 4 * sqrt( (double) 2 );
    
    //
    // forward transform scaling (smoothing) coefficients
    //
    D4_H[0] = (1 + sqrt_3)/denom;
    D4_H[1] = (3 + sqrt_3)/denom;
    D4_H[2] = (3 - sqrt_3)/denom;
    D4_H[3] = (1 - sqrt_3)/denom;
    //
    // forward transform wavelet coefficients (a.k.a. high
    // pass filter coefficients)
    //
    D4_G[0] =  D4_H[3];
    D4_G[1] = -D4_H[2];
    D4_G[2] =  D4_H[1];
    D4_G[3] = -D4_H[0];

}


WWavelet::~WWavelet()
{
	if(c_buff)
		free(c_buff);
}

// Haar wavelet inverse packet transform ( frequency ordered )
void WWavelet::haar_inverse_step(WAVELET_REAL *src, WAVELET_REAL *dest, unsigned int N, unsigned int reverse)
{
	unsigned int i;
	unsigned int j;
	const unsigned int N_half = N >> 1;
	WAVELET_REAL *lp = src;
	WAVELET_REAL *hp = &src[N_half];
		
	if(reverse)
	{
		lp = &src[N_half];
		hp = src;
	}
		
	for(i = 0, j = 0; i < N_half; i++, j += 2)
	{
		WAVELET_REAL a = lp[i];
		WAVELET_REAL b = hp[i];
		dest[j] = a + b;
		dest[j + 1] = a - b;
	}
}

// Haar wavelet packet transform ( frequency ordered )
void WWavelet::haar_forward_step(WAVELET_REAL *src, WAVELET_REAL *dest, unsigned int N, unsigned int reverse)
{

	unsigned int i;
	unsigned int j;
	const unsigned int N_half = N >> 1;
	WAVELET_REAL *lp = dest;
	WAVELET_REAL *hp = &dest[N_half];
		
	if(reverse)
	{
		lp = &dest[N_half];
		hp = dest;
	}
		
	for(i = 0, j = 0; i < N; i += 2, j++)
	{
		WAVELET_REAL a = src[i];
		WAVELET_REAL b = src[i + 1];
		lp[j] = (a + b) / 2.0;	// scaling function ( low pass filter)
		hp[j] = (a - b) / 2.0;	// wavelet function (high pass filter)
	}
}

// Daubechies D4
void WWavelet::daubechies4_forward_step(WAVELET_REAL *src, WAVELET_REAL *dest, unsigned int N, unsigned int reverse)	
{

	unsigned int i;
	unsigned int j;
	const unsigned int N_half = N >> 1;
	WAVELET_REAL *lp = dest;
	WAVELET_REAL *hp = &dest[N_half];
		
	if(reverse)
	{
		lp = &dest[N_half];
		hp = dest;
	}

	for(i = 0, j = 0; i < N - 3; i += 2, j++)
	{
		lp[j] = src[i] * D4_H[0] + src[i + 1] * D4_H[1] + src[i + 2] * D4_H[2] + src[i + 3] * D4_H[3];	// scaling function ( low pass filter)
		hp[j] = src[i] * D4_G[0] + src[i + 1] * D4_G[1] + src[i + 2] * D4_G[2] + src[i + 3] * D4_G[3];	// wavelet functio (high pass filter)
	}

	lp[j] = src[N - 2] * D4_H[0] + src[N - 1] * D4_H[1] + src[0] * D4_H[2] + src[1] * D4_H[3];
	hp[j] = src[N - 2] * D4_G[0] + src[N - 1] * D4_G[1] + src[0] * D4_G[2] + src[1] * D4_G[3];	
	
}



// Daubechies D4 inverse packet transform ( frequency ordered )
void WWavelet::daubechies4_inverse_step(WAVELET_REAL *src, WAVELET_REAL *dest, unsigned int N, unsigned int reverse)
{

	unsigned int i;
	unsigned int j;
	const unsigned int N_half = N >> 1;
	WAVELET_REAL *lp = src;
	WAVELET_REAL *hp = &src[N_half];
		
	if(reverse)
	{
		lp = &src[N_half];
		hp = src;
	}
		
	dest[0] = lp[N_half - 1] * D4_H[2] + hp[N_half - 1] * D4_G[2] + lp[0] * D4_H[0] + hp[0] * D4_G[0];
	dest[1] = lp[N_half - 1] * D4_H[3] + hp[N_half - 1] * D4_G[3] + lp[0] * D4_H[1] + hp[0] * D4_G[1];

	for(i = 0, j = 2; i < (N_half - 1); i++, j += 2)
	{

		dest[j] = lp[i] * D4_H[2] + hp[i] * D4_G[2] + lp[i + 1] * D4_H[0] + hp[i + 1] * D4_G[0];
		dest[j + 1] = lp[i] * D4_H[3] + hp[i] * D4_G[3] + lp[i + 1] * D4_H[1] + hp[i + 1] * D4_G[1];
	}	
}




int WWavelet::ForwardTrans(WAVELET_REAL* data, unsigned int N, unsigned int Level)
{

	if(N > 1 && (N & ( N - 1)) == 0)
	{
		// reallocate buffer if needed
		if(c_nBuffSize != N)
		{
			if(c_buff)
				free(c_buff);

			c_nBuffSize = 0;

			// allocate internal buffers
			c_buff = (WAVELET_REAL*) malloc(N * sizeof(WAVELET_REAL));
			if(c_buff == 0)
				return 0;

			c_nBuffSize = N;
		}


		unsigned int i;
		WAVELET_REAL *src = data;
		WAVELET_REAL *dest = c_buff;


		unsigned int Roots = 1;
		unsigned int LevelCounter = 0;
		unsigned int N1 = N;		
		while(N1 > 1)
		{
			unsigned int reverse = 0;
			for (i = 0; i < Roots; i++)
			{
				WAVELET_REAL *s = &src[N1 * i];
				WAVELET_REAL *d = &dest[N1 * i];
				forward_step(s, d, N1, reverse);
				reverse = reverse ^ 1;

			}

			// switch buffers
			WAVELET_REAL *tmp = src;
			src = dest;
			dest = tmp;


			LevelCounter++;
			if(LevelCounter == Level)
				break;
				
			N1 = N1 >> 1;
			Roots = Roots << 1;
		}

		// copy result data into input buffer
		if(src != data)
		{
			for(i = 0; i < N; i++)
				data[i] = src[i];
		}
		return LevelCounter;
	}

	if(c_buff)
		free(c_buff);

	c_buff = 0;
	c_nBuffSize = 0;

	return 0;
}



// combine low pass filter and high pass filter data to original data
int WWavelet::InverseTrans(WAVELET_REAL* data, unsigned int N, unsigned int Level)
{

	if(N > 1 && (N & ( N - 1)) == 0)
	{
		// reallocate buffer if needed
		if(c_nBuffSize != N)
		{
			if(c_buff)
				free(c_buff);

			c_nBuffSize = 0;

			// allocate internal buffers
			c_buff = (WAVELET_REAL*) malloc(N * sizeof(WAVELET_REAL));
			if(c_buff == 0)
				return 0;

			c_nBuffSize = N;
		}


		unsigned int i;
		WAVELET_REAL *src = data;
		WAVELET_REAL *dest = c_buff;

		while(Level)
		{
			unsigned int reverse = 0;
			unsigned int N1 = N / ( 1 << (Level - 1));
			unsigned int Roots =  1 << ( Level - 1);
			for (i = 0; i < Roots; i++)
			{
				WAVELET_REAL *s = &src[N1 * i];
				WAVELET_REAL *d = &dest[N1 * i];
				inverse_step(s, d, N1, reverse);
				reverse = reverse ^ 1;

			}

			// switch buffers
			WAVELET_REAL *tmp = src;
			src = dest;
			dest = tmp;
			Level--;

			
		}

		// copy result data into input buffer
		if(src != data)
		{
			for(i = 0; i < N; i++)
				data[i] = src[i];
		}

		return 1;
	}

	if(c_buff)
		free(c_buff);

	c_buff = 0;
	c_nBuffSize = 0;

	return 0;
}

