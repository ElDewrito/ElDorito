/*
 *  libzplay - windows ( WIN32 ) multimedia library for playing mp3, mp2, ogg, wav, flac and raw PCM streams
 *  FFT class
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
 *	SUPPORTED BY:
 * ============================================================================
 * FFT part is a routine made by Mr.Ooura. This routine is a freeware.
 * Contact Mr.Ooura for details of distributing licenses.
 * http://momonga.t.u-tokyo.ac.jp/~ooura/fft.html
 *
*/

#ifndef _W_FFT_H_
#define _W_FFT_H_


#ifndef HIGHPREC
#define REAL_IS_FLOAT
#else
#define REAL_IS_DOUBLE
#endif

#if 0
#undef REAL_IS_FLOAT
#undef REAL_IS_DOUBLE
#undef REAL_IS_LONGDOUBLE
#endif

/*
Fast Fourier/Cosine/Sine Transform
    dimension   :one
    data length :power of 2
    decimation  :frequency
    radix       :split-radix
    data        :inplace
    table       :use
functions
    cdft: Complex Discrete Fourier Transform
    rdft: Real Discrete Fourier Transform
    ddct: Discrete Cosine Transform
    ddst: Discrete Sine Transform
    dfct: Cosine Transform of RDFT (Real Symmetric DFT)
    dfst: Sine Transform of RDFT (Real Anti-symmetric DFT)
function prototypes
    void cdft(int, int, REAL *, int *, REAL *);
    void rdft(int, int, REAL *, int *, REAL *);
    void ddct(int, int, REAL *, int *, REAL *);
    void ddst(int, int, REAL *, int *, REAL *);
    void dfct(int, REAL *, REAL *, int *, REAL *);
    void dfst(int, REAL *, REAL *, int *, REAL *);


-------- Complex DFT (Discrete Fourier Transform) --------
    [definition]
        <case1>
            X[k] = sum_j=0^n-1 x[j]*exp(2*pi*i*j*k/n), 0<=k<n
        <case2>
            X[k] = sum_j=0^n-1 x[j]*exp(-2*pi*i*j*k/n), 0<=k<n
        (notes: sum_j=0^n-1 is a summation from j=0 to n-1)
    [usage]
        <case1>
            ip[0] = 0; // first time only
            cdft(2*n, 1, a, ip, w);
        <case2>
            ip[0] = 0; // first time only
            cdft(2*n, -1, a, ip, w);
    [parameters]
        2*n            :data length (int)
                        n >= 1, n = power of 2
        a[0...2*n-1]   :input/output data (REAL *)
                        input data
                            a[2*j] = Re(x[j]), 
                            a[2*j+1] = Im(x[j]), 0<=j<n
                        output data
                            a[2*k] = Re(X[k]), 
                            a[2*k+1] = Im(X[k]), 0<=k<n
        ip[0...*]      :work area for bit reversal (int *)
                        length of ip >= 2+sqrt(n)
                        strictly, 
                        length of ip >= 
                            2+(1<<(int)(log(n+0.5)/log(2))/2).
                        ip[0],ip[1] are pointers of the cos/sin table.
        w[0...n/2-1]   :cos/sin table (REAL *)
                        w[],ip[] are initialized if ip[0] == 0.
    [remark]
        Inverse of 
            cdft(2*n, -1, a, ip, w);
        is 
            cdft(2*n, 1, a, ip, w);
            for (j = 0; j <= 2 * n - 1; j++) {
                a[j] *= 1.0 / n;
            }
        .


-------- Real DFT / Inverse of Real DFT --------
    [definition]
        <case1> RDFT
            R[k] = sum_j=0^n-1 a[j]*cos(2*pi*j*k/n), 0<=k<=n/2
            I[k] = sum_j=0^n-1 a[j]*sin(2*pi*j*k/n), 0<k<n/2
        <case2> IRDFT (excluding scale)
            a[k] = (R[0] + R[n/2]*cos(pi*k))/2 + 
                   sum_j=1^n/2-1 R[j]*cos(2*pi*j*k/n) + 
                   sum_j=1^n/2-1 I[j]*sin(2*pi*j*k/n), 0<=k<n
    [usage]
        <case1>
            ip[0] = 0; // first time only
            rdft(n, 1, a, ip, w);
        <case2>
            ip[0] = 0; // first time only
            rdft(n, -1, a, ip, w);
    [parameters]
        n              :data length (int)
                        n >= 2, n = power of 2
        a[0...n-1]     :input/output data (REAL *)
                        <case1>
                            output data
                                a[2*k] = R[k], 0<=k<n/2
                                a[2*k+1] = I[k], 0<k<n/2
                                a[1] = R[n/2]
                        <case2>
                            input data
                                a[2*j] = R[j], 0<=j<n/2
                                a[2*j+1] = I[j], 0<j<n/2
                                a[1] = R[n/2]
        ip[0...*]      :work area for bit reversal (int *)
                        length of ip >= 2+sqrt(n/2)
                        strictly, 
                        length of ip >= 
                            2+(1<<(int)(log(n/2+0.5)/log(2))/2).
                        ip[0],ip[1] are pointers of the cos/sin table.
        w[0...n/2-1]   :cos/sin table (REAL *)
                        w[],ip[] are initialized if ip[0] == 0.
    [remark]
        Inverse of 
            rdft(n, 1, a, ip, w);
        is 
            rdft(n, -1, a, ip, w);
            for (j = 0; j <= n - 1; j++) {
                a[j] *= 2.0 / n;
            }
        .


-------- DCT (Discrete Cosine Transform) / Inverse of DCT --------
    [definition]
        <case1> IDCT (excluding scale)
            C[k] = sum_j=0^n-1 a[j]*cos(pi*j*(k+1/2)/n), 0<=k<n
        <case2> DCT
            C[k] = sum_j=0^n-1 a[j]*cos(pi*(j+1/2)*k/n), 0<=k<n
    [usage]
        <case1>
            ip[0] = 0; // first time only
            ddct(n, 1, a, ip, w);
        <case2>
            ip[0] = 0; // first time only
            ddct(n, -1, a, ip, w);
    [parameters]
        n              :data length (int)
                        n >= 2, n = power of 2
        a[0...n-1]     :input/output data (REAL *)
                        output data
                            a[k] = C[k], 0<=k<n
        ip[0...*]      :work area for bit reversal (int *)
                        length of ip >= 2+sqrt(n/2)
                        strictly, 
                        length of ip >= 
                            2+(1<<(int)(log(n/2+0.5)/log(2))/2).
                        ip[0],ip[1] are pointers of the cos/sin table.
        w[0...n*5/4-1] :cos/sin table (REAL *)
                        w[],ip[] are initialized if ip[0] == 0.
    [remark]
        Inverse of 
            ddct(n, -1, a, ip, w);
        is 
            a[0] *= 0.5;
            ddct(n, 1, a, ip, w);
            for (j = 0; j <= n - 1; j++) {
                a[j] *= 2.0 / n;
            }
        .


-------- DST (Discrete Sine Transform) / Inverse of DST --------
    [definition]
        <case1> IDST (excluding scale)
            S[k] = sum_j=1^n A[j]*sin(pi*j*(k+1/2)/n), 0<=k<n
        <case2> DST
            S[k] = sum_j=0^n-1 a[j]*sin(pi*(j+1/2)*k/n), 0<k<=n
    [usage]
        <case1>
            ip[0] = 0; // first time only
            ddst(n, 1, a, ip, w);
        <case2>
            ip[0] = 0; // first time only
            ddst(n, -1, a, ip, w);
    [parameters]
        n              :data length (int)
                        n >= 2, n = power of 2
        a[0...n-1]     :input/output data (REAL *)
                        <case1>
                            input data
                                a[j] = A[j], 0<j<n
                                a[0] = A[n]
                            output data
                                a[k] = S[k], 0<=k<n
                        <case2>
                            output data
                                a[k] = S[k], 0<k<n
                                a[0] = S[n]
        ip[0...*]      :work area for bit reversal (int *)
                        length of ip >= 2+sqrt(n/2)
                        strictly, 
                        length of ip >= 
                            2+(1<<(int)(log(n/2+0.5)/log(2))/2).
                        ip[0],ip[1] are pointers of the cos/sin table.
        w[0...n*5/4-1] :cos/sin table (REAL *)
                        w[],ip[] are initialized if ip[0] == 0.
    [remark]
        Inverse of 
            ddst(n, -1, a, ip, w);
        is 
            a[0] *= 0.5;
            ddst(n, 1, a, ip, w);
            for (j = 0; j <= n - 1; j++) {
                a[j] *= 2.0 / n;
            }
        .


-------- Cosine Transform of RDFT (Real Symmetric DFT) --------
    [definition]
        C[k] = sum_j=0^n a[j]*cos(pi*j*k/n), 0<=k<=n
    [usage]
        ip[0] = 0; // first time only
        dfct(n, a, t, ip, w);
    [parameters]
        n              :data length - 1 (int)
                        n >= 2, n = power of 2
        a[0...n]       :input/output data (REAL *)
                        output data
                            a[k] = C[k], 0<=k<=n
        t[0...n/2]     :work area (REAL *)
        ip[0...*]      :work area for bit reversal (int *)
                        length of ip >= 2+sqrt(n/4)
                        strictly, 
                        length of ip >= 
                            2+(1<<(int)(log(n/4+0.5)/log(2))/2).
                        ip[0],ip[1] are pointers of the cos/sin table.
        w[0...n*5/8-1] :cos/sin table (REAL *)
                        w[],ip[] are initialized if ip[0] == 0.
    [remark]
        Inverse of 
            a[0] *= 0.5;
            a[n] *= 0.5;
            dfct(n, a, t, ip, w);
        is 
            a[0] *= 0.5;
            a[n] *= 0.5;
            dfct(n, a, t, ip, w);
            for (j = 0; j <= n; j++) {
                a[j] *= 2.0 / n;
            }
        .


-------- Sine Transform of RDFT (Real Anti-symmetric DFT) --------
    [definition]
        S[k] = sum_j=1^n-1 a[j]*sin(pi*j*k/n), 0<k<n
    [usage]
        ip[0] = 0; // first time only
        dfst(n, a, t, ip, w);
    [parameters]
        n              :data length + 1 (int)
                        n >= 2, n = power of 2
        a[0...n-1]     :input/output data (REAL *)
                        output data
                            a[k] = S[k], 0<k<n
                        (a[0] is used for work area)
        t[0...n/2-1]   :work area (REAL *)
        ip[0...*]      :work area for bit reversal (int *)
                        length of ip >= 2+sqrt(n/4)
                        strictly, 
                        length of ip >= 
                            2+(1<<(int)(log(n/4+0.5)/log(2))/2).
                        ip[0],ip[1] are pointers of the cos/sin table.
        w[0...n*5/8-1] :cos/sin table (REAL *)
                        w[],ip[] are initialized if ip[0] == 0.
    [remark]
        Inverse of 
            dfst(n, a, t, ip, w);
        is 
            dfst(n, a, t, ip, w);
            for (j = 1; j <= n - 1; j++) {
                a[j] *= 2.0 / n;
            }
        .


Appendix :
    The cos/sin table is recalculated when the larger table required.
    w[] and ip[] are compatible with all routines.
*/




