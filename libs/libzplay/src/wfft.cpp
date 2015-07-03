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

#include "wfft.h"
#include "debug.h"

void cdft(int n, int isgn, REAL *a, int *ip, REAL *w)
{
    void makewt(int nw, int *ip, REAL *w);
    void cftfsub(int n, REAL *a, int *ip, int nw, REAL *w);
    void cftbsub(int n, REAL *a, int *ip, int nw, REAL *w);
    int nw;
    
    nw = ip[0];
    if (n > (nw << 2)) {
        nw = n >> 2;
        makewt(nw, ip, w);
    }
    if (isgn >= 0) {
        cftfsub(n, a, ip + 2, nw, w);
    } else {
        cftbsub(n, a, ip + 2, nw, w);
    }
}


void rdft(int n, int isgn, REAL *a, int *ip, REAL *w)
{
    void makewt(int nw, int *ip, REAL *w);
    void makect(int nc, int *ip, REAL *c);
    void cftfsub(int n, REAL *a, int *ip, int nw, REAL *w);
    void cftbsub(int n, REAL *a, int *ip, int nw, REAL *w);
    void rftfsub(int n, REAL *a, int nc, REAL *c);
    void rftbsub(int n, REAL *a, int nc, REAL *c);
    int nw, nc;
    REAL xi;
    
    nw = ip[0];
    if (n > (nw << 2)) {
        nw = n >> 2;
        makewt(nw, ip, w);
    }
    nc = ip[1];
    if (n > (nc << 2)) {
        nc = n >> 2;
        makect(nc, ip, w + nw);
    }
    if (isgn >= 0) {
        if (n > 4) {
            cftfsub(n, a, ip + 2, nw, w);
            rftfsub(n, a, nc, w + nw);
        } else if (n == 4) {
            cftfsub(n, a, ip + 2, nw, w);
        }
        xi = a[0] - a[1];
        a[0] += a[1];
        a[1] = xi;
    } else {
        a[1] = 0.5 * (a[0] - a[1]);
        a[0] -= a[1];
        if (n > 4) {
            rftbsub(n, a, nc, w + nw);
            cftbsub(n, a, ip + 2, nw, w);
        } else if (n == 4) {
            cftbsub(n, a, ip + 2, nw, w);
        }
    }
}


void ddct(int n, int isgn, REAL *a, int *ip, REAL *w)
{
    void makewt(int nw, int *ip, REAL *w);
    void makect(int nc, int *ip, REAL *c);
    void cftfsub(int n, REAL *a, int *ip, int nw, REAL *w);
    void cftbsub(int n, REAL *a, int *ip, int nw, REAL *w);
    void rftfsub(int n, REAL *a, int nc, REAL *c);
    void rftbsub(int n, REAL *a, int nc, REAL *c);
    void dctsub(int n, REAL *a, int nc, REAL *c);
    int j, nw, nc;
    REAL xr;
    
    nw = ip[0];
    if (n > (nw << 2)) {
        nw = n >> 2;
        makewt(nw, ip, w);
    }
    nc = ip[1];
    if (n > nc) {
        nc = n;
        makect(nc, ip, w + nw);
    }
    if (isgn < 0) {
        xr = a[n - 1];
        for (j = n - 2; j >= 2; j -= 2) {
            a[j + 1] = a[j] - a[j - 1];
            a[j] += a[j - 1];
        }
        a[1] = a[0] - xr;
        a[0] += xr;
        if (n > 4) {
            rftbsub(n, a, nc, w + nw);
            cftbsub(n, a, ip + 2, nw, w);
        } else if (n == 4) {
            cftbsub(n, a, ip + 2, nw, w);
        }
    }
    dctsub(n, a, nc, w + nw);
    if (isgn >= 0) {
        if (n > 4) {
            cftfsub(n, a, ip + 2, nw, w);
            rftfsub(n, a, nc, w + nw);
        } else if (n == 4) {
            cftfsub(n, a, ip + 2, nw, w);
        }
        xr = a[0] - a[1];
        a[0] += a[1];
        for (j = 2; j < n; j += 2) {
            a[j - 1] = a[j] - a[j + 1];
            a[j] += a[j + 1];
        }
        a[n - 1] = xr;
    }
}


void ddst(int n, int isgn, REAL *a, int *ip, REAL *w)
{
    void makewt(int nw, int *ip, REAL *w);
    void makect(int nc, int *ip, REAL *c);
    void cftfsub(int n, REAL *a, int *ip, int nw, REAL *w);
    void cftbsub(int n, REAL *a, int *ip, int nw, REAL *w);
    void rftfsub(int n, REAL *a, int nc, REAL *c);
    void rftbsub(int n, REAL *a, int nc, REAL *c);
    void dstsub(int n, REAL *a, int nc, REAL *c);
    int j, nw, nc;
    REAL xr;
    
    nw = ip[0];
    if (n > (nw << 2)) {
        nw = n >> 2;
        makewt(nw, ip, w);
    }
    nc = ip[1];
    if (n > nc) {
        nc = n;
        makect(nc, ip, w + nw);
    }
    if (isgn < 0) {
        xr = a[n - 1];
        for (j = n - 2; j >= 2; j -= 2) {
            a[j + 1] = -a[j] - a[j - 1];
            a[j] -= a[j - 1];
        }
        a[1] = a[0] + xr;
        a[0] -= xr;
        if (n > 4) {
            rftbsub(n, a, nc, w + nw);
            cftbsub(n, a, ip + 2, nw, w);
        } else if (n == 4) {
            cftbsub(n, a, ip + 2, nw, w);
        }
    }
    dstsub(n, a, nc, w + nw);
    if (isgn >= 0) {
        if (n > 4) {
            cftfsub(n, a, ip + 2, nw, w);
            rftfsub(n, a, nc, w + nw);
        } else if (n == 4) {
            cftfsub(n, a, ip + 2, nw, w);
        }
        xr = a[0] - a[1];
        a[0] += a[1];
        for (j = 2; j < n; j += 2) {
            a[j - 1] = -a[j] - a[j + 1];
            a[j] -= a[j + 1];
        }
        a[n - 1] = -xr;
    }
}


void dfct(int n, REAL *a, REAL *t, int *ip, REAL *w)
{
    void makewt(int nw, int *ip, REAL *w);
    void makect(int nc, int *ip, REAL *c);
    void cftfsub(int n, REAL *a, int *ip, int nw, REAL *w);
    void rftfsub(int n, REAL *a, int nc, REAL *c);
    void dctsub(int n, REAL *a, int nc, REAL *c);
    int j, k, l, m, mh, nw, nc;
    REAL xr, xi, yr, yi;
    
    nw = ip[0];
    if (n > (nw << 3)) {
        nw = n >> 3;
        makewt(nw, ip, w);
    }
    nc = ip[1];
    if (n > (nc << 1)) {
        nc = n >> 1;
        makect(nc, ip, w + nw);
    }
    m = n >> 1;
    yi = a[m];
    xi = a[0] + a[n];
    a[0] -= a[n];
    t[0] = xi - yi;
    t[m] = xi + yi;
    if (n > 2) {
        mh = m >> 1;
        for (j = 1; j < mh; j++) {
            k = m - j;
            xr = a[j] - a[n - j];
            xi = a[j] + a[n - j];
            yr = a[k] - a[n - k];
            yi = a[k] + a[n - k];
            a[j] = xr;
            a[k] = yr;
            t[j] = xi - yi;
            t[k] = xi + yi;
        }
        t[mh] = a[mh] + a[n - mh];
        a[mh] -= a[n - mh];
        dctsub(m, a, nc, w + nw);
        if (m > 4) {
            cftfsub(m, a, ip + 2, nw, w);
            rftfsub(m, a, nc, w + nw);
        } else if (m == 4) {
            cftfsub(m, a, ip + 2, nw, w);
        }
        a[n - 1] = a[0] - a[1];
        a[1] = a[0] + a[1];
        for (j = m - 2; j >= 2; j -= 2) {
            a[2 * j + 1] = a[j] + a[j + 1];
            a[2 * j - 1] = a[j] - a[j + 1];
        }
        l = 2;
        m = mh;
        while (m >= 2) {
            dctsub(m, t, nc, w + nw);
            if (m > 4) {
                cftfsub(m, t, ip + 2, nw, w);
                rftfsub(m, t, nc, w + nw);
            } else if (m == 4) {
                cftfsub(m, t, ip + 2, nw, w);
            }
            a[n - l] = t[0] - t[1];
            a[l] = t[0] + t[1];
            k = 0;
            for (j = 2; j < m; j += 2) {
                k += l << 2;
                a[k - l] = t[j] - t[j + 1];
                a[k + l] = t[j] + t[j + 1];
            }
            l <<= 1;
            mh = m >> 1;
            for (j = 0; j < mh; j++) {
                k = m - j;
                t[j] = t[m + k] - t[m + j];
                t[k] = t[m + k] + t[m + j];
            }
            t[mh] = t[m + mh];
            m = mh;
        }
        a[l] = t[0];
        a[n] = t[2] - t[1];
        a[0] = t[2] + t[1];
    } else {
        a[1] = a[0];
        a[2] = t[0];
        a[0] = t[1];
    }
}


void dfst(int n, REAL *a, REAL *t, int *ip, REAL *w)
{
    void makewt(int nw, int *ip, REAL *w);
    void makect(int nc, int *ip, REAL *c);
    void cftfsub(int n, REAL *a, int *ip, int nw, REAL *w);
    void rftfsub(int n, REAL *a, int nc, REAL *c);
    void dstsub(int n, REAL *a, int nc, REAL *c);
    int j, k, l, m, mh, nw, nc;
    REAL xr, xi, yr, yi;
    
    nw = ip[0];
    if (n > (nw << 3)) {
        nw = n >> 3;
        makewt(nw, ip, w);
    }
    nc = ip[1];
    if (n > (nc << 1)) {
        nc = n >> 1;
        makect(nc, ip, w + nw);
    }
    if (n > 2) {
        m = n >> 1;
        mh = m >> 1;
        for (j = 1; j < mh; j++) {
            k = m - j;
            xr = a[j] + a[n - j];
            xi = a[j] - a[n - j];
            yr = a[k] + a[n - k];
            yi = a[k] - a[n - k];
            a[j] = xr;
            a[k] = yr;
            t[j] = xi + yi;
            t[k] = xi - yi;
        }
        t[0] = a[mh] - a[n - mh];
        a[mh] += a[n - mh];
        a[0] = a[m];
        dstsub(m, a, nc, w + nw);
        if (m > 4) {
            cftfsub(m, a, ip + 2, nw, w);
            rftfsub(m, a, nc, w + nw);
        } else if (m == 4) {
            cftfsub(m, a, ip + 2, nw, w);
        }
        a[n - 1] = a[1] - a[0];
        a[1] = a[0] + a[1];
        for (j = m - 2; j >= 2; j -= 2) {
            a[2 * j + 1] = a[j] - a[j + 1];
            a[2 * j - 1] = -a[j] - a[j + 1];
        }
        l = 2;
        m = mh;
        while (m >= 2) {
            dstsub(m, t, nc, w + nw);
            if (m > 4) {
                cftfsub(m, t, ip + 2, nw, w);
                rftfsub(m, t, nc, w + nw);
            } else if (m == 4) {
                cftfsub(m, t, ip + 2, nw, w);
            }
            a[n - l] = t[1] - t[0];
            a[l] = t[0] + t[1];
            k = 0;
            for (j = 2; j < m; j += 2) {
                k += l << 2;
                a[k - l] = -t[j] - t[j + 1];
                a[k + l] = t[j] - t[j + 1];
            }
            l <<= 1;
            mh = m >> 1;
            for (j = 1; j < mh; j++) {
                k = m - j;
                t[j] = t[m + k] + t[m + j];
                t[k] = t[m + k] - t[m + j];
            }
            t[0] = t[m + mh];
            m = mh;
        }
        a[l] = t[0];
    }
    a[0] = 0;
}


/* -------- initializing routines -------- */


void makewt(int nw, int *ip, REAL *w)
{
    int j, nwh, nw0, nw1;
    REAL delta, wn4r, wk1r, wk1i, wk3r, wk3i;
    
    ip[0] = nw;
    ip[1] = 1;
    if (nw > 2) {
        nwh = nw >> 1;
        //delta = atan(1.0) / nwh;
        delta = PI/4 / nwh;
        wn4r = COS(delta * nwh);
        w[0] = 1;
        w[1] = wn4r;
        if (nwh >= 4) {
            w[2] = 0.5 / COS(delta * 2);
            w[3] = 0.5 / COS(delta * 6);
        }
        for (j = 4; j < nwh; j += 4) {
            w[j] = COS(delta * j);
            w[j + 1] = SIN(delta * j);
            w[j + 2] = COS(3 * delta * j);
            w[j + 3] = SIN(3 * delta * j);
        }
        nw0 = 0;
        while (nwh > 2) {
            nw1 = nw0 + nwh;
            nwh >>= 1;
            w[nw1] = 1;
            w[nw1 + 1] = wn4r;
            if (nwh >= 4) {
                wk1r = w[nw0 + 4];
                wk3r = w[nw0 + 6];
                w[nw1 + 2] = 0.5 / wk1r;
                w[nw1 + 3] = 0.5 / wk3r;
            }
            for (j = 4; j < nwh; j += 4) {
                wk1r = w[nw0 + 2 * j];
                wk1i = w[nw0 + 2 * j + 1];
                wk3r = w[nw0 + 2 * j + 2];
                wk3i = w[nw0 + 2 * j + 3];
                w[nw1 + j] = wk1r;
                w[nw1 + j + 1] = wk1i;
                w[nw1 + j + 2] = wk3r;
                w[nw1 + j + 3] = wk3i;
            }
            nw0 = nw1;
        }
    }
}


void makect(int nc, int *ip, REAL *c)
{
    int j, nch;
    REAL delta;
    
    ip[1] = nc;
    if (nc > 1) {
        nch = nc >> 1;
        //delta = atan(1.0) / nch;
        delta = PI/4 / nch;
        c[0] = COS(delta * nch);
        c[nch] = 0.5 * c[0];
        for (j = 1; j < nch; j++) {
            c[j] = 0.5 * COS(delta * j);
            c[nc - j] = 0.5 * SIN(delta * j);
        }
    }
}


/* -------- child routines -------- */


#ifndef CDFT_RECURSIVE_N  /* length of the recursive FFT mode */
#define CDFT_RECURSIVE_N 512  /* <= (L1 cache size) / 16 */
#endif

void cftfsub(int n, REAL *a, int *ip, int nw, REAL *w)
{
    void bitrv2(int n, int *ip, REAL *a);
    void bitrv216(REAL *a);
    void bitrv208(REAL *a);
    void cftf1st(int n, REAL *a, REAL *w);
    void cftrec1(int n, REAL *a, int nw, REAL *w);
    void cftrec2(int n, REAL *a, int nw, REAL *w);
    void cftexp1(int n, REAL *a, int nw, REAL *w);
    void cftfx41(int n, REAL *a, int nw, REAL *w);
    void cftf161(REAL *a, REAL *w);
    void cftf081(REAL *a, REAL *w);
    void cftf040(REAL *a);
    void cftx020(REAL *a);
    int m;
    
    if (n > 32) {
        m = n >> 2;
        cftf1st(n, a, &w[nw - m]);
        if (n > CDFT_RECURSIVE_N) {
            cftrec1(m, a, nw, w);
            cftrec2(m, &a[m], nw, w);
            cftrec1(m, &a[2 * m], nw, w);
            cftrec1(m, &a[3 * m], nw, w);
        } else if (m > 32) {
            cftexp1(n, a, nw, w);
        } else {
            cftfx41(n, a, nw, w);
        }
        bitrv2(n, ip, a);
    } else if (n > 8) {
        if (n == 32) {
            cftf161(a, &w[nw - 8]);
            bitrv216(a);
        } else {
            cftf081(a, w);
            bitrv208(a);
        }
    } else if (n == 8) {
        cftf040(a);
    } else if (n == 4) {
        cftx020(a);
    }
}


