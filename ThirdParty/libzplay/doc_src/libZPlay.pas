unit libZPlayDelphi;

interface


/// <INCLUDE .\\delphi\\zplay\\TWaveOutMapper.txt>
const TWaveOutMapper: Integer = -1; 

/// <INCLUDE .\\delphi\\zplay\\TWaveInMapper.txt>
const TWaveInMapper: Integer = -1; 

/// <INCLUDE .\\delphi\\zplay\\TSettingID.txt>
type TSettingID = (
        sidWaveBufferSize = 1,
        sidAccurateLength = 2,
        sidAccurateSeek = 3,
        sidSamplerate = 4,
        sidChannelNumber = 5,
        sidBitPerSample = 6,
        sidBigEndian = 7
);

/// <INCLUDE .\\delphi\\zplay\\TFFTGraphSize.txt>
type TFFTGraphSize = (FFTGraphMinWidth = 100, FFTGraphMinHeight = 60);

        
/// <INCLUDE .\\delphi\\zplay\\TStreamFormat.txt>
type TStreamFormat = (sfUnknown = 0,
                        sfMp3 = 1,
                        sfOgg = 2,
                        sfWav = 3,
                        sfPCM = 4,
                        sfFLAC = 5,
                        sfFLACOgg = 6,
		                sfAC3 = 7,
		                sfAacADTS = 8,
		                sfAutodetect = 1000
);

/// <INCLUDE .\\delphi\\zplay\\TBPMDetectionMethod.txt>
type TBPMDetectionMethod = (
        dmPeaks = 0,
        dmAutoCorrelation = 1
);

/// <INCLUDE .\\delphi\\zplay\\TFFTGraphParamID.txt>
type TFFTGraphParamID = (
        gpFFTPoints = 1,
        gpGraphType = 2,
        gpWindow = 3,
        gpHorizontalScale = 4,
        gpSubgrid = 5,
        gpTransparency = 6 ,
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
);

/// <INCLUDE .\\delphi\\zplay\\TFFTGraphType.txt>
type TFFTGraphType = (
        gtLinesLeftOnTop = 0,
        gtLinesRightOnTop  = 1,
        gtAreaLeftOnTop = 2,
        gtAreaRightOnTop = 3,
        gtBarsLeftOnTop = 4,
        gtBarsRightOnTop = 5 ,
        gtSpectrum = 6
);

/// <INCLUDE .\\delphi\\zplay\\TTimeFormat.txt>
type TTimeFormat = (
        tfMillisecond = 1,
        tfSecond = 2,
        tfHMS = 4,
        tfSamples = 8
);

/// <INCLUDE .\\delphi\\zplay\\TFFTWindow.txt>
type TFFTWindow = (
        fwRectangular = 1,
        fwHamming = 2,
        fwHann = 3,
        fwCosine = 4,
        fwLanczos = 5,
        fwBartlett = 6,
        fwTriangular = 7,
        fwGauss = 8,
        fwBartlettHann = 9,
        fwBLackman = 10,
        fwNuttall = 11,
        fwBlackManHarris = 12,
        fwBlackmanNuttall = 13,
        fwFlatTop = 14
);


/// <INCLUDE .\\delphi\\zplay\\TSeekMethod.txt>
type TSeekMethod = (
        smFromBeginning = 1,
        smFromEnd = 2,
        smFromCurrentForward = 4,
        smFromCurrentBackward = 8
);


/// <INCLUDE .\\delphi\\zplay\\TID3Version.txt>
type TID3Version = (
        id3Version1 = 1,
        id3Version2 = 2
);



/// <INCLUDE .\\delphi\\zplay\\TStreamLoadInfo.txt>
type TStreamLoadInfo = record
	  NumberOfBuffers: Cardinal;
	  NumberOfBytes: Cardinal;
end;



/// <INCLUDE .\\delphi\\zplay\\TWaveOutFormat.txt>
type TWaveOutFormat = (
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
);