#include <stdlib.h>
#include <math.h>

#ifdef REAL_IS_FLOAT
	typedef float REAL;

	#define SIN(x) sinf(x)
	#define COS(x) cosf(x)

	#define CDFT_RECURSIVE_N 1024
#endif

#ifdef REAL_IS_DOUBLE
	typedef double REAL;

	#define SIN(x) sin(x)
	#define COS(x) cos(x)

	#define CDFT_RECURSIVE_N 512
#endif

#ifdef REAL_IS_LONGDOUBLE
	typedef long double REAL;

	#define SIN(x) sinl(x)
	#define COS(x) cosl(x)

	#define CDFT_RECURSIVE_N 512
#endif

#define PI 3.1415926535897932384626433832795029L
#define PI2 6.283185307179586476925286766559L



void cdft(int n, int isgn, REAL *a, int *ip, REAL *w);
void rdft(int n, int isgn, REAL *a, int *ip, REAL *w);
void ddct(int n, int isgn, REAL *a, int *ip, REAL *w);
void ddst(int n, int isgn, REAL *a, int *ip, REAL *w);
void dfct(int n, REAL *a, REAL *t, int *ip, REAL *w);
void dfst(int n, REAL *a, REAL *t, int *ip, REAL *w);



void fft(REAL *data, long size);
void ifft(REAL* data, long size);
void realfft_split(REAL *data,long n);
void irealfft_split(REAL *data,long n);