void cftbsub(int n, REAL *a, int *ip, int nw, REAL *w)
{
    void bitrv2conj(int n, int *ip, REAL *a);
    void bitrv216neg(REAL *a);
    void bitrv208neg(REAL *a);
    void cftb1st(int n, REAL *a, REAL *w);
    void cftrec1(int n, REAL *a, int nw, REAL *w);
    void cftrec2(int n, REAL *a, int nw, REAL *w);
    void cftexp1(int n, REAL *a, int nw, REAL *w);
    void cftfx41(int n, REAL *a, int nw, REAL *w);
    void cftf161(REAL *a, REAL *w);
    void cftf081(REAL *a, REAL *w);
    void cftb040(REAL *a);
    void cftx020(REAL *a);
    int m;
    
    if (n > 32) {
        m = n >> 2;
        cftb1st(n, a, &w[nw - m]);
        if (n > CDFT_RECURSIVE_N) {
            cftrec1(m, a, nw, w);
            cftrec2(m, &a[m], nw, w);
            cftrec1(m, &a[2 * m], nw, w);
            cftrec1(m, &a[3 * m], nw, w);
        } else if (m > 32) {
            cftexp1(n, a, nw, w);
        } else {
            cftfx41(n, a, nw, w);
        }
        bitrv2conj(n, ip, a);
    } else if (n > 8) {
        if (n == 32) {
            cftf161(a, &w[nw - 8]);
            bitrv216neg(a);
        } else {
            cftf081(a, w);
            bitrv208neg(a);
        }
    } else if (n == 8) {
        cftb040(a);
    } else if (n == 4) {
        cftx020(a);
    }
}


void bitrv2(int n, int *ip, REAL *a)
{
    int j, j1, k, k1, l, m, m2;
    REAL xr, xi, yr, yi;
    
    ip[0] = 0;
    l = n;
    m = 1;
    while ((m << 3) < l) {
        l >>= 1;
        for (j = 0; j < m; j++) {
            ip[m + j] = ip[j] + l;
        }
        m <<= 1;
    }
    m2 = 2 * m;
    if ((m << 3) == l) {
        for (k = 0; k < m; k++) {
            for (j = 0; j < k; j++) {
                j1 = 2 * j + ip[k];
                k1 = 2 * k + ip[j];
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m2;
                k1 += 2 * m2;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m2;
                k1 -= m2;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m2;
                k1 += 2 * m2;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
            }
            j1 = 2 * k + m2 + ip[k];
            k1 = j1 + m2;
            xr = a[j1];
            xi = a[j1 + 1];
            yr = a[k1];
            yi = a[k1 + 1];
            a[j1] = yr;
            a[j1 + 1] = yi;
            a[k1] = xr;
            a[k1 + 1] = xi;
        }
    } else {
        for (k = 1; k < m; k++) {
            for (j = 0; j < k; j++) {
                j1 = 2 * j + ip[k];
                k1 = 2 * k + ip[j];
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m2;
                k1 += m2;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
            }
        }
    }
}


void bitrv2conj(int n, int *ip, REAL *a)
{
    int j, j1, k, k1, l, m, m2;
    REAL xr, xi, yr, yi;
    
    ip[0] = 0;
    l = n;
    m = 1;
    while ((m << 3) < l) {
        l >>= 1;
        for (j = 0; j < m; j++) {
            ip[m + j] = ip[j] + l;
        }
        m <<= 1;
    }
    m2 = 2 * m;
    if ((m << 3) == l) {
        for (k = 0; k < m; k++) {
            for (j = 0; j < k; j++) {
                j1 = 2 * j + ip[k];
                k1 = 2 * k + ip[j];
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m2;
                k1 += 2 * m2;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m2;
                k1 -= m2;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m2;
                k1 += 2 * m2;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
            }
            k1 = 2 * k + ip[k];
            a[k1 + 1] = -a[k1 + 1];
            j1 = k1 + m2;
            k1 = j1 + m2;
            xr = a[j1];
            xi = -a[j1 + 1];
            yr = a[k1];
            yi = -a[k1 + 1];
            a[j1] = yr;
            a[j1 + 1] = yi;
            a[k1] = xr;
            a[k1 + 1] = xi;
            k1 += m2;
            a[k1 + 1] = -a[k1 + 1];
        }
    } else {
        a[1] = -a[1];
        a[m2 + 1] = -a[m2 + 1];
        for (k = 1; k < m; k++) {
            for (j = 0; j < k; j++) {
                j1 = 2 * j + ip[k];
                k1 = 2 * k + ip[j];
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m2;
                k1 += m2;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
            }
            k1 = 2 * k + ip[k];
            a[k1 + 1] = -a[k1 + 1];
            a[k1 + m2 + 1] = -a[k1 + m2 + 1];
        }
    }
}


void bitrv216(REAL *a)
{
    REAL x1r, x1i, x2r, x2i, x3r, x3i, x4r, x4i, 
        x5r, x5i, x7r, x7i, x8r, x8i, x10r, x10i, 
        x11r, x11i, x12r, x12i, x13r, x13i, x14r, x14i;
    
    x1r = a[2];
    x1i = a[3];
    x2r = a[4];
    x2i = a[5];
    x3r = a[6];
    x3i = a[7];
    x4r = a[8];
    x4i = a[9];
    x5r = a[10];
    x5i = a[11];
    x7r = a[14];
    x7i = a[15];
    x8r = a[16];
    x8i = a[17];
    x10r = a[20];
    x10i = a[21];
    x11r = a[22];
    x11i = a[23];
    x12r = a[24];
    x12i = a[25];
    x13r = a[26];
    x13i = a[27];
    x14r = a[28];
    x14i = a[29];
    a[2] = x8r;
    a[3] = x8i;
    a[4] = x4r;
    a[5] = x4i;
    a[6] = x12r;
    a[7] = x12i;
    a[8] = x2r;
    a[9] = x2i;
    a[10] = x10r;
    a[11] = x10i;
    a[14] = x14r;
    a[15] = x14i;
    a[16] = x1r;
    a[17] = x1i;
    a[20] = x5r;
    a[21] = x5i;
    a[22] = x13r;
    a[23] = x13i;
    a[24] = x3r;
    a[25] = x3i;
    a[26] = x11r;
    a[27] = x11i;
    a[28] = x7r;
    a[29] = x7i;
}


void bitrv216neg(REAL *a)
{
    REAL x1r, x1i, x2r, x2i, x3r, x3i, x4r, x4i, 
        x5r, x5i, x6r, x6i, x7r, x7i, x8r, x8i, 
        x9r, x9i, x10r, x10i, x11r, x11i, x12r, x12i, 
        x13r, x13i, x14r, x14i, x15r, x15i;
    
    x1r = a[2];
    x1i = a[3];
    x2r = a[4];
    x2i = a[5];
    x3r = a[6];
    x3i = a[7];
    x4r = a[8];
    x4i = a[9];
    x5r = a[10];
    x5i = a[11];
    x6r = a[12];
    x6i = a[13];
    x7r = a[14];
    x7i = a[15];
    x8r = a[16];
    x8i = a[17];
    x9r = a[18];
    x9i = a[19];
    x10r = a[20];
    x10i = a[21];
    x11r = a[22];
    x11i = a[23];
    x12r = a[24];
    x12i = a[25];
    x13r = a[26];
    x13i = a[27];
    x14r = a[28];
    x14i = a[29];
    x15r = a[30];
    x15i = a[31];
    a[2] = x15r;
    a[3] = x15i;
    a[4] = x7r;
    a[5] = x7i;
    a[6] = x11r;
    a[7] = x11i;
    a[8] = x3r;
    a[9] = x3i;
    a[10] = x13r;
    a[11] = x13i;
    a[12] = x5r;
    a[13] = x5i;
    a[14] = x9r;
    a[15] = x9i;
    a[16] = x1r;
    a[17] = x1i;
    a[18] = x14r;
    a[19] = x14i;
    a[20] = x6r;
    a[21] = x6i;
    a[22] = x10r;
    a[23] = x10i;
    a[24] = x2r;
    a[25] = x2i;
    a[26] = x12r;
    a[27] = x12i;
    a[28] = x4r;
    a[29] = x4i;
    a[30] = x8r;
    a[31] = x8i;
}


void bitrv208(REAL *a)
{
    REAL x1r, x1i, x3r, x3i, x4r, x4i, x6r, x6i;
    
    x1r = a[2];
    x1i = a[3];
    x3r = a[6];
    x3i = a[7];
    x4r = a[8];
    x4i = a[9];
    x6r = a[12];
    x6i = a[13];
    a[2] = x4r;
    a[3] = x4i;
    a[6] = x6r;
    a[7] = x6i;
    a[8] = x1r;
    a[9] = x1i;
    a[12] = x3r;
    a[13] = x3i;
}


void bitrv208neg(REAL *a)
{
    REAL x1r, x1i, x2r, x2i, x3r, x3i, x4r, x4i, 
        x5r, x5i, x6r, x6i, x7r, x7i;
    
    x1r = a[2];
    x1i = a[3];
    x2r = a[4];
    x2i = a[5];
    x3r = a[6];
    x3i = a[7];
    x4r = a[8];
    x4i = a[9];
    x5r = a[10];
    x5i = a[11];
    x6r = a[12];
    x6i = a[13];
    x7r = a[14];
    x7i = a[15];
    a[2] = x7r;
    a[3] = x7i;
    a[4] = x3r;
    a[5] = x3i;
    a[6] = x5r;
    a[7] = x5i;
    a[8] = x1r;
    a[9] = x1i;
    a[10] = x6r;
    a[11] = x6i;
    a[12] = x2r;
    a[13] = x2i;
    a[14] = x4r;
    a[15] = x4i;
}


void cftf1st(int n, REAL *a, REAL *w)
{
    int j, j0, j1, j2, j3, k, m, mh;
    REAL wn4r, csc1, csc3, wk1r, wk1i, wk3r, wk3i, 
        wd1r, wd1i, wd3r, wd3i;
    REAL x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i, 
        y0r, y0i, y1r, y1i, y2r, y2i, y3r, y3i;
    
    mh = n >> 3;
    m = 2 * mh;
    j1 = m;
    j2 = j1 + m;
    j3 = j2 + m;
    x0r = a[0] + a[j2];
    x0i = a[1] + a[j2 + 1];
    x1r = a[0] - a[j2];
    x1i = a[1] - a[j2 + 1];
    x2r = a[j1] + a[j3];
    x2i = a[j1 + 1] + a[j3 + 1];
    x3r = a[j1] - a[j3];
    x3i = a[j1 + 1] - a[j3 + 1];
    a[0] = x0r + x2r;
    a[1] = x0i + x2i;
    a[j1] = x0r - x2r;
    a[j1 + 1] = x0i - x2i;
    a[j2] = x1r - x3i;
    a[j2 + 1] = x1i + x3r;
    a[j3] = x1r + x3i;
    a[j3 + 1] = x1i - x3r;
    wn4r = w[1];
    csc1 = w[2];
    csc3 = w[3];
    wd1r = 1;
    wd1i = 0;
    wd3r = 1;
    wd3i = 0;
    k = 0;
    for (j = 2; j < mh - 2; j += 4) {
        k += 4;
        wk1r = csc1 * (wd1r + w[k]);
        wk1i = csc1 * (wd1i + w[k + 1]);
        wk3r = csc3 * (wd3r + w[k + 2]);
        wk3i = csc3 * (wd3i - w[k + 3]);
        wd1r = w[k];
        wd1i = w[k + 1];
        wd3r = w[k + 2];
        wd3i = -w[k + 3];
        j1 = j + m;
        j2 = j1 + m;
        j3 = j2 + m;
        x0r = a[j] + a[j2];
        x0i = a[j + 1] + a[j2 + 1];
        x1r = a[j] - a[j2];
        x1i = a[j + 1] - a[j2 + 1];
        y0r = a[j + 2] + a[j2 + 2];
        y0i = a[j + 3] + a[j2 + 3];
        y1r = a[j + 2] - a[j2 + 2];
        y1i = a[j + 3] - a[j2 + 3];
        x2r = a[j1] + a[j3];
        x2i = a[j1 + 1] + a[j3 + 1];
        x3r = a[j1] - a[j3];
        x3i = a[j1 + 1] - a[j3 + 1];
        y2r = a[j1 + 2] + a[j3 + 2];
        y2i = a[j1 + 3] + a[j3 + 3];
        y3r = a[j1 + 2] - a[j3 + 2];
        y3i = a[j1 + 3] - a[j3 + 3];
        a[j] = x0r + x2r;
        a[j + 1] = x0i + x2i;
        a[j + 2] = y0r + y2r;
        a[j + 3] = y0i + y2i;
        a[j1] = x0r - x2r;
        a[j1 + 1] = x0i - x2i;
        a[j1 + 2] = y0r - y2r;
        a[j1 + 3] = y0i - y2i;
        x0r = x1r - x3i;
        x0i = x1i + x3r;
        a[j2] = wk1r * x0r - wk1i * x0i;
        a[j2 + 1] = wk1r * x0i + wk1i * x0r;
        x0r = y1r - y3i;
        x0i = y1i + y3r;
        a[j2 + 2] = wd1r * x0r - wd1i * x0i;
        a[j2 + 3] = wd1r * x0i + wd1i * x0r;
        x0r = x1r + x3i;
        x0i = x1i - x3r;
        a[j3] = wk3r * x0r + wk3i * x0i;
        a[j3 + 1] = wk3r * x0i - wk3i * x0r;
        x0r = y1r + y3i;
        x0i = y1i - y3r;
        a[j3 + 2] = wd3r * x0r + wd3i * x0i;
        a[j3 + 3] = wd3r * x0i - wd3i * x0r;
        j0 = m - j;
        j1 = j0 + m;
        j2 = j1 + m;
        j3 = j2 + m;
        x0r = a[j0] + a[j2];
        x0i = a[j0 + 1] + a[j2 + 1];
        x1r = a[j0] - a[j2];
        x1i = a[j0 + 1] - a[j2 + 1];
        y0r = a[j0 - 2] + a[j2 - 2];
        y0i = a[j0 - 1] + a[j2 - 1];
        y1r = a[j0 - 2] - a[j2 - 2];
        y1i = a[j0 - 1] - a[j2 - 1];
        x2r = a[j1] + a[j3];
        x2i = a[j1 + 1] + a[j3 + 1];
        x3r = a[j1] - a[j3];
        x3i = a[j1 + 1] - a[j3 + 1];
        y2r = a[j1 - 2] + a[j3 - 2];
        y2i = a[j1 - 1] + a[j3 - 1];
        y3r = a[j1 - 2] - a[j3 - 2];
        y3i = a[j1 - 1] - a[j3 - 1];
        a[j0] = x0r + x2r;
        a[j0 + 1] = x0i + x2i;
        a[j0 - 2] = y0r + y2r;
        a[j0 - 1] = y0i + y2i;
        a[j1] = x0r - x2r;
        a[j1 + 1] = x0i - x2i;
        a[j1 - 2] = y0r - y2r;
        a[j1 - 1] = y0i - y2i;
        x0r = x1r - x3i;
        x0i = x1i + x3r;
        a[j2] = wk1i * x0r - wk1r * x0i;
        a[j2 + 1] = wk1i * x0i + wk1r * x0r;
        x0r = y1r - y3i;
        x0i = y1i + y3r;
        a[j2 - 2] = wd1i * x0r - wd1r * x0i;
        a[j2 - 1] = wd1i * x0i + wd1r * x0r;
        x0r = x1r + x3i;
        x0i = x1i - x3r;
        a[j3] = wk3i * x0r + wk3r * x0i;
        a[j3 + 1] = wk3i * x0i - wk3r * x0r;
        x0r = y1r + y3i;
        x0i = y1i - y3r;
        a[j3 - 2] = wd3i * x0r + wd3r * x0i;
        a[j3 - 1] = wd3i * x0i - wd3r * x0r;
    }
    wk1r = csc1 * (wd1r + wn4r);
    wk1i = csc1 * (wd1i + wn4r);
    wk3r = csc3 * (wd3r - wn4r);
    wk3i = csc3 * (wd3i - wn4r);
    j0 = mh;
    j1 = j0 + m;
    j2 = j1 + m;
    j3 = j2 + m;
    x0r = a[j0 - 2] + a[j2 - 2];
    x0i = a[j0 - 1] + a[j2 - 1];
    x1r = a[j0 - 2] - a[j2 - 2];
    x1i = a[j0 - 1] - a[j2 - 1];
    x2r = a[j1 - 2] + a[j3 - 2];
    x2i = a[j1 - 1] + a[j3 - 1];
    x3r = a[j1 - 2] - a[j3 - 2];
    x3i = a[j1 - 1] - a[j3 - 1];
    a[j0 - 2] = x0r + x2r;
    a[j0 - 1] = x0i + x2i;
    a[j1 - 2] = x0r - x2r;
    a[j1 - 1] = x0i - x2i;
    x0r = x1r - x3i;
    x0i = x1i + x3r;
    a[j2 - 2] = wk1r * x0r - wk1i * x0i;
    a[j2 - 1] = wk1r * x0i + wk1i * x0r;
    x0r = x1r + x3i;
    x0i = x1i - x3r;
    a[j3 - 2] = wk3r * x0r + wk3i * x0i;
    a[j3 - 1] = wk3r * x0i - wk3i * x0r;
    x0r = a[j0] + a[j2];
    x0i = a[j0 + 1] + a[j2 + 1];
    x1r = a[j0] - a[j2];
    x1i = a[j0 + 1] - a[j2 + 1];
    x2r = a[j1] + a[j3];
    x2i = a[j1 + 1] + a[j3 + 1];
    x3r = a[j1] - a[j3];
    x3i = a[j1 + 1] - a[j3 + 1];
    a[j0] = x0r + x2r;
    a[j0 + 1] = x0i + x2i;
    a[j1] = x0r - x2r;
    a[j1 + 1] = x0i - x2i;
    x0r = x1r - x3i;
    x0i = x1i + x3r;
    a[j2] = wn4r * (x0r - x0i);
    a[j2 + 1] = wn4r * (x0i + x0r);
    x0r = x1r + x3i;
    x0i = x1i - x3r;
    a[j3] = -wn4r * (x0r + x0i);
    a[j3 + 1] = -wn4r * (x0i - x0r);
    x0r = a[j0 + 2] + a[j2 + 2];
    x0i = a[j0 + 3] + a[j2 + 3];
    x1r = a[j0 + 2] - a[j2 + 2];
    x1i = a[j0 + 3] - a[j2 + 3];
    x2r = a[j1 + 2] + a[j3 + 2];
    x2i = a[j1 + 3] + a[j3 + 3];
    x3r = a[j1 + 2] - a[j3 + 2];
    x3i = a[j1 + 3] - a[j3 + 3];
    a[j0 + 2] = x0r + x2r;
    a[j0 + 3] = x0i + x2i;
    a[j1 + 2] = x0r - x2r;
    a[j1 + 3] = x0i - x2i;
    x0r = x1r - x3i;
    x0i = x1i + x3r;
    a[j2 + 2] = wk1i * x0r - wk1r * x0i;
    a[j2 + 3] = wk1i * x0i + wk1r * x0r;
    x0r = x1r + x3i;
    x0i = x1i - x3r;
    a[j3 + 2] = wk3i * x0r + wk3r * x0i;
    a[j3 + 3] = wk3i * x0i - wk3r * x0r;
}