/// <INCLUDE .\\delphi\\zplay\\TWaveOutFunctionality.txt>
type TWaveOutFunctionality = (
supportPitchControl = 1,
supportPlaybackRateControl = 2,
supportVolumeControl = 4,
supportSeparateLeftRightVolume = 8,
supportSync = 16,
supportSampleAccuratePosition = 32,
supportDirectSound = 64
);


/// <INCLUDE .\\delphi\\zplay\\TWaveOutInfo.txt>
type TWaveOutInfo = record
    ManufacturerID: Cardinal;
    ProductID: Cardinal;
    DriverVersion: Cardinal;
    Formats: Cardinal;
    Channels: Cardinal;
    Support: Cardinal;
    ProductName: AnsiString;
end;

/// <INCLUDE .\\delphi\\zplay\\TWaveOutInfoW.txt>
type TWaveOutInfoW = record
    ManufacturerID: Cardinal;
    ProductID: Cardinal;
    DriverVersion: Cardinal;
    Formats: Cardinal;
    Channels: Cardinal;
    Support: Cardinal;
    ProductName: WideString;
end;



/// <INCLUDE .\\delphi\\zplay\\TWaveInInfo.txt>
type TWaveInInfo = record
    ManufacturerID: Cardinal;
    ProductID: Cardinal;
    DriverVersion: Cardinal;
    Formats: Cardinal;
    Channels: Cardinal;
    ProductName: AnsiString;
end;

/// <INCLUDE .\\delphi\\zplay\\TWaveInInfoW.txt>
type TWaveInInfoW = record
    ManufacturerID: Cardinal;
    ProductID: Cardinal;
    DriverVersion: Cardinal;
    Formats: Cardinal;
    Channels: Cardinal;
    ProductName: WideString;
end;



/// <INCLUDE .\\delphi\\zplay\\TStreamHMSTime.txt>
type TStreamHMSTime = record
    hour: Cardinal;
    minute: Cardinal;
    second: Cardinal;
    millisecond: Cardinal;
end;

/// <INCLUDE .\\delphi\\zplay\\TStreamTime.txt>
type TStreamTime = record
	  sec: Cardinal;
	  ms: Cardinal;
    samples: Cardinal;
    hms: TStreamHMSTime
end;



/// <INCLUDE .\\delphi\\zplay\\TID3Info.txt>
type TID3Info = record
    Title: AnsiString;
    Artist: AnsiString;
    Album: AnsiString;
    Year: AnsiString;
    Comment: AnsiString;
    Track: AnsiString;
    Genre: AnsiString;
end;

/// <INCLUDE .\\delphi\\zplay\\TID3InfoW.txt>
type TID3InfoW = record
    Title: WideString;
    Artist: WideString;
    Album: WideString;
    Year: WideString;
    Comment: WideString;
    Track: WideString;
    Genre: WideString;
end;


/// <INCLUDE .\\delphi\\zplay\\TID3Picture.txt>
type
  TID3Picture = record
	PicturePresent: Boolean;
	PictureType: Integer;
	Description: AnsiString;
  Bitmap: TPicture;
end;

/// <INCLUDE .\\delphi\\zplay\\TID3PictureW.txt>
type
  TID3PictureW = record
	PicturePresent: Boolean;
	PictureType: Integer;
	Description: WideString;
  Bitmap: TPicture;
end;


/// <INCLUDE .\\delphi\\zplay\\TID3InfoEx.txt>
type
  TID3InfoEx = record
	Title: AnsiString;
	Artist: AnsiString;
	Album: AnsiString;
	Year: AnsiString;
	Comment: AnsiString;
	Track: AnsiString;
  Genre: AnsiString;
	AlbumArtist: AnsiString;
	Composer: AnsiString;
	OriginalArtist: AnsiString;
	Copyright: AnsiString;
	URL: AnsiString;
	Encoder: AnsiString;
	Publisher: AnsiString;
	BPM: Cardinal;
	Picture: TID3Picture;