// FFT WINDOW
#define  FFT_WINDOW_RECTANGULAR 1
#define  FFT_WINDOW_HAMMING 2
#define  FFT_WINDOW_HANN 3	
#define  FFT_WINDOW_COSINE	4
#define  FFT_WINDOW_LANCZOS 5	
#define  FFT_WINDOW_BARTLETT 6
#define  FFT_WINDOW_TRIANGULAR	7
#define  FFT_WINDOW_GAUSS 8
#define  FFT_WINDOW_BARTLETT_HANN 9
#define  FFT_WINDOW_BLACKMAN 10
#define  FFT_WINDOW_NUTTALL 11
#define  FFT_WINDOW_BLACKMAN_HARRIS 12
#define  FFT_WINDOW_BLACKMAN_NUTTALL 13
#define  FFT_WINDOW_FLAT_TOP 14

class WFFT {
public:
	WFFT();
	~WFFT();

// =========================================================================================================================
	// initialize class
	int Initialize(unsigned int nFFTPoints, unsigned int nSampleRate, unsigned int nChannel, unsigned int nBitPerSample);

	//	PARAMETERS:
	//		nFFTPoints
	//			Number of FFT points. Must be power of 2.
	//
	//		nSampleRate
	//			Sample rate.
	//
	//		nChannel
	//			Number of channels. Must be 1 or 2
	//
	//		nBitPerSample
	//			Bit per sample. Must be 8, 16 or 32.
	//
	//	RETURN VALUES:
	//		1	- all OK
	//		0	- error
// ==========================================================================================================================
	// get number of harmonics
	unsigned int GetNumberOfHarmonics();

