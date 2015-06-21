 
12 Jun 2015
====================================================================================================================== 	
 url: http://www.npcglib.org/~stathis/blog/precompiled-openssl 	
====================================================================================================================== 
These are the pre-built OpenSSL Libraries v1.0.2c. They are compiled with Cygwin/MSVC  
for 32/64-bit Windows, using Visual Studio 2013. 
 
------------------------------------------------------------------------------ 
32-bit OpenSSL Libraries [shared] [runtime: dynamic (/MD)]]
------------------------------------------------------------------------------ 
release runtime dlls: bin\libeay32MD.dll bin\ssleay32MD.dll 
 release import libs: lib\libeay32MD.lib lib\ssleay32MD.lib 	
  debug runtime dlls: bin\libeay32MDd.dll bin\ssleay32MDd.dll 
   debug import libs: lib\libeay32MDd.lib lib\ssleay32MDd.lib 
 
------------------------------------------------------------------------------ 
32-bit OpenSSL Libraries [static] [runtime: static (/MT)]]
------------------------------------------------------------------------------ 
        release libs: lib\libeay32MT.lib lib\ssleay32MT.lib 	
          debug libs: lib\libeay32MTd.lib lib\ssleay32MTd.lib 	
 
------------------------------------------------------------------------------ 
64-bit OpenSSL Libraries [shared] [runtime: dynamic (/MD)]]
------------------------------------------------------------------------------ 
release runtime dlls: bin64\libeay32MD.dll bin64\ssleay32MD.dll 
 release import libs: lib64\libeay32MD.lib lib64\ssleay32MD.lib 	
  debug runtime dlls: bin64\libeay32MDd.dll bin64\ssleay32MDd.dll 
   debug import libs: lib64\libeay32MDd.lib lib64\ssleay32MDd.lib 
 
------------------------------------------------------------------------------ 
64-bit OpenSSL Libraries [static] [runtime: static (/MT)]]
------------------------------------------------------------------------------ 
        release libs: lib64\libeay32MT.lib lib64\ssleay32MT.lib 	
          debug libs: lib64\libeay32MTd.lib lib64\ssleay32MTd.lib 	
 
====================================================================================================================== 
These prebuild OpenSSL binaries are not well tested and you are using them at your own risk. 
If you have any comments or problems send me an email at: 
stathis <stathis@npcglib.org> 