end;

/// <INCLUDE .\\delphi\\zplay\\TID3InfoExW.txt>
type
  TID3InfoExW = record
	Title: WideString;
	Artist: WideString;
	Album: WideString;
	Year: WideString;
	Comment: WideString;
	Track: WideString;
  Genre: WideString;
	AlbumArtist: WideString;
	Composer: WideString;
	OriginalArtist: WideString;
	Copyright: WideString;
	URL: WideString;
	Encoder: WideString;
	Publisher: WideString;
	BPM: Cardinal;
	Picture: TID3PictureW;
end;



/// <INCLUDE .\\delphi\\zplay\\TStreamInfo.txt>
type TStreamInfo = record
	  SamplingRate: Cardinal;
	  ChannelNumber: Cardinal;
    VBR: Boolean;
    Bitrate: Cardinal;
    Length: TStreamTime;
	  Description: AnsiString;
end;

/// <INCLUDE .\\delphi\\zplay\\TStreamInfoW.txt>
type TStreamInfoW = record
 	  SamplingRate: Cardinal;
	  ChannelNumber: Cardinal;
    VBR: Boolean;
    Bitrate: Cardinal;
    Length: TStreamTime;
	  Description: WideString;
end;


/// <INCLUDE .\\delphi\\zplay\\TStreamStatus.txt>
type TStreamStatus = record
    fPlay: Boolean;
    fPause: Boolean;
    fEcho: Boolean;
    fEqualizer: Boolean;
    fVocalCut: Boolean;
    fSideCut: Boolean;
    fChannelMix: Boolean;
    fSlideVolume: Boolean;
    nLoop: Integer;
    fReverse: Boolean;
    nSongIndex: Integer;
	  nSongsInQueue: Integer;
end;


// ======================================================================

/// <INCLUDE .\\delphi\\zplay\\TEchoEffect.txt>
type TEchoEffect = record
    LeftDelay: Integer;
    LeftSrcVolume: Integer;
    LeftEchoVolume: Integer;
    RightDelay: Integer;
    RightSrcVolume: Integer;
    RightEchoVolume: Integer;
end;


type PECHO_EFFECT = ^TEchoEffect;


/// <INCLUDE .\\delphi\\zplay\\TDynamicEchoEffectArray.txt>
type TDynamicEchoEffectArray = array of TEchoEffect;
/// <INCLUDE .\\delphi\\zplay\\TDynamicIntegerArray.txt>
type TDynamicIntegerArray = array of Integer;



/// <INCLUDE .\\delphi\\zplay\\TFFTGraphHorizontalScale.txt>
type TFFTGraphHorizontalScale = ( gsLogarithmic = 0, gsLinear = 1);

/// <INCLUDE .\\delphi\\zplay\\TCallbackMessage.txt>
type TCallbackMessage = (
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
MsgWaveBuffer = 33554432,
MsgAll = -1,
MsgAllAsync = 65535,
MsgAllSync = -65536);


// =======================================================================

/// <INCLUDE .\\delphi\\zplay\\TCallbackFunc.txt>
type TCallbackFunc = function(objptr: Pointer; user_data: Integer; msg: TCallbackMessage; param1: Cardinal; param2: Cardinal): Integer; stdcall;



// ===========================
/// <INCLUDE .\\delphi\\zplay\\ZPlay.txt>
 type ZPlay = class
  private
    { Private declarations }


    error_msg: AnsiString;
    error_msgW: WideString;
	objptr: Pointer;

  protected
    { Protected declarations }
  public
    { Public declarations }
        


/// <INCLUDE .\\delphi\\zplay\\Constructor.txt>
    constructor Create;
    
  /// <INCLUDE .\\delphi\\zplay\\Destructor.txt>
    destructor Destroy; override;

