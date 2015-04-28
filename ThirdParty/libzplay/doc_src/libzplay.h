
namespace libZPlayCpp {

/// <INCLUDE .\\Cpp\\zplay\\zplay_handle.txt>
#define ZPLAY_HANDLE char*

/// <INCLUDE .\\Cpp\\zplay\\TStreamFormat.txt>
enum TStreamFormat
{
		sfUnknown = 0,
		sfMp3 = 1,
		sfOgg = 2,
		sfWav = 3,
		sfPCM = 4,
		sfFLAC = 5,
		sfFLACOgg = 6,
		sfAC3 = 7,
		sfAacADTS = 8,
		sfWaveIn = 9,
		sfAutodetect = 1000
};


/// <INCLUDE .\\Cpp\\zplay\\WaveOutWaveMapper.txt>
#define WaveOutWaveMapper 0xFFFFFFFF

/// <INCLUDE .\\Cpp\\zplay\\WaveInWaveMapper.txt>
#define WaveInWaveMapper 0xFFFFFFFF

/// <INCLUDE .\\Cpp\\zplay\\TFFTGraphSize.txt>
enum TFFTGraphSize
{
	FFTGraphMinWidth = 100,
	FFTGraphMinHeight = 60
};


/// <INCLUDE .\\Cpp\\zplay\\TBMPDetectionMethod.txt>
enum TBMPDetectionMethod
{
	dmPeaks = 0,
	dmAutoCorrelation = 1
};


/// <INCLUDE .\\Cpp\\zplay\\TFFTGraphHorizontalScale.txt>
enum TFFTGraphHorizontalScale
{
	gsLogarithmic = 0,
	gsLinear = 1
};


/// <INCLUDE .\\Cpp\\zplay\\TSeekMethod.txt>
enum TSeekMethod
{
	smFromBeginning = 1,
	smFromEnd = 2,
	smFromCurrentForward = 4,
	smFromCurrentBackward = 8
};



/// <INCLUDE .\\Cpp\\zplay\\TTimeFormat.txt>
enum TTimeFormat
{
	tfMillisecond = 1,
	tfSecond = 2,
	tfHMS = 4,
	tfSamples = 8
};

/// <INCLUDE .\\Cpp\\zplay\\TStreamHMSTime.txt>
typedef struct {
	unsigned int hour;
	unsigned int minute;
	unsigned int second;
	unsigned int millisecond;
} TStreamHMSTime;


/// <INCLUDE .\\Cpp\\zplay\\TStreamTime.txt>
typedef struct {
	unsigned int sec;
	unsigned int ms;
	unsigned int samples;
	TStreamHMSTime hms;
} TStreamTime;


/// <INCLUDE .\\Cpp\\zplay\\TEchoEffect.txt>
typedef struct {
	int nLeftDelay;	
	int nLeftSrcVolume;	
	int nLeftEchoVolume;
	int nRightDelay;
	int nRightSrcVolume;
	int nRightEchoVolume;
} TEchoEffect;


/// <INCLUDE .\\Cpp\\zplay\\TWaveOutFormat.txt>
enum TWaveOutFormat
{
	format_invalid = 0,
	format_11khz_8bit_mono = 1,
	format_11khz_8bit_stereo = 2,
	format_11khz_16bit_mono = 4,
	format_11khz_16bit_stereo = 8,
	format_22khz_8bit_mono = 16,
	format_22khz_8bit_stereo = 32,
	format_22khz_16bit_mono = 64,
	format_22khz_16bit_stereo = 128,
	format_44khz_8bit_mono = 256,
	format_44khz_8bit_stereo = 512,
	format_44khz_16bit_mono = 1024,
	format_44khz_16bit_stereo = 2048
};


/// <INCLUDE .\\Cpp\\zplay\\TWaveOutFunctionality.txt>
enum TWaveOutFunctionality
{
	supportPitchControl = 1,
	supportPlaybackRateControl = 2,
	supportVolumeControl = 4,
	supportSeparateLeftRightVolume = 8,
	supportSync = 16,
	supportSampleAccuratePosition = 32,
	supportDirectSound = 6
}; 


/// <INCLUDE .\\Cpp\\zplay\\TWaveOutInfo.txt>
typedef struct {
	unsigned int ManufacturerID;
	unsigned int ProductID;
	unsigned int DriverVersion;
	unsigned int Formats;
	unsigned int Channels;
	unsigned int Support;
	char *ProductName;
} TWaveOutInfo;


/// <INCLUDE .\\Cpp\\zplay\\TWaveOutInfoW.txt>
typedef struct {
	unsigned int ManufacturerID;
	unsigned int ProductID;
	unsigned int DriverVersion;
	unsigned int Formats;
	unsigned int Channels;
	unsigned int Support;
	wchar_t *ProductName;
} TWaveOutInfoW;


/// <INCLUDE .\\Cpp\\zplay\\TWaveInInfo.txt>
typedef struct {
	unsigned int ManufacturerID;
	unsigned int ProductID;
	unsigned int DriverVersion;
	unsigned int Formats;
	unsigned int Channels;
	char *ProductName;
} TWaveInInfo;

/// <INCLUDE .\\Cpp\\zplay\\TWaveInInfoW.txt>
typedef struct {
	unsigned int ManufacturerID;
	unsigned int ProductID;
	unsigned int DriverVersion;
	unsigned int Formats;
	unsigned int Channels;
	wchar_t *ProductName;
} TWaveInInfoW;



/// <INCLUDE .\\Cpp\\zplay\\TStreamStatus.txt>
typedef struct {
	int fPlay;
	int fPause;
	int fEcho;
	int fEqualizer;	
	int fVocalCut;
	int fSideCut;
	int fChannelMix;
	int fSlideVolume;
	int nLoop;
	int fReverse;
	int nSongIndex;
	int nSongsInQueue;
} TStreamStatus;





/// <INCLUDE .\\Cpp\\zplay\\TID3Version.txt>
enum TID3Version
{
	id3Version1 = 1,
	id3Version2 = 2
};


/// <INCLUDE .\\Cpp\\zplay\\TID3Picture.txt>
typedef struct {
	unsigned int PicturePresent;
	unsigned int CanDrawPicture;
	char *MIMEType;
	int PictureType;
	char *Description;
	void *PictureData;
	unsigned int PictureDataSize;
	void *hBitmap;
	unsigned int Width;
	unsigned int Height;
	char reserved[128];
} TID3Picture;


/// <INCLUDE .\\Cpp\\zplay\\TID3PictureW.txt>
typedef struct {
	unsigned int PicturePresent;
	unsigned int CanDrawPicture;
	wchar_t *MIMEType;
	int PictureType;
	wchar_t *Description;
	void *PictureData;
	unsigned int PictureDataSize;
	void *hBitmap;
	unsigned int Width;
	unsigned int Height;
	char reserved[128];
} TID3PictureW;


/// <INCLUDE .\\Cpp\\zplay\\TId3InfoEx.txt>
typedef struct {
	char *Title;
	char *Artist;
	char *Album;
	char *Year;
	char *Comment;
	char *TrackNum; 
	char *Genre;
	char *AlbumArtist;
	char *Composer;
	char *OriginalArtist;
	char *Copyright;
	char *URL;
	char *Encoder;
	char *Publisher;
	unsigned int BPM;
	TID3Picture Picture;
	char reserved[128];
} TID3InfoEx;

/// <INCLUDE .\\Cpp\\zplay\\TId3InfoExW.txt>
typedef struct {
	wchar_t *Title;
	wchar_t *Artist;
	wchar_t *Album;
	wchar_t *Year;
	wchar_t *Comment;
	wchar_t *TrackNum; 
	wchar_t *Genre;
	wchar_t *AlbumArtist;
	wchar_t *Composer;
	wchar_t *OriginalArtist;
	wchar_t *Copyright;
	wchar_t *URL;
	wchar_t *Encoder;
	wchar_t *Publisher;
	unsigned int BPM;
	TID3PictureW Picture;
	char reserved[128];
} TID3InfoExW;



/// <INCLUDE .\\Cpp\\zplay\\TId3Info.txt>
typedef struct {
	char *Title;
	char *Artist;
	char *Album;
	char *Year;
	char *Comment;
	char *TrackNum; 
	char *Genre;
} TID3Info;


/// <INCLUDE .\\Cpp\\zplay\\TID3InfoW.txt>
typedef struct {
	wchar_t *Title;
	wchar_t *Artist;
	wchar_t *Album;
	wchar_t *Year;
	wchar_t *Comment;
	wchar_t *TrackNum; 
	wchar_t *Genre;
} TID3InfoW;



/// <INCLUDE .\\Cpp\\zplay\\TStreamInfo.txt>
typedef struct {
	int SamplingRate;
	int ChannelNumber;
	int VBR;
	int Bitrate;
	TStreamTime Length;
	char *Description;
} TStreamInfo;


/// <INCLUDE .\\Cpp\\zplay\\TStreamInfoW.txt>
typedef struct {
	int SamplingRate;
	int ChannelNumber;
	int VBR;
	int Bitrate;
	TStreamTime Length;
	wchar_t *Description;
} TStreamInfoW;


/// <INCLUDE .\\Cpp\\zplay\\TStreamLoadInfo.txt>
typedef struct {
	unsigned int NumberOfBuffers;
	unsigned int NumberOfBytes;
} TStreamLoadInfo;


/// <INCLUDE .\\Cpp\\zplay\\TFFTGraphType.txt>
enum TFFTGraphType
{
	gtLinesLeftOnTop = 0,
	gtLinesRightOnTop,
	gtAreaLeftOnTop,
	gtAreaRightOnTop,
	gtBarsLeftOnTop,
	gtBarsRightOnTop,
	gtSpectrum
};


/// <INCLUDE .\\Cpp\\zplay\\TFFTWindow.txt>
enum TFFTWindow
{
	fwRectangular = 1,
	fwHamming,
	fwHann,
	fwCosine,
	fwLanczos,
	fwBartlett,
	fwTriangular,
	fwGauss,
	fwBartlettHann,
	fwBlackman,
	fwNuttall,
	fwBlackmanHarris,
	fwBlackmanNuttall,
	fwFlatTop
};



/// <INCLUDE .\\Cpp\\zplay\\TFFTGraphParamID.txt>
enum TFFTGraphParamID
{
	gpFFTPoints = 1,
	gpGraphType,
	gpWindow,
	gpHorizontalScale,
	gpSubgrid,
	gpTransparency,
	gpFrequencyScaleVisible,
	gpDecibelScaleVisible,
	gpFrequencyGridVisible,
	gpDecibelGridVisible,
	gpBgBitmapVisible,
	gpBgBitmapHandle,
	gpColor1,
	gpColor2,
	gpColor3,
	gpColor4,
	gpColor5,
	gpColor6,
	gpColor7,
	gpColor8,
	gpColor9,
	gpColor10,
	gpColor11,
	gpColor12,
	gpColor13,
	gpColor14,
	gpColor15,
	gpColor16
};





/// <INCLUDE .\\Cpp\\zplay\\TCallbackMessage.txt>
enum  TCallbackMessage
{