void cftb1st(int n, REAL *a, REAL *w)
{
    int j, j0, j1, j2, j3, k, m, mh;
    REAL wn4r, csc1, csc3, wk1r, wk1i, wk3r, wk3i, 
        wd1r, wd1i, wd3r, wd3i;
    REAL x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i, 
        y0r, y0i, y1r, y1i, y2r, y2i, y3r, y3i;
    
    mh = n >> 3;
    m = 2 * mh;
    j1 = m;
    j2 = j1 + m;
    j3 = j2 + m;
    x0r = a[0] + a[j2];
    x0i = -a[1] - a[j2 + 1];
    x1r = a[0] - a[j2];
    x1i = -a[1] + a[j2 + 1];
    x2r = a[j1] + a[j3];
    x2i = a[j1 + 1] + a[j3 + 1];
    x3r = a[j1] - a[j3];
    x3i = a[j1 + 1] - a[j3 + 1];
    a[0] = x0r + x2r;
    a[1] = x0i - x2i;
    a[j1] = x0r - x2r;
    a[j1 + 1] = x0i + x2i;
    a[j2] = x1r + x3i;
    a[j2 + 1] = x1i + x3r;
    a[j3] = x1r - x3i;
    a[j3 + 1] = x1i - x3r;
    wn4r = w[1];
    csc1 = w[2];
    csc3 = w[3];
    wd1r = 1;
    wd1i = 0;
    wd3r = 1;
    wd3i = 0;
    k = 0;
    for (j = 2; j < mh - 2; j += 4) {
        k += 4;
        wk1r = csc1 * (wd1r + w[k]);
        wk1i = csc1 * (wd1i + w[k + 1]);
        wk3r = csc3 * (wd3r + w[k + 2]);
        wk3i = csc3 * (wd3i - w[k + 3]);
        wd1r = w[k];
        wd1i = w[k + 1];
        wd3r = w[k + 2];
        wd3i = -w[k + 3];
        j1 = j + m;
        j2 = j1 + m;
        j3 = j2 + m;
        x0r = a[j] + a[j2];
        x0i = -a[j + 1] - a[j2 + 1];
        x1r = a[j] - a[j2];
        x1i = -a[j + 1] + a[j2 + 1];
        y0r = a[j + 2] + a[j2 + 2];
        y0i = -a[j + 3] - a[j2 + 3];
        y1r = a[j + 2] - a[j2 + 2];
        y1i = -a[j + 3] + a[j2 + 3];
        x2r = a[j1] + a[j3];
        x2i = a[j1 + 1] + a[j3 + 1];
        x3r = a[j1] - a[j3];
        x3i = a[j1 + 1] - a[j3 + 1];
        y2r = a[j1 + 2] + a[j3 + 2];
        y2i = a[j1 + 3] + a[j3 + 3];
        y3r = a[j1 + 2] - a[j3 + 2];
        y3i = a[j1 + 3] - a[j3 + 3];
        a[j] = x0r + x2r;
        a[j + 1] = x0i - x2i;
        a[j + 2] = y0r + y2r;
        a[j + 3] = y0i - y2i;
        a[j1] = x0r - x2r;
        a[j1 + 1] = x0i + x2i;
        a[j1 + 2] = y0r - y2r;
        a[j1 + 3] = y0i + y2i;
        x0r = x1r + x3i;
        x0i = x1i + x3r;
        a[j2] = wk1r * x0r - wk1i * x0i;
        a[j2 + 1] = wk1r * x0i + wk1i * x0r;
        x0r = y1r + y3i;
        x0i = y1i + y3r;
        a[j2 + 2] = wd1r * x0r - wd1i * x0i;
        a[j2 + 3] = wd1r * x0i + wd1i * x0r;
        x0r = x1r - x3i;
        x0i = x1i - x3r;
        a[j3] = wk3r * x0r + wk3i * x0i;
        a[j3 + 1] = wk3r * x0i - wk3i * x0r;
        x0r = y1r - y3i;
        x0i = y1i - y3r;
        a[j3 + 2] = wd3r * x0r + wd3i * x0i;
        a[j3 + 3] = wd3r * x0i - wd3i * x0r;
        j0 = m - j;
        j1 = j0 + m;
        j2 = j1 + m;
        j3 = j2 + m;
        x0r = a[j0] + a[j2];
        x0i = -a[j0 + 1] - a[j2 + 1];
        x1r = a[j0] - a[j2];
        x1i = -a[j0 + 1] + a[j2 + 1];
        y0r = a[j0 - 2] + a[j2 - 2];
        y0i = -a[j0 - 1] - a[j2 - 1];
        y1r = a[j0 - 2] - a[j2 - 2];
        y1i = -a[j0 - 1] + a[j2 - 1];
        x2r = a[j1] + a[j3];
        x2i = a[j1 + 1] + a[j3 + 1];
        x3r = a[j1] - a[j3];
        x3i = a[j1 + 1] - a[j3 + 1];
        y2r = a[j1 - 2] + a[j3 - 2];
        y2i = a[j1 - 1] + a[j3 - 1];
        y3r = a[j1 - 2] - a[j3 - 2];
        y3i = a[j1 - 1] - a[j3 - 1];
        a[j0] = x0r + x2r;
        a[j0 + 1] = x0i - x2i;
        a[j0 - 2] = y0r + y2r;
        a[j0 - 1] = y0i - y2i;
        a[j1] = x0r - x2r;
        a[j1 + 1] = x0i + x2i;
        a[j1 - 2] = y0r - y2r;
        a[j1 - 1] = y0i + y2i;
        x0r = x1r + x3i;
        x0i = x1i + x3r;
        a[j2] = wk1i * x0r - wk1r * x0i;
        a[j2 + 1] = wk1i * x0i + wk1r * x0r;
        x0r = y1r + y3i;
        x0i = y1i + y3r;
        a[j2 - 2] = wd1i * x0r - wd1r * x0i;
        a[j2 - 1] = wd1i * x0i + wd1r * x0r;
        x0r = x1r - x3i;
        x0i = x1i - x3r;
        a[j3] = wk3i * x0r + wk3r * x0i;
        a[j3 + 1] = wk3i * x0i - wk3r * x0r;
        x0r = y1r - y3i;
        x0i = y1i - y3r;
        a[j3 - 2] = wd3i * x0r + wd3r * x0i;
        a[j3 - 1] = wd3i * x0i - wd3r * x0r;
    }
    wk1r = csc1 * (wd1r + wn4r);
    wk1i = csc1 * (wd1i + wn4r);
    wk3r = csc3 * (wd3r - wn4r);
    wk3i = csc3 * (wd3i - wn4r);
    j0 = mh;
    j1 = j0 + m;
    j2 = j1 + m;
    j3 = j2 + m;
    x0r = a[j0 - 2] + a[j2 - 2];
    x0i = -a[j0 - 1] - a[j2 - 1];
    x1r = a[j0 - 2] - a[j2 - 2];
    x1i = -a[j0 - 1] + a[j2 - 1];
    x2r = a[j1 - 2] + a[j3 - 2];
    x2i = a[j1 - 1] + a[j3 - 1];
    x3r = a[j1 - 2] - a[j3 - 2];
    x3i = a[j1 - 1] - a[j3 - 1];
    a[j0 - 2] = x0r + x2r;
    a[j0 - 1] = x0i - x2i;
    a[j1 - 2] = x0r - x2r;
    a[j1 - 1] = x0i + x2i;
    x0r = x1r + x3i;
    x0i = x1i + x3r;
    a[j2 - 2] = wk1r * x0r - wk1i * x0i;
    a[j2 - 1] = wk1r * x0i + wk1i * x0r;
    x0r = x1r - x3i;
    x0i = x1i - x3r;
    a[j3 - 2] = wk3r * x0r + wk3i * x0i;
    a[j3 - 1] = wk3r * x0i - wk3i * x0r;
    x0r = a[j0] + a[j2];
    x0i = -a[j0 + 1] - a[j2 + 1];
    x1r = a[j0] - a[j2];
    x1i = -a[j0 + 1] + a[j2 + 1];
    x2r = a[j1] + a[j3];
    x2i = a[j1 + 1] + a[j3 + 1];
    x3r = a[j1] - a[j3];
    x3i = a[j1 + 1] - a[j3 + 1];
    a[j0] = x0r + x2r;
    a[j0 + 1] = x0i - x2i;
    a[j1] = x0r - x2r;
    a[j1 + 1] = x0i + x2i;
    x0r = x1r + x3i;
    x0i = x1i + x3r;
    a[j2] = wn4r * (x0r - x0i);
    a[j2 + 1] = wn4r * (x0i + x0r);
    x0r = x1r - x3i;
    x0i = x1i - x3r;
    a[j3] = -wn4r * (x0r + x0i);
    a[j3 + 1] = -wn4r * (x0i - x0r);
    x0r = a[j0 + 2] + a[j2 + 2];
    x0i = -a[j0 + 3] - a[j2 + 3];
    x1r = a[j0 + 2] - a[j2 + 2];
    x1i = -a[j0 + 3] + a[j2 + 3];
    x2r = a[j1 + 2] + a[j3 + 2];
    x2i = a[j1 + 3] + a[j3 + 3];
    x3r = a[j1 + 2] - a[j3 + 2];
    x3i = a[j1 + 3] - a[j3 + 3];
    a[j0 + 2] = x0r + x2r;
    a[j0 + 3] = x0i - x2i;
    a[j1 + 2] = x0r - x2r;
    a[j1 + 3] = x0i + x2i;
    x0r = x1r + x3i;
    x0i = x1i + x3r;
    a[j2 + 2] = wk1i * x0r - wk1r * x0i;
    a[j2 + 3] = wk1i * x0i + wk1r * x0r;
    x0r = x1r - x3i;
    x0i = x1i - x3r;
    a[j3 + 2] = wk3i * x0r + wk3r * x0i;
    a[j3 + 3] = wk3i * x0i - wk3r * x0r;
}


void cftrec1(int n, REAL *a, int nw, REAL *w)
{
    void cftrec1(int n, REAL *a, int nw, REAL *w);
    void cftrec2(int n, REAL *a, int nw, REAL *w);
    void cftmdl1(int n, REAL *a, REAL *w);
    void cftexp1(int n, REAL *a, int nw, REAL *w);
    int m;
    
    m = n >> 2;
    cftmdl1(n, a, &w[nw - 2 * m]);
    if (n > CDFT_RECURSIVE_N) {
        cftrec1(m, a, nw, w);
        cftrec2(m, &a[m], nw, w);
        cftrec1(m, &a[2 * m], nw, w);
        cftrec1(m, &a[3 * m], nw, w);
    } else {
        cftexp1(n, a, nw, w);
    }
}


void cftrec2(int n, REAL *a, int nw, REAL *w)
{
    void cftrec1(int n, REAL *a, int nw, REAL *w);
    void cftrec2(int n, REAL *a, int nw, REAL *w);
    void cftmdl2(int n, REAL *a, REAL *w);
    void cftexp2(int n, REAL *a, int nw, REAL *w);
    int m;
    
    m = n >> 2;
    cftmdl2(n, a, &w[nw - n]);
    if (n > CDFT_RECURSIVE_N) {
        cftrec1(m, a, nw, w);
        cftrec2(m, &a[m], nw, w);
        cftrec1(m, &a[2 * m], nw, w);
        cftrec2(m, &a[3 * m], nw, w);
    } else {
        cftexp2(n, a, nw, w);
    }
}


void cftexp1(int n, REAL *a, int nw, REAL *w)
{
    void cftmdl1(int n, REAL *a, REAL *w);
    void cftmdl2(int n, REAL *a, REAL *w);
    void cftfx41(int n, REAL *a, int nw, REAL *w);
    void cftfx42(int n, REAL *a, int nw, REAL *w);
    int j, k, l;
    
    l = n >> 2;
    while (l > 128) {
        for (k = l; k < n; k <<= 2) {
            for (j = k - l; j < n; j += 4 * k) {
                cftmdl1(l, &a[j], &w[nw - (l >> 1)]);
                cftmdl2(l, &a[k + j], &w[nw - l]);
                cftmdl1(l, &a[2 * k + j], &w[nw - (l >> 1)]);
            }
        }
        cftmdl1(l, &a[n - l], &w[nw - (l >> 1)]);
        l >>= 2;
    }
    for (k = l; k < n; k <<= 2) {
        for (j = k - l; j < n; j += 4 * k) {
            cftmdl1(l, &a[j], &w[nw - (l >> 1)]);
            cftfx41(l, &a[j], nw, w);
            cftmdl2(l, &a[k + j], &w[nw - l]);
            cftfx42(l, &a[k + j], nw, w);
            cftmdl1(l, &a[2 * k + j], &w[nw - (l >> 1)]);
            cftfx41(l, &a[2 * k + j], nw, w);
        }
    }
    cftmdl1(l, &a[n - l], &w[nw - (l >> 1)]);
    cftfx41(l, &a[n - l], nw, w);
}


void cftexp2(int n, REAL *a, int nw, REAL *w)
{
    void cftmdl1(int n, REAL *a, REAL *w);
    void cftmdl2(int n, REAL *a, REAL *w);
    void cftfx41(int n, REAL *a, int nw, REAL *w);
    void cftfx42(int n, REAL *a, int nw, REAL *w);
    int j, k, l, m;
    
    m = n >> 1;
    l = n >> 2;
    while (l > 128) {
        for (k = l; k < m; k <<= 2) {
            for (j = k - l; j < m; j += 2 * k) {
                cftmdl1(l, &a[j], &w[nw - (l >> 1)]);
                cftmdl1(l, &a[m + j], &w[nw - (l >> 1)]);
            }
            for (j = 2 * k - l; j < m; j += 4 * k) {
                cftmdl2(l, &a[j], &w[nw - l]);
                cftmdl2(l, &a[m + j], &w[nw - l]);
            }
        }
        l >>= 2;
    }
    for (k = l; k < m; k <<= 2) {
        for (j = k - l; j < m; j += 2 * k) {
            cftmdl1(l, &a[j], &w[nw - (l >> 1)]);
            cftfx41(l, &a[j], nw, w);
            cftmdl1(l, &a[m + j], &w[nw - (l >> 1)]);
            cftfx41(l, &a[m + j], nw, w);
        }
        for (j = 2 * k - l; j < m; j += 4 * k) {
            cftmdl2(l, &a[j], &w[nw - l]);
            cftfx42(l, &a[j], nw, w);
            cftmdl2(l, &a[m + j], &w[nw - l]);
            cftfx42(l, &a[m + j], nw, w);
        }
    }
}