/// <INCLUDE .\\delphi\\zplay\\SetSettings.txt>
    Function SetSettings(SettingID: TSettingID; Value: Integer): Integer;
/// <INCLUDE .\\delphi\\zplay\\GetSettings.txt>
    Function GetSettings(SettingID: TSettingID; Value: Integer): Integer;
/// <INCLUDE .\\delphi\\zplay\\GetVersion.txt>
    Function GetVersion(): Integer; stdcall;
/// <INCLUDE .\\delphi\\zplay\\EnumerateWaveOut.txt>
    Function EnumerateWaveOut(): Integer;
/// <INCLUDE .\\delphi\\zplay\\EnumerateWaveIn.txt>
    Function EnumerateWaveIn(): Integer;
/// <INCLUDE .\\delphi\\zplay\\GetWaveOutInfo.txt>
    Function GetWaveOutInfo(Index: Cardinal; out WaveOutInfo: TWaveOutInfo): Integer;

/// <INCLUDE .\\delphi\\zplay\\GetWaveInInfo.txt>
    Function GetWaveInInfo(Index: Cardinal; out WaveInInfo: TWaveInInfo): Integer;

/// <INCLUDE .\\delphi\\zplay\\GetWaveInInfoW.txt>
    Function GetWaveInInfoW(Index: Cardinal; out WaveInInfoW: TWaveInInfoW): Integer;

/// <INCLUDE .\\delphi\\zplay\\GetWaveOutInfoW.txt>
    Function GetWaveOutInfoW(Index: Cardinal; out WaveOutInfo: TWaveOutInfoW): Integer;
/// <INCLUDE .\\delphi\\zplay\\SetWaveOutDevice.txt>
    Function SetWaveOutDevice(Index: Cardinal): Integer;

/// <INCLUDE .\\delphi\\zplay\\SetWaveInDevice.txt>
    Function SetWaveInDevice(Index: Cardinal): Integer;

/// <INCLUDE .\\delphi\\zplay\\GetError.txt>
    Function GetError(): AnsiString;
/// <INCLUDE .\\delphi\\zplay\\GetErrorW.txt>
    Function GetErrorW(): WideString;
/// <INCLUDE .\\delphi\\zplay\\GetFileFormat.txt>
    Function GetFileFormat(const FileName: AnsiString): TStreamFormat;
/// <INCLUDE .\\delphi\\zplay\\GetFileFormatW.txt>
    Function GetFileFormatW(const FileName: WideString): TStreamFormat;
  
/// <INCLUDE .\\delphi\\zplay\\SetCallbackFunc.txt>
    Function SetCallbackFunc(CallbackFunc: TCallbackFunc; Messages: TCallbackMessage; UserData: Integer): Boolean;

/// <INCLUDE .\\delphi\\zplay\\GetStreamInfo.txt>
    Procedure GetStreamInfo(out Info: TStreamInfo);
/// <INCLUDE .\\delphi\\zplay\\GetStreamInfoW.txt>
    Procedure GetStreamInfoW(out Info: TStreamInfoW);
/// <INCLUDE .\\delphi\\zplay\\OpenFile.txt>
    Function OpenFile(const FileName: AnsiString; Format: TStreamFormat): Boolean; overload;
/// <INCLUDE .\\delphi\\zplay\\OpenFileW.txt>
    Function OpenFileW(const FileName: WideString; Format: TStreamFormat): Boolean; overload;
   
/// <INCLUDE .\\delphi\\zplay\\SetWaveOutFile.txt> 
    Function SetWaveOutFile(const FileName: AnsiString; Format: TStreamFormat; fOutputToSoundcard: Boolean): Boolean; overload;
    
/// <INCLUDE .\\delphi\\zplay\\SetWaveOutFileW.txt>
    Function SetWaveOutFileW(const FileName: WideString; Format: TStreamFormat; fOutputToSoundcard: Boolean): Boolean; overload;