	MsgStopAsync = 1,
	MsgPlayAsync = 2,
	MsgEnterLoopAsync = 4,
	MsgExitLoopAsync = 8,
	MsgEnterVolumeSlideAsync = 16,
	MsgExitVolumeSlideAsync = 32,
	MsgStreamBufferDoneAsync = 64,
	MsgStreamNeedMoreDataAsync = 128,
	MsgNextSongAsync = 256,
	MsgStop = 65536,
	MsgPlay = 131072,
	MsgEnterLoop = 262144,
	MsgExitLoop = 524288,
	MsgEnterVolumeSlide = 1048576,
	MsgExitVolumeSlide = 2097152,
	MsgStreamBufferDone = 4194304,
	MsgStreamNeedMoreData = 8388608,
	MsgNextSong = 16777216,
	MsgWaveBuffer = 33554432
};


/// <INCLUDE .\\Cpp\\zplay\\TCallbackFunc.txt>
typedef int  (__stdcall * TCallbackFunc)(void* instance, void *user_data, TCallbackMessage message, unsigned int param1, unsigned int param2);


/// <INCLUDE .\\Cpp\\zplay\\TSettingID.txt>
enum TSettingID {
	sidWaveBufferSize = 1,
	sidAccurateLength = 2,
	sidAccurateSeek = 3,
	sidSamplerate = 4,
	sidChannelNumber = 5,
	sidBitPerSample = 6,
	sidBigEndian = 7,
	sidWaveInBufferSize = 8
};

// =============================================================================================================
//
//	INTERFACE FOR ZPlay CLASS
//
// =============================================================================================================

/// <INCLUDE .\\Cpp\\zplay\\ZPlay.txt>
class ZPlay
{
	public:


/// <INCLUDE .\\Cpp\\zplay\\SetSettings.txt>
virtual int __stdcall SetSettings(TSettingID nSettingID, int nValue) = 0;
	
/// <INCLUDE .\\Cpp\\zplay\\GetSettings.txt>
virtual int __stdcall GetSettings(TSettingID nSettingID) = 0;

/// <INCLUDE .\\Cpp\\zplay\\GetVersion.txt>
virtual int __stdcall GetVersion() = 0;

/// <INCLUDE .\\Cpp\\zplay\\GetError.txt>
virtual char * __stdcall GetError() = 0;
		
/// <INCLUDE .\\Cpp\\zplay\\GetErrorW.txt>
virtual wchar_t * __stdcall GetErrorW() = 0;

/// <INCLUDE .\\Cpp\\zplay\\GetFileFormat.txt>
virtual TStreamFormat __stdcall GetFileFormat(const char *pchFileName) = 0;
	
/// <INCLUDE .\\Cpp\\zplay\\GetFileFormatW.txt>
virtual TStreamFormat __stdcall GetFileFormatW(const wchar_t *pchFileName) = 0;
	
/// <INCLUDE .\\Cpp\\zplay\\EnumerateWaveOut.txt>
virtual int __stdcall EnumerateWaveOut() = 0;

/// <INCLUDE .\\Cpp\\zplay\\GetWaveOutInfo.txt> 
virtual int __stdcall GetWaveOutInfo(unsigned int nIndex, TWaveOutInfo *pWaveOutInfo) = 0;

/// <INCLUDE .\\Cpp\\zplay\\GetWaveOutInfoW.txt> 
virtual int __stdcall GetWaveOutInfoW(unsigned int nIndex, TWaveOutInfoW *pWaveOutInfo) = 0;

/// <INCLUDE .\\Cpp\\zplay\\SetWaveOutDevice.txt>
virtual int __stdcall SetWaveOutDevice(unsigned int nIndex) = 0;

/// <INCLUDE .\\Cpp\\zplay\\OpenFile.txt>
virtual int __stdcall OpenFile(const char *sFileName, TStreamFormat nFormat)= 0;
		
/// <INCLUDE .\\Cpp\\zplay\\OpenFile.txt>
virtual int __stdcall OpenFileW(const wchar_t *sFileName, TStreamFormat nFormat)= 0;
	
/// <INCLUDE .\\Cpp\\zplay\\AddFile.txt>
virtual int __stdcall AddFile(const char *sFileName, TStreamFormat nFormat)= 0;

/// <INCLUDE .\\Cpp\\zplay\\AddFileW.txt>
virtual int __stdcall AddFileW(const wchar_t *sFileName, TStreamFormat nFormat)= 0;

/// <INCLUDE .\\Cpp\\zplay\\OpenStream.txt>
virtual int __stdcall OpenStream(int fBuffered, int fDynamic, const void *sMemStream, unsigned int nStreamSize, TStreamFormat nFormat) = 0;

/// <INCLUDE .\\Cpp\\zplay\\PushDataToStream.txt>
virtual int __stdcall PushDataToStream(const void *sMemNewData, unsigned int nNewDataize) = 0;

/// <INCLUDE .\\Cpp\\zplay\\IsStreamDataFree.txt>
virtual int __stdcall IsStreamDataFree(const void *sMemNewData) = 0;

/// <INCLUDE .\\Cpp\\zplay\\GetDynamicStreamLoad.txt>
virtual void __stdcall GetDynamicStreamLoad(TStreamLoadInfo *pStreamLoadInfo) = 0;

/// <INCLUDE .\\Cpp\\zplay\\GetStreamInfo.txt>
virtual void __stdcall GetStreamInfo(TStreamInfo *pInfo) = 0;

/// <INCLUDE .\\Cpp\\zplay\\GetStreamInfoW.txt>
virtual void __stdcall GetStreamInfoW(TStreamInfoW *pInfo) = 0;

/// <INCLUDE .\\Cpp\\zplay\\Close.txt>
virtual int __stdcall Close() = 0;

/// <INCLUDE .\\Cpp\\zplay\\Play.txt>
virtual int __stdcall Play() = 0;

/// <INCLUDE .\\Cpp\\zplay\\PlayLoop.txt>
virtual int __stdcall PlayLoop(TTimeFormat fFormatStartTime, TStreamTime *pStartTime, TTimeFormat fFormatEndTime, TStreamTime *pEndTime, unsigned int nNumOfCycles, unsigned int fContinuePlaying) = 0;

/// <INCLUDE .\\Cpp\\zplay\\Seek.txt>
virtual int __stdcall Seek(TTimeFormat fFormat, TStreamTime *pTime, TSeekMethod nMoveMethod) = 0;

/// <INCLUDE .\\Cpp\\zplay\\ReverseMode.txt>
virtual int __stdcall ReverseMode(int fEnable) = 0;

/// <INCLUDE .\\Cpp\\zplay\\Stop.txt>
virtual int __stdcall Stop() = 0;

/// <INCLUDE .\\Cpp\\zplay\\Pause.txt>
virtual int __stdcall Pause() = 0;

/// <INCLUDE .\\Cpp\\zplay\\Resume.txt>
virtual int __stdcall Resume() = 0;

/// <INCLUDE .\\Cpp\\zplay\\GetPosition.txt>
virtual void __stdcall GetPosition(TStreamTime *pTime) = 0;

/// <INCLUDE .\\Cpp\\zplay\\SetMasterVolume.txt>
virtual int __stdcall SetMasterVolume(unsigned int nLeftVolume, unsigned int nRightVolume) = 0;

/// <INCLUDE .\\Cpp\\zplay\\SetPlayerVolume.txt>
virtual int __stdcall SetPlayerVolume(unsigned int nLeftVolume, unsigned int nRightVolume) = 0;

/// <INCLUDE .\\Cpp\\zplay\\GetMasterVolume.txt>
virtual void __stdcall GetMasterVolume(unsigned int *pnLeftVolume, unsigned int *pnRightVolume) = 0;

/// <INCLUDE .\\Cpp\\zplay\\GetPlayerVolume.txt>
virtual void __stdcall GetPlayerVolume(unsigned int *pnLeftVolume,unsigned int *pnRightVolume) = 0;

/// <INCLUDE .\\Cpp\\zplay\\GetBitrate.txt>
virtual int __stdcall GetBitrate(int fAverage) = 0;

/// <INCLUDE .\\Cpp\\zplay\\GetStatus.txt>
virtual void __stdcall GetStatus(TStreamStatus *pStatus) = 0;

/// <INCLUDE .\\Cpp\\zplay\\MixChannels.txt>
virtual int __stdcall MixChannels(int fEnable, unsigned int nLeftPercent, unsigned int nRightPercent) = 0;

/// <INCLUDE .\\Cpp\\zplay\\GetVUData.txt>
virtual void __stdcall GetVUData(unsigned int *pnLeftChannel, unsigned int *pnRightChannel) = 0;

/// <INCLUDE .\\Cpp\\zplay\\SlideVolume.txt>
virtual int __stdcall SlideVolume(int fFormatStart, TStreamTime *pTimeStart, unsigned int nStartVolumeLeft, unsigned int nStartVolumeRight, int fFormatEnd, TStreamTime *pTimeEnd, unsigned int nEndVolumeLeft,unsigned int nEndVolumeRight) = 0;

/// <INCLUDE .\\Cpp\\zplay\\EnableEqualizer.txt>
virtual int __stdcall EnableEqualizer(int fEnable) = 0;

/// <INCLUDE .\\Cpp\\zplay\\SetEqualizerPoints.txt>
virtual int __stdcall SetEqualizerPoints(int *pnFreqPoint, int nNumOfPoints) = 0;

/// <INCLUDE .\\Cpp\\zplay\\GetEqualizerPoints.txt>
virtual int __stdcall GetEqualizerPoints(int *pnFreqPoint, int nFreqPointNumber) = 0;

/// <INCLUDE .\\Cpp\\zplay\\SetEqualizerParam.txt>
virtual int __stdcall SetEqualizerParam(int nPreAmpGain, int *pnBandGain, int nNumberOfBands) = 0;

/// <INCLUDE .\\Cpp\\zplay\\GetEqualizerParam.txt>
virtual int __stdcall GetEqualizerParam(int *pnPreAmpGain, int *pnBandGain,	int nNumberOfBands) = 0;

/// <INCLUDE .\\Cpp\\zplay\\SetEqualizerPreampGain.txt>
virtual int __stdcall SetEqualizerPreampGain(int nGain) = 0;

/// <INCLUDE .\\Cpp\\zplay\\SetEqualizerPreampGain.txt>
virtual int __stdcall GetEqualizerPreampGain() = 0; 

/// <INCLUDE .\\Cpp\\zplay\\SetEqualizerBandGain.txt>
virtual int __stdcall SetEqualizerBandGain(int nBandIndex, int nGain) = 0;

/// <INCLUDE .\\Cpp\\zplay\\GetEqualizerBandGain.txt>
virtual int __stdcall GetEqualizerBandGain(int nBandIndex) = 0;

/// <INCLUDE .\\Cpp\\zplay\\LoadID3.txt>
virtual int __stdcall LoadID3(TID3Version nId3Version, TID3Info *pId3Info) = 0;

/// <INCLUDE .\\Cpp\\zplay\\LoadID3W.txt>
virtual int __stdcall LoadID3W(TID3Version nId3Version,	TID3InfoW *pId3Info) = 0;

/// <INCLUDE .\\Cpp\\zplay\\LoadFileID3.txt>
virtual int __stdcall LoadFileID3(const char *pchFileName, TStreamFormat nFormat, TID3Version nId3Version, TID3Info *pId3Info) = 0;

/// <INCLUDE .\\Cpp\\zplay\\LoadFileID3W.txt>	
virtual int __stdcall LoadFileID3W(const wchar_t *pchFileName, TStreamFormat nFormat, TID3Version nId3Version, TID3InfoW *pId3Info) = 0;

/// <INCLUDE .\\Cpp\\zplay\\EnableEcho.txt>
virtual int __stdcall EnableEcho(int fEnable) = 0;

/// <INCLUDE .\\Cpp\\zplay\\SetEchoParam.txt>
virtual int __stdcall SetEchoParam(TEchoEffect *pEchoEffect, int nNumberOfEffects) = 0;

/// <INCLUDE .\\Cpp\\zplay\\GetEchoParam.txt>
virtual int __stdcall GetEchoParam(TEchoEffect *pEchoEffect, int nNumberOfEffects) = 0;

/// <INCLUDE .\\Cpp\\zplay\\SetCallbackFunc.txt>
virtual int __stdcall SetCallbackFunc(TCallbackFunc pCallbackFunc,TCallbackMessage nMessages, void *user_data) = 0;

/// <INCLUDE .\\Cpp\\zplay\\SetRate.txt>
virtual int __stdcall SetRate(unsigned int nRate)= 0;

/// <INCLUDE .\\Cpp\\zplay\\GetRate.txt>
virtual unsigned int __stdcall GetRate()= 0;

/// <INCLUDE .\\Cpp\\zplay\\SetPitch.txt>
virtual int __stdcall SetPitch(unsigned int nPitch)= 0;

/// <INCLUDE .\\Cpp\\zplay\\GetPitch.txt>
virtual unsigned int __stdcall GetPitch()= 0;

/// <INCLUDE .\\Cpp\\zplay\\SetTempo.txt>
virtual int __stdcall SetTempo(unsigned int nTempo)= 0;

/// <INCLUDE .\\Cpp\\zplay\\GetTempo.txt>
virtual unsigned int __stdcall GetTempo()= 0;

/// <INCLUDE .\\Cpp\\zplay\\StereoCut.txt>
virtual int __stdcall StereoCut(int fEnable, int fOutputCenter, int fBassToSides)= 0;

/// <INCLUDE .\\Cpp\\zplay\\Release.txt>
virtual void __stdcall Release() = 0;

/// <INCLUDE .\\Cpp\\zplay\\DetectBPM.txt>
virtual int __stdcall DetectBPM(TBMPDetectionMethod nMethod) = 0;

/// <INCLUDE .\\Cpp\\zplay\\DetectFileBPM.txt>
virtual int __stdcall DetectFileBPM(const char *pchFileName, TStreamFormat nFormat, TBMPDetectionMethod nMethod)= 0;

/// <INCLUDE .\\Cpp\\zplay\\DetectFileBPMW.txt>
virtual int __stdcall DetectFileBPMW(const wchar_t *pchFileName, TStreamFormat nFormat, TBMPDetectionMethod nMethod)= 0;

/// <INCLUDE .\\Cpp\\zplay\\GetFFTData.txt>
virtual int __stdcall GetFFTData(int nFFTPoints, TFFTWindow nFFTWindow, int *pnHarmonicNumber,int *pnHarmonicFreq,int *pnLeftAmplitude,int *pnRightAmplitude,int *pnLeftPhase,int *pnRightPhase) = 0;

/// <INCLUDE .\\Cpp\\zplay\\DrawFFTGraphOnHDC.txt>
virtual int __stdcall DrawFFTGraphOnHDC(void* hdc, int nX, int nY, int nWidth, int nHeight) = 0;

/// <INCLUDE .\\Cpp\\zplay\\DrawFFTGraphOnHWND.txt>
virtual int __stdcall DrawFFTGraphOnHWND(void* hwnd, int nX, int nY, int nWidth, int nHeight) = 0;

/// <INCLUDE .\\Cpp\\zplay\\SetFFTGraphParam.txt>
virtual int __stdcall SetFFTGraphParam(TFFTGraphParamID nParamID, int nValue) = 0;

/// <INCLUDE .\\Cpp\\zplay\\GetFFTGraphParam.txt>
virtual int __stdcall GetFFTGraphParam(TFFTGraphParamID nParamID) = 0;



/// <INCLUDE .\\Cpp\\zplay\\LoadID3Ex.txt>
virtual int __stdcall LoadID3Ex(TID3InfoEx *pId3Info, unsigned int fDecodeEmbededPicture) = 0;

/// <INCLUDE .\\Cpp\\zplay\\LoadID3ExW.txt>
virtual int __stdcall LoadID3ExW(TID3InfoExW *pId3Info, unsigned int fDecodeEmbededPicture) = 0;

/// <INCLUDE .\\Cpp\\zplay\\LoadFileID3Ex.txt>
virtual int __stdcall LoadFileID3Ex(const char *pchFileName, TStreamFormat nFormat, TID3InfoEx *pId3Info, unsigned int fDecodeEmbededPicture) = 0;

/// <INCLUDE .\\Cpp\\zplay\\LoadFileID3ExW.txt>
virtual int __stdcall LoadFileID3ExW(const wchar_t *pchFileName, TStreamFormat nFormat, TID3InfoExW *pId3Info, unsigned int fDecodeEmbededPicture) = 0;

/// <INCLUDE .\\Cpp\\zplay\\DrawBitmapToHDC.txt>
virtual int __stdcall DrawBitmapToHDC(void *hdc, int X, int Y, int Width, int Height, void *hBitmap) = 0;

/// <INCLUDE .\\Cpp\\zplay\\DrawBitmapToHWND.txt>
virtual int __stdcall DrawBitmapToHWND(void *hwnd, int X, int Y, int Width, int Height, void *hBitmap) = 0;



/// <INCLUDE .\\Cpp\\zplay\\EnumerateWaveIn.txt>
virtual int __stdcall EnumerateWaveIn() = 0;

/// <INCLUDE .\\Cpp\\zplay\\GetWaveInInfo.txt>
virtual int __stdcall GetWaveInInfo(unsigned int nIndex, TWaveInInfo *pWaveInInfo) = 0;

/// <INCLUDE .\\Cpp\\zplay\\GetWaveInInfoW.txt>
virtual int __stdcall GetWaveInInfoW(unsigned int nIndex, TWaveInInfoW *pWaveInInfo) = 0;

/// <INCLUDE .\\Cpp\\zplay\\SetWaveInDevice.txt>
virtual int __stdcall SetWaveInDevice(unsigned int nIndex) = 0;


/// <INCLUDE .\\Cpp\\zplay\\SetWaveOutFile.txt>
virtual int __stdcall SetWaveOutFile(const char *sFileName, TStreamFormat nFormat, unsigned int fOutputToSoundcard)= 0;

/// <INCLUDE .\\Cpp\\zplay\\SetWaveOutFileW.txt>
virtual int __stdcall SetWaveOutFileW(const wchar_t *sFileName, TStreamFormat nFormat, unsigned int fOutputToSoundcard)= 0;



};




// =============================================================================================================
//
//	END OF INTERFACE FOR ZPlay CLASS
//
// =============================================================================================================


#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================================================
//
//	CLASS FACTORY FOR ZPlay INTERFACE
//
// =============================================================================================================


/// <INCLUDE .\\Cpp\\zplay\\CreateZPlay.txt>
__declspec (dllimport)  ZPlay *  __stdcall CreateZPlay(); 


// =============================================================================================================
//
//	END OF CLASS FACTORY FOR ZPLAY INTERFACE
//
// =============================================================================================================


// =============================================================================================================
//
//	C WRAPPER FOR ZPlay CLASS
//
// =============================================================================================================

/// <INCLUDE .\\Cpp\\zplay\\CreateZPlay.txt>
__declspec (dllimport) ZPLAY_HANDLE __stdcall zplay_CreateZPlay(); 

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\Release.txt>
__declspec (dllimport) int __stdcall zplay_DestroyZPlay(ZPLAY_HANDLE handle); 

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\GetVersion.txt>
__declspec (dllimport) int __stdcall zplay_GetVersion(ZPLAY_HANDLE handle);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\GetError.txt>
__declspec (dllimport) char * __stdcall zplay_GetError(ZPLAY_HANDLE handle);			
		
/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\GetErrorW.txt>
__declspec (dllimport) wchar_t * __stdcall zplay_GetErrorW(ZPLAY_HANDLE handle);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\GetFileFormat.txt>
__declspec (dllimport) TStreamFormat __stdcall zplay_GetFileFormat(ZPLAY_HANDLE handle, const char *pchFileName);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\GetFileFormatW.txt>
__declspec (dllimport) TStreamFormat __stdcall zplay_GetFileFormatW(ZPLAY_HANDLE handle, const wchar_t *pchFileName);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\EnumerateWaveOut.txt> 	
__declspec (dllimport) int __stdcall zplay_EnumerateWaveOut(ZPLAY_HANDLE handle);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\GetWaveOutInfo.txt> 
__declspec (dllimport) int __stdcall zplay_GetWaveOutInfo(ZPLAY_HANDLE handle, unsigned int nIndex,	TWaveOutInfo *pWaveOutInfo);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\GetWaveOutInfoW.txt> 	
__declspec (dllimport) int __stdcall zplay_GetWaveOutInfoW(ZPLAY_HANDLE handle,unsigned int nIndex,	TWaveOutInfoW *pWaveOutInfo);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\SetWaveOutDevice.txt>  	
__declspec (dllimport) int __stdcall zplay_SetWaveOutDevice(ZPLAY_HANDLE handle, unsigned int nIndex);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\OpenFile.txt>
__declspec (dllimport) int __stdcall zplay_OpenFile(ZPLAY_HANDLE handle, const char *sFileName, TStreamFormat nFormat);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>		
/// <INCLUDE .\\Cpp\\zplay\\OpenFileW.txt>
__declspec (dllimport) int __stdcall zplay_OpenFileW(ZPLAY_HANDLE handle, const wchar_t *sFileName, TStreamFormat nFormat);
		
/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>		
/// <INCLUDE .\\Cpp\\zplay\\AddFile.txt>
__declspec (dllimport) int __stdcall zplay_AddFile(ZPLAY_HANDLE handle, const char *sFileName, TStreamFormat nFormat);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>		
/// <INCLUDE .\\Cpp\\zplay\\AddFileW.txt>                                  
__declspec (dllimport) int __stdcall zplay_AddFileW(ZPLAY_HANDLE handle, const wchar_t *sFileName, TStreamFormat nFormat);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\OpenStream.txt>
__declspec (dllimport) int __stdcall zplay_OpenStream(ZPLAY_HANDLE handle, int fBuffered, int fManaged, const void *sMemStream, unsigned int nStreamSize, TStreamFormat nFormat);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\PushDataToStream.txt>	
__declspec (dllimport) int __stdcall zplay_PushDataToStream(ZPLAY_HANDLE handle, const void *sMemNewData, unsigned int nNewDataize);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\IsStreamDataFree.txt>	
__declspec (dllimport) int __stdcall zplay_IsStreamDataFree(ZPLAY_HANDLE handle, const void *sMemNewData);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\GetDynamicStreamLoad.txt>   	
__declspec (dllimport) void __stdcall zplay_GetDynamicStreamLoad(ZPLAY_HANDLE handle,TStreamLoadInfo *pStreamLoadInfo);
	     

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\GetStreamInfo.txt>
__declspec (dllimport) void __stdcall zplay_GetStreamInfo(ZPLAY_HANDLE handle,	TStreamInfo *pInfo);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\GetStreamInfoW.txt>
__declspec (dllimport) void __stdcall zplay_GetStreamInfoW(ZPLAY_HANDLE handle, TStreamInfoW *pInfo);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>		
/// <INCLUDE .\\Cpp\\zplay\\Close.txt>
__declspec (dllimport) int __stdcall zplay_Close(ZPLAY_HANDLE handle);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>	
/// <INCLUDE .\\Cpp\\zplay\\Play.txt>
__declspec (dllimport) int __stdcall zplay_Play(ZPLAY_HANDLE handle);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>	
/// <INCLUDE .\\Cpp\\zplay\\PlayLoop.txt>
__declspec (dllimport) int __stdcall zplay_PlayLoop(ZPLAY_HANDLE handle, TTimeFormat fFormatStartTime, TStreamTime *pStartTime, TTimeFormat fFormatEndTime, TStreamTime *pEndTime,	unsigned int nNumOfCycles, unsigned int fContinuePlaying);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>	
/// <INCLUDE .\\Cpp\\zplay\\Seek.txt>
__declspec (dllimport) int __stdcall zplay_Seek(ZPLAY_HANDLE handle, TTimeFormat fFormat, TStreamTime *pTime, TSeekMethod nMoveMethod);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\ReverseMode.txt>
__declspec (dllimport) int __stdcall zplay_ReverseMode(ZPLAY_HANDLE handle, int fEnable);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>	
/// <INCLUDE .\\Cpp\\zplay\\Stop.txt>
__declspec (dllimport) int __stdcall zplay_Stop(ZPLAY_HANDLE handle);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>	
/// <INCLUDE .\\Cpp\\zplay\\Pause.txt>
__declspec (dllimport) int __stdcall zplay_Pause(ZPLAY_HANDLE handle);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>	
/// <INCLUDE .\\Cpp\\zplay\\Resume.txt>
__declspec (dllimport) int __stdcall zplay_Resume(ZPLAY_HANDLE handle);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\GetPosition.txt>
__declspec (dllimport) void __stdcall zplay_GetPosition(ZPLAY_HANDLE handle, TStreamTime *pTime);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\SetMasterVolume.txt>
__declspec (dllimport) int __stdcall zplay_SetMasterVolume(ZPLAY_HANDLE handle, unsigned int nLeftVolume, unsigned int nRightVolume);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\SetPlayerVolume.txt>
__declspec (dllimport) int __stdcall zplay_SetPlayerVolume(ZPLAY_HANDLE handle, unsigned int nLeftVolume, unsigned int nRightVolume);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\GetMasterVolume.txt>
__declspec (dllimport) void __stdcall zplay_GetMasterVolume(ZPLAY_HANDLE handle, unsigned int *pnLeftVolume, unsigned int *pnRightVolume);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\GetPlayerVolume.txt>
__declspec (dllimport) void __stdcall zplay_GetPlayerVolume(ZPLAY_HANDLE handle, unsigned int *pnLeftVolume, unsigned int *pnRightVolume);


/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\GetBitrate.txt>
__declspec (dllimport) int __stdcall zplay_GetBitrate(ZPLAY_HANDLE handle, int fAverage);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\GetStatus.txt>
__declspec (dllimport) void __stdcall zplay_GetStatus(ZPLAY_HANDLE handle, TStreamStatus *pStatus);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\MixChannels.txt>	
__declspec (dllimport) int __stdcall zplay_MixChannels(ZPLAY_HANDLE handle, int fEnable, unsigned int nLeftPercent, unsigned int nRightPercent);
		
/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\GetVUData.txt>
__declspec (dllimport) void __stdcall zplay_GetVUData(ZPLAY_HANDLE handle, unsigned int *pnLeftChannel,unsigned int *pnRightChannel);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\SlideVolume.txt>
__declspec (dllimport) int __stdcall zplay_SlideVolume(ZPLAY_HANDLE handle, int fFormatStart, TStreamTime *pTimeStart, unsigned int nStartVolumeLeft, unsigned int nStartVolumeRight, int fFormatEnd, TStreamTime *pTimeEnd, unsigned int nEndVolumeLeft, unsigned int nEndVolumeRight);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\EnableEqualizer.txt>
__declspec (dllimport) int __stdcall zplay_EnableEqualizer(ZPLAY_HANDLE handle, int fEnable);


/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\SetEqualizerPoints.txt>
__declspec (dllimport) int __stdcall zplay_SetEqualizerPoints(ZPLAY_HANDLE handle, int *pnFreqPoint, int nNumOfPoints);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\GetEqualizerPoints.txt>
__declspec (dllimport) int __stdcall zplay_GetEqualizerPoints(ZPLAY_HANDLE handle, int *pnFreqPoint, int nFreqPointNumber);


/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\SetEqualizerParam.txt>
__declspec (dllimport) int __stdcall zplay_SetEqualizerParam(ZPLAY_HANDLE handle, int nPreAmpGain, int *pnBandGain,	int nNumberOfBands);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\GetEqualizerParam.txt>
__declspec (dllimport) int __stdcall zplay_GetEqualizerParam(ZPLAY_HANDLE handle, int *pnPreAmpGain, int *pnBandGain, int nNumberOfBands);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\LoadID3.txt>	
__declspec (dllimport) int __stdcall zplay_LoadID3(ZPLAY_HANDLE handle, TID3Version nId3Version, TID3Info *pId3Info);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\LoadID3W.txt>
__declspec (dllimport) int __stdcall zplay_LoadID3W(ZPLAY_HANDLE handle, TID3Version nId3Version, TID3InfoW *pId3Info);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\LoadFileID3.txt>
__declspec (dllimport) int __stdcall zplay_LoadFileID3(ZPLAY_HANDLE handle, const char *pchFileName, TStreamFormat nFormat, TID3Version nId3Version, TID3Info *pId3Info);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\LoadFileID3.txt>
__declspec (dllimport) int __stdcall zplay_LoadFileID3W(ZPLAY_HANDLE handle, const wchar_t *pchFileName, TStreamFormat nFormat, TID3Version nId3Version, TID3InfoW *pId3Info);




// =============================================================================================================
//	ECHO
// =============================================================================================================


/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\EnableEcho.txt>
__declspec (dllimport) int __stdcall zplay_EnableEcho(ZPLAY_HANDLE handle,int fEnable);


/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\SetEchoParam.txt>
__declspec (dllimport) int __stdcall zplay_SetEchoParam(ZPLAY_HANDLE handle, TEchoEffect *pEchoEffect, int nNumberOfEffects);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\GetEchoParam.txt>
__declspec (dllimport) int __stdcall zplay_GetEchoParam(ZPLAY_HANDLE handle, TEchoEffect *pEchoEffectm, int nNumberOfEffects);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\SetCallbackFunc.txt> 	
__declspec (dllimport) int __stdcall zplay_SetCallbackFunc(ZPLAY_HANDLE handle, TCallbackFunc pCallbackFunc, TCallbackMessage nMessages, void *user_data);


/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\GetFFTData.txt>
__declspec (dllimport) int __stdcall zplay_GetFFTData(
			ZPLAY_HANDLE handle,
			int nFFTPoints,
			TFFTWindow nFFTWindow,
			int *pnHarmonicNumber,
			int *pnHarmonicFreq,
			int *pnLeftAmplitude,
			int *pnRightAmplitude,
			int *pnLeftPhase,
			int *pnRightPhase
		);

    	
/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\SetRate.txt>
__declspec (dllimport) int __stdcall zplay_SetRate(ZPLAY_HANDLE handle, unsigned int nRate);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\GetRate.txt>
__declspec (dllimport) unsigned int __stdcall zplay_GetRate(ZPLAY_HANDLE handle);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\SetPitch.txt>
__declspec (dllimport) int __stdcall zplay_SetPitch(ZPLAY_HANDLE handle, unsigned int nPitch);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\GetPitch.txt>
__declspec (dllimport) unsigned int __stdcall zplay_GetPitch(ZPLAY_HANDLE handle);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\SetTempo.txt>
__declspec (dllimport) int __stdcall zplay_SetTempo(ZPLAY_HANDLE handle, unsigned int nTempo);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\GetTempo.txt>
__declspec (dllimport) unsigned int __stdcall zplay_GetTempo(ZPLAY_HANDLE handle);


/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\StereoCut.txt>
__declspec (dllimport) int __stdcall zplay_StereoCut(ZPLAY_HANDLE handle, int fEnable, int fOutputCenter, int fBassToSides);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\DrawFFTGraphOnHDC.txt>
__declspec (dllimport) int __stdcall zplay_DrawFFTGraphOnHDC(ZPLAY_HANDLE handle, void* hdc, int nX, int nY, int nWidth, int nHeight);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\DrawFFTGraphOnHWND.txt>
__declspec (dllimport) int __stdcall zplay_DrawFFTGraphOnHWND(ZPLAY_HANDLE handle, void* hwnd, int nX, int nY, int nWidth, int nHeight);



/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\SetFFTGraphParam.txt>
__declspec (dllimport) int __stdcall zplay_SetFFTGraphParam(ZPLAY_HANDLE handle, TFFTGraphParamID nParamID, int nParamValue);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\GetFFTGraphParam.txt>
__declspec (dllimport) int __stdcall zplay_GetFFTGraphParam(ZPLAY_HANDLE handle, TFFTGraphParamID nParamID);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\DetectBPM.txt>
__declspec (dllimport) int __stdcall zplay_DetectBPM(ZPLAY_HANDLE handle, TBMPDetectionMethod nMethod);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\DetectFileBPM.txt>
__declspec (dllimport) int __stdcall zplay_DetectFileBPM(ZPLAY_HANDLE handle, const char *pchFileName, TStreamFormat nFormat, TBMPDetectionMethod nMethod);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\DetectFileBPMW.txt>
__declspec (dllimport) int __stdcall zplay_DetectFileBPMW(ZPLAY_HANDLE handle, const wchar_t *pchFileName, TStreamFormat nFormat, TBMPDetectionMethod nMethod);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\SetEqualizerPreampGain.txt>
__declspec (dllimport) int __stdcall zplay_SetEqualizerPreampGain(ZPLAY_HANDLE handle, int nGain);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\GetEqualizerPreampGain.txt>
__declspec (dllimport) int __stdcall zplay_GetEqualizerPreampGain(ZPLAY_HANDLE handle);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\SetEqualizerBandGain.txt>
__declspec (dllimport) int __stdcall zplay_SetEqualizerBandGain(ZPLAY_HANDLE handle, int nBandIndex, int nGain);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\GetEqualizerBandGain.txt>
__declspec (dllimport) int __stdcall zplay_GetEqualizerBandGain(ZPLAY_HANDLE handle, int nBandIndex);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\SetSettings.txt>
__declspec (dllimport) int __stdcall zplay_SetSettings(ZPLAY_HANDLE handle, TSettingID nSettingID, int nValue);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\GetSettings.txt>
__declspec (dllimport) int __stdcall zplay_GetSettings(ZPLAY_HANDLE handle, TSettingID nSettingID);


/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\LoadID3Ex.txt>
W_DECLSPEC int __stdcall zplay_LoadID3Ex(ZPLAY_HANDLE handle, TID3InfoEx *pId3Info, unsigned int fDecodeEmbededPicture);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\LoadID3ExW.txt>
W_DECLSPEC int __stdcall zplay_LoadID3ExW(ZPLAY_HANDLE handle, TID3InfoExW *pId3Info, unsigned int fDecodeEmbededPicture);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\LoadFileID3Ex.txt>
W_DECLSPEC int __stdcall zplay_LoadFileID3Ex(ZPLAY_HANDLE handle, const char *pchFileName, TStreamFormat nFormat, TID3InfoEx *pId3Info, unsigned int fDecodeEmbededPicture);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\LoadFileID3ExW.txt>
W_DECLSPEC int __stdcall zplay_LoadFileID3ExW(ZPLAY_HANDLE handle, const wchar_t *pchFileName, TStreamFormat nFormat, TID3InfoExW *pId3Info, unsigned int fDecodeEmbededPicture);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\DrawBitmapToHDC.txt>
W_DECLSPEC int __stdcall zplay_DrawBitmapToHDC(ZPLAY_HANDLE handle, void *hdc, int X, int Y, int Width, int Height, void *hBitmap);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\DrawBitmapToHWND.txt>
W_DECLSPEC int __stdcall zplay_DrawBitmapToHWND(ZPLAY_HANDLE handle, void *hwnd, int X, int Y, int Width, int Height, void *hBitmap);


/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\EnumerateWaveIn.txt>
W_DECLSPEC  int __stdcall zplay_EnumerateWaveIn(ZPLAY_HANDLE handle);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\GetWaveInInfo.txt>
W_DECLSPEC  int __stdcall zplay_GetWaveInInfo(ZPLAY_HANDLE handle, unsigned int nIndex, TWaveInInfo *pWaveInInfo);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\GetWaveInInfoW.txt>
W_DECLSPEC  int __stdcall zplay_GetWaveInInfoW(ZPLAY_HANDLE handle, unsigned int nIndex, TWaveInInfoW *pWaveInInfo);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\SetWaveInDevice.txt>
W_DECLSPEC  int __stdcall zplay_SetWaveInDevice(ZPLAY_HANDLE handle, unsigned int nIndex);


/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\SetWaveOutFile.txt>
W_DECLSPEC int __stdcall zplay_SetWaveOutFile(ZPLAY_HANDLE handle, const char *sFileName, TStreamFormat nFormat, unsigned int fOutputToSoundcard);

/// <param name="handle">ZPLAY handle. Get this handle with <see cref="zplay_CreateZPlay"/>.</param>
/// <INCLUDE .\\Cpp\\zplay\\SetWaveOutFileW.txt>
W_DECLSPEC int __stdcall zplay_SetWaveOutFileW(ZPLAY_HANDLE handle, const wchar_t *sFileName, TStreamFormat nFormat, unsigned int fOutputToSoundcard);



__declspec (dllimport) unsigned int __stdcall zplay_strlen(const char *s);
__declspec (dllimport) unsigned int __stdcall zplay_wcslen(const wchar_t *s);
__declspec (dllimport) void __stdcall zplay_memcpy(void *dest, const void *src, unsigned int n);
__declspec (dllimport) void __stdcall zplay_memmove(void *dest, const void *src, unsigned int n);
__declspec (dllimport) char * __stdcall zplay_strcpy(char *dest, const char *src);
__declspec (dllimport) wchar_t * __stdcall zplay_wcscpy(wchar_t *dest, const wchar_t *src);
__declspec (dllimport) void * __stdcall zplay_memset(void *s, int c, unsigned int n);





// =============================================================================================================

#ifdef __cplusplus
}
#endif

}



