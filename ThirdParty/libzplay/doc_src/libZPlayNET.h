namespace libZPlayVC
{

 
/// <INCLUDE .\\vcnet\\zplay\\TSettingID.txt>
	public enum class TSettingID: int
	{
		sidWaveBufferSize = 1,
		sidAccurateLength = 2,
		sidAccurateSeek = 3,
		sidSamplerate = 4,
		sidChannelNumber = 5,
		sidBitPerSample = 6,
		sidBigEndian = 7
	};

/// <INCLUDE .\\vcnet\\zplay\\TStreamFormat.txt>
    public enum class TStreamFormat: int
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
        sfAutodetect = 1000
    };


/// <INCLUDE .\\vcnet\\zplay\\TStreamHMSTime.txt>
	public value class TStreamHMSTime
	{
	public:
		System::UInt32 hour;
		System::UInt32 minute;
		System::UInt32 second;
		System::UInt32 millisecond;
	};

/// <INCLUDE .\\vcnet\\zplay\\TStreamTime.txt>
	public value class TStreamTime
	{
	public:
		System::UInt32 sec;
		System::UInt32 ms;
		System::UInt32 samples;
		TStreamHMSTime hms;
	};

/// <INCLUDE .\\vcnet\\zplay\\TStreamInfo.txt>
	public value class TStreamInfo
	{
	public:
		int SamplingRate;
		int ChannelNumber;
		bool VBR;
		int Bitrate;
		TStreamTime Length;
		System::String ^Description;
	};



/// <INCLUDE .\\vcnet\\zplay\\TWaveOutInfo.txt>
	public value class TWaveOutInfo
	{
	public:
		System::UInt32 ManufacturerID;
		System::UInt32 ProductID;
		System::UInt32 DriverVersion;
		System::UInt32 Formats;
		System::UInt32 Channels;
		System::UInt32 Support;
		System::String ^ProductName;
	};

/// <INCLUDE .\\vcnet\\zplay\\TWaveInInfo.txt>
    public value class TWaveInInfo
    {
    public:
        System::UInt32 ManufacturerID;]
        System::UInt32 ProductID;
        System::UInt32 DriverVersion;
        System::UInt32 Formats;
        System::UInt32 Channels;
        System::String ^ProductName;
    };


/// <INCLUDE .\\vcnet\\zplay\\TFFTWindow.txt>
	public enum class TFFTWindow: int
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




/// <INCLUDE .\\vcnet\\zplay\\TTimeFormat.txt>
	public enum class TTimeFormat: System::UInt32
	{
		tfMillisecond = 1,
		tfSecond = 2,
		tfHMS = 4,
		tfSamples = 8
	};

/// <INCLUDE .\\vcnet\\zplay\\TSeekMethod.txt>
	public enum class TSeekMethod: int
	{
		smFromBeginning = 1,
		smFromEnd = 2,
		smFromCurrentForward = 4,
		smFromCurrentBackward = 8
	};

/// <INCLUDE .\\vcnet\\zplay\\TStreamLoadInfo.txt>
	public value class TStreamLoadInfo
	{
	public:
		System::UInt32 NumberOfBuffers;
		System::UInt32 NumberOfBytes;
	};




/// <INCLUDE .\\vcnet\\zplay\\TEchoEffect.txt>
	public value class TEchoEffect
	{
	public:
		int nLeftDelay;
		int nLeftSrcVolume;
		int nLeftEchoVolume;
		int nRightDelay;
		int nRightSrcVolume;
		int nRightEchoVolume;
	};

/// <INCLUDE .\\vcnet\\zplay\\TID3Version.txt>
	public enum class TID3Version: int
	{
		id3Version1 = 1,
		id3Version2 = 2
	};


/// <INCLUDE .\\vcnet\\zplay\\TFFTGraphHorizontalScale.txt>
	public enum class TFFTGraphHorizontalScale: int
	{
		gsLogarithmic = 0,
		gsLinear = 1
	};