void cftmdl1(int n, REAL *a, REAL *w)
{
    int j, j0, j1, j2, j3, k, m, mh;
    REAL wn4r, wk1r, wk1i, wk3r, wk3i;
    REAL x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i;
    
    mh = n >> 3;
    m = 2 * mh;
    j1 = m;
    j2 = j1 + m;
    j3 = j2 + m;
    x0r = a[0] + a[j2];
    x0i = a[1] + a[j2 + 1];
    x1r = a[0] - a[j2];
    x1i = a[1] - a[j2 + 1];
    x2r = a[j1] + a[j3];
    x2i = a[j1 + 1] + a[j3 + 1];
    x3r = a[j1] - a[j3];
    x3i = a[j1 + 1] - a[j3 + 1];
    a[0] = x0r + x2r;
    a[1] = x0i + x2i;
    a[j1] = x0r - x2r;
    a[j1 + 1] = x0i - x2i;
    a[j2] = x1r - x3i;
    a[j2 + 1] = x1i + x3r;
    a[j3] = x1r + x3i;
    a[j3 + 1] = x1i - x3r;
    wn4r = w[1];
    k = 0;
    for (j = 2; j < mh; j += 2) {
        k += 4;
        wk1r = w[k];
        wk1i = w[k + 1];
        wk3r = w[k + 2];
        wk3i = -w[k + 3];
        j1 = j + m;
        j2 = j1 + m;
        j3 = j2 + m;
        x0r = a[j] + a[j2];
        x0i = a[j + 1] + a[j2 + 1];
        x1r = a[j] - a[j2];
        x1i = a[j + 1] - a[j2 + 1];
        x2r = a[j1] + a[j3];
        x2i = a[j1 + 1] + a[j3 + 1];
        x3r = a[j1] - a[j3];
        x3i = a[j1 + 1] - a[j3 + 1];
        a[j] = x0r + x2r;
        a[j + 1] = x0i + x2i;
        a[j1] = x0r - x2r;
        a[j1 + 1] = x0i - x2i;
        x0r = x1r - x3i;
        x0i = x1i + x3r;
        a[j2] = wk1r * x0r - wk1i * x0i;
        a[j2 + 1] = wk1r * x0i + wk1i * x0r;
        x0r = x1r + x3i;
        x0i = x1i - x3r;
        a[j3] = wk3r * x0r + wk3i * x0i;
        a[j3 + 1] = wk3r * x0i - wk3i * x0r;
        j0 = m - j;
        j1 = j0 + m;
        j2 = j1 + m;
        j3 = j2 + m;
        x0r = a[j0] + a[j2];
        x0i = a[j0 + 1] + a[j2 + 1];
        x1r = a[j0] - a[j2];
        x1i = a[j0 + 1] - a[j2 + 1];
        x2r = a[j1] + a[j3];
        x2i = a[j1 + 1] + a[j3 + 1];
        x3r = a[j1] - a[j3];
        x3i = a[j1 + 1] - a[j3 + 1];
        a[j0] = x0r + x2r;
        a[j0 + 1] = x0i + x2i;
        a[j1] = x0r - x2r;
        a[j1 + 1] = x0i - x2i;
        x0r = x1r - x3i;
        x0i = x1i + x3r;
        a[j2] = wk1i * x0r - wk1r * x0i;
        a[j2 + 1] = wk1i * x0i + wk1r * x0r;
        x0r = x1r + x3i;
        x0i = x1i - x3r;
        a[j3] = wk3i * x0r + wk3r * x0i;
        a[j3 + 1] = wk3i * x0i - wk3r * x0r;
    }
    j0 = mh;
    j1 = j0 + m;
    j2 = j1 + m;
    j3 = j2 + m;
    x0r = a[j0] + a[j2];
    x0i = a[j0 + 1] + a[j2 + 1];
    x1r = a[j0] - a[j2];
    x1i = a[j0 + 1] - a[j2 + 1];
    x2r = a[j1] + a[j3];
    x2i = a[j1 + 1] + a[j3 + 1];
    x3r = a[j1] - a[j3];
    x3i = a[j1 + 1] - a[j3 + 1];
    a[j0] = x0r + x2r;
    a[j0 + 1] = x0i + x2i;
    a[j1] = x0r - x2r;
    a[j1 + 1] = x0i - x2i;
    x0r = x1r - x3i;
    x0i = x1i + x3r;
    a[j2] = wn4r * (x0r - x0i);
    a[j2 + 1] = wn4r * (x0i + x0r);
    x0r = x1r + x3i;
    x0i = x1i - x3r;
    a[j3] = -wn4r * (x0r + x0i);
    a[j3 + 1] = -wn4r * (x0i - x0r);
}


void cftmdl2(int n, REAL *a, REAL *w)
{
    int j, j0, j1, j2, j3, k, kr, m, mh;
    REAL wn4r, wk1r, wk1i, wk3r, wk3i, wd1r, wd1i, wd3r, wd3i;
    REAL x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i, y0r, y0i, y2r, y2i;
    
    mh = n >> 3;
    m = 2 * mh;
    wn4r = w[1];
    j1 = m;
    j2 = j1 + m;
    j3 = j2 + m;
    x0r = a[0] - a[j2 + 1];
    x0i = a[1] + a[j2];
    x1r = a[0] + a[j2 + 1];
    x1i = a[1] - a[j2];
    x2r = a[j1] - a[j3 + 1];
    x2i = a[j1 + 1] + a[j3];
    x3r = a[j1] + a[j3 + 1];
    x3i = a[j1 + 1] - a[j3];
    y0r = wn4r * (x2r - x2i);
    y0i = wn4r * (x2i + x2r);
    a[0] = x0r + y0r;
    a[1] = x0i + y0i;
    a[j1] = x0r - y0r;
    a[j1 + 1] = x0i - y0i;
    y0r = wn4r * (x3r - x3i);
    y0i = wn4r * (x3i + x3r);
    a[j2] = x1r - y0i;
    a[j2 + 1] = x1i + y0r;
    a[j3] = x1r + y0i;
    a[j3 + 1] = x1i - y0r;
    k = 0;
    kr = 2 * m;
    for (j = 2; j < mh; j += 2) {
        k += 4;
        wk1r = w[k];
        wk1i = w[k + 1];
        wk3r = w[k + 2];
        wk3i = -w[k + 3];
        kr -= 4;
        wd1i = w[kr];
        wd1r = w[kr + 1];
        wd3i = w[kr + 2];
        wd3r = -w[kr + 3];
        j1 = j + m;
        j2 = j1 + m;
        j3 = j2 + m;
        x0r = a[j] - a[j2 + 1];
        x0i = a[j + 1] + a[j2];
        x1r = a[j] + a[j2 + 1];
        x1i = a[j + 1] - a[j2];
        x2r = a[j1] - a[j3 + 1];
        x2i = a[j1 + 1] + a[j3];
        x3r = a[j1] + a[j3 + 1];
        x3i = a[j1 + 1] - a[j3];
        y0r = wk1r * x0r - wk1i * x0i;
        y0i = wk1r * x0i + wk1i * x0r;
        y2r = wd1r * x2r - wd1i * x2i;
        y2i = wd1r * x2i + wd1i * x2r;
        a[j] = y0r + y2r;
        a[j + 1] = y0i + y2i;
        a[j1] = y0r - y2r;
        a[j1 + 1] = y0i - y2i;
        y0r = wk3r * x1r + wk3i * x1i;
        y0i = wk3r * x1i - wk3i * x1r;
        y2r = wd3r * x3r + wd3i * x3i;
        y2i = wd3r * x3i - wd3i * x3r;
        a[j2] = y0r + y2r;
        a[j2 + 1] = y0i + y2i;
        a[j3] = y0r - y2r;
        a[j3 + 1] = y0i - y2i;
        j0 = m - j;
        j1 = j0 + m;
        j2 = j1 + m;
        j3 = j2 + m;
        x0r = a[j0] - a[j2 + 1];
        x0i = a[j0 + 1] + a[j2];
        x1r = a[j0] + a[j2 + 1];
        x1i = a[j0 + 1] - a[j2];
        x2r = a[j1] - a[j3 + 1];
        x2i = a[j1 + 1] + a[j3];
        x3r = a[j1] + a[j3 + 1];
        x3i = a[j1 + 1] - a[j3];
        y0r = wd1i * x0r - wd1r * x0i;
        y0i = wd1i * x0i + wd1r * x0r;
        y2r = wk1i * x2r - wk1r * x2i;
        y2i = wk1i * x2i + wk1r * x2r;
        a[j0] = y0r + y2r;
        a[j0 + 1] = y0i + y2i;
        a[j1] = y0r - y2r;
        a[j1 + 1] = y0i - y2i;
        y0r = wd3i * x1r + wd3r * x1i;
        y0i = wd3i * x1i - wd3r * x1r;
        y2r = wk3i * x3r + wk3r * x3i;
        y2i = wk3i * x3i - wk3r * x3r;
        a[j2] = y0r + y2r;
        a[j2 + 1] = y0i + y2i;
        a[j3] = y0r - y2r;
        a[j3 + 1] = y0i - y2i;
    }
    wk1r = w[m];
    wk1i = w[m + 1];
    j0 = mh;
    j1 = j0 + m;
    j2 = j1 + m;
    j3 = j2 + m;
    x0r = a[j0] - a[j2 + 1];
    x0i = a[j0 + 1] + a[j2];
    x1r = a[j0] + a[j2 + 1];
    x1i = a[j0 + 1] - a[j2];
    x2r = a[j1] - a[j3 + 1];
    x2i = a[j1 + 1] + a[j3];
    x3r = a[j1] + a[j3 + 1];
    x3i = a[j1 + 1] - a[j3];
    y0r = wk1r * x0r - wk1i * x0i;
    y0i = wk1r * x0i + wk1i * x0r;
    y2r = wk1i * x2r - wk1r * x2i;
    y2i = wk1i * x2i + wk1r * x2r;
    a[j0] = y0r + y2r;
    a[j0 + 1] = y0i + y2i;
    a[j1] = y0r - y2r;
    a[j1 + 1] = y0i - y2i;
    y0r = wk1i * x1r - wk1r * x1i;
    y0i = wk1i * x1i + wk1r * x1r;
    y2r = wk1r * x3r - wk1i * x3i;
    y2i = wk1r * x3i + wk1i * x3r;
    a[j2] = y0r - y2r;
    a[j2 + 1] = y0i - y2i;
    a[j3] = y0r + y2r;
    a[j3 + 1] = y0i + y2i;
}


void cftfx41(int n, REAL *a, int nw, REAL *w)
{
    void cftf161(REAL *a, REAL *w);
    void cftf162(REAL *a, REAL *w);
    void cftf081(REAL *a, REAL *w);
    void cftf082(REAL *a, REAL *w);
    
    if (n == 128) {
        cftf161(a, &w[nw - 8]);
        cftf162(&a[32], &w[nw - 32]);
        cftf161(&a[64], &w[nw - 8]);
        cftf161(&a[96], &w[nw - 8]);
    } else {
        cftf081(a, &w[nw - 16]);
        cftf082(&a[16], &w[nw - 16]);
        cftf081(&a[32], &w[nw - 16]);
        cftf081(&a[48], &w[nw - 16]);
    }
}


void cftfx42(int n, REAL *a, int nw, REAL *w)
{
    void cftf161(REAL *a, REAL *w);
    void cftf162(REAL *a, REAL *w);
    void cftf081(REAL *a, REAL *w);
    void cftf082(REAL *a, REAL *w);
    
    if (n == 128) {
        cftf161(a, &w[nw - 8]);
        cftf162(&a[32], &w[nw - 32]);
        cftf161(&a[64], &w[nw - 8]);
        cftf162(&a[96], &w[nw - 32]);
    } else {
        cftf081(a, &w[nw - 16]);
        cftf082(&a[16], &w[nw - 16]);
        cftf081(&a[32], &w[nw - 16]);
        cftf082(&a[48], &w[nw - 16]);
    }
}


void cftf161(REAL *a, REAL *w)
{
    REAL wn4r, wk1r, wk1i, 
        x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i, 
        y0r, y0i, y1r, y1i, y2r, y2i, y3r, y3i, 
        y4r, y4i, y5r, y5i, y6r, y6i, y7r, y7i, 
        y8r, y8i, y9r, y9i, y10r, y10i, y11r, y11i, 
        y12r, y12i, y13r, y13i, y14r, y14i, y15r, y15i;
    
    wn4r = w[1];
    wk1i = wn4r * w[2];
    wk1r = wk1i + w[2];
    x0r = a[0] + a[16];
    x0i = a[1] + a[17];
    x1r = a[0] - a[16];
    x1i = a[1] - a[17];
    x2r = a[8] + a[24];
    x2i = a[9] + a[25];
    x3r = a[8] - a[24];
    x3i = a[9] - a[25];
    y0r = x0r + x2r;
    y0i = x0i + x2i;
    y4r = x0r - x2r;
    y4i = x0i - x2i;
    y8r = x1r - x3i;
    y8i = x1i + x3r;
    y12r = x1r + x3i;
    y12i = x1i - x3r;
    x0r = a[2] + a[18];
    x0i = a[3] + a[19];
    x1r = a[2] - a[18];
    x1i = a[3] - a[19];
    x2r = a[10] + a[26];
    x2i = a[11] + a[27];
    x3r = a[10] - a[26];
    x3i = a[11] - a[27];
    y1r = x0r + x2r;
    y1i = x0i + x2i;
    y5r = x0r - x2r;
    y5i = x0i - x2i;
    x0r = x1r - x3i;
    x0i = x1i + x3r;
    y9r = wk1r * x0r - wk1i * x0i;
    y9i = wk1r * x0i + wk1i * x0r;
    x0r = x1r + x3i;
    x0i = x1i - x3r;
    y13r = wk1i * x0r - wk1r * x0i;
    y13i = wk1i * x0i + wk1r * x0r;
    x0r = a[4] + a[20];
    x0i = a[5] + a[21];
    x1r = a[4] - a[20];
    x1i = a[5] - a[21];
    x2r = a[12] + a[28];
    x2i = a[13] + a[29];
    x3r = a[12] - a[28];
    x3i = a[13] - a[29];
    y2r = x0r + x2r;
    y2i = x0i + x2i;
    y6r = x0r - x2r;
    y6i = x0i - x2i;
    x0r = x1r - x3i;
    x0i = x1i + x3r;
    y10r = wn4r * (x0r - x0i);
    y10i = wn4r * (x0i + x0r);
    x0r = x1r + x3i;
    x0i = x1i - x3r;
    y14r = wn4r * (x0r + x0i);
    y14i = wn4r * (x0i - x0r);
    x0r = a[6] + a[22];
    x0i = a[7] + a[23];
    x1r = a[6] - a[22];
    x1i = a[7] - a[23];
    x2r = a[14] + a[30];
    x2i = a[15] + a[31];
    x3r = a[14] - a[30];
    x3i = a[15] - a[31];
    y3r = x0r + x2r;
    y3i = x0i + x2i;
    y7r = x0r - x2r;
    y7i = x0i - x2i;
    x0r = x1r - x3i;
    x0i = x1i + x3r;
    y11r = wk1i * x0r - wk1r * x0i;
    y11i = wk1i * x0i + wk1r * x0r;
    x0r = x1r + x3i;
    x0i = x1i - x3r;
    y15r = wk1r * x0r - wk1i * x0i;
    y15i = wk1r * x0i + wk1i * x0r;
    x0r = y12r - y14r;
    x0i = y12i - y14i;
    x1r = y12r + y14r;
    x1i = y12i + y14i;
    x2r = y13r - y15r;
    x2i = y13i - y15i;
    x3r = y13r + y15r;
    x3i = y13i + y15i;
    a[24] = x0r + x2r;
    a[25] = x0i + x2i;
    a[26] = x0r - x2r;
    a[27] = x0i - x2i;
    a[28] = x1r - x3i;
    a[29] = x1i + x3r;
    a[30] = x1r + x3i;
    a[31] = x1i - x3r;
    x0r = y8r + y10r;
    x0i = y8i + y10i;
    x1r = y8r - y10r;
    x1i = y8i - y10i;
    x2r = y9r + y11r;
    x2i = y9i + y11i;
    x3r = y9r - y11r;
    x3i = y9i - y11i;
    a[16] = x0r + x2r;
    a[17] = x0i + x2i;
    a[18] = x0r - x2r;
    a[19] = x0i - x2i;
    a[20] = x1r - x3i;
    a[21] = x1i + x3r;
    a[22] = x1r + x3i;
    a[23] = x1i - x3r;
    x0r = y5r - y7i;
    x0i = y5i + y7r;
    x2r = wn4r * (x0r - x0i);
    x2i = wn4r * (x0i + x0r);
    x0r = y5r + y7i;
    x0i = y5i - y7r;
    x3r = wn4r * (x0r - x0i);
    x3i = wn4r * (x0i + x0r);
    x0r = y4r - y6i;
    x0i = y4i + y6r;
    x1r = y4r + y6i;
    x1i = y4i - y6r;
    a[8] = x0r + x2r;
    a[9] = x0i + x2i;
    a[10] = x0r - x2r;
    a[11] = x0i - x2i;
    a[12] = x1r - x3i;
    a[13] = x1i + x3r;
    a[14] = x1r + x3i;
    a[15] = x1i - x3r;
    x0r = y0r + y2r;
    x0i = y0i + y2i;
    x1r = y0r - y2r;
    x1i = y0i - y2i;
    x2r = y1r + y3r;
    x2i = y1i + y3i;
    x3r = y1r - y3r;
    x3i = y1i - y3i;
    a[0] = x0r + x2r;
    a[1] = x0i + x2i;
    a[2] = x0r - x2r;
    a[3] = x0i - x2i;
    a[4] = x1r - x3i;
    a[5] = x1i + x3r;
    a[6] = x1r + x3i;
    a[7] = x1i - x3r;
}


void cftf162(REAL *a, REAL *w)
{
    REAL wn4r, wk1r, wk1i, wk2r, wk2i, wk3r, wk3i, 
        x0r, x0i, x1r, x1i, x2r, x2i, 
        y0r, y0i, y1r, y1i, y2r, y2i, y3r, y3i, 
        y4r, y4i, y5r, y5i, y6r, y6i, y7r, y7i, 
        y8r, y8i, y9r, y9i, y10r, y10i, y11r, y11i, 
        y12r, y12i, y13r, y13i, y14r, y14i, y15r, y15i;
    
    wn4r = w[1];
    wk1r = w[4];
    wk1i = w[5];
    wk3r = w[6];
    wk3i = w[7];
    wk2r = w[8];
    wk2i = w[9];
    x1r = a[0] - a[17];
    x1i = a[1] + a[16];
    x0r = a[8] - a[25];
    x0i = a[9] + a[24];
    x2r = wn4r * (x0r - x0i);
    x2i = wn4r * (x0i + x0r);
    y0r = x1r + x2r;
    y0i = x1i + x2i;
    y4r = x1r - x2r;
    y4i = x1i - x2i;
    x1r = a[0] + a[17];
    x1i = a[1] - a[16];
    x0r = a[8] + a[25];
    x0i = a[9] - a[24];
    x2r = wn4r * (x0r - x0i);
    x2i = wn4r * (x0i + x0r);
    y8r = x1r - x2i;
    y8i = x1i + x2r;
    y12r = x1r + x2i;
    y12i = x1i - x2r;
    x0r = a[2] - a[19];
    x0i = a[3] + a[18];
    x1r = wk1r * x0r - wk1i * x0i;
    x1i = wk1r * x0i + wk1i * x0r;
    x0r = a[10] - a[27];
    x0i = a[11] + a[26];
    x2r = wk3i * x0r - wk3r * x0i;
    x2i = wk3i * x0i + wk3r * x0r;
    y1r = x1r + x2r;
    y1i = x1i + x2i;
    y5r = x1r - x2r;
    y5i = x1i - x2i;
    x0r = a[2] + a[19];
    x0i = a[3] - a[18];
    x1r = wk3r * x0r - wk3i * x0i;
    x1i = wk3r * x0i + wk3i * x0r;
    x0r = a[10] + a[27];
    x0i = a[11] - a[26];
    x2r = wk1r * x0r + wk1i * x0i;
    x2i = wk1r * x0i - wk1i * x0r;
    y9r = x1r - x2r;
    y9i = x1i - x2i;
    y13r = x1r + x2r;
    y13i = x1i + x2i;
    x0r = a[4] - a[21];
    x0i = a[5] + a[20];
    x1r = wk2r * x0r - wk2i * x0i;
    x1i = wk2r * x0i + wk2i * x0r;
    x0r = a[12] - a[29];
    x0i = a[13] + a[28];
    x2r = wk2i * x0r - wk2r * x0i;
    x2i = wk2i * x0i + wk2r * x0r;
    y2r = x1r + x2r;
    y2i = x1i + x2i;
    y6r = x1r - x2r;
    y6i = x1i - x2i;
    x0r = a[4] + a[21];
    x0i = a[5] - a[20];
    x1r = wk2i * x0r - wk2r * x0i;
    x1i = wk2i * x0i + wk2r * x0r;
    x0r = a[12] + a[29];
    x0i = a[13] - a[28];
    x2r = wk2r * x0r - wk2i * x0i;
    x2i = wk2r * x0i + wk2i * x0r;
    y10r = x1r - x2r;
    y10i = x1i - x2i;
    y14r = x1r + x2r;
    y14i = x1i + x2i;
    x0r = a[6] - a[23];
    x0i = a[7] + a[22];
    x1r = wk3r * x0r - wk3i * x0i;
    x1i = wk3r * x0i + wk3i * x0r;
    x0r = a[14] - a[31];
    x0i = a[15] + a[30];
    x2r = wk1i * x0r - wk1r * x0i;
    x2i = wk1i * x0i + wk1r * x0r;
    y3r = x1r + x2r;
    y3i = x1i + x2i;
    y7r = x1r - x2r;
    y7i = x1i - x2i;
    x0r = a[6] + a[23];
    x0i = a[7] - a[22];
    x1r = wk1i * x0r + wk1r * x0i;
    x1i = wk1i * x0i - wk1r * x0r;
    x0r = a[14] + a[31];
    x0i = a[15] - a[30];
    x2r = wk3i * x0r - wk3r * x0i;
    x2i = wk3i * x0i + wk3r * x0r;
    y11r = x1r + x2r;
    y11i = x1i + x2i;
    y15r = x1r - x2r;
    y15i = x1i - x2i;
    x1r = y0r + y2r;
    x1i = y0i + y2i;
    x2r = y1r + y3r;
    x2i = y1i + y3i;
    a[0] = x1r + x2r;
    a[1] = x1i + x2i;
    a[2] = x1r - x2r;
    a[3] = x1i - x2i;
    x1r = y0r - y2r;
    x1i = y0i - y2i;
    x2r = y1r - y3r;
    x2i = y1i - y3i;
    a[4] = x1r - x2i;
    a[5] = x1i + x2r;
    a[6] = x1r + x2i;
    a[7] = x1i - x2r;
    x1r = y4r - y6i;
    x1i = y4i + y6r;
    x0r = y5r - y7i;
    x0i = y5i + y7r;
    x2r = wn4r * (x0r - x0i);
    x2i = wn4r * (x0i + x0r);
    a[8] = x1r + x2r;
    a[9] = x1i + x2i;
    a[10] = x1r - x2r;
    a[11] = x1i - x2i;
    x1r = y4r + y6i;
    x1i = y4i - y6r;
    x0r = y5r + y7i;
    x0i = y5i - y7r;
    x2r = wn4r * (x0r - x0i);
    x2i = wn4r * (x0i + x0r);
    a[12] = x1r - x2i;
    a[13] = x1i + x2r;
    a[14] = x1r + x2i;
    a[15] = x1i - x2r;
    x1r = y8r + y10r;
    x1i = y8i + y10i;
    x2r = y9r - y11r;
    x2i = y9i - y11i;
    a[16] = x1r + x2r;
    a[17] = x1i + x2i;
    a[18] = x1r - x2r;
    a[19] = x1i - x2i;
    x1r = y8r - y10r;
    x1i = y8i - y10i;
    x2r = y9r + y11r;
    x2i = y9i + y11i;
    a[20] = x1r - x2i;
    a[21] = x1i + x2r;
    a[22] = x1r + x2i;
    a[23] = x1i - x2r;
    x1r = y12r - y14i;
    x1i = y12i + y14r;
    x0r = y13r + y15i;
    x0i = y13i - y15r;
    x2r = wn4r * (x0r - x0i);
    x2i = wn4r * (x0i + x0r);
    a[24] = x1r + x2r;
    a[25] = x1i + x2i;
    a[26] = x1r - x2r;
    a[27] = x1i - x2i;
    x1r = y12r + y14i;
    x1i = y12i - y14r;
    x0r = y13r - y15i;
    x0i = y13i + y15r;
    x2r = wn4r * (x0r - x0i);
    x2i = wn4r * (x0i + x0r);
    a[28] = x1r - x2i;
    a[29] = x1i + x2r;
    a[30] = x1r + x2i;
    a[31] = x1i - x2r;
}


void cftf081(REAL *a, REAL *w)
{
    REAL wn4r, x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i, 
        y0r, y0i, y1r, y1i, y2r, y2i, y3r, y3i, 
        y4r, y4i, y5r, y5i, y6r, y6i, y7r, y7i;
    
    wn4r = w[1];
    x0r = a[0] + a[8];
    x0i = a[1] + a[9];
    x1r = a[0] - a[8];
    x1i = a[1] - a[9];
    x2r = a[4] + a[12];
    x2i = a[5] + a[13];
    x3r = a[4] - a[12];
    x3i = a[5] - a[13];
    y0r = x0r + x2r;
    y0i = x0i + x2i;
    y2r = x0r - x2r;
    y2i = x0i - x2i;
    y1r = x1r - x3i;
    y1i = x1i + x3r;
    y3r = x1r + x3i;
    y3i = x1i - x3r;
    x0r = a[2] + a[10];
    x0i = a[3] + a[11];
    x1r = a[2] - a[10];
    x1i = a[3] - a[11];
    x2r = a[6] + a[14];
    x2i = a[7] + a[15];
    x3r = a[6] - a[14];
    x3i = a[7] - a[15];
    y4r = x0r + x2r;
    y4i = x0i + x2i;
    y6r = x0r - x2r;
    y6i = x0i - x2i;
    x0r = x1r - x3i;
    x0i = x1i + x3r;
    x2r = x1r + x3i;
    x2i = x1i - x3r;
    y5r = wn4r * (x0r - x0i);
    y5i = wn4r * (x0r + x0i);
    y7r = wn4r * (x2r - x2i);
    y7i = wn4r * (x2r + x2i);
    a[8] = y1r + y5r;
    a[9] = y1i + y5i;
    a[10] = y1r - y5r;
    a[11] = y1i - y5i;
    a[12] = y3r - y7i;
    a[13] = y3i + y7r;
    a[14] = y3r + y7i;
    a[15] = y3i - y7r;
    a[0] = y0r + y4r;
    a[1] = y0i + y4i;
    a[2] = y0r - y4r;
    a[3] = y0i - y4i;
    a[4] = y2r - y6i;
    a[5] = y2i + y6r;
    a[6] = y2r + y6i;
    a[7] = y2i - y6r;
}


void cftf082(REAL *a, REAL *w)
{
    REAL wn4r, wk1r, wk1i, x0r, x0i, x1r, x1i, 
        y0r, y0i, y1r, y1i, y2r, y2i, y3r, y3i, 
        y4r, y4i, y5r, y5i, y6r, y6i, y7r, y7i;
    
    wn4r = w[1];
    wk1r = w[4];
    wk1i = w[5];
    y0r = a[0] - a[9];
    y0i = a[1] + a[8];
    y1r = a[0] + a[9];
    y1i = a[1] - a[8];
    x0r = a[4] - a[13];
    x0i = a[5] + a[12];
    y2r = wn4r * (x0r - x0i);
    y2i = wn4r * (x0i + x0r);
    x0r = a[4] + a[13];
    x0i = a[5] - a[12];
    y3r = wn4r * (x0r - x0i);
    y3i = wn4r * (x0i + x0r);
    x0r = a[2] - a[11];
    x0i = a[3] + a[10];
    y4r = wk1r * x0r - wk1i * x0i;
    y4i = wk1r * x0i + wk1i * x0r;
    x0r = a[2] + a[11];
    x0i = a[3] - a[10];
    y5r = wk1i * x0r - wk1r * x0i;
    y5i = wk1i * x0i + wk1r * x0r;
    x0r = a[6] - a[15];
    x0i = a[7] + a[14];
    y6r = wk1i * x0r - wk1r * x0i;
    y6i = wk1i * x0i + wk1r * x0r;
    x0r = a[6] + a[15];
    x0i = a[7] - a[14];
    y7r = wk1r * x0r - wk1i * x0i;
    y7i = wk1r * x0i + wk1i * x0r;
    x0r = y0r + y2r;
    x0i = y0i + y2i;
    x1r = y4r + y6r;
    x1i = y4i + y6i;
    a[0] = x0r + x1r;
    a[1] = x0i + x1i;
    a[2] = x0r - x1r;
    a[3] = x0i - x1i;
    x0r = y0r - y2r;
    x0i = y0i - y2i;
    x1r = y4r - y6r;
    x1i = y4i - y6i;
    a[4] = x0r - x1i;
    a[5] = x0i + x1r;
    a[6] = x0r + x1i;
    a[7] = x0i - x1r;
    x0r = y1r - y3i;
    x0i = y1i + y3r;
    x1r = y5r - y7r;
    x1i = y5i - y7i;
    a[8] = x0r + x1r;
    a[9] = x0i + x1i;
    a[10] = x0r - x1r;
    a[11] = x0i - x1i;
    x0r = y1r + y3i;
    x0i = y1i - y3r;
    x1r = y5r + y7r;
    x1i = y5i + y7i;
    a[12] = x0r - x1i;
    a[13] = x0i + x1r;
    a[14] = x0r + x1i;
    a[15] = x0i - x1r;
}


void cftf040(REAL *a)
{
    REAL x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i;
    
    x0r = a[0] + a[4];
    x0i = a[1] + a[5];
    x1r = a[0] - a[4];
    x1i = a[1] - a[5];
    x2r = a[2] + a[6];
    x2i = a[3] + a[7];
    x3r = a[2] - a[6];
    x3i = a[3] - a[7];
    a[0] = x0r + x2r;
    a[1] = x0i + x2i;
    a[4] = x0r - x2r;
    a[5] = x0i - x2i;
    a[2] = x1r - x3i;
    a[3] = x1i + x3r;
    a[6] = x1r + x3i;
    a[7] = x1i - x3r;
}


void cftb040(REAL *a)
{
    REAL x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i;
    
    x0r = a[0] + a[4];
    x0i = a[1] + a[5];
    x1r = a[0] - a[4];
    x1i = a[1] - a[5];
    x2r = a[2] + a[6];
    x2i = a[3] + a[7];
    x3r = a[2] - a[6];
    x3i = a[3] - a[7];
    a[0] = x0r + x2r;
    a[1] = x0i + x2i;
    a[4] = x0r - x2r;
    a[5] = x0i - x2i;
    a[2] = x1r + x3i;
    a[3] = x1i - x3r;
    a[6] = x1r - x3i;
    a[7] = x1i + x3r;
}


void cftx020(REAL *a)
{
    REAL x0r, x0i;
    
    x0r = a[0] - a[2];
    x0i = a[1] - a[3];
    a[0] += a[2];
    a[1] += a[3];
    a[2] = x0r;
    a[3] = x0i;
}


void rftfsub(int n, REAL *a, int nc, REAL *c)
{
    int j, k, kk, ks, m;
    REAL wkr, wki, xr, xi, yr, yi;
    
    m = n >> 1;
    ks = 2 * nc / m;
    kk = 0;
    for (j = 2; j < m; j += 2) {
        k = n - j;
        kk += ks;
        wkr = 0.5 - c[nc - kk];
        wki = c[kk];
        xr = a[j] - a[k];
        xi = a[j + 1] + a[k + 1];
        yr = wkr * xr - wki * xi;
        yi = wkr * xi + wki * xr;
        a[j] -= yr;
        a[j + 1] -= yi;
        a[k] += yr;
        a[k + 1] -= yi;
    }
}


void rftbsub(int n, REAL *a, int nc, REAL *c)
{
    int j, k, kk, ks, m;
    REAL wkr, wki, xr, xi, yr, yi;
    
    m = n >> 1;
    ks = 2 * nc / m;
    kk = 0;
    for (j = 2; j < m; j += 2) {
        k = n - j;
        kk += ks;
        wkr = 0.5 - c[nc - kk];
        wki = c[kk];
        xr = a[j] - a[k];
        xi = a[j + 1] + a[k + 1];
        yr = wkr * xr + wki * xi;
        yi = wkr * xi - wki * xr;
        a[j] -= yr;
        a[j + 1] -= yi;
        a[k] += yr;
        a[k + 1] -= yi;
    }
}


void dctsub(int n, REAL *a, int nc, REAL *c)
{
    int j, k, kk, ks, m;
    REAL wkr, wki, xr;
    
    m = n >> 1;
    ks = nc / n;
    kk = 0;
    for (j = 1; j < m; j++) {
        k = n - j;
        kk += ks;
        wkr = c[kk] - c[nc - kk];
        wki = c[kk] + c[nc - kk];
        xr = wki * a[j] - wkr * a[k];
        a[j] = wkr * a[j] + wki * a[k];
        a[k] = xr;
    }
    a[m] *= c[0];
}


