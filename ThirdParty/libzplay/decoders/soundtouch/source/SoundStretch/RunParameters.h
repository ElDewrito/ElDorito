////////////////////////////////////////////////////////////////////////////////
///
/// A class for parsing the 'soundstretch' application command line parameters
///
/// Author        : Copyright (c) Olli Parviainen
/// Author e-mail : oparviai 'at' iki.fi
/// SoundTouch WWW: http://www.surina.net/soundtouch
///
////////////////////////////////////////////////////////////////////////////////
//
// Last changed  : $Date: 2008-12-25 19:54:41 +0200 (Thu, 25 Dec 2008) $
// File revision : $Revision: 4 $
//
// $Id: RunParameters.h 43 2008-12-25 17:54:41Z oparviai $
//
////////////////////////////////////////////////////////////////////////////////
//
// License :
//
//  SoundTouch audio processing library
//  Copyright (c) Olli Parviainen
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
////////////////////////////////////////////////////////////////////////////////

#ifndef RUNPARAMETERS_H
#define RUNPARAMETERS_H

#include "STTypes.h"
#include <string>

using namespace std;

/// Parses command line parameters into program parameters
class RunParameters
{
private:
    void throwIllegalParamExp(const string &str) const;
    void throwLicense() const;
    void parseSwitchParam(const string &str);
    void checkLimits();
    float parseSwitchValue(const string &str) const;

public:
    char  *inFileName;
    char  *outFileName;
    float tempoDelta;
    float pitchDelta;
    float rateDelta;
    int   quick;
    int   noAntiAlias;
    float goalBPM;
    BOOL  detectBPM;

    RunParameters(const int nParams, const char *paramStr[]);
};

#endif