/// <INCLUDE .\\vcnet\\zplay\\TFFTGraphParamID.txt>
    public enum class TFFTGraphParamID: int
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

/// <INCLUDE .\\vcnet\\zplay\\TFFTGraphType.txt>
    public enum class TFFTGraphType: int
    {
        gtLinesLeftOnTop = 0,
        gtLinesRightOnTop,
        gtAreaLeftOnTop,
        gtAreaRightOnTop,
        gtBarsLeftOnTop,
        gtBarsRightOnTop,
		gtSpectrum
    };



/// <INCLUDE .\\vcnet\\zplay\\TStreamStatus.txt>
	public value class TStreamStatus
	{
	public:
		bool fPlay;
		bool fPause;
		bool fEcho;
		bool fEqualizer;
		bool fVocalCut;
		bool fSideCut;
		bool fChannelMix;
		bool fSlideVolume;
		int nLoop;
		bool fReverse;
		int nSongIndex;
		int nSongsInQueue;
	};


	


/// <INCLUDE .\\vcnet\\zplay\\TID3Info.txt>
	public value class TID3Info
	{
	public:
		System::String ^Title;
		System::String ^Artist;
		System::String ^Album;
		System::String ^Genre;
		System::String ^Comment;
		System::String ^Year;
		System::String ^Track;
	};
	
	
	
/// <INCLUDE .\\vcnet\\zplay\\TID3Picture.txt>	
	public value class TID3Picture
   {

	public:
		bool PicturePresent;
		int PictureType;
		System::String ^Description;
		Bitmap ^Bitmap;
		System::IO::MemoryStream ^BitStream;
	};


/// <INCLUDE .\\vcnet\\zplay\\TID3InfoEx.txt>
	public value class TID3InfoEx
	{
	public:
		System::String ^Title;
		System::String ^Artist;
		System::String ^Album;
		System::String ^Year;
		System::String ^Comment;
		System::String ^Track;
		System::String ^Genre;
		System::String ^AlbumArtist;
		System::String ^Composer;
		System::String ^OriginalArtist;
		System::String ^Copyright;
		System::String ^URL;
		System::String ^Encoder;
		System::String ^Publisher;
		int BPM;
		TID3Picture Picture;
	};


/// <INCLUDE .\\vcnet\\zplay\\TBPMDetectionMethod.txt>
	public enum class TBPMDetectionMethod: int
	{
		dmPeaks = 0,
		dmAutoCorrelation
	};


/// <INCLUDE .\\vcnet\\zplay\\TFFTGraphSize.txt>
	public enum class TFFTGraphSize: int
	{
		FFTGraphMinWidth = 100,
		FFTGraphMinHeight = 60
	};

/// <INCLUDE .\\vcnet\\zplay\\TWaveOutMapper.txt>
	public enum class TWaveOutMapper: System::UInt32
	{
		WaveOutWaveMapper = 4294967295
	};

/// <INCLUDE .\\vcnet\\zplay\\TWaveInMapper.txt>
	public enum class TWaveInMapper: System::UInt32
	{
		WaveInWaveMapper = 4294967295
	};

/// <INCLUDE .\\vcnet\\zplay\\TCallbackMessage.txt>
	public enum class TCallbackMessage: int
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


/// <INCLUDE .\\vcnet\\zplay\\TWaveOutFormat.txt>
	public enum class TWaveOutFormat: System::UInt32
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

/// <INCLUDE .\\vcnet\\zplay\\TWaveOutFunctionality.txt>
	public enum class TWaveOutFunctionality: System::UInt32
	{
		supportPitchControl = 1,
		supportPlaybackRateControl = 2,
		supportVolumeControl = 4,
		supportSeparateLeftRightVolume = 8,
		supportSync = 16,
		supportSampleAccuratePosition = 32,
		supportDirectSound = 6
	};