/// <INCLUDE .\\delphi\\zplay\\AddFile.txt>
    Function AddFile(const FileName: AnsiString; Format: TStreamFormat): Boolean; overload;
/// <INCLUDE .\\delphi\\zplay\\AddFileW.txt>
    Function AddFileW(const FileName: WideString; Format: TStreamFormat): Boolean; overload;



/// <INCLUDE .\\delphi\\zplay\\OpenStream.txt>
    Function OpenStream(Buffered: Integer;	Managed: Integer;  MemStream: Pointer; StreamSize: Cardinal; Format: TStreamFormat): Boolean;
/// <INCLUDE .\\delphi\\zplay\\PushDataToStream.txt>
    Function PushDataToStream(const MemNewData: Pointer; NewDataize: Cardinal): Boolean;
/// <INCLUDE .\\delphi\\zplay\\IsStreamDataFree.txt>
    Function IsStreamDataFree(const MemNewData: Pointer): Boolean;
/// <INCLUDE .\\delphi\\zplay\\GetDynamicStreamLoad.txt>
    Procedure GetDynamicStreamLoad(out Load: TStreamLoadInfo);
/// <INCLUDE .\\delphi\\zplay\\Close.txt>
    Function Close(): Boolean;
/// <INCLUDE .\\delphi\\zplay\\LoadID3.txt>
    Function LoadID3(Id3Version: TID3Version; out Info: TID3Info): Boolean;
/// <INCLUDE .\\delphi\\zplay\\LoadID3W.txt>
    Function LoadID3W(Id3Version: TID3Version; out Info: TID3InfoW): Boolean;  
/// <INCLUDE .\\delphi\\zplay\\LoadID3Ex.txt>
   	Function LoadID3Ex(out Info: TID3InfoEx; fDecodeEmbededPicture: Boolean): Boolean;
/// <INCLUDE .\\delphi\\zplay\\LoadID3ExW.txt>
    Function LoadID3ExW(out Info: TID3InfoExW; fDecodeEmbededPicture: Boolean): Boolean;
/// <INCLUDE .\\delphi\\zplay\\LoadFileID3.txt>
    Function LoadFileID3(FileName: AnsiString; Format: TStreamFormat; Id3Version: TID3Version; out Info: TID3Info): Boolean;
/// <INCLUDE .\\delphi\\zplay\\LoadFileID3W.txt>
    Function LoadFileID3W(FileName: WideString; Format: TStreamFormat; Id3Version: TID3Version; out Info: TID3InfoW): Boolean;
/// <INCLUDE .\\delphi\\zplay\\LoadFileID3Ex.txt>   
    Function LoadFileID3Ex(FileName: AnsiString; Format: TStreamFormat;  out Info: TID3InfoEx; fDecodeEmbededPicture: Boolean): Boolean;
/// <INCLUDE .\\delphi\\zplay\\LoadFileID3ExW.txt>
    Function LoadFileID3ExW(FileName: WideString; Format: TStreamFormat;  out Info: TID3InfoExW; fDecodeEmbededPicture: Boolean): Boolean;
/// <INCLUDE .\\delphi\\zplay\\GetStatus.txt>
    Procedure GetStatus(out Status: TStreamStatus);
/// <INCLUDE .\\delphi\\zplay\\GetPosition.txt>
    Procedure GetPosition(out Position: TStreamTime);
/// <INCLUDE .\\delphi\\zplay\\StartPlayback.txt>
    Function StartPlayback(): Boolean;
/// <INCLUDE .\\delphi\\zplay\\StopPlayback.txt>
    Function StopPlayback(): Boolean;
/// <INCLUDE .\\delphi\\zplay\\PausePlayback.txt>
    Function PausePlayback(): Boolean;
/// <INCLUDE .\\delphi\\zplay\\ResumePlayback.txt>
    Function ResumePlayback(): Boolean;