void dstsub(int n, REAL *a, int nc, REAL *c)
{
    int j, k, kk, ks, m;
    REAL wkr, wki, xr;
    
    m = n >> 1;
    ks = nc / n;
    kk = 0;
    for (j = 1; j < m; j++) {
        k = n - j;
        kk += ks;
        wkr = c[kk] - c[nc - kk];
        wki = c[kk] + c[nc - kk];
        xr = wki * a[k] - wkr * a[j];
        a[k] = wkr * a[k] + wki * a[j];
        a[j] = xr;
    }
    a[m] *= c[0];
}



// ===========================================================================================================================



//
//	                 FFT library
//
//  (one-dimensional complex and real FFTs for array 
//  lengths of 2^n)
//
//	Author: Toth Laszlo (tothl@inf.u-szeged.hu)
//  
//	Research Group on Artificial Intelligence
//  H-6720 Szeged, Aradi vertanuk tere 1, Hungary
//	
//	Last modified: 97.05.29
/////////////////////////////////////////////////////////

#include <math.h>
#include <stdlib.h>


#define pi 3.1415926535897932384626433832795029L

/////////////////////////////////////////////////////////
//Gives back "i" bit-reversed where "size" is the array
//length
//currently none of the routines call it

long bitreverse(long i, long size){

	long result,mask;

	result=0;
	
	for(;size>1;size>>=1){
		mask=i&1;
		i>>=1;
		result<<=1;
		result|=mask;
	}


/*
	long sizze;
	
     __asm{       //the same in assebly
     mov eax,i
	 mov ecx,sizze
     mov ebx,0
   l:shr eax,1
     rcl ebx,1 
	 shr ecx,1
	 cmp ecx,1
     jnz l 
     mov result,ebx
     }

*/

	 return result;
}

////////////////////////////////////////////////////////
//Bit-reverser for the Bruun FFT
//Parameters as of "bitreverse()"

long bruun_reverse(long i, long sizze){

	long result, add;

	result=0;
	add=sizze;

	while(1){
	if(i!=0) {
		while((i&1)==0) { i>>=1; add>>=1;}
		i>>=1; add>>=1;
		result+=add;
	}
	else {result<<=1;result+=add; return result;}
	if(i!=0) {
		while((i&1)==0) { i>>=1; add>>=1;}
		i>>=1; add>>=1;
		result-=add;
	}
	else {result<<=1;result-=add; return result;}
	}
}

/*assembly version
long bruun_reverse(long i, long sizze){

	long result;

	result=0;

	__asm{
		mov edx,sizze
		mov eax,i
		mov ebx,0
	 l: bsf cx,eax
		jz kesz1
		inc cx
		shr edx,cl
	    add ebx,edx
		shr eax,cl
		bsf cx,eax
		jz kesz2
		inc cx
		shr edx,cl
		sub ebx,edx
		shr eax,cl
		jmp l
kesz1:
		shl ebx,1
		
		add ebx,edx
		jmp vege
kesz2:
		shl ebx,1
		sub ebx,edx
	
  vege: mov result,ebx
	}
	return result;
}*/

/////////////////////////////////////////////////////////
// Decimation-in-freq radix-2 in-place butterfly
// data: array of doubles:
// re(0),im(0),re(1),im(1),...,re(size-1),im(size-1)
// it means that size=array_length/2 !!
// 
// suggested use:
// intput in normal order
// output in bit-reversed order
//
// Source: Rabiner-Gold: Theory and Application of DSP,
// Prentice Hall,1978

void dif_butterfly(REAL *data,long size){

	long angle,astep,dl;
	REAL xr,yr,xi,yi,wr,wi,dr,di,ang;
	REAL *l1, *l2, *end, *ol2;

     astep=1;
	 end=data+size+size;
     for(dl=size;dl>1;dl>>=1,astep+=astep){
					  l1=data;
					  l2=data+dl;
                      for(;l2<end;l1=l2,l2=l2+dl){
							ol2=l2;
                            for(angle=0;l1<ol2;l1+=2,l2+=2){
                               ang=2*pi*angle/size;
                               wr=cos(ang);
                               wi=-sin(ang);
                               xr=*l1+*l2;
                               xi=*(l1+1)+*(l2+1);
                               dr=*l1-*l2;
                               di=*(l1+1)-*(l2+1);
                               yr=dr*wr-di*wi;
                               yi=dr*wi+di*wr;
                               *(l1)=xr;*(l1+1)=xi;
                               *(l2)=yr;*(l2+1)=yi;
                               angle+=astep;
							   }
					  }
	 }
}

/////////////////////////////////////////////////////////
// Decimation-in-time radix-2 in-place inverse butterfly
// data: array of doubles:
// re(0),im(0),re(1),im(1),...,re(size-1),im(size-1)
// it means that size=array_length/2 !!
//
// suggested use:
// intput in bit-reversed order
// output in normal order
//
// Source: Rabiner-Gold: Theory and Application of DSP,
// Prentice Hall,1978

void inverse_dit_butterfly(REAL *data,long size){

	long angle,astep,dl;
	REAL xr,yr,xi,yi,wr,wi,dr,di,ang;
	REAL *l1, *l2, *end, *ol2;

     astep=size>>1;
	 end=data+size+size;
     for(dl=2;astep>0;dl+=dl,astep>>=1){
					  l1=data;
					  l2=data+dl;
                      for(;l2<end;l1=l2,l2=l2+dl){
							ol2=l2;
                            for(angle=0;l1<ol2;l1+=2,l2+=2){
                               ang=2*pi*angle/size;
                               wr=cos(ang);
                               wi=sin(ang);
                               xr=*l1;
                               xi=*(l1+1);
                               yr=*l2;
                               yi=*(l2+1);
                               dr=yr*wr-yi*wi;
                               di=yr*wi+yi*wr;
                               *(l1)=xr+dr;*(l1+1)=xi+di;
                               *(l2)=xr-dr;*(l2+1)=xi-di;
                               angle+=astep;
							   }
					  }
	 }
}

/////////////////////////////////////////////////////////
// data shuffling into bit-reversed order
// data: array of doubles:
// re(0),im(0),re(1),im(1),...,re(size-1),im(size-1)
// it means that size=array_length/2 !!
//
// Source: Rabiner-Gold: Theory and Application of DSP,
// Prentice Hall,1978 

void unshuffle(REAL *data, long size){

	long i,j,k,l,m;
	REAL re,im;

	//old version - turned out to be a bit slower
    /*for(i=0;i<size-1;i++){
             j=bitreverse(i,size);
             if (j>i){   //swap
                    re=data[i+i];im=data[i+i+1];
                    data[i+i]=data[j+j];data[i+i+1]=data[j+j+1];
                    data[j+j]=re;data[j+j+1]=im;
			 }
	}*/

l=size-1;
m=size>>1;
for (i=0,j=0; i<l ; i++){
	  if (i<j){
				re=data[j+j]; im=data[j+j+1];
				data[j+j]=data[i+i]; data[j+j+1]=data[i+i+1];
				data[i+i]=re; data[i+i+1]=im;
				}
	  k=m;
	  while (k<=j){
				j-=k;
				k>>=1;	
				}
	  j+=k;
      }
}

/////////////////////////////////////////////////////////
// used by realfft 
// parameters as above
//
// Source: Brigham: The Fast Fourier Transform
// Prentice Hall, ?

void realize(REAL *data, long size){

	REAL xr,yr,xi,yi,wr,wi,dr,di,ang,astep;
	REAL *l1, *l2;

	l1=data;
	l2=data+size+size-2;
    xr=*l1;
    xi=*(l1+1);
    *l1=xr+xi;
    *(l1+1)=xr-xi; 
	l1+=2;
	astep=pi/size;
    for(ang=astep;l1<=l2;l1+=2,l2-=2,ang+=astep){
                               xr=(*l1+*l2)/2;
                               yi=(-(*l1)+(*l2))/2;
                               yr=(*(l1+1)+*(l2+1))/2;
                               xi=(*(l1+1)-*(l2+1))/2;
                               wr=cos(ang);
                               wi=-sin(ang);
                               dr=yr*wr-yi*wi;
                               di=yr*wi+yi*wr;
                               *l1=xr+dr;
                               *(l1+1)=xi+di;      
                               *l2=xr-dr;
                               *(l2+1)=-xi+di;
	}
}

/////////////////////////////////////////////////////////
// used by inverse realfft 
// parameters as above
//
// Source: Brigham: The Fast Fourier Transform
// Prentice Hall, ?

void unrealize(REAL *data, long size){

	REAL xr,yr,xi,yi,wr,wi,dr,di,ang,astep;
	REAL *l1, *l2;

	l1=data;
	l2=data+size+size-2;
    xr=(*l1)/2;
    xi=(*(l1+1))/2;
    *l1=xr+xi;
    *(l1+1)=xr-xi; 
	l1+=2;
	astep=pi/size;
    for(ang=astep;l1<=l2;l1+=2,l2-=2,ang+=astep){
                               xr=(*l1+*l2)/2;
                               yi=-(-(*l1)+(*l2))/2;
                               yr=(*(l1+1)+*(l2+1))/2;
                               xi=(*(l1+1)-*(l2+1))/2;
                               wr=cos(ang);
                               wi=-sin(ang);
                               dr=yr*wr-yi*wi;
                               di=yr*wi+yi*wr;
                               *l2=xr+dr;
                               *(l1+1)=xi+di;      
                               *l1=xr-dr;
                               *(l2+1)=-xi+di;
	}
}

/////////////////////////////////////////////////////////
// in-place Radix-2 FFT for complex values
// data: array of doubles:
// re(0),im(0),re(1),im(1),...,re(size-1),im(size-1)
// it means that size=array_length/2 !!
// 
// output is in similar order, normalized by array length
//
// Source: see the routines it calls ...

void fft(REAL *data, long size)
{

	REAL *l, *end;

	dif_butterfly(data,size);
	unshuffle(data,size);

	end=data+size+size;
	for(l=data;l<end;l++){*l=*l/size;};
}


/////////////////////////////////////////////////////////
// in-place Radix-2 inverse FFT for complex values
// data: array of doubles:
// re(0),im(0),re(1),im(1),...,re(size-1),im(size-1)
// it means that size=array_length/2 !!
// 
// output is in similar order, NOT normalized by 
// array length
//
// Source: see the routines it calls ...

void ifft(REAL* data, long size)
{

	unshuffle(data,size);
	inverse_dit_butterfly(data,size);
}

/////////////////////////////////////////////////////////
// in-place Radix-2 FFT for real values
// (by the so-called "packing method")
// data: array of doubles:
// re(0),re(1),re(2),...,re(size-1)
// 
// output:
// re(0),re(size/2),re(1),im(1),re(2),im(2),...,re(size/2-1),im(size/2-1)
// normalized by array length
//
// Source: see the routines it calls ...

void realfft_packed(REAL *data, long size){

	REAL *l, *end;
	REAL div;

	size>>=1;
	dif_butterfly(data,size);
	unshuffle(data,size);
	realize(data,size);

	end=data+size+size;
	div=size+size;
	for(l=data;l<end;l++){*l=*l/div;};
}


/////////////////////////////////////////////////////////
// in-place Radix-2 inverse FFT for real values
// (by the so-called "packing method")
// data: array of doubles:
// re(0),re(size/2),re(1),im(1),re(2),im(2),...,re(size/2-1),im(size/2-1) 
// 
// output:
// re(0),re(1),re(2),...,re(size-1)
// NOT normalized by array length
//
//Source: see the routines it calls ...

void irealfft_packed(REAL *data, long size){

	REAL *l, *end;

	size>>=1;
	unrealize(data,size);
	unshuffle(data,size);
	inverse_dit_butterfly(data,size);

	end=data+size+size;
	for(l=data;l<end;l++){*l=(*l)*2;};
}


/////////////////////////////////////////////////////////
// Bruun FFT for real values
// data: array of doubles:
// re(0),re(1),re(2),...,re(size-1)
// 
// output:
// re(0),re(size/2),...,re(i),im(i)... pairs in 
// "bruun-reversed" order
// normalized by array length
//
// Source: 
// Bruun: z-Transform DFT Filters and FFT's
// IEEE Trans. ASSP, ASSP-26, No. 1, February 1978
//
// Comments:
// (this version is implemented in a manner that every 
// cosine is calculated only once;
// faster than the other version (see next)

void realfft_bruun(REAL *data, long size){

	REAL *end, *l0, *l1, *l2, *l3;
	long dl, dl2, dl_o, dl2_o, i, j, k, kk;
	REAL d0,d1,d2,d3,c,c2,p4;

	end=data+size;
	//first filterings, when there're only two taps
	size>>=1;
	dl=size;
	dl2=dl/2;
	for(;dl>1;dl>>=1,dl2>>=1){
		l0=data;
		l3=data+dl;
		for(i=0;i<dl;i++){
			d0=*l0;
			d2=*l3;
			*l0=d0+d2;
			*l3=d0-d2;
			l0++;
			l3++;
		}
	}
	l0=data;l1=data+1;
	d0=*l0;d1=*l1;
	*l0=d0+d1;*l1=d0-d1;
	l1+=2;
	*l1=-(*l1);
	
	//the remaining filterings
	
	p4=pi/(2*size);
	j=1;
	kk=1;
	dl_o=size/2;
	dl2_o=size/4;
	while(dl_o>1){
	for(k=0;k<kk;k++){
	c2=p4*bruun_reverse(j,size);
	c=2*cos(c2);
	c2=2*sin(c2);
	dl=dl_o;
	dl2=dl2_o;
	for(;dl>1;dl>>=1,dl2>>=1){
			l0=data+((dl*j)<<1);
			l1=l0+dl2;l2=l0+dl;l3=l1+dl;
			for(i=0;i<dl2;i++){
				d1=(*l1)*c;
				d2=(*l2)*c;
				d3=*l3+(*l1);
				d0=*l0+(*l2);
				*l0=d0+d1;
				*l1=d3+d2;
				*l2=d0-d1;
				*l3=d3-d2;
				l0++;
				l1++;
				l2++;
				l3++;
			}
		}
		//real conversion
		l3-=4;
		*l3=*l3-c*(*l0)/2;
		*l0=-c2*(*l0)/2;
		*l1=*l1+c*(*l2)/2;
		*l2=-c2*(*l2)/2;
	j++;
	}
	dl_o>>=1;
	dl2_o>>=1;
	kk<<=1;
	}

	//division with array length
	size<<=1;
	for(i=0;i<size;i++) data[i]=data[i]/size;
}

/////////////////////////////////////////////////////////
// Bruun FFT for real values
// data: array of doubles:
// re(0),re(1),re(2),...,re(size-1)
// 
// output:
// re(0),re(size/2),re(1),im(1),re(2),im(2),...,re(size/2-1),im(size/2-1)
// normalized by array length
//
// Source: see the routines it calls ...
void realfft_bruun_unshuffled(REAL *data, long size){

	REAL *data2;
	long i,j,k;

	realfft_bruun(data,size);
	//unshuffling - cannot be done in-place (?)
	data2=(REAL *)malloc(size*sizeof(double));
	for(i=1,k=size>>1;i<k;i++){
		j=bruun_reverse(i,k);
		data2[j+j]=data[i+i];
		data2[j+j+1]=data[i+i+1];
	}
	for(i=2;i<size;i++) data[i]=data2[i];
	free(data2);
}


/////////////////////////////////////////////////////////
// Bruun FFT for real values
// data: array of doubles:
// re(0),re(1),re(2),...,re(size-1)
// 
// output:
// re(0),re(size/2),...,re(i),im(i)... pairs in 
// "bruun-reversed" order
// normalized by array length
//
// Source: 
// Bruun: z-Transform DFT Filters and FFT's
// IEEE Trans. ASSP, ASSP-26, No. 1, February 1978
//
// Comments:
// (this version is implemented in a row-by-row manner;
// control structure is simpler, but there are too
// much cosine calls - with a cosine lookup table
// probably this would be slightly faster than bruun_fft

/*void realfft_bruun2(REAL *data, long size){

	REAL *end, *l0, *l1, *l2, *l3;
	long dl, dl2, i, j;
	REAL d0,d1,d2,d3,c,c2,p4;

	end=data+size;
	p4=pi/(size);
	size>>=1;
	dl=size;
	dl2=dl/2;
	//first filtering, when there're only two taps
	for(;dl>1;dl>>=1,dl2>>=1){
		l0=data;
		l3=data+dl;
		for(i=0;i<dl;i++){
			d0=*l0;
			d2=*l3;
			*l0=d0+d2;
			*l3=d0-d2;
			l0++;
			l3++;
		}
		//the remaining filterings
		j=1;
		while(l3<end){
			l0=l3;l1=l0+dl2;l2=l0+dl;l3=l1+dl;
			c=2*cos(p4*bruun_reverse(j,size));
			for(i=0;i<dl2;i++){
				d0=*l0;
				d1=*l1;
				d2=*l2;
				d3=*l3;
				*l0=d0+c*d1+d2;
				*l2=d0-c*d1+d2;
				*l1=d1+c*d2+d3;
				*l3=d1-c*d2+d3;
				l0++;
				l1++;
				l2++;
				l3++;
			}
			j++;
		}
	}

	//the last row: transform of real data	
	//the first two cells 
	l0=data;l1=data+1;
	d0=*l0;d1=*l1;
	*l0=d0+d1;*l1=d0-d1;
	l1+=2;
	*l1=-(*l1);
	l0+=4;l1+=2;
	//the remaining cells
	j=1;
	while(l0<end){
			c=p4*bruun_reverse(j,size);
			c2=sin(c);
			c=cos(c);
			*l0=*l0-c*(*l1);
			*l1=-c2*(*l1);
			l0+=2;
			l1+=2;
			*l0=*l0+c*(*l1);
			*l1=-c2*(*l1);
			l0+=2;
			l1+=2;
			j++;
			}
	//division with array length
	for(i=0;i<size;i++) data[i]=data[i]/size;
}
*/

//the same as realfft_bruun_unshuffled,
//but calls realfft_bruun2
/*void realfft_bruun_unshuffled2(REAL *data, long size){

	REAL *data2;
	long i,j,k;

	realfft_bruun2(data,size);
	//unshuffling - cannot be done in-place (?)
	data2=(REAL *)malloc(size*sizeof(double));
	for(i=1,k=size>>1;i<k;i++){
		j=bruun_reverse(i,k);
		data2[j+j]=data[i+i];
		data2[j+j+1]=data[i+i+1];
	}
	for(i=2;i<size;i++) data[i]=data2[i];
	free(data2);
}*/

/////////////////////////////////////////////////////////
// Sorensen in-place split-radix FFT for real values
// data: array of doubles:
// re(0),re(1),re(2),...,re(size-1)
// 
// output:
// re(0),re(1),re(2),...,re(size/2),im(size/2-1),...,im(1)
// normalized by array length
//
// Source: 
// Sorensen et al: Real-Valued Fast Fourier Transform Algorithms,
// IEEE Trans. ASSP, ASSP-35, No. 6, June 1987

void realfft_split(REAL *data,long n){

  long i,j,k,i5,i6,i7,i8,i0,id,i1,i2,i3,i4,n2,n4,n8;
  REAL t1,t2,t3,t4,t5,t6,a3,ss1,ss3,cc1,cc3,a,e,sqrt2;
  
  sqrt2=sqrt(2.0);
  n4=n-1;
  
  //data shuffling
      for (i=0,j=0,n2=n/2; i<n4 ; i++){
	  if (i<j){
				t1=data[j];
				data[j]=data[i];
				data[i]=t1;
				}
	  k=n2;
	  while (k<=j){
				j-=k;
				k>>=1;	
				}
	  j+=k;
      }
	
/*----------------------*/
	
	//length two butterflies	
	i0=0;
	id=4;
   do{
       for (; i0<n4; i0+=id){ 
			i1=i0+1;
			t1=data[i0];
			data[i0]=t1+data[i1];
			data[i1]=t1-data[i1];
		}
	   id<<=1;
       i0=id-2;
       id<<=1;
    } while ( i0<n4 );

   /*----------------------*/
   //L shaped butterflies
n2=2;
for(k=n;k>2;k>>=1){  
	n2<<=1;
	n4=n2>>2;
	n8=n2>>3;
	e = 2*pi/(n2);
	i1=0;
	id=n2<<1;
	do{ 
	    for (; i1<n; i1+=id){
			i2=i1+n4;
			i3=i2+n4;
			i4=i3+n4;
			t1=data[i4]+data[i3];
			data[i4]-=data[i3];
			data[i3]=data[i1]-t1;
			data[i1]+=t1;
			if (n4!=1){
				i0=i1+n8;
				i2+=n8;
				i3+=n8;
				i4+=n8;
				t1=(data[i3]+data[i4])/sqrt2;
				t2=(data[i3]-data[i4])/sqrt2;
				data[i4]=data[i2]-t1;
				data[i3]=-data[i2]-t1;
				data[i2]=data[i0]-t2;
				data[i0]+=t2;
			}
	     }
		 id<<=1;
	     i1=id-n2;
	     id<<=1;
	  } while ( i1<n );
	a=e;
	for (j=2; j<=n8; j++){  
	      a3=3*a;
	      cc1=cos(a);
	      ss1=sin(a);
	      cc3=cos(a3);
	      ss3=sin(a3);
	      a=j*e;
	      i=0;
	      id=n2<<1;
	      do{
		   for (; i<n; i+=id){  
			  i1=i+j-1;
			  i2=i1+n4;
			  i3=i2+n4;
			  i4=i3+n4;
			  i5=i+n4-j+1;
			  i6=i5+n4;
			  i7=i6+n4;
			  i8=i7+n4;
			  t1=data[i3]*cc1+data[i7]*ss1;
			  t2=data[i7]*cc1-data[i3]*ss1;
			  t3=data[i4]*cc3+data[i8]*ss3;
			  t4=data[i8]*cc3-data[i4]*ss3;
			  t5=t1+t3;
			  t6=t2+t4;
			  t3=t1-t3;
			  t4=t2-t4;
			  t2=data[i6]+t6;
			  data[i3]=t6-data[i6];
			  data[i8]=t2;
			  t2=data[i2]-t3;
			  data[i7]=-data[i2]-t3;
			  data[i4]=t2;
			  t1=data[i1]+t5;
			  data[i6]=data[i1]-t5;
			  data[i1]=t1;
			  t1=data[i5]+t4;
			  data[i5]-=t4;
			  data[i2]=t1;
		     }
		   id<<=1;
		   i=id-n2;
		   id<<=1;
		 } while(i<n);
	   }
      }

	//division with array length
   for(i=0;i<n;i++) data[i]/=n;
}


/////////////////////////////////////////////////////////
// Sorensen in-place split-radix FFT for real values
// data: array of doubles:
// re(0),re(1),re(2),...,re(size-1)
// 
// output:
// re(0),re(size/2),re(1),im(1),re(2),im(2),...,re(size/2-1),im(size/2-1)
// normalized by array length
//
// Source: 
// Source: see the routines it calls ...

void realfft_split_unshuffled(REAL *data,long n){

	REAL *data2;
	long i,j;

	realfft_split(data,n);
	//unshuffling - not in-place
	data2=(REAL *)malloc(n*sizeof(double));
	j=n/2;
	data2[0]=data[0];
	data2[1]=data[j];
	for(i=1;i<j;i++) {data2[i+i]=data[i];data2[i+i+1]=data[n-i];}
	for(i=0;i<n;i++) data[i]=data2[i];
	free(data2);
}

/////////////////////////////////////////////////////////
// Sorensen in-place inverse split-radix FFT for real values
// data: array of doubles:
// re(0),re(1),re(2),...,re(size/2),im(size/2-1),...,im(1)
// 
// output:
// re(0),re(1),re(2),...,re(size-1)
// NOT normalized by array length
//
// Source: 
// Sorensen et al: Real-Valued Fast Fourier Transform Algorithms,
// IEEE Trans. ASSP, ASSP-35, No. 6, June 1987

void irealfft_split(REAL *data,long n){

  long i,j,k,i5,i6,i7,i8,i0,id,i1,i2,i3,i4,n2,n4,n8,n1;
  REAL t1,t2,t3,t4,t5,a3,ss1,ss3,cc1,cc3,a,e,sqrt2;
  
  sqrt2=sqrt(2.0);
  
n1=n-1;
n2=n<<1;
for(k=n;k>2;k>>=1){  
	id=n2;
	n2>>=1;
	n4=n2>>2;
	n8=n2>>3;
	e = 2*pi/(n2);
	i1=0;
	do{ 
	    for (; i1<n; i1+=id){
			i2=i1+n4;
			i3=i2+n4;
			i4=i3+n4;
			t1=data[i1]-data[i3];
			data[i1]+=data[i3];
			data[i2]*=2;
			data[i3]=t1-2*data[i4];
			data[i4]=t1+2*data[i4];
			if (n4!=1){
				i0=i1+n8;
				i2+=n8;
				i3+=n8;
				i4+=n8;
				t1=(data[i2]-data[i0])/sqrt2;
				t2=(data[i4]+data[i3])/sqrt2;
				data[i0]+=data[i2];
				data[i2]=data[i4]-data[i3];
				data[i3]=2*(-t2-t1);
				data[i4]=2*(-t2+t1);
			}
	     }
		 id<<=1;
	     i1=id-n2;
	     id<<=1;
	  } while ( i1<n1 );
	a=e;
	for (j=2; j<=n8; j++){  
	      a3=3*a;
	      cc1=cos(a);
	      ss1=sin(a);
	      cc3=cos(a3);
	      ss3=sin(a3);
	      a=j*e;
	      i=0;
	      id=n2<<1;
	      do{
		   for (; i<n; i+=id){  
			  i1=i+j-1;
			  i2=i1+n4;
			  i3=i2+n4;
			  i4=i3+n4;
			  i5=i+n4-j+1;
			  i6=i5+n4;
			  i7=i6+n4;
			  i8=i7+n4;
			  t1=data[i1]-data[i6];
			  data[i1]+=data[i6];
			  t2=data[i5]-data[i2];
			  data[i5]+=data[i2];
			  t3=data[i8]+data[i3];
			  data[i6]=data[i8]-data[i3];
			  t4=data[i4]+data[i7];
			  data[i2]=data[i4]-data[i7];
			  t5=t1-t4;
			  t1+=t4;
			  t4=t2-t3;
			  t2+=t3;
			  data[i3]=t5*cc1+t4*ss1;
			  data[i7]=-t4*cc1+t5*ss1;
			  data[i4]=t1*cc3-t2*ss3;
			  data[i8]=t2*cc3+t1*ss3;
			  }
		   id<<=1;
		   i=id-n2;
		   id<<=1;
		 } while(i<n1);
	   }
	}	

   /*----------------------*/
	i0=0;
	id=4;
   do{
       for (; i0<n1; i0+=id){ 
			i1=i0+1;
			t1=data[i0];
			data[i0]=t1+data[i1];
			data[i1]=t1-data[i1];
		}
	   id<<=1;
       i0=id-2;
       id<<=1;
    } while ( i0<n1 );

/*----------------------*/

//data shuffling
      for (i=0,j=0,n2=n/2; i<n1 ; i++){
	  if (i<j){
				t1=data[j];
				data[j]=data[i];
				data[i]=t1;
				}
	  k=n2;
	  while (k<=j){
				j-=k;
				k>>=1;	
				}
	  j+=k;
      }	
}


/////////////////////////////////////////////////////////
// Sorensen in-place radix-2 FFT for real values
// data: array of doubles:
// re(0),re(1),re(2),...,re(size-1)
// 
// output:
// re(0),re(1),re(2),...,re(size/2),im(size/2-1),...,im(1)
// normalized by array length
//
// Source: 
// Sorensen et al: Real-Valued Fast Fourier Transform Algorithms,
// IEEE Trans. ASSP, ASSP-35, No. 6, June 1987

void realfft_radix2(REAL *data,long n){

    REAL  xt,a,e, t1, t2, cc, ss;
    long  i, j, k, n1, n2, n3, n4, i1, i2, i3, i4;

	n4=n-1;
  //data shuffling
      for (i=0,j=0,n2=n/2; i<n4 ; i++){
	  if (i<j){
				xt=data[j];
				data[j]=data[i];
				data[i]=xt;
				}
	  k=n2;
	  while (k<=j){
				j-=k;
				k>>=1;	
				}
	  j+=k;
      }
	
/* -------------------- */
    for (i=0; i<n; i += 2)  
      {
	 xt = data[i];
	 data[i] = xt + data[i+1];
	 data[i+1] = xt - data[i+1];
      }
/* ------------------------ */
    n2 = 1;
    for (k=n;k>2;k>>=1){ 
		n4 = n2;
		n2 = n4 << 1;
		n1 = n2 << 1;
		e = 2*pi/(n1);
		for (i=0; i<n; i+=n1){  
			xt = data[i];
			data[i] = xt + data[i+n2];
			data[i+n2] = xt-data[i+n2];
			data[i+n4+n2] = -data[i+n4+n2];
			a = e;
			n3=n4-1;
			for (j = 1; j <=n3; j++){
				i1 = i+j;
				i2 = i - j + n2;
				i3 = i1 + n2;
				i4 = i - j + n1;
				cc = cos(a);
				ss = sin(a);
				a += e;
				t1 = data[i3] * cc + data[i4] * ss;
				t2 = data[i3] * ss - data[i4] * cc;
				data[i4] = data[i2] - t2;
				data[i3] = -data[i2] - t2;
				data[i2] = data[i1] - t1;
				data[i1] += t1;
		  }
	  }
  }
	
	//division with array length
   for(i=0;i<n;i++) data[i]/=n;
}


/////////////////////////////////////////////////////////
// Sorensen in-place split-radix FFT for real values
// data: array of doubles:
// re(0),re(1),re(2),...,re(size-1)
// 
// output:
// re(0),re(size/2),re(1),im(1),re(2),im(2),...,re(size/2-1),im(size/2-1)
// normalized by array length
//
// Source: 
// Source: see the routines it calls ...

void realfft_radix2_unshuffled(REAL *data,long n){
	
	REAL *data2;
	long i,j;

	//unshuffling - not in-place
	realfft_radix2(data,n);
	data2=(REAL *)malloc(n*sizeof(double));
	j=n/2;
	data2[0]=data[0];
	data2[1]=data[j];
	for(i=1;i<j;i++) {data2[i+i]=data[i];data2[i+i+1]=data[n-i];}
	for(i=0;i<n;i++) data[i]=data2[i];
	free(data2);
}



WFFT::WFFT()
{
	c_nFFTPoints = 0;		// number of fft points
	c_SqrtFFTPoints = 0;			// sqrt of FFT points
	c_nHarmonicNumber = 0;	// number of harmonics

	c_nFFTWindow = FFT_WINDOW_RECTANGULAR;

	c_nSampleRate = 0;		// samplerate
	c_nChannel = 0;		// number of channels ( 1, 2 )
	c_nBitPerSample = 0;	// bits per sample ( 8, 16, 32 )

	c_window = 0;					// window
	c_harmonics = 0;				// frequency of each harmonic
	c_samplesl = 0;				// samples
	c_samplesr = 0;				// samples
	c_amplitude = 0;				// amplitude of harmonics
	c_phase = 0;					// phase of harmonic
	c_bit_reversal = 0;			// working area for bit reversal
	c_cos_sin_table = 0;			// cos/sin table
}


WFFT::~WFFT()
{
	_free();
}


int WFFT::Initialize(unsigned int nFFTPoints, unsigned int nSampleRate, unsigned int nChannel, unsigned int nBitPerSample)
{
	// check if nFFTPoints is power of 2
	if(nFFTPoints == 0 || (nFFTPoints & (nFFTPoints - 1)))
		return 0;	// nFFTPoints is not power of 2


	if(nSampleRate == 0)
		return 0;	// invalid sample rate

	if(nChannel < 1 || nChannel > 2)
		return 0;	// invalid channel number

	if(nBitPerSample != 8 && nBitPerSample != 16 && nBitPerSample != 32)
		return 0;	// invalid bit per sample

	// free if needed
	Free();

	c_nFFTPoints = nFFTPoints;
	c_SqrtFFTPoints = (REAL) sqrt((double) nFFTPoints);
	c_nHarmonicNumber = nFFTPoints / 2 + 1;

	if(_alloc() == 0)
	{
		c_nFFTPoints = 0;
		c_SqrtFFTPoints = 0;
		c_nHarmonicNumber = 0;
		return 0;	// memory allocation fail
	}

	c_nSampleRate = nSampleRate;
	c_nChannel = nChannel;
	c_nBitPerSample = nBitPerSample;

	// calculate harmonic frequencies
	unsigned int i;
	REAL fFirstHarmonic = (REAL) c_nSampleRate / (REAL) c_nFFTPoints; 
	c_harmonics[0] = 0;
	for(i = 1; i < c_nHarmonicNumber; i++)
		c_harmonics[i] =  ( fFirstHarmonic + fFirstHarmonic * (REAL) (i - 1));


	return 1;
}


unsigned int WFFT::GetNumberOfHarmonics()
{
	return c_nHarmonicNumber;
}