/// <INCLUDE .\\vcnet\\zplay\\ZPlay.txt>
	public ref class ZPlay
	{

public:
/// <INCLUDE .\\vcnet\\zplay\\TCallbackFunc.txt>
	delegate int TCallbackFunc(System::UInt32 objptr, int user_data, TCallbackMessage msg, System::UInt32 param1, System::UInt32 param2);

	public:
/// <INCLUDE .\\vcnet\\zplay\\Constructor.txt>
		ZPlay();
		
	
	private protected:
/// <INCLUDE .\\vcnet\\zplay\\Destructor.txt>
		~ZPlay();

	public:
/// <INCLUDE .\\vcnet\\zplay\\GetVersion.txt>
		int GetVersion();

/// <INCLUDE .\\vcnet\\zplay\\GetError.txt>
		System::String ^GetError();

/// <INCLUDE .\\vcnet\\zplay\\GetFileFormat.txt>
		TStreamFormat GetFileFormat(System::String ^FileName);

/// <INCLUDE .\\vcnet\\zplay\\OpenFile.txt>
		bool OpenFile(System::String ^FileName, TStreamFormat Format);

/// <INCLUDE .\\vcnet\\zplay\\AddFile.txt>
		bool AddFile(System::String ^FileName, TStreamFormat Format);

/// <INCLUDE .\\vcnet\\zplay\\OpenStream.txt>
		bool OpenStream(bool Buffered, bool Dynamic, array<System::Byte> ^%MemStream, System::UInt32 StreamSize, TStreamFormat nFormat);

/// <INCLUDE .\\vcnet\\zplay\\PushDataToStream.txt>
		bool PushDataToStream(array<System::Byte> ^%MemNewData, System::UInt32 NewDatSize);

/// <INCLUDE .\\vcnet\\zplay\\IsStreamDataFree.txt>
		bool IsStreamDataFree(array<System::Byte> ^%MemNewData);

/// <INCLUDE .\\vcnet\\zplay\\Close.txt>
		bool Close();

/// <INCLUDE .\\vcnet\\zplay\\GetPosition.txt>
		void GetPosition(TStreamTime %time);

/// <INCLUDE .\\vcnet\\zplay\\Seek.txt>
		bool Seek(TTimeFormat TimeFormat, TStreamTime %Position, TSeekMethod MoveMethod);

/// <INCLUDE .\\vcnet\\zplay\\ReverseMode.txt>
		bool ReverseMode(bool Enable);

/// <INCLUDE .\\vcnet\\zplay\\PlayLoop.txt>
		bool PlayLoop(TTimeFormat TimeFormatStart, TStreamTime %StartPosition, TTimeFormat TimeFormatEnd, TStreamTime %EndPosition, System::UInt32 NumberOfCycles, bool ContinuePlaying);

/// <INCLUDE .\\vcnet\\zplay\\StartPlayback.txt>
		bool StartPlayback();

/// <INCLUDE .\\vcnet\\zplay\\StopPlayback.txt>
		bool StopPlayback();

/// <INCLUDE .\\vcnet\\zplay\\PausePlayback.txt>
		bool PausePlayback();

/// <INCLUDE .\\vcnet\\zplay\\ResumePlayback.txt>
		bool ResumePlayback();

/// <INCLUDE .\\vcnet\\zplay\\SetEqualizerParam.txt>
		bool SetEqualizerParam(int PreAmpGain, array<int> ^%BandGain, int NumberOfBands);

/// <INCLUDE .\\vcnet\\zplay\\GetEqualizerParam.txt>
		int GetEqualizerParam(int %PreAmpGain, array<int> ^%BandGain);

/// <INCLUDE .\\vcnet\\zplay\\EnableEqualizer.txt>
		bool EnableEqualizer(bool Enable);

/// <INCLUDE .\\vcnet\\zplay\\SetEqualizerPreampGain.txt>
		bool SetEqualizerPreampGain(int Gain);

/// <INCLUDE .\\vcnet\\zplay\\GetEqualizerPreampGain.txt>
		int GetEqualizerPreampGain();

/// <INCLUDE .\\vcnet\\zplay\\SetEqualizerBandGain.txt>
		bool SetEqualizerBandGain(int BandIndex, int Gain);

/// <INCLUDE .\\vcnet\\zplay\\GetEqualizerBandGain.txt>
		int GetEqualizerBandGain(int BandIndex);

/// <INCLUDE .\\vcnet\\zplay\\SetEqualizerPoints.txt>
		bool SetEqualizerPoints(array<int> ^%FreqPointArray, int NumberOfPoints);

/// <INCLUDE .\\vcnet\\zplay\\GetEqualizerPoints.txt>
		int GetEqualizerPoints(array<int> ^%FreqPointArray);

/// <INCLUDE .\\vcnet\\zplay\\EnableEcho.txt>
		bool EnableEcho(bool Enable);

/// <INCLUDE .\\vcnet\\zplay\\SetEchoParam.txt>
		bool SetEchoParam(array<TEchoEffect> ^%EchoEffectArray, int NumberOfEffects);

/// <INCLUDE .\\vcnet\\zplay\\GetEchoParam.txt>
		int GetEchoParam(array<TEchoEffect> ^%EchoEffectArray);

/// <INCLUDE .\\vcnet\\zplay\\SetMasterVolume.txt>
		bool SetMasterVolume(int LeftVolume, int RightVolume);

/// <INCLUDE .\\vcnet\\zplay\\SetPlayerVolume.txt>
		bool SetPlayerVolume(int LeftVolume, int RightVolume);

/// <INCLUDE .\\vcnet\\zplay\\GetMasterVolume.txt>
		void GetMasterVolume(int %LeftVolume, int %RightVolume);

/// <INCLUDE .\\vcnet\\zplay\\GetPlayerVolume.txt>
		void GetPlayerVolume(int %LeftVolume, int %RightVolume);

/// <INCLUDE .\\vcnet\\zplay\\SlideVolume.txt>
		bool SlideVolume(TTimeFormat TimeFormatStart, TStreamTime %TimeStart, int StartVolumeLeft, int StartVolumeRight, TTimeFormat TimeFormatEnd, TStreamTime %TimeEnd, int EndVolumeLeft, int EndVolumeRight);

/// <INCLUDE .\\vcnet\\zplay\\SetPitch.txt>
		bool SetPitch(int Pitch);

/// <INCLUDE .\\vcnet\\zplay\\GetPitch.txt>
		int GetPitch();

/// <INCLUDE .\\vcnet\\zplay\\SetRate.txt>
		bool SetRate(int Rate);

/// <INCLUDE .\\vcnet\\zplay\\GetRate.txt>
		int GetRate();

/// <INCLUDE .\\vcnet\\zplay\\SetTempo.txt>
		bool SetTempo(int Tempo);

/// <INCLUDE .\\vcnet\\zplay\\GetTempo.txt>
		int GetTempo();

/// <INCLUDE .\\vcnet\\zplay\\GetBitrate.txt>
		int GetBitrate(bool Average);

/// <INCLUDE .\\vcnet\\zplay\\LoadID3.txt>
		bool LoadID3(TID3Version Id3Version, TID3Info %Info);
		
/// <INCLUDE .\\vcnet\\zplay\\LoadID3Ex.txt>	
		bool LoadID3Ex(TID3InfoEx %Info, bool fDecodePicture);

/// <INCLUDE .\\vcnet\\zplay\\LoadFileID3.txt>
		bool LoadFileID3(System::String ^FileName, TStreamFormat Format, TID3Version Id3Version, TID3Info %Info);
		
/// <INCLUDE .\\vcnet\\zplay\\LoadFileID3Ex.txt>	
		bool LoadFileID3Ex(System::String ^FileName, TStreamFormat Format,TID3InfoEx %Info, bool fDecodePicture);

/// <INCLUDE .\\vcnet\\zplay\\SetCallbackFunc.txt>
		bool SetCallbackFunc(TCallbackFunc ^CallbackFunc, TCallbackMessage Messages, int UserData);

/// <INCLUDE .\\vcnet\\zplay\\DetectBPM.txt>
		int DetectBPM(TBPMDetectionMethod Method);

/// <INCLUDE .\\vcnet\\zplay\\DetectFileBPM.txt>
		int DetectFileBPM(System::String ^FileName, TStreamFormat Format, TBPMDetectionMethod Method);

/// <INCLUDE .\\vcnet\\zplay\\GetFFTData.txt>
		bool GetFFTData(int FFTPoints, TFFTWindow FFTWindow, int %HarmonicNumber, array<int> ^%HarmonicFreq, array<int> ^%LeftAmplitude, array<int> ^%RightAmplitude, array<int> ^%LeftPhase, array<int> ^%RightPhase);

/// <INCLUDE .\\vcnet\\zplay\\DrawFFTGraphOnHDC.txt>
		bool DrawFFTGraphOnHDC(System::IntPtr hdc, int X, int Y, int Width, int Height);

/// <INCLUDE .\\vcnet\\zplay\\DrawFFTGraphOnHWND.txt>
		bool DrawFFTGraphOnHWND(System::IntPtr hwnd, int X, int Y, int Width, int Height);

/// <INCLUDE .\\vcnet\\zplay\\SetFFTGraphParam.txt>
		bool SetFFTGraphParam(TFFTGraphParamID ParamID, int ParamValue);

/// <INCLUDE .\\vcnet\\zplay\\GetFFTGraphParam.txt>
		int GetFFTGraphParam(TFFTGraphParamID ParamID);

/// <INCLUDE .\\vcnet\\zplay\\StereoCut.txt>
		bool StereoCut(bool Enable, bool OutputCenter, bool BassToSides);

/// <INCLUDE .\\vcnet\\zplay\\MixChannels.txt>
		bool MixChannels(bool Enable, System::UInt32 LeftPercent, System::UInt32 RightPercent);

/// <INCLUDE .\\vcnet\\zplay\\GetVUData.txt>
		void GetVUData(int %LeftChannel, int %RightChannel);

/// <INCLUDE .\\vcnet\\zplay\\GetStreamInfo.txt>
		void GetStreamInfo(TStreamInfo %info);

/// <INCLUDE .\\vcnet\\zplay\\GetStatus.txt>
		void GetStatus(TStreamStatus %status);
		
/// <INCLUDE .\\vcnet\\zplay\\GetDynamicStreamLoad.txt>
		void GetDynamicStreamLoad(TStreamLoadInfo %StreamLoadInfo);

/// <INCLUDE .\\vcnet\\zplay\\EnumerateWaveOut.txt>
		int EnumerateWaveOut();

/// <INCLUDE .\\vcnet\\zplay\\EnumerateWaveIn.txt>
		int EnumerateWaveIn();

/// <INCLUDE .\\vcnet\\zplay\\GetWaveOutInfo.txt>
		bool GetWaveOutInfo(System::UInt32 Index, TWaveOutInfo %Info);

/// <INCLUDE .\\vcnet\\zplay\\GetWaveInInfo.txt>
		bool GetWaveInInfo(System::UInt32 Index, TWaveInInfo %Info);

/// <INCLUDE .\\vcnet\\zplay\\SetWaveOutDevice.txt>
		bool SetWaveOutDevice(System::UInt32 Index);

/// <INCLUDE .\\vcnet\\zplay\\SetWaveInDevice.txt>
		bool SetWaveInDevice(System::UInt32 Index);

/// <INCLUDE .\\vcnet\\zplay\\SetSettings.txt>
		int SetSettings(TSettingID SettingID, int Value);

/// <INCLUDE .\\vcnet\\zplay\\GetSettings.txt>
		int GetSettings(TSettingID SettingID);

/// <INCLUDE .\\vcnet\\zplay\\SetWaveOutFile.txt>
 	bool SetWaveOutFile(System::String ^FileName, TStreamFormat Format, bool fOutputToSoundcard);
      

	};

}