/// <INCLUDE .\\delphi\\zplay\\Seek.txt>
    Function Seek(TimeFormat: TTimeFormat; var Position: TStreamTime; MoveMethod: TSeekMethod): Boolean;
/// <INCLUDE .\\delphi\\zplay\\PlayLoop.txt>
    Function PlayLoop(TimeFormatStart: TTimeFormat; var StartPosition: TStreamTime; TimeFormatEnd: TTimeFormat; var EndPosition: TStreamTime; NumOfCycles: Cardinal; ContinuePlaying: Cardinal): Boolean;
/// <INCLUDE .\\delphi\\zplay\\ReverseMode.txt>
    Function ReverseMode(Enable: Boolean): Boolean;
/// <INCLUDE .\\delphi\\zplay\\SlideVolume.txt>
    Function SlideVolume(TimeFormatStart: TTimeFormat;
                          TimeStart: TStreamTime;
                          StartVolumeLeft: Cardinal;
                          StartVolumeRight: Cardinal;
                          TimeFormatEnd: TTimeFormat;
                          TimeEnd: TStreamTime;
                          EndVolumeLeft: Cardinal;
                          EndVolumeRight: Cardinal): Boolean;

/// <INCLUDE .\\delphi\\zplay\\SetMasterVolume.txt>
    Function SetMasterVolume(LeftVolume: Cardinal; RightVolume: Cardinal): Boolean;
/// <INCLUDE .\\delphi\\zplay\\SetPlayerVolume.txt>
    Function SetPlayerVolume(LeftVolume: Cardinal; RightVolume: Cardinal): Boolean;
/// <INCLUDE .\\delphi\\zplay\\GetMasterVolume.txt>
    Procedure GetMasterVolume(out LeftVolume: Integer; out RightVolume: Integer);
/// <INCLUDE .\\delphi\\zplay\\GetPlayerVolume.txt>
    Procedure GetPlayerVolume(out LeftVolume: Integer; out RightVolume: Integer);
/// <INCLUDE .\\delphi\\zplay\\GetVUData.txt>
    Procedure GetVUData(out LeftChannel: Cardinal; out RightChannel: Cardinal);
/// <INCLUDE .\\delphi\\zplay\\GetBitrate.txt>
    Function GetBitrate(Average: Boolean): Integer;
/// <INCLUDE .\\delphi\\zplay\\EnableEcho.txt>
    Function EnableEcho(Enable: Boolean): Boolean;
/// <INCLUDE .\\delphi\\zplay\\SetEchoParam.txt>
    Function SetEchoParam(var EchoEffectArray: array of TEchoEffect; NumberOfEffects: Integer): Boolean;
/// <INCLUDE .\\delphi\\zplay\\GetEchoParam.txt>
    Function GetEchoParam(out EchoEffectArray: TDynamicEchoEffectArray): Integer;
/// <INCLUDE .\\delphi\\zplay\\EnableEqualizer.txt>
    Function EnableEqualizer(Enable: Boolean): Boolean;
/// <INCLUDE .\\delphi\\zplay\\SetEqualizerPoints.txt>
    Function SetEqualizerPoints(var FreqPointArray: array of Integer; NumberOfPoints: Integer): Boolean;
/// <INCLUDE .\\delphi\\zplay\\GetEqualizerPoints.txt>
    Function GetEqualizerPoints(out FreqPointArray: TDynamicIntegerArray): Integer;
/// <INCLUDE .\\delphi\\zplay\\SetEqualizerParam.txt>
    Function SetEqualizerParam(PreAmpGain: Integer; var BandGain: array of Integer; NumberOfBands:Integer): Boolean;
/// <INCLUDE .\\delphi\\zplay\\GetEqualizerParam.txt>
    Function GetEqualizerParam(out PreAmpGain: Integer; out BandGain: TDynamicIntegerArray): Integer;
/// <INCLUDE .\\delphi\\zplay\\SetEqualizerPreampGain.txt>
    Function SetEqualizerPreampGain(Gain: Integer): Boolean;
/// <INCLUDE .\\delphi\\zplay\\GetEqualizerPreampGain.txt>
    Function GetEqualizerPreampGain(): Integer;
/// <INCLUDE .\\delphi\\zplay\\SetEqualizerBandGain.txt>
    Function SetEqualizerBandGain(BandIndex: Integer; Gain: Integer): Boolean;
/// <INCLUDE .\\delphi\\zplay\\GetEqualizerBandGain.txt>
    Function GetEqualizerBandGain(BandIndex: Integer): Integer;
/// <INCLUDE .\\delphi\\zplay\\StereoCut.txt>
    Function StereoCut(Enable: Boolean; OutputCenter: Boolean; BassToSides: Boolean):Boolean;
/// <INCLUDE .\\delphi\\zplay\\MixChannels.txt>
    Function MixChannels(Enable: Boolean; LeftPercent: Cardinal; RightPercent: Cardinal): Boolean;
/// <INCLUDE .\\delphi\\zplay\\SetRate.txt>
    Function SetRate(Rate: Cardinal): Boolean;
/// <INCLUDE .\\delphi\\zplay\\GetRate.txt>
    Function GetRate(): Cardinal;
/// <INCLUDE .\\delphi\\zplay\\SetPitch.txt>
    Function SetPitch(Pitch: Cardinal): Boolean;
/// <INCLUDE .\\delphi\\zplay\\GetPitch.txt>
    Function GetPitch(): Cardinal;
/// <INCLUDE .\\delphi\\zplay\\SetTempo.txt>
    Function SetTempo(Tempo: Cardinal): Boolean;
/// <INCLUDE .\\delphi\\zplay\\GetTempo.txt>
    Function GetTempo(): Cardinal;
/// <INCLUDE .\\delphi\\zplay\\GetFFTData.txt>
    Function GetFFTData(FFTPoints: Integer; FFTWindow: TFFTWindow; out HarmonicNumber: Integer;
                         HarmonicFreq: TDynamicIntegerArray;  LeftAmplitude: TDynamicIntegerArray;
                          RightAmplitude: TDynamicIntegerArray;
                           LeftPhase: TDynamicIntegerArray;  RightPhase : TDynamicIntegerArray): Boolean;


/// <INCLUDE .\\delphi\\zplay\\DrawFFTGraphOnHDC.txt>
    Function DrawFFTGraphOnHDC(hdc: HDC; X: Integer; Y: Integer; Width: Integer; Height: Integer): Boolean;
/// <INCLUDE .\\delphi\\zplay\\DrawFFTGraphOnHWND.txt>
    Function DrawFFTGraphOnHWND(hwnd: HWND; X: Integer; Y: Integer; Width: Integer; Height: Integer): Boolean;
/// <INCLUDE .\\delphi\\zplay\\SetFFTGraphParam.txt>
    Function SetFFTGraphParam(ParamID: TFFTGraphParamID; ParamValue: Integer): Boolean;
 /// <INCLUDE .\\delphi\\zplay\\GetFFTGraphParam.txt>   
    Function GetFFTGraphParam(ParamID: TFFTGraphParamID): Integer;


/// <INCLUDE .\\delphi\\zplay\\DetectBPM.txt>
    Function DetectBPM(Method: TBMPDetectionMethod): Integer;
/// <INCLUDE .\\delphi\\zplay\\DetectFileBPM.txt>
    Function DetectFileBPM(const FileName: AnsiString; Format: TStreamFormat; Method: TBMPDetectionMethod): Integer;
/// <INCLUDE .\\delphi\\zplay\\DetectFileBPMW.txt>
    Function DetectFileBPMW(const FileName: WideString; Format: TStreamFormat; Method: TBMPDetectionMethod): Integer;



  published
    { Published declarations }

  end;

end.