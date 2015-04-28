namespace libZPlayC#
{

/// <INCLUDE .\\csharp\\zplay\\TCallbackFunc.txt>
public delegate int TCallbackFunc(uint objptr, int user_data, TCallbackMessage msg, uint param1, uint param2);;

/// <INCLUDE .\\csharp\\zplay\\TSettingID.txt>
public enum TSettingID
{
    sidWaveBufferSize = 1,
    sidAccurateLength = 2,
    sidAccurateSeek = 3,
    sidSamplerate = 4,
    sidChannelNumber = 5,
    sidBitPerSample = 6,
    sidBigEndian = 7
}

/// <INCLUDE .\\csharp\\zplay\\TStreamFormat.txt>
public enum TStreamFormat
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
}


/// <INCLUDE .\\csharp\\zplay\\TStreamInfo.txt>
public struct TStreamInfo
    {
        public int SamplingRate;
        public int ChannelNumber;
        public bool VBR;
        public int Bitrate;
        public TStreamTime Length;
        public string Description;
}




/// <INCLUDE .\\csharp\\zplay\\TWaveOutInfo.txt>
public struct TWaveOutInfo
    {
        public uint ManufacturerID;
        public uint ProductID;
        public uint DriverVersion;
        public uint Formats;
        public uint Channels;
        public uint Support;
        public string ProductName;
}



/// <INCLUDE .\\csharp\\zplay\\TWaveInInfo.txt>
public struct TWaveInInfo
    {
        public uint ManufacturerID;
        public uint ProductID;
        public uint DriverVersion;
        public uint Formats;
        public uint Channels;
        public string ProductName;
}



/// <INCLUDE .\\csharp\\zplay\\TFFTWindow.txt>
public enum TFFTWindow
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
}




/// <INCLUDE .\\csharp\\zplay\\TTimeFormat.txt>
public enum TTimeFormat
{
    tfMillisecond = 1,
    tfSecond = 2,
    tfHMS = 4,
    tfSamples = 8
}

/// <INCLUDE .\\csharp\\zplay\\TSeekMethod.txt>
public enum TSeekMethod
{
    smFromBeginning = 1,
    smFromEnd = 2,
    smFromCurrentForward = 4,
    smFromCurrentBackward = 8
}

/// <INCLUDE .\\csharp\\zplay\\TStreamLoadInfo.txt>
public struct TStreamLoadInfo
    {
        public uint NumberOfBuffers;
        public uint NumberOfBytes;
}





/// <INCLUDE .\\csharp\\zplay\\TEchoEffect.txt>
public struct TEchoEffect
    {
        public int nLeftDelay;
        public int nLeftSrcVolume;
        public int nLeftEchoVolume;
        public int nRightDelay;
        public int nRightSrcVolume;
        public int nRightEchoVolume;
}


/// <INCLUDE .\\csharp\\zplay\\TID3Version.txt>
public enum TID3Version
{
    id3Version1 = 1,
    id3Version2 = 2
}


/// <INCLUDE .\\csharp\\zplay\\TFFTGraphHorizontalScale.txt>
public enum TFFTGraphHorizontalScale
{
    gsLogarithmic = 0,
    gsLinear = 1
}

/// <INCLUDE .\\csharp\\zplay\\TFFTGraphParamID.txt>
public enum TFFTGraphParamID
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
}

/// <INCLUDE .\\csharp\\zplay\\TFFTGraphType.txt>
public enum TFFTGraphType
{
    gtLinesLeftOnTop = 0,
    gtLinesRightOnTop,
    gtAreaLeftOnTop,
    gtAreaRightOnTop,
    gtBarsLeftOnTop,
    gtBarsRightOnTop,
    gtSpectrum
}



/// <INCLUDE .\\csharp\\zplay\\TStreamStatus.txt>
public struct TStreamStatus
    {
        public bool fPlay;
        public bool fPause;
        public bool fEcho;
        public bool fEqualizer;
        public bool fVocalCut;
        public bool fSideCut;
        public bool fChannelMix;
        public bool fSlideVolume;
        public int nLoop;
        public bool fReverse;
        public int nSongIndex;
        public int nSongsInQueue;
}



/// <INCLUDE .\\csharp\\zplay\\TStreamHMSTime.txt>
public struct TStreamHMSTime
    {
        public uint hour;
        public uint minute;
        public uint second;
        public uint millisecond;
}


/// <INCLUDE .\\csharp\\zplay\\TStreamTime.txt>
public struct TStreamTime
    {
        public uint sec;
        public uint ms;
        public uint samples;
        public TStreamHMSTime hms;
}



/// <INCLUDE .\\csharp\\zplay\\TID3Info.txt>
public struct TID3Info
    {
        public string Title;
        public string Artist;
        public string Album;
        public string Year;
        public string Comment;
        public string Track;
        public string Genre;
}


/// <INCLUDE .\\csharp\\zplay\\TID3Picture.txt>
public struct TID3Picture
   {
		public bool PicturePresent;
		public int PictureType;
		public string Description;
		public Bitmap Bitmap;
		public System.IO.MemoryStream BitStream;
};


/// <INCLUDE .\\csharp\\zplay\\TID3InfoEx.txt>
public struct TID3InfoEx
	{
		public string Title;
		public string Artist;
		public string Album;
		public string Year;
		public string Comment;
		public string Track;
		public string Genre;
		public string AlbumArtist;
		public string Composer;
		public string OriginalArtist;
		public string Copyright;
		public string URL;
		public string Encoder;
		public string Publisher;
		public int BPM;
		public TID3Picture Picture;
};





/// <INCLUDE .\\csharp\\zplay\\TBPMDetectionMethod.txt>
public enum TBPMDetectionMethod
{
    dmPeaks = 0,
    dmAutoCorrelation
}


/// <INCLUDE .\\csharp\\zplay\\TFFTGraphSize.txt>
public enum TFFTGraphSize
{
    FFTGraphMinWidth = 100,
    FFTGraphMinHeight = 60
}

/// <INCLUDE .\\csharp\\zplay\\TWaveOutMapper.txt>
public enum TWaveOutMapper
{
    WaveOutWaveMapper = 4294967295
}

/// <INCLUDE .\\csharp\\zplay\\TWaveInMapper.txt>
public enum TWaveInMapper
{
    WaveInWaveMapper = 4294967295
}

/// <INCLUDE .\\csharp\\zplay\\TCallbackMessage.txt>
public enum TCallbackMessage
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
}


/// <INCLUDE .\\csharp\\zplay\\TWaveOutFormat.txt>
public enum TWaveOutFormat
{
    format_invalid = 0,
    format_11khz_8bit_mono = 1,
    format_11khz_8bit_stereo = 2,
    format_11khz_16bit_mono = 4,
    format_11khz_16bit_stereo = 8,
    format_11khz_16bit_stereo
    format_22khz_8bit_mono = 16,
    format_22khz_8bit_stereo = 32,
    format_22khz_16bit_mono = 64,
    format_22khz_16bit_stereo = 128,
    format_22khz_16bit_stereo
    format_44khz_8bit_mono = 256,
    format_44khz_8bit_stereo = 512,
    format_44khz_16bit_mono = 1024,
    format_44khz_16bit_stereo = 2048
}

/// <INCLUDE .\\csharp\\zplay\\TWaveOutFunctionality.txt>
public enum TWaveOutFunctionality
{
    supportPitchControl = 1,
    supportPlaybackRateControl = 2,
    supportVolumeControl = 4,
    supportSeparateLeftRightVolume = 8,
    supportSync = 16,
    supportSampleAccuratePosition = 32,
    supportDirectSound = 6
}


/// <INCLUDE .\\csharp\\zplay\\ZPlay.txt>
    public class ZPlay
    {


/// <INCLUDE .\\csharp\\zplay\\ZPlayConstructor.txt>
public ZPlay();

/// <INCLUDE .\\csharp\\zplay\\ZPlayDestructor.txt>
 ~ZPlay();

/// <INCLUDE .\\csharp\\zplay\\GetVersion.txt>
public int GetVersion();

/// <INCLUDE .\\csharp\\zplay\\GetError.txt>
public string GetError();

/// <INCLUDE .\\csharp\\zplay\\GetFileFormat.txt>
public TStreamFormat GetFileFormat(string FileName);

/// <INCLUDE .\\csharp\\zplay\\OpenFile.txt>
public bool OpenFile(string FileName, TStreamFormat Format);

/// <INCLUDE .\\csharp\\zplay\\SetWaveOutFile.txt>
public bool SetWaveOutFile(string FileName, TStreamFormat Format, bool fOutputToSoundcard);

/// <INCLUDE .\\csharp\\zplay\\AddFile.txt>
public bool AddFile(string FileName, TStreamFormat Format);

/// <INCLUDE .\\csharp\\zplay\\OpenStream.txt>
public bool OpenStream(bool Buffered, bool Dynamic, ref byte[] MemStream, uint StreamSize, TStreamFormat nFormat);

/// <INCLUDE .\\csharp\\zplay\\PushDataToStream.txt>
public bool PushDataToStream(ref byte[] MemNewData, uint NewDatSize);

/// <INCLUDE .\\csharp\\zplay\\IsStreamDataFree.txt>
public bool IsStreamDataFree(ref byte[] MemNewData);

/// <INCLUDE .\\csharp\\zplay\\Close.txt>
public bool Close();

/// <INCLUDE .\\csharp\\zplay\\GetPosition.txt>
public void GetPosition(ref TStreamTime time);

/// <INCLUDE .\\csharp\\zplay\\Seek.txt>
public bool Seek(TTimeFormat TimeFormat, ref TStreamTime Position, TSeekMethod MoveMethod);

/// <INCLUDE .\\csharp\\zplay\\ReverseMode.txt>
public bool ReverseMode(bool Enable);

/// <INCLUDE .\\csharp\\zplay\\PlayLoop.txt>
public bool PlayLoop(TTimeFormat TimeFormatStart, ref TStreamTime StartPosition, TTimeFormat TimeFormatEnd, ref TStreamTime EndPosition, uint NumberOfCycles, bool ContinuePlaying);

/// <INCLUDE .\\csharp\\zplay\\StartPlayback.txt>
public bool StartPlayback();

/// <INCLUDE .\\csharp\\zplay\\StopPlayback.txt>
public bool StopPlayback();

/// <INCLUDE .\\csharp\\zplay\\PausePlayback.txt>
public bool PausePlayback();

/// <INCLUDE .\\csharp\\zplay\\ResumePlayback.txt>
public bool ResumePlayback();

/// <INCLUDE .\\csharp\\zplay\\SetEqualizerParam.txt>
public bool SetEqualizerParam(int PreAmpGain, ref int[] BandGain, int NumberOfBands);

/// <INCLUDE .\\csharp\\zplay\\GetEqualizerParam.txt>
public int GetEqualizerParam(ref int PreAmpGain, ref int[] BandGain);

/// <INCLUDE .\\csharp\\zplay\\EnableEqualizer.txt>
public bool EnableEqualizer(bool Enable);

/// <INCLUDE .\\csharp\\zplay\\SetEqualizerPreampGain.txt>
public bool SetEqualizerPreampGain(int Gain);

/// <INCLUDE .\\csharp\\zplay\\GetEqualizerPreampGain.txt>
public int GetEqualizerPreampGain();

/// <INCLUDE .\\csharp\\zplay\\SetEqualizerBandGain.txt>
public bool SetEqualizerBandGain(int BandIndex, int Gain);

/// <INCLUDE .\\csharp\\zplay\\GetEqualizerBandGain.txt>
public int GetEqualizerBandGain(int BandIndex);

/// <INCLUDE .\\csharp\\zplay\\SetEqualizerPoints.txt>
public bool SetEqualizerPoints(ref int[] FreqPointArray, int NumberOfPoints);

/// <INCLUDE .\\csharp\\zplay\\GetEqualizerPoints.txt>
public int GetEqualizerPoints(ref int[] FreqPointArray);

/// <INCLUDE .\\csharp\\zplay\\EnableEcho.txt>
public bool EnableEcho(bool Enable);

/// <INCLUDE .\\csharp\\zplay\\SetEchoParam.txt>
public bool SetEchoParam(ref TEchoEffect[] EchoEffectArray, int NumberOfEffects);

/// <INCLUDE .\\csharp\\zplay\\GetEchoParam.txt>
public int GetEchoParam(ref TEchoEffect[] EchoEffectArray);

/// <INCLUDE .\\csharp\\zplay\\SetMasterVolume.txt>
public bool SetMasterVolume(int LeftVolume, int RightVolume);

/// <INCLUDE .\\csharp\\zplay\\SetPlayerVolume.txt>
public bool SetPlayerVolume(int LeftVolume, int RightVolume);

/// <INCLUDE .\\csharp\\zplay\\GetMasterVolume.txt>
public void GetMasterVolume(ref int LeftVolume, ref int RightVolume);

/// <INCLUDE .\\csharp\\zplay\\GetPlayerVolume.txt>
public void GetPlayerVolume(ref int LeftVolume, ref int RightVolume);

/// <INCLUDE .\\csharp\\zplay\\SlideVolume.txt>
public bool SlideVolume(TTimeFormat TimeFormatStart, ref TStreamTime TimeStart, int StartVolumeLeft, int StartVolumeRight, TTimeFormat TimeFormatEnd, ref TStreamTime TimeEnd, int EndVolumeLeft, int EndVolumeRight);

/// <INCLUDE .\\csharp\\zplay\\SetPitch.txt>
public bool SetPitch(int Pitch);

/// <INCLUDE .\\csharp\\zplay\\GetPitch.txt>
public int GetPitch();

/// <INCLUDE .\\csharp\\zplay\\SetRate.txt>
public bool SetRate(int Rate);

/// <INCLUDE .\\csharp\\zplay\\GetRate.txt>
public int GetRate();

/// <INCLUDE .\\csharp\\zplay\\SetTempo.txt>
public bool SetTempo(int Tempo);

/// <INCLUDE .\\csharp\\zplay\\GetTempo.txt>
public int GetTempo();

/// <INCLUDE .\\csharp\\zplay\\GetBitrate.txt>
public int GetBitrate(bool Average);

/// <INCLUDE .\\csharp\\zplay\\LoadID3.txt>
public bool LoadID3(TID3Version Id3Version, ref TID3Info Info);

/// <INCLUDE .\\csharp\\zplay\\LoadID3Ex.txt>
public bool LoadID3Ex(ref TID3InfoEx Info, bool fDecodePicture);

/// <INCLUDE .\\csharp\\zplay\\LoadFileID3.txt>
public bool LoadFileID3(string FileName, TStreamFormat Format, TID3Version Id3Version, ref TID3Info Info);

/// <INCLUDE .\\csharp\\zplay\\LoadFileID3Ex.txt>
public bool LoadFileID3Ex(string FileName, TStreamFormat Format, ref TID3InfoEx Info, bool fDecodePicture);

/// <INCLUDE .\\csharp\\zplay\\SetCallbackFunc.txt>
public bool SetCallbackFunc(TCallbackFunc CallbackFunc, TCallbackMessage Messages, int UserData);

/// <INCLUDE .\\csharp\\zplay\\DetectBPM.txt>
public int DetectBPM(TBPMDetectionMethod Method);

/// <INCLUDE .\\csharp\\zplay\\DetectFileBPM.txt>
public int DetectFileBPM(string FileName, TStreamFormat Format, TBPMDetectionMethod Method);

/// <INCLUDE .\\csharp\\zplay\\GetFFTData.txt>
public bool GetFFTData(int FFTPoints, TFFTWindow FFTWindow, ref int HarmonicNumber, ref int[] HarmonicFreq, ref int[] LeftAmplitude, ref int[] RightAmplitude, ref int[] LeftPhase, ref int[] RightPhase);

/// <INCLUDE .\\csharp\\zplay\\DrawFFTGraphOnHDC.txt>
public bool DrawFFTGraphOnHDC(System.IntPtr hdc, int X, int Y, int Width, int Height);

/// <INCLUDE .\\csharp\\zplay\\DrawFFTGraphOnHWND.txt>
public bool DrawFFTGraphOnHWND(System.IntPtr hwnd, int X, int Y, int Width, int Height);

/// <INCLUDE .\\csharp\\zplay\\SetFFTGraphParam.txt>
public bool SetFFTGraphParam(TFFTGraphParamID ParamID, int ParamValue);

/// <INCLUDE .\\csharp\\zplay\\GetFFTGraphParam.txt>
public int GetFFTGraphParam(TFFTGraphParamID ParamID);

/// <INCLUDE .\\csharp\\zplay\\StereoCut.txt>
public bool StereoCut(bool Enable, bool OutputCenter, bool BassToSides);

/// <INCLUDE .\\csharp\\zplay\\MixChannels.txt>
public bool MixChannels(bool Enable, uint LeftPercent, uint RightPercent);

/// <INCLUDE .\\csharp\\zplay\\GetVUData.txt>
public void GetVUData(ref int LeftChannel, ref int RightChannel);

/// <INCLUDE .\\csharp\\zplay\\GetStreamInfo.txt>
public void GetStreamInfo(ref TStreamInfo info);

/// <INCLUDE .\\csharp\\zplay\\GetStatus.txt>
public void GetStatus(ref TStreamStatus status);

/// <INCLUDE .\\csharp\\zplay\\GetDynamicStreamLoad.txt>
public void GetDynamicStreamLoad(ref TStreamLoadInfo StreamLoadInfo);

/// <INCLUDE .\\csharp\\zplay\\EnumerateWaveOut.txt>
public int EnumerateWaveOut();

/// <INCLUDE .\\csharp\\zplay\\GetWaveOutInfo.txt>
public bool GetWaveOutInfo(uint Index, ref TWaveOutInfo Info);

/// <INCLUDE .\\csharp\\zplay\\SetWaveOutDevice.txt>
public bool SetWaveOutDevice(uint Index);

/// <INCLUDE .\\csharp\\zplay\\EnumerateWaveIn.txt>
public int EnumerateWaveIn();

/// <INCLUDE .\\csharp\\zplay\\GetWaveInInfo.txt>
public bool GetWaveInInfo(uint Index, ref TWaveInInfo Info);

/// <INCLUDE .\\csharp\\zplay\\SetWaveInDevice.txt>
public bool SetWaveInDevice(uint Index);

/// <INCLUDE .\\csharp\\zplay\\SetSettings.txt>
public int SetSettings(TSettingID SettingID, int Value);

/// <INCLUDE .\\csharp\\zplay\\GetSettings.txt>
public int GetSettings(TSettingID SettingID);

}
}