int WFFT::GetHarmonicFreqReal(REAL *pHarmonicFreq)
{
	if(pHarmonicFreq == NULL)
		return c_nHarmonicNumber;

	unsigned int i;
	for(i = 0; i < c_nHarmonicNumber; i++)
		pHarmonicFreq[i] = c_harmonics[i];
		
	return 	c_nHarmonicNumber;
}



int WFFT::GetHarmonicFreqInt(int *pHarmonicFreq)
{
	if(pHarmonicFreq == NULL)
		return c_nHarmonicNumber;

	unsigned int i;
	for(i = 0; i < c_nHarmonicNumber; i++)
		pHarmonicFreq[i] = (int) c_harmonics[i];
		
	return 	c_nHarmonicNumber;

}

int WFFT::GetWindow()
{
	return c_nFFTWindow;
}


int WFFT::SetWindow(unsigned int nWindow)
{
	if(c_nFFTPoints == 0 || c_window == 0)
		return 0;	// not initialized


	unsigned int i;
	double n;
	double N = (double) c_nFFTPoints;

	c_nFFTWindow = nWindow;

	for(i = 0; i < c_nFFTPoints; i++)
	{
		n = (double) i;

		switch(nWindow)
		{
			default:
			case FFT_WINDOW_RECTANGULAR: // Rectangular window
				c_window[i] = 1.0;
			break;

			case FFT_WINDOW_HAMMING: // Hamming window
				c_window[i] = (REAL) ( 0.53836 - 0.46164 * cos( (PI * 2.0 * n) /  (N - 1.0)));
			break;

			case FFT_WINDOW_HANN: // Hann window 
				c_window[i] = (REAL) ( 0.5 * (1 - cos((2.0 * PI * n) /  (N - 1.0))) );
			break;

			case FFT_WINDOW_COSINE: // Cosine window 
				c_window[i] = (REAL) ( sin( (PI * n) / (N - 1.0)) ); 
			break;

			case FFT_WINDOW_LANCZOS: // Lanczos window
			{
				double x = (( 2.0 * n / (N - 1.0)) - 1.0) * PI;
				c_window[i] = (REAL) ( sin(x) / x ); 
			}
			break;

			case FFT_WINDOW_BARTLETT: // Bartlett window (zero valued end-points)
			{
				double x =  N - 1.0 ;
				c_window[i] = (REAL) ( (2.0 / x) * ( x / 2.0 -  fabs(n - (x / 2.0))));
			}
			break;

			case FFT_WINDOW_TRIANGULAR: // Triangular window (non-zero end-points)
				c_window[i] = (REAL) ( ( 2.0 / N ) * ( ( N / 2.0) - fabs(n - ((N - 1.0) / 2.0  ) )  ) );
			break;

			case FFT_WINDOW_GAUSS: // Gauss windows
			{
				double q = 0.3;
				double exponent = -0.5 * pow(  (n -  (N - 1.0) / 2.0  ) / ( q *  (N - 1.0) / 2.0)  , 2);
				c_window[i] = (REAL) exp(exponent);
			}
			break;

			case FFT_WINDOW_BARTLETT_HANN: // Bartlett–Hann window
			{
				double a0 = 0.62;
				double a1 = 0.48;
				double a2 = 0.38;
				c_window[i] = (REAL) ( a0 - a1 * fabs(n / (N - 1.0) - 0.5) - a2 * cos( (2 * PI * n) / ( N - 1)) );
			}
			break;

			case FFT_WINDOW_BLACKMAN: // Blackman windows
			{
				double alpha = 0.16;
				double a0 = (1 - alpha) / 2.0;
				double a1 = 0.5;
				double a2 = alpha / 2.0;
				c_window[i] = (REAL) ( a0 - a1 * cos( (2 * PI * n) / (N - 1.0)) + a2 * cos(( 4 * PI * n) / (N - 1.0)));
			}
			break;

			case FFT_WINDOW_NUTTALL: // Nuttall window, continuous first derivative 
			{
				double a0 = 0.355768;
				double a1 = 0.487396;
				double a2 = 0.144232;
				double a3 = 0.012604;
				double x = N - 1.0;
				c_window[i] = (REAL) ( a0 - a1 * cos(( 2 * PI * n) / x) + a2 * cos((4 * PI * n) / x) - a3 * cos((6 * PI * n) / x) );
			}
			break;

			case FFT_WINDOW_BLACKMAN_HARRIS: // Blackman–Harris window
			{
				double a0 = 0.35875;
				double a1 = 0.48829;
				double a2 = 0.14128;
				double a3 = 0.01168;
				double x = N - 1.0;
				c_window[i] = (REAL) ( a0 - a1 * cos(( 2 * PI * n) / x) + a2 * cos((4 * PI * n) / x) - a3 * cos((6 * PI * n) / x) );
			}
			break;

			case FFT_WINDOW_BLACKMAN_NUTTALL: // Blackman–Nuttall window
			{
				double a0 = 0.3635819;
				double a1 = 0.4891755;
				double a2 = 0.1365995;
				double a3 = 0.0106411;
				double x = N - 1.0;
				c_window[i] = (REAL) ( a0 - a1 * cos(( 2 * PI * n) / x) + a2 * cos((4 * PI * n) / x) - a3 * cos((6 * PI * n) / x) );
			}
			break;

			case FFT_WINDOW_FLAT_TOP: // Flat top window
			{
				double a0 = 1.0;
				double a1 = 1.93;
				double a2 = 1.29;
				double a3 = 0.388;
				double a4 = 0.032; 
				double x = N - 1.0;
				c_window[i] = (REAL) ( a0 - a1 * cos(( 2 * PI * n) / x) + a2 * cos((4 * PI * n) / x) - a3 * cos((6 * PI * n) / x) +  a4 * cos((8 * PI * n) / x));
			}
			break;
		}
	}

	return 1;
}


int WFFT::Free()
{
	// free memory
	_free();

	c_nFFTPoints = 0;		// number of fft points
	c_SqrtFFTPoints = 0;			// sqrt of FFT points
	c_nHarmonicNumber = 0;	// number of harmonics

	c_nSampleRate = 0;		// samplerate
	c_nChannel = 0;		// number of channels ( 1, 2 )
	c_nBitPerSample = 0;	// bits per sample ( 8, 16, 32 )

	return 1;

}


int WFFT::_alloc()
{
	// before this function set c_nFFTPoints and c_nHarmonicNumber
	ASSERT_W(c_nFFTPoints);
	ASSERT_W(c_nHarmonicNumber);

	// first free all
	_free();

	// now allocate
	c_samplesl = (REAL*) malloc(c_nFFTPoints * sizeof(REAL));
	c_samplesr = (REAL*) malloc(c_nFFTPoints * sizeof(REAL));
	c_window = (REAL*) malloc(c_nFFTPoints * sizeof(REAL));

	c_bit_reversal = (int*) malloc( (2 + (int) sqrt((REAL) c_nFFTPoints / 2.0)) * sizeof(int));
	c_cos_sin_table = (REAL*) malloc(c_nFFTPoints / 2 * sizeof(REAL));

	c_harmonics = (REAL*) malloc(c_nHarmonicNumber * sizeof(REAL));
	c_amplitude = (REAL*) malloc(c_nHarmonicNumber * sizeof(REAL));
	c_phase = (REAL*) malloc(c_nHarmonicNumber * sizeof(REAL));

	// check allocation
	if(c_samplesl == 0 || c_samplesr == 0  || c_window == 0 || c_bit_reversal == 0 || c_cos_sin_table == 0 ||
		c_harmonics == 0 || c_amplitude == 0 || c_phase == 0)
	{
		_free();
		return 0;
	}

	c_bit_reversal[0] = 0;	// force initialization
	c_cos_sin_table[0] = 0;
		
	// set window to 1.0, rectangular window
	unsigned int i;
	for(i = 0; i < c_nFFTPoints; i++)
		c_window[i] = 1.0;	


	return 1;
}

int WFFT::_free()
{

	if(c_window)
	{
		free(c_window);				
		c_window = 0;
	}

	if(c_samplesl)
	{
		free(c_samplesl);				
		c_samplesl = 0;
	}

	if(c_samplesr)
	{
		free(c_samplesr);				
		c_samplesr = 0;
	}

	if(c_amplitude)
	{
		free(c_amplitude);				
		c_amplitude = 0;
	}

	if(c_phase)
	{
		free(c_phase);				
		c_phase = 0;
	}

	if(c_bit_reversal)
	{
		free(c_bit_reversal);				
		c_bit_reversal = 0;
	}

	if(c_cos_sin_table)
	{
		free(c_cos_sin_table);				
		c_cos_sin_table = 0;
	}

	if(c_harmonics)
	{
		free(c_harmonics);				
		c_harmonics = 0;
	}

	return 1;
}

int WFFT::CalculateFFT(void *pSamples, unsigned int nNumberOfSamples)
{
	// load sampes int internal sample buffer and apply FFT window
	unsigned int i;
	unsigned int j;

	if(c_nFFTPoints == 0 || pSamples == 0 || nNumberOfSamples != c_nFFTPoints)
		return 0;
	
	
	if(c_nChannel == 2)
	{
		// get samples and do FFT window
		switch(c_nBitPerSample)
		{
			case 8:
			{
				char *sample = (char*) pSamples;
				for(i = 0, j = 0; i < nNumberOfSamples; i++, j += 2)
				{
					c_samplesl[i] = (REAL) sample[j    ] * c_window[i];
					c_samplesr[i] = (REAL) sample[j + 1] * c_window[i];
				}
			}
			break;

			case 16:
			{
				short *sample = (short*) pSamples;
				for(i = 0, j = 0; i < nNumberOfSamples; i++, j += 2)
				{
					c_samplesl[i] = (REAL) sample[j    ] * c_window[i];
					c_samplesr[i] = (REAL) sample[j + 1] * c_window[i];
				}
			}
			break;

			case 32:
			{
				int *sample = (int*) pSamples;
				for(i = 0, j = 0; i < nNumberOfSamples; i++, j += 2)
				{
					c_samplesl[i] = (REAL) sample[j    ] * c_window[i];
					c_samplesr[i] = (REAL) sample[j + 1] * c_window[i];
				}
			}
			break;
		}


		// calculate fft
		rdft(c_nFFTPoints, 1, c_samplesl, c_bit_reversal, c_cos_sin_table);
		rdft(c_nFFTPoints, 1, c_samplesr, c_bit_reversal, c_cos_sin_table);
		return 1;

	}


// get samples and do FFT window
	switch(c_nBitPerSample)
	{
		case 8:
		{
			char *sample = (char*) pSamples;
			for(i = 0; i < nNumberOfSamples; i++)
				c_samplesl[i] = (REAL) sample[i] * c_window[i];
		}
		break;

		case 16:
		{
			short *sample = (short*) pSamples;
			for(i = 0; i < nNumberOfSamples; i++)
				c_samplesl[i] = (REAL) sample[i] * c_window[i];

		}
		break;

		case 32:
		{
			int *sample = (int*) pSamples;
			for(i = 0; i < nNumberOfSamples; i++)
				c_samplesl[i] = (REAL) sample[i] * c_window[i];
		}
		break;
	}


	// calculate fft
	rdft(c_nFFTPoints, 1, c_samplesl, c_bit_reversal, c_cos_sin_table);

	return 1;

}


void WFFT::_convertFFTtoAmplitudeReal(unsigned int FFTPoints, unsigned int SqrtFFTPoints, REAL *fft, REAL *amplitude)
{
	REAL re;
	REAL im;
	REAL amp;

	re = fft[0];
	if(re < 0)
		re = re * (-1.0);

	amp = re  / SqrtFFTPoints + 1.0;
	amp = 20.0 * log10(amp);
	amplitude[0] =  amp;

	re = fft[1];
	if(re < 0)
		re = re * (-10.);

	amp = re  / SqrtFFTPoints + 1.0;
	amp = 20.0 * log10(amp);
	amplitude[FFTPoints / 2] =  amp;

	unsigned int i;
	for(i = 1; i < FFTPoints / 2; i++)
	{
		re = fft[i * 2] ;
		im = fft[i * 2 + 1];
		amp = 20.0 * log10(sqrt(re * re + im * im) /  SqrtFFTPoints + 1.0);
		amplitude[i] = amp;
	}
}

void WFFT::_convertFFTtoAmplitudeInt(unsigned int FFTPoints, unsigned int SqrtFFTPoints, REAL *fft, int *amplitude)
{
	REAL re;
	REAL im;
	REAL amp;

	re = fft[0];
	if(re < 0)
		re = re * (-1.0);

	amp = re  / SqrtFFTPoints + 1.0;
	amp = 20.0 * log10(amp);
	amplitude[0] =  (int) amp;

	re = fft[1];
	if(re < 0)
		re = re * (-10.);

	amp = re  / SqrtFFTPoints + 1.0;
	amp = 20.0 * log10(amp);
	amplitude[FFTPoints / 2] =  (int) amp;

	unsigned int i;
	for(i = 1; i < FFTPoints / 2; i++)
	{
		re = fft[i * 2] ;
		im = fft[i * 2 + 1];
		amp = 20.0 * log10(sqrt(re * re + im * im) /  SqrtFFTPoints + 1.0);
		amplitude[i] = (int) amp;
	}
}



void WFFT::_convertFFTtoPhaseReal(unsigned int FFTPoints, unsigned int SqrtFFTPoints, REAL *fft, REAL *phase)
{
	REAL re;
	REAL im;

	phase[0] =  0;
	phase[FFTPoints / 2] =  0;

	unsigned int i;
	for(i = 1; i < FFTPoints / 2; i++)
	{
		re = fft[i * 2] ;
		im = fft[i * 2 + 1];
		if(re != 0.0)
		{
			phase[i] = atan(im/re) * 57.295779513082320876798154814105;
			return;
		}
		
		phase[i] = pi / 2.0;
	
	}
}


void WFFT::_convertFFTtoPhaseInt(unsigned int FFTPoints, unsigned int SqrtFFTPoints, REAL *fft, int *phase)
{
	REAL re;
	REAL im;

	phase[0] =  0;
	phase[FFTPoints / 2] =  0;

	unsigned int i;
	for(i = 1; i < FFTPoints / 2; i++)
	{
		re = fft[i * 2] ;
		im = fft[i * 2 + 1];
		if(re != 0.0)
		{
			phase[i] = (int) (atan(im/re) * 57.295779513082320876798154814105);
			return;
		}
		
		phase[i] = 90;
	}
}


int WFFT::GetAmplitudeReal(REAL *pLeftAmplitude, REAL *pRightAmplitude)
{
	if(c_nFFTPoints == 0)
		return 0;

	ASSERT_W(c_samplesl);
	ASSERT_W(c_samplesr);

	if(pLeftAmplitude)
		_convertFFTtoAmplitudeReal(c_nFFTPoints, c_SqrtFFTPoints, c_samplesl, pLeftAmplitude);

	if(pRightAmplitude)
		_convertFFTtoAmplitudeReal(c_nFFTPoints, c_SqrtFFTPoints, c_samplesr, pRightAmplitude);

	return 1;
}


int WFFT::GetAmplitudeInt(int *pLeftAmplitude, int *pRightAmplitude)
{
	if(c_nFFTPoints == 0)
		return 0;

	ASSERT_W(c_samplesl);
	ASSERT_W(c_samplesr);

	if(pLeftAmplitude)
		_convertFFTtoAmplitudeInt(c_nFFTPoints, c_SqrtFFTPoints, c_samplesl, pLeftAmplitude);

	if(pRightAmplitude)
		_convertFFTtoAmplitudeInt(c_nFFTPoints, c_SqrtFFTPoints, c_samplesr, pRightAmplitude);

	return 1;
}

int WFFT::GetPhaseReal(REAL *pLeftPhase, REAL *pRightPhase)
{
	if(c_nFFTPoints == 0)
		return 0;

	ASSERT_W(c_samplesl);
	ASSERT_W(c_samplesr);

	if(pLeftPhase)
		_convertFFTtoPhaseReal(c_nFFTPoints, c_SqrtFFTPoints, c_samplesl, pLeftPhase);

	if(pRightPhase)
		_convertFFTtoPhaseReal(c_nFFTPoints, c_SqrtFFTPoints, c_samplesr, pRightPhase);

	return 1;
}

int WFFT::GetPhaseInt(int *pLeftPhase, int *pRightPhase)
{
	if(c_nFFTPoints == 0)
		return 0;

	ASSERT_W(c_samplesl);
	ASSERT_W(c_samplesr);

	if(pLeftPhase)
		_convertFFTtoPhaseInt(c_nFFTPoints, c_SqrtFFTPoints, c_samplesl, pLeftPhase);

	if(pRightPhase)
		_convertFFTtoPhaseInt(c_nFFTPoints, c_SqrtFFTPoints, c_samplesr, pRightPhase);

	return 1;

}