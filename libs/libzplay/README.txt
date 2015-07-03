===========================================================================================
CONTENTS
===========================================================================================
1.  About
2.  Supported formats
3.  Supported languages
4.  Features
5.  Archive contents
6.  How to build library
7.  Warning
8.  DISCLAIMER
9.  COPYRIGHT
10. MP3 and AAC LICENSE
11. Author


===========================================================================================
1. ABOUT
===========================================================================================
libZPlay ver. 2.02 (Win32)

Version: 2.02
Date: May, 2010. 

This is multimedia library for playing mp3, mp2, mp1, ogg, flac, oga, ac3, aac, wav and pcm files and streams. 

This library integrate all decoders for supported formats. You don't need additional libraries.
Library is using native WINAPI functions from kernel32.dll, user32.dll, gdi32.dll and winmm.dll.
All these libraries are standard libraries on all Microsoft Windows. There is no need for MFC or .NET support.
Just pure WINAPI. 

Library will play music directly to soundcard.
Simple and easy. No need for additional programming.
You can play music using only 3 lines of code (create class, open file and start playing).
Simple, simple, simple ... 


libZPlay library is combination of multiple decoding/encoding libraries.
Read topic 10. COPYRIGHT for more informations.

 
===========================================================================================
2. Supported formats
=========================================================================================== 
 
- MP3  -> mp3, mp2, mp1  MPEG-1, MPEG-2, MPEG 2.5 - Layer I, Layer II, Layer III.
          Read topic 9: MP3 and AAC LICENSE 
 
- OGG  -> ogg  VORBIS OGG  

- AC3  -> A/52 aka. ac3

- AAC  -> HE, LC, MAIN and LTP profile, MPEG2 and MPEG-4 AAC decoder.
          Read topic 9: MP3 and AAC LICENSE

- FLAC -> flac, oga  Native FLAC and OGG flac 
 
- WAVE -> wav  PCM WAVE with RIFF header, uncompressed, 8, 16 bit per sample  

- PCM  -> any file  Raw PCM audio format, uncomressed, 8 16 bit per sample  

 
===========================================================================================
3. Supported languages
===========================================================================================

Binary distribution is customized for these languages:

- Standard C/C++ (native) 
- Visual C++ .NET (VC.NET wrapper class) 
- Visual Basic .NET (VB.NET wrapper class) 
- C# .NET (C#.NET wrapper class) 
- Delphi (Delphi wrapper class) 
- and any other language which can use exported __stdcall functions from dll library 
 

libZPlay library is written in C++ and C++ interface is native interface for this library.
But there are wrapper classes for Visual C++ .NET, Visual Basic .NET, C# .NET and Delphi.  
 
===========================================================================================
4. Features
===========================================================================================

- supports mp3, ogg, flac, ac3, aac, wav, pcm 
- supports wave-in input (get samples from line-in, microphone, ....)
- supports encoding samples into mp3, ogg, flac, wave and aac streams
- UNICODE support for functions using strings ( filename, error messages, ID3 info, ... ) 
- read info tags from file ( ID3v1, ID3v2, VORBIS comment, wave RIFF list, FLAC VORBIS comment, embeded picture ) 
- easy drawing embeded picture to screen
- REVERSE PLAYING (you can play song backward in real time, "real" backward playing without predecoding in PCM) 
- built in equaliter, user can specify own subbands, add subbands, ... 
- built in echo and reverb sound processor 
- pitch/rate/tempo change 
- stereo cut effect 
- detect beat-per-minute using 2 methods ( peak detection and autocorrelation) 
- loop playing (you can play just one portion of file, repeat this portion ... ) 
- channel mixer (you can mix left and right chanel, convert stereo to mono ... ) 
- volume slide control (fade-in and fade-out) 
- internal volume control (without affecting wave out device volume) 
- pulling VU meter data (you can simply create VU meter) 
- pulling FFT data (you can simply build FFT spectrum analyzer) 
- built in FFT spectrum graph, library will draw complete spectrum graph on your window 
- callback mechanism (class will inform you about events like play, stop, fade, ...) 
- access to decoded PCM data throug callback message 
- play data from disk files, UNICODE SUPPORT 
- play data from allocated memory 
- play dynamic streams (push small memory chunks into stream while song is playing, for download streams) 
- selecting wave output (for multiple waveout devices) 
- easy play, pause, seek ... 
- class interface for Visual C++ .NET, Visual Basic .NET, C# .NET and Delphi. 
 

===========================================================================================
5. ARCHIVE CONTENTS
===========================================================================================

.\
  decoders\
      a52dec     - liba52 project and source files
      faac       - FAAC library (AAC encoder)
      faad       - FAAD library (AAC decoder)
      jpeglib    - The Independent JPEG Group's JPEG software (IJG) library
      lame       - LAME library (mp3 encoder)
      libFLAC    - libFLAC library (FLAC encoder/decoder)
      libmad     - libmad library (mp3 decoder)
      libogg     - libogg library
      libvorbis  - libvorbis library (vorbis encoder/decoder)
      lpng       - libpng library (png picture decoder)
      soundtouch - SoundTouch library
      zlib       - zlib library

  doc_src        - documentation source code, need Doc-O-Matic to build documentation
  example        - C++ examples
  image          - images

  include        - libZPlay include file for C++
  license        - license informations for encoder/decoder libraries
  src            - libZPlay source files
  vc++           - libZPlay VC++ project and solution files

===========================================================================================
6. HOW TO BUILD LIBRARY
===========================================================================================
Library is written in C++.
To build this library you need MSVC++ 6.0 + SP6 + Processor Pack or later.
Binary distribution is compiled with Visual C++ 2005 Express Edition.

There are 2 types of libZPlay library:
- full version
- PF (patent free) version, without mp3 and aac support

Open .\vc++\libzplay.dsw or .\vc++\libzplay.sln and build libzplay_dynamic or libzplay_pf_dynamic.


===========================================================================================
7. WARNING 
===========================================================================================
Documentation for VB.NET, Delphi and C# was generated from C++ documentation.
Function declarations are tested and there is all OK. But comments,
examples and parametes explanations can contain C++ syntax code or some other strange
expressions because I didn't read all this.
I just adapted function declarations.  
 

===========================================================================================
8. DISCLAIMER
=========================================================================================== 
This library is distributed as freeware.
This software is provided "as is", without any guarantee made as to its suitability or
fitness for any particular use.
It may contain bugs, so use of this tool is at your own risk.
Author takes no responsibility for any damage that may unintentionally be caused through its use.  
 
===========================================================================================
9. COPYRIGHT 
===========================================================================================
libZPlay is released under GNU GPL license. 

NOTE: Read topic 11: MP3 and AAC license.

 *
 * Supported by:
 *
 * ============================================================================
 * libmad - MPEG audio decoder library
 * Copyright (C) 2000-2004 Underbit Technologies, Inc. <support@underbit.com>
 * http://www.underbit.com/products/mad/
 * GNU General Public License ( GPL.TXT )
 * ============================================================================
 * THE OggVorbis SOURCE CODE IS (C) COPYRIGHT 1994-2002 
 * by the Xiph.Org Foundation http://www.xiph.org/
 * BSD-STYLE SOURCE LICENSE  ( XIPH.TXT )
 * ============================================================================
 * LIBA52
 * free library for decoding ATSC A/52 streams.
 * It is released under the terms of the GPL license.
 * ============================================================================
 * FLAC - Free Lossless Audio Codec
 * Copyright (C) 2001,2002,2003,2004,2005,2006,2007  Josh Coalson
 * http://flac.sourceforge.net/
 *
 * This file is part the FLAC project.  FLAC is comprised of several
 * components distributed under difference licenses.  The codec libraries
 * are distributed under Xiph.Org's BSD-like license (see the file
 * XIPH.TXT in this distribution).  All other programs, libraries, and
 * plugins are distributed under the LGPL or GPL (see LGPL.TXT and
 * GPL.TXT).  The documentation is distributed under the Gnu FDL (see
 * FDL.TXT).  Each file in the FLAC distribution contains at the top the
 * terms under which it may be distributed.
 *
 * Since this particular file is relevant to all components of FLAC,
 * it may be distributed under the Xiph.Org license, which is the least
 * restrictive of those mentioned above.  See the file XIPH.TXT in this
 * distribution.
 * ============================================================================
 * FAAD2 - Freeware Advanced Audio (AAC) Decoder including SBR decoding
 * Copyright (C) 2003-2005 M. Bakker, Nero AG, http://www.nero.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * Any non-GPL usage of this software or parts of this software is strictly
 * forbidden.
 *
 * The "appropriate copyright message" mentioned in section 2c of the GPLv2
 * must read: "Code from FAAD2 is copyright (c) Nero AG, www.nero.com"
 *
 * Commercial non-GPL licensing of this software is possible.
 * For more info contact Nero AG through Mpeg4AAClicense@nero.com.
 *
 * ============================================================================
 * FAAC - Freeware Advanced Audio Coder Copyright (C) 2001 M. Bakker
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version. 
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details. 
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
 *
 * ============================================================================
 * libmp3lame encoder
 * LAME 3.xx LAME Ain't an MP3 Encoder http://www.mp3dev.org March 2001 
 * 
 * Originally developed by Mike Cheng (www.uq.net.au/~zzmcheng).
 * Now maintained by Mark Taylor (www.mp3dev.org). 
 *
 * This code is distributed under the GNU LESSER PUBLIC LICENSE (LGPL, see www.gnu.org)
 * with the following modification: 
 * 
 * If you determine that distribution of LAME requires a patent license,
 * and you obtain a patent license, you may distribute LAME even though redistribution
 * of LAME may also require a patent license. 
 *
 * You agree not to enforce any patent claims for any aspect of MPEG audio compression,
 * or any other techniques contained in the LAME source code. 
 *
 * Copyrights (c) 1999-1007 by The LAME Project Copyrights (c) 1999,2000,2001
 * by Mark Taylor Copyrights (c) 1999,2000,2001 by Mark Taylor Copyrights
 * (c) 1998 by Michael Cheng Copyrights (c) 1995,1996,1997 by Michael Hipp: mpglib
 *
 * ============================================================================
 * SoundTouch library Copyright (c) Olli Parviainen 2002-2009 
 * http://www.surina.net/soundtouch
 * GNU Lesser General Public License vesrion 2.1 (LGPL.TXT )
 * ============================================================================
 * Center Cut DSP Plugin for Winamp 2.x 
 * Copyright (C) 2004  Moitah (moitah@yahoo.com)
 * GNU General Public License ( GPL.TXT )
 * ============================================================================
 * Shibatch Super Equalizer ver 0.03 for winamp
 * written by Naoki Shibata  shibatch@users.sourceforge.net
 * http://shibatch.sourceforge.net/
 *
 * Shibatch Super Equalizer (except FFT part) is distributed under LGPL.
 * See LGPL.txt for details.
 * FFT part is a routine made by Mr.Ooura. This routine is a freeware.
 * Contact Mr.Ooura for details of distributing licenses.
 * http://momonga.t.u-tokyo.ac.jp/~ooura/fft.html
 * ============================================================================
 * The Independent JPEG Group's JPEG software
 * JPEG decoding library
 * Copyright (C) 1991-2010, Thomas G. Lane, Guido Vollbeding.
 * www.ijg.org
 * ============================================================================
 * ZLIB DATA COMPRESSION LIBRARY
 * (C) 1995-2010 Jean-loup Gailly and Mark Adler
 * http://zlib.net/ 
 * ============================================================================
 * libpng library
 * PNG picture decoding library
 * Copyright (c) 2004, 2006-2007 Glenn Randers-Pehrson
 * http://www.libpng.org/
 * ============================================================================
 * libzplay documentation is released under GNU Free Documentation License.
 * Read FDL.txt
 *
 *
 * ==================================================================================


Read ./license directory for more informations.

===========================================================================================
10. MP3 and AAC license
===========================================================================================
MP3 and AAC are closed formats and protected by patents.
libZPlay library IS NOT end-user decoder product. But when you use libZPlay
library to build and sold applications based on libZPlay as end-user products,
you need to read this:

For information regarding patent licensing for AAC, please see Via Licensing's website:
http://www.vialicensing.com/

For information regarding patent licensing for MP3, please see Mp3Licensing's website:
http://mp3licensing.com/



If you don't need MP3 and AAC format support, you can use PF (patent free) version
from .\binary\pf


===========================================================================================
11. AUTHOR
===========================================================================================
Zoran Cindori 

Email: zcindori@inet.hr
       zcindori@users.sourceforge.net 

Web: http://libzplay.sourceforge.net/ 





