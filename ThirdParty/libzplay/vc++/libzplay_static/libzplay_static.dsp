# Microsoft Developer Studio Project File - Name="libzplay_static" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libzplay_static - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libzplay_static.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libzplay_static.mak" CFG="libzplay_static - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libzplay_static - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libzplay_static - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libzplay_static - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
LINK32=link.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "FLAC__NO_DLL" /D "LIB_ZPLAY_STATIC" /YX /FD /c
# ADD BASE RSC /l 0x41a /d "NDEBUG"
# ADD RSC /l 0x41a /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libzplay_static - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
LINK32=link.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "FLAC__NO_DLL" /D "LIB_ZPLAY_STATIC" /YX /FD /GZ /c
# ADD BASE RSC /l 0x41a /d "_DEBUG"
# ADD RSC /l 0x41a /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Release/libzplay_static_d.lib"

!ENDIF 

# Begin Target

# Name "libzplay_static - Win32 Release"
# Name "libzplay_static - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\src\libzplay.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\waacdecoder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\waacencoder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\wac3decoder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\waudioprocessor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\wbmpfont.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\wbpmdetect.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\wbpmdetect1.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\wbpmdetect2.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\wbpmdetect3.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\wcentercutprocessor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\wdecoder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\wechoprocessor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\wencoder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\weqprocessor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\wfft.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\wflacdecoder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\wflacencoder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\winputprocessor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\wmp3decoder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\wmp3encoder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\wmp3x.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\woggdecoder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\woutputprocessor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\wpcmdecoder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\wqueue.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\wspectrum.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\wstprocessor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\wtags.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\wtools.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\wvorbisencoder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\wwavedecoder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\wwaveencoder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\wwavein.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\wwavelet.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\libzplay.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\src\debug.h
# End Source File
# Begin Source File

SOURCE=..\..\src\waacdecoder.h
# End Source File
# Begin Source File

SOURCE=..\..\src\waacencoder.h
# End Source File
# Begin Source File

SOURCE=..\..\src\wac3decoder.h
# End Source File
# Begin Source File

SOURCE=..\..\src\waudioprocessor.h
# End Source File
# Begin Source File

SOURCE=..\..\src\wbmpfont.h
# End Source File
# Begin Source File

SOURCE=..\..\src\wbpmdetect.h
# End Source File
# Begin Source File

SOURCE=..\..\src\wbpmdetect1.h
# End Source File
# Begin Source File

SOURCE=..\..\src\wbpmdetect2.h
# End Source File
# Begin Source File

SOURCE=..\..\src\wbpmdetect3.h
# End Source File
# Begin Source File

SOURCE=..\..\src\wcentercutprocessor.h
# End Source File
# Begin Source File

SOURCE=..\..\src\wdecoder.h
# End Source File
# Begin Source File

SOURCE=..\..\src\wechoprocessor.h
# End Source File
# Begin Source File

SOURCE=..\..\src\wencoder.h
# End Source File
# Begin Source File

SOURCE=..\..\src\weqprocessor.h
# End Source File
# Begin Source File

SOURCE=..\..\src\wfft.h
# End Source File
# Begin Source File

SOURCE=..\..\src\wflacdecoder.h
# End Source File
# Begin Source File

SOURCE=..\..\src\wflacencoder.h
# End Source File
# Begin Source File

SOURCE=..\..\src\winputprocessor.h
# End Source File
# Begin Source File

SOURCE=..\..\src\wmp3decoder.h
# End Source File
# Begin Source File

SOURCE=..\..\src\wmp3encoder.h
# End Source File
# Begin Source File

SOURCE=..\..\src\wmp3x.h
# End Source File
# Begin Source File

SOURCE=..\..\src\woggdecoder.h
# End Source File
# Begin Source File

SOURCE=..\..\src\woutputprocessor.h
# End Source File
# Begin Source File

SOURCE=..\..\src\wpcmdecoder.h
# End Source File
# Begin Source File

SOURCE=..\..\src\wqueue.h
# End Source File
# Begin Source File

SOURCE=..\..\src\wspectrum.h
# End Source File
# Begin Source File

SOURCE=..\..\src\wstprocessor.h
# End Source File
# Begin Source File

SOURCE=..\..\src\wtags.h
# End Source File
# Begin Source File

SOURCE=..\..\src\wtools.h
# End Source File
# Begin Source File

SOURCE=..\..\src\wvorbisencoder.h
# End Source File
# Begin Source File

SOURCE=..\..\src\wwavedecoder.h
# End Source File
# Begin Source File

SOURCE=..\..\src\wwaveencoder.h
# End Source File
# Begin Source File

SOURCE=..\..\src\wwavein.h
# End Source File
# Begin Source File

SOURCE=..\..\src\wwavelet.h
# End Source File
# End Group
# End Target
# End Project