	//	PARAMETER:
	//		None.
	//
	//	RETURN VALUES:
	//		Number of harmonics.
// ==========================================================================================================================
	// get harmonics frequencies
	int GetHarmonicFreqReal(REAL *pHarmonicFreq);

	//	PARAMETERS:
	//		pHarmonicFreq
	//			Pointer to buffer receiving harmonic frequencies. User must allocate this buffer.
	//			Call GetNumberOfHarmonics to determine number of harmonics and then allocate memory for this buffer.
	//			If this parameter is NULL, function returns number of harmonics.
	//
	//	RETURN VALUES:
	//		Function returns number of harmonics.
	//		If pHarmonicFreq is NULL, function returns number of harmonics.
// ==========================================================================================================================
	// get harmonics frequencies
	int GetHarmonicFreqInt(int *pHarmonicFreq);

	//	PARAMETERS:
	//		pHarmonicFreq
	//			Pointer to buffer receiving harmonic frequencies. User must allocate this buffer.
	//			Call GetNumberOfHarmonics to determine number of harmonics and then allocate memory for this buffer.
	//			If this parameter is NULL, function returns number of harmonics.
	//
	//	RETURN VALUES:
	//		Number of harmonics.
	//		If pHarmonicFreq is NULL, function returns number of harmonics.
// ==========================================================================================================================

	// set FFT window
	int SetWindow(unsigned int nWindow);
	//
	//	PARAMETERS:
	//		nWindow
	//			FFT window.
	//
	//	RETURN VALUES:
	//		1	- all OK
	//		0	- error
// =======================================

	int GetWindow();

// ==========================================================================================================================
	// free initialized memory
	int Free();

	//	PARAMETERS:
	//		None.
	//
	//	RETURN VALUES:
	//		1	- all OK
	//		0	- error
// ==========================================================================================================================
	// calculate FFT

	int CalculateFFT(void *pSamples, unsigned int nNumberOfSamples);

	// PARAMETERS:
	//		pSamples
	//			Pointer to buffer with PCM samples.
	//
	//		nNumberOfSamples
	//			Number of samples in buffer.
	//
	//	RETURN VALUES:
	//		1	- all OK
	//		0	- error
// =========================================================================================================================	
	// get amplitudes from FFT
	int GetAmplitudeReal(REAL *pLeftAmplitude, REAL *pRightAmplitude);

	//	PARAMETERS:
	//		pLeftAmplitude
	//			Pointer to buffer receiving amplitudes of left channel harmonics.
	//			User must allocate this memory. Use GetNumberOfHarmonics to determine number of
	//			harmonics and allocate enough memory.
	//			If this value is NULL, function will not fill this buffer.
	//
	//		pRightAmplitude
	//			Pointer to buffer receiving amplitudes of right channel harmonics.
	//			User must allocate this memory. Use GetNumberOfHarmonics to determine number of
	//			harmonics and allocate enough memory.
	//			If this value is NULL, function will not fill this buffer.
	//
	//	RETURN VALUES:
	//		1	- all OK
	//		0	- error

// =========================================================================================================================	
	// get amplitudes from FFT
	int GetAmplitudeInt(int *pLeftAmplitude, int *pRightAmplitude);

	//	PARAMETERS:
	//		pLeftAmplitude
	//			Pointer to buffer receiving amplitudes of left channel harmonics.
	//			User must allocate this memory. Use GetNumberOfHarmonics to determine number of
	//			harmonics and allocate enough memory.
	//			If this value is NULL, function will not fill this buffer.
	//
	//		pRightAmplitude
	//			Pointer to buffer receiving amplitudes of right channel harmonics.
	//			User must allocate this memory. Use GetNumberOfHarmonics to determine number of
	//			harmonics and allocate enough memory.
	//			If this value is NULL, function will not fill this buffer.
	//
	//	RETURN VALUES:
	//		1	- all OK
	//		0	- error
// ==========================================================================================================================
	// get FFT phase
	int GetPhaseReal(REAL *pLeftPhase, REAL *pRightPhase);

	//	PARAMETERS:
	//		pLeftPhase
	//			Pointer to buffer receiving phase of left channel harmonics.
	//			User must allocate this memory. Use GetNumberOfHarmonics to determine number of
	//			harmonics and allocate enough memory.
	//			If this value is NULL, function will not fill this buffer.
	//		
	//		pRightPhase
	//			Pointer to buffer receiving phase of left channel harmonics.
	//			User must allocate this memory. Use GetNumberOfHarmonics to determine number of
	//			harmonics and allocate enough memory.
	//			If this value is NULL, function will not fill this buffer.
	//
	//	RETURN VALUES:
	//		1	- all OK
	//		0	- error	

// ==========================================================================================================================
	// get FFT phase
	int GetPhaseInt(int *pLeftPhase, int *pRightPhase);

	//	PARAMETERS:
	//		pLeftPhase
	//			Pointer to buffer receiving phase of left channel harmonics.
	//			User must allocate this memory. Use GetNumberOfHarmonics to determine number of
	//			harmonics and allocate enough memory.
	//			If this value is NULL, function will not fill this buffer.
	//		
	//		pRightPhase
	//			Pointer to buffer receiving phase of left channel harmonics.
	//			User must allocate this memory. Use GetNumberOfHarmonics to determine number of
	//			harmonics and allocate enough memory.
	//			If this value is NULL, function will not fill this buffer.
	//
	//	RETURN VALUES:
	//		1	- all OK
	//		0	- error	

// ==========================================================================================================================


	void *GetSamplesBuffer();



// ==========================================================================================================================
private:
	unsigned int c_nFFTPoints;		// number of fft points
	REAL c_SqrtFFTPoints;			// sqrt of FFT points
	unsigned int c_nHarmonicNumber;	// number of harmonics
	int c_nFFTWindow;

	unsigned int c_nSampleRate;		// samplerate
	unsigned int c_nChannel;		// number of channels ( 1, 2 )
	unsigned int c_nBitPerSample;	// bits per sample ( 8, 16, 32 )

	REAL *c_window;					// window
	REAL *c_harmonics;				// frequency of each harmonic
	REAL *c_samplesl;				// samples
	REAL *c_samplesr;				// samples
	REAL *c_amplitude;				// amplitude of harmonics
	REAL *c_phase;					// phase of harmonic
	int *c_bit_reversal;			// working area for bit reversal
	REAL *c_cos_sin_table;			// cos/sin table


	int _alloc();
	int _free();

	static void _convertFFTtoAmplitudeReal(unsigned int FFTPoints, unsigned int SqrtFFTPoints, REAL *fft, REAL *amplitude);
	static void _convertFFTtoAmplitudeInt(unsigned int FFTPoints, unsigned int SqrtFFTPoints, REAL *fft, int *amplitude);

	static void _convertFFTtoPhaseReal(unsigned int FFTPoints, unsigned int SqrtFFTPoints, REAL *fft, REAL *phase);
	static void _convertFFTtoPhaseInt(unsigned int FFTPoints, unsigned int SqrtFFTPoints, REAL *fft, int *phase);

};


#endif
