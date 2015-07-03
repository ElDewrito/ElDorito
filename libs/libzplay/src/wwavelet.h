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
 
 
 
#ifndef _W_WAVELET_H_
#define _W_WAVELET_H_


#define HAAR_WAVELET 0
#define D4_WAVELET 1


#define WAVELET_REAL float


typedef void (* forward_step_func)(WAVELET_REAL *src, WAVELET_REAL *dest, unsigned int N, unsigned int reverse);
typedef void (* inverse_step_func)(WAVELET_REAL *src, WAVELET_REAL *dest, unsigned int N, unsigned int reverse);

class WWavelet  {
public:
	WWavelet(unsigned int WaveletType);
	~WWavelet();


	// transform data into 2 subsets ( low pass filter and high pass filter )
	int ForwardTrans(WAVELET_REAL* data, unsigned int N, unsigned int Level);
	
	// combine low pass filter and high pass filter data to original data
	int InverseTrans(WAVELET_REAL* data, unsigned int N, unsigned int Level);



private:
	WAVELET_REAL *c_buff;
	unsigned int c_nBuffSize;


	forward_step_func forward_step;
	inverse_step_func inverse_step;


	// Haar wavelet inverse packet transform ( frequency ordered )
	static void haar_inverse_step(WAVELET_REAL *src, WAVELET_REAL *dest, unsigned int N, unsigned int reverse);

	// Haar wavelet packet transform ( frequency ordered )
	static void haar_forward_step(WAVELET_REAL *src, WAVELET_REAL *dest, unsigned int N, unsigned int reverse);

	// Daubechies D4
	static void daubechies4_forward_step(WAVELET_REAL *src, WAVELET_REAL *dest, unsigned int N, unsigned int reverse);	
	
	// Daubechies D4 inverse packet transform ( frequency ordered )
	static void daubechies4_inverse_step(WAVELET_REAL *src, WAVELET_REAL *dest, unsigned int N, unsigned int reverse);


};








#endif

