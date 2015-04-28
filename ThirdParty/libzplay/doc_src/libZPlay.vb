Namespace libZPlayVB


''' <INCLUDE .\\vb\\zplay\\TCallbackFunc.txt>
Public Delegate Function TCallbackFunc(ByVal objptr As UInteger, ByVal user_data As Integer, ByVal msg As TCallbackMessage, ByVal param1 As UInteger, ByVal param2 As UInteger) As Integer



''' <INCLUDE .\\vb\\zplay\\TSettingID.txt>
Public Enum TSettingID As Integer

    sidWaveBufferSize = 1
    sidAccurateLength = 2
    sidAccurateSeek = 3
    sidSamplerate = 4
    sidChannelNumber = 5
    sidBitPerSample = 6
    sidBigEndian = 7
End Enum

''' <INCLUDE .\\vb\\zplay\\TStreamFormat.txt>
Public Enum TStreamFormat As Integer

    sfUnknown = 0
    sfMp3 = 1
    sfOgg = 2
    sfWav = 3
    sfPCM = 4
    sfFLAC = 5
    sfFLACOgg = 6
    sfAC3 = 7
    sfAacADTS = 8
    sfWaveIn = 9
    sfAutodetect = 1000
End Enum


''' <INCLUDE .\\vb\\zplay\\TStreamInfo.txt>
Public Structure TStreamInfo

        Public SamplingRate As Integer
        Public ChannelNumber As Integer
        Public VBR As Boolean
        Public Bitrate As Integer
        Public Length As TStreamTime
        Public Description As String
End Structure


''' <INCLUDE .\\vb\\zplay\\TWaveOutInfo.txt>
Public Structure TWaveOutInfo

        Public ManufacturerID As UInteger
        Public ProductID As UInteger
        Public DriverVersion As UInteger
        Public Formats As UInteger
        Public Channels As UInteger
        Public Support As UInteger
        Public ProductName As String
End Structure



''' <INCLUDE .\\vb\\zplay\\TWaveInInfo.txt>
Public Structure TWaveInInfo

        Public ManufacturerID As UInteger
        Public ProductID As UInteger
        Public DriverVersion As UInteger
        Public Formats As UInteger
        Public Channels As UInteger
        Public ProductName As String
End Structure



''' <INCLUDE .\\vb\\zplay\\TFFTWindow.txt>
Public Enum TFFTWindow As Integer

    fwRectangular = 1
    fwHamming
    fwHann
    fwCosine
    fwLanczos
    fwBartlett
    fwTriangular
    fwGauss
    fwBartlettHann
    fwBlackman
    fwNuttall
    fwBlackmanHarris
    fwBlackmanNuttall
    fwFlatTop
End Enum




''' <INCLUDE .\\vb\\zplay\\TTimeFormat.txt>
Public Enum TTimeFormat As UInteger

    tfMillisecond = 1
    tfSecond = 2
    tfHMS = 4
    tfSamples = 8
End Enum

''' <INCLUDE .\\vb\\zplay\\TSeekMethod.txt>
Public Enum TSeekMethod As Integer

    smFromBeginning = 1
    smFromEnd = 2
    smFromCurrentForward = 4
    smFromCurrentBackward = 8
End Enum

''' <INCLUDE .\\vb\\zplay\\TStreamLoadInfo.txt>
Public Structure TStreamLoadInfo

        Public NumberOfBuffers As UInteger
        Public NumberOfBytes As UInteger
End Structure


''' <INCLUDE .\\vb\\zplay\\TEchoEffect.txt>
Public Structure TEchoEffect

        Public nLeftDelay As Integer
        Public nLeftSrcVolume As Integer
        Public nLeftEchoVolume As Integer
        Public nRightDelay As Integer
        Public nRightSrcVolume As Integer
        Public nRightEchoVolume As Integer
End Structure


''' <INCLUDE .\\vb\\zplay\\TID3Version.txt>
Public Enum TID3Version As Integer

    id3Version1 = 1
    id3Version2 = 2
End Enum


''' <INCLUDE .\\vb\\zplay\\TFFTGraphHorizontalScale.txt>
Public Enum TFFTGraphHorizontalScale As Integer

    gsLogarithmic = 0
    gsLinear = 1
End Enum

''' <INCLUDE .\\vb\\zplay\\TFFTGraphParamID.txt>
Public Enum TFFTGraphParamID As Integer

    gpFFTPoints = 1
    gpGraphType
    gpWindow
    gpHorizontalScale
    gpSubgrid
    gpTransparency
    gpFrequencyScaleVisible
    gpDecibelScaleVisible
    gpFrequencyGridVisible
    gpDecibelGridVisible
    gpBgBitmapVisible
    gpBgBitmapHandle
    gpColor1
    gpColor2
    gpColor3
    gpColor4
    gpColor5
    gpColor6
    gpColor7
    gpColor8
    gpColor9
    gpColor10
    gpColor11
    gpColor12
    gpColor13
    gpColor14
    gpColor15
    gpColor16
End Enum

''' <INCLUDE .\\vb\\zplay\\TFFTGraphType.txt>
Public Enum TFFTGraphType As Integer

    gtLinesLeftOnTop = 0
    gtLinesRightOnTop
    gtAreaLeftOnTop
    gtAreaRightOnTop
    gtBarsLeftOnTop
    gtBarsRightOnTop
    gtSpectrum
End Enum


''' <INCLUDE .\\vb\\zplay\\TStreamStatus.txt>
Public Structure TStreamStatus

        Public fPlay As Boolean
        Public fPause As Boolean
        Public fEcho As Boolean
        Public fEqualizer As Boolean
        Public fVocalCut As Boolean
        Public fSideCut As Boolean
        Public fChannelMix As Boolean
        Public fSlideVolume As Boolean
        Public nLoop As Integer
        Public fReverse As Boolean
        Public nSongIndex As Integer
        Public nSongsInQueue As Integer
End Structure

''' <INCLUDE .\\vb\\zplay\\TStreamHMSTime.txt>
Public Structure TStreamHMSTime

        Public hour As UInteger
        Public minute As UInteger
        Public second As UInteger
        Public millisecond As UInteger
End Structure

''' <INCLUDE .\\vb\\zplay\\TStreamTime.txt>
Public Structure TStreamTime

        Public sec As UInteger
        Public ms As UInteger
        Public samples As UInteger
        Public hms As TStreamHMSTime
End Structure

''' <INCLUDE .\\vb\\zplay\\TID3Info.txt>
Public Structure TID3Info

        Public Title As String
        Public Artist As String
        Public Album As String
        Public Year As String
        Public Comment As String
        Public Track As String
        Public Genre As String
End Structure

''' <INCLUDE .\\vb\\zplay\\TID3Picture.txt>
Public Structure TID3Picture

        Public PicturePresent As Boolean
        Public PictureType As Integer
        Public Description As String
        Public Bitmap As Bitmap
        Public BitStream As System.IO.MemoryStream
End Structure

''' <INCLUDE .\\vb\\zplay\\TID3InfoEx.txt>
Public Structure TID3InfoEx

        Public Title As String
        Public Artist As String
        Public Album As String
        Public Year As String
        Public Comment As String
        Public Track As String
        Public Genre As String
        Public AlbumArtist As String
        Public Composer As String
        Public OriginalArtist As String
        Public Copyright As String
        Public URL As String
        Public Encoder As String
        Public Publisher As String
        Public BPM As Integer
        Public Picture As TID3Picture
End Structure

''' <INCLUDE .\\vb\\zplay\\TBPMDetectionMethod.txt>
Public Enum TBPMDetectionMethod As Integer

    dmPeaks = 0
    dmAutoCorrelation
End Enum


''' <INCLUDE .\\vb\\zplay\\TFFTGraphSize.txt>
Public Enum TFFTGraphSize As Integer

    FFTGraphMinWidth = 100
    FFTGraphMinHeight = 60
End Enum

''' <INCLUDE .\\vb\\zplay\\TWaveOutMapper.txt>
Public Enum TWaveOutMapper As UInteger

    WaveOutWaveMapper = 4294967295
End Enum

''' <INCLUDE .\\vb\\zplay\\TWaveInMapper.txt>
Public Enum TWaveInMapper As UInteger

    WaveInWaveMapper = 4294967295
End Enum

''' <INCLUDE .\\vb\\zplay\\TCallbackMessage.txt>
Public Enum TCallbackMessage As Integer

    MsgStopAsync = 1
    MsgPlayAsync = 2
    MsgEnterLoopAsync = 4
    MsgExitLoopAsync = 8
    MsgEnterVolumeSlideAsync = 16
    MsgExitVolumeSlideAsync = 32
    MsgStreamBufferDoneAsync = 64
    MsgStreamNeedMoreDataAsync = 128
    MsgNextSongAsync = 256
    MsgStop = 65536
    MsgPlay = 131072
    MsgEnterLoop = 262144
    MsgExitLoop = 524288
    MsgEnterVolumeSlide = 1048576
    MsgExitVolumeSlide = 2097152
    MsgStreamBufferDone = 4194304
    MsgStreamNeedMoreData = 8388608
    MsgNextSong = 16777216
    MsgWaveBuffer = 33554432
End Enum


''' <INCLUDE .\\vb\\zplay\\TWaveOutFormat.txt>
Public Enum TWaveOutFormat As UInteger

    format_invalid = 0
    format_11khz_8bit_mono = 1
    format_11khz_8bit_stereo = 2
    format_11khz_16bit_mono = 4
    format_11khz_16bit_stereo = 8
    format_11khz_16bit_stereo
    format_22khz_8bit_mono = 16
    format_22khz_8bit_stereo = 32
    format_22khz_16bit_mono = 64
    format_22khz_16bit_stereo = 128
    format_22khz_16bit_stereo
    format_44khz_8bit_mono = 256
    format_44khz_8bit_stereo = 512
    format_44khz_16bit_mono = 1024
    format_44khz_16bit_stereo = 2048
End Enum

''' <INCLUDE .\\vb\\zplay\\TWaveOutFunctionality.txt>
Public Enum TWaveOutFunctionality As UInteger

    supportPitchControl = 1
    supportPlaybackRateControl = 2
    supportVolumeControl = 4
    supportSeparateLeftRightVolume = 8
    supportSync = 16
    supportSampleAccuratePosition = 32
    supportDirectSound = 6
End Enum


''' <INCLUDE .\\vb\\zplay\\zplay.txt>   
Public Class ZPlay


''' <INCLUDE .\\vb\\zplay\\New.txt>
Public Sub New()
End Sub

''' <INCLUDE .\\vb\\zplay\\Finalize.txt>
Protected Overrides Sub Finalize()
End Sub

''' <INCLUDE .\\vb\\zplay\\GetVersion.txt>
Public Function GetVersion() As Integer
End Function

''' <INCLUDE .\\vb\\zplay\\GetError.txt>
Public Function GetError() As String
End Function

''' <INCLUDE .\\vb\\zplay\\GetFileFormat.txt>
Public Function GetFileFormat(ByVal FileName As String) As TStreamFormat
End Function

''' <INCLUDE .\\vb\\zplay\\OpenFile.txt>
Public Function OpenFile(ByVal FileName As String, ByVal Format As TStreamFormat) As Boolean
End Function

''' <INCLUDE .\\vb\\zplay\\SetWaveOutFile.txt>
Public Function SetWaveOutFile(ByVal FileName As String, ByVal Format As TStreamFormat, ByRef fOutputToSoundcard As Boolean) As Boolean
End Function

''' <INCLUDE .\\vb\\zplay\\AddFile.txt>
Public Function AddFile(ByVal FileName As String, ByVal Format As TStreamFormat) As Boolean
End Function

''' <INCLUDE .\\vb\\zplay\\OpenStream.txt>
Public Function OpenStream(ByVal Buffered As Boolean, ByVal Dynamic As Boolean, ByRef MemStream() As Byte, ByVal StreamSize As UInteger, ByVal nFormat As TStreamFormat) As Boolean
End Function


''' <INCLUDE .\\vb\\zplay\\PushDataToStream.txt>
Public Function PushDataToStream(ByRef MemNewData() As Byte, ByVal NewDatSize As UInteger) As Boolean
End Function

''' <INCLUDE .\\vb\\zplay\\IsStreamDataFree.txt>
Public Function IsStreamDataFree(ByRef MemNewData() As Byte) As Boolean
End Function

''' <INCLUDE .\\vb\\zplay\\Close.txt>
Public Function Close() As Boolean
End Function

''' <INCLUDE .\\vb\\zplay\\GetPosition.txt>
Public Sub GetPosition(ByRef time As TStreamTime)
End Sub

''' <INCLUDE .\\vb\\zplay\\Seek.txt>
Public Function Seek(ByVal TimeFormat As TTimeFormat, ByRef Position As TStreamTime, ByVal MoveMethod As TSeekMethod) As Boolean
End Function

''' <INCLUDE .\\vb\\zplay\\ReverseMode.txt>
Public Function ReverseMode(ByVal Enable As Boolean) As Boolean
End Function

''' <INCLUDE .\\vb\\zplay\\PlayLoop.txt>
Public Function PlayLoop(ByVal TimeFormatStart As TTimeFormat, ByRef StartPosition As TStreamTime, ByVal TimeFormatEnd As TTimeFormat, ByRef EndPosition As TStreamTime, ByVal NumberOfCycles As UInteger, ByVal ContinuePlaying As Boolean) As Boolean
End Function

''' <INCLUDE .\\vb\\zplay\\StartPlayback.txt>
Public Function StartPlayback() As Boolean
End Function

''' <INCLUDE .\\vb\\zplay\\StopPlayback.txt>
Public Function StopPlayback() As Boolean
End Function

''' <INCLUDE .\\vb\\zplay\\PausePlayback.txt>
Public Function PausePlayback() As Boolean
End Function

''' <INCLUDE .\\vb\\zplay\\ResumePlayback.txt>
Public Function ResumePlayback() As Boolean
End Function

''' <INCLUDE .\\vb\\zplay\\SetEqualizerParam.txt>
Public Function SetEqualizerParam(ByVal PreAmpGain As Integer, ByRef BandGain() As Integer, ByVal NumberOfBands As Integer) As Boolean
End Function

''' <INCLUDE .\\vb\\zplay\\GetEqualizerParam.txt>
Public Function GetEqualizerParam(ByRef PreAmpGain As Integer, ByRef BandGain() As Integer) As Integer
End Function

''' <INCLUDE .\\vb\\zplay\\EnableEqualizer.txt>
Public Function EnableEqualizer(ByVal Enable As Boolean) As Boolean
End Function

''' <INCLUDE .\\vb\\zplay\\SetEqualizerPreampGain.txt>
Public Function SetEqualizerPreampGain(ByVal Gain As Integer) As Boolean
End Function

''' <INCLUDE .\\vb\\zplay\\GetEqualizerPreampGain.txt>
Public Function GetEqualizerPreampGain() As Integer
End Function

''' <INCLUDE .\\vb\\zplay\\SetEqualizerBandGain.txt>
Public Function SetEqualizerBandGain(ByVal BandIndex As Integer, ByVal Gain As Integer) As Boolean
End Function

''' <INCLUDE .\\vb\\zplay\\GetEqualizerBandGain.txt>
Public Function GetEqualizerBandGain(ByVal BandIndex As Integer) As Integer
End Function

''' <INCLUDE .\\vb\\zplay\\SetEqualizerPoints.txt>
Public Function SetEqualizerPoints(ByRef FreqPointArray() As Integer, ByVal NumberOfPoints As Integer) As Boolean
End Function

''' <INCLUDE .\\vb\\zplay\\GetEqualizerPoints.txt>
Public Function GetEqualizerPoints(ByRef FreqPointArray() As Integer) As Integer
End Function

''' <INCLUDE .\\vb\\zplay\\EnableEcho.txt>
Public Function EnableEcho(ByVal Enable As Boolean) As Boolean
End Function

''' <INCLUDE .\\vb\\zplay\\SetEchoParam.txt>
Public Function SetEchoParam(ByRef EchoEffectArray() As TEchoEffect, ByVal NumberOfEffects As Integer) As Boolean
End Function

''' <INCLUDE .\\vb\\zplay\\GetEchoParam.txt>
Public Function GetEchoParam(ByRef EchoEffectArray() As TEchoEffect) As Integer
End Function

''' <INCLUDE .\\vb\\zplay\\SetMasterVolume.txt>
Public Function SetMasterVolume(ByVal LeftVolume As Integer, ByVal RightVolume As Integer) As Boolean
End Function

''' <INCLUDE .\\vb\\zplay\\SetPlayerVolume.txt>
Public Function SetPlayerVolume(ByVal LeftVolume As Integer, ByVal RightVolume As Integer) As Boolean
End Function


''' <INCLUDE .\\vb\\zplay\\GetMasterVolume.txt>
Public Sub GetMasterVolume(ByRef LeftVolume As Integer, ByRef RightVolume As Integer)
End Sub

''' <INCLUDE .\\vb\\zplay\\GetPlayerVolume.txt>
Public Sub GetPlayerVolume(ByRef LeftVolume As Integer, ByRef RightVolume As Integer)
End Sub

''' <INCLUDE .\\vb\\zplay\\SlideVolume.txt>
Public Function SlideVolume(ByVal TimeFormatStart As TTimeFormat, ByRef TimeStart As TStreamTime, ByVal StartVolumeLeft As Integer, ByVal StartVolumeRight As Integer, ByVal TimeFormatEnd As TTimeFormat, ByRef TimeEnd As TStreamTime, ByVal EndVolumeLeft As Integer, ByVal EndVolumeRight As Integer) As Boolean
End Function

''' <INCLUDE .\\vb\\zplay\\SetPitch.txt>
Public Function SetPitch(ByVal Pitch As Integer) As Boolean
End Function

''' <INCLUDE .\\vb\\zplay\\GetPitch.txt>
Public Function GetPitch() As Integer
End Function

''' <INCLUDE .\\vb\\zplay\\SetRate.txt>
Public Function SetRate(ByVal Rate As Integer) As Boolean
End Function

''' <INCLUDE .\\vb\\zplay\\GetRate.txt>
Public Function GetRate() As Integer
End Function

''' <INCLUDE .\\vb\\zplay\\SetTempo.txt>
Public Function SetTempo(ByVal Tempo As Integer) As Boolean
End Function

''' <INCLUDE .\\vb\\zplay\\GetTempo.txt>
Public Function GetTempo() As Integer
End Function

''' <INCLUDE .\\vb\\zplay\\GetBitrate.txt>
Public Function GetBitrate(ByVal Average As Boolean) As Integer
End Function

''' <INCLUDE .\\vb\\zplay\\LoadID3.txt>
Public Function LoadID3(ByVal Id3Version As TID3Version, ByRef Info As TID3Info) As Boolean
End Function

''' <INCLUDE .\\vb\\zplay\\LoadID3Ex.txt>
Public Function LoadID3Ex(ByRef Info As TID3InfoEx, ByVal fDecodePicture As Boolean) As Boolean
End Function
 
''' <INCLUDE .\\vb\\zplay\\LoadFileID3.txt>
Public Function LoadFileID3(ByVal FileName As String, ByVal Format As TStreamFormat, ByVal Id3Version As TID3Version, ByRef Info As TID3Info) As Boolean
End Function

''' <INCLUDE .\\vb\\zplay\\LoadFileID3Ex.txt>
Public Function LoadFileID3Ex(ByVal FileName As String, ByVal Format As TStreamFormat, ByRef Info As TID3InfoEx, ByVal fDecodePicture As Boolean) As Boolean
End Function

''' <INCLUDE .\\vb\\zplay\\SetCallbackFunc.txt>
Public Function SetCallbackFunc(ByVal CallbackFunc As TCallbackFunc, ByVal Messages As TCallbackMessage, ByVal UserData As Integer) As Boolean
End Function

''' <INCLUDE .\\vb\\zplay\\DetectBPM.txt>
Public Function DetectBPM(ByVal Method As TBPMDetectionMethod) As Integer
End Function

''' <INCLUDE .\\vb\\zplay\\DetectFileBPM.txt>
Public Function DetectFileBPM(ByVal FileName As String, ByVal Format As TStreamFormat, ByVal Method As TBPMDetectionMethod) As Integer
End Function

''' <INCLUDE .\\vb\\zplay\\GetFFTData.txt>
Public Function GetFFTData(ByVal FFTPoints As Integer, ByVal FFTWindow As TFFTWindow, ByRef HarmonicNumber As Integer, ByRef HarmonicFreq() As Integer, ByRef LeftAmplitude() As Integer, ByRef RightAmplitude() As Integer, ByRef LeftPhase() As Integer, ByRef RightPhase() As Integer) As Boolean
End Function

''' <INCLUDE .\\vb\\zplay\\DrawFFTGraphOnHDC.txt>
Public Function DrawFFTGraphOnHDC(ByVal hdc As System.IntPtr, ByVal X As Integer, ByVal Y As Integer, ByVal Width As Integer, ByVal Height As Integer) As Boolean
End Function

''' <INCLUDE .\\vb\\zplay\\DrawFFTGraphOnHWND.txt>
Public Function DrawFFTGraphOnHWND(ByVal hwnd As System.IntPtr, ByVal X As Integer, ByVal Y As Integer, ByVal Width As Integer, ByVal Height As Integer) As Boolean
End Function

''' <INCLUDE .\\vb\\zplay\\SetFFTGraphParam.txt>
Public Function SetFFTGraphParam(ByVal ParamID As TFFTGraphParamID, ByVal ParamValue As Integer) As Boolean
End Function

''' <INCLUDE .\\vb\\zplay\\GetFFTGraphParam.txt>
Public Function GetFFTGraphParam(ByVal ParamID As TFFTGraphParamID) As Integer
End Function

''' <INCLUDE .\\vb\\zplay\\StereoCut.txt>
Public Function StereoCut(ByVal Enable As Boolean, ByVal OutputCenter As Boolean, ByVal BassToSides As Boolean) As Boolean
End Function

''' <INCLUDE .\\vb\\zplay\\MixChannels.txt>
Public Function MixChannels(ByVal Enable As Boolean, ByVal LeftPercent As UInteger, ByVal RightPercent As UInteger) As Boolean
End Function

''' <INCLUDE .\\vb\\zplay\\GetVUData.txt>
Public Sub GetVUData(ByRef LeftChannel As Integer, ByRef RightChannel As Integer)
End Sub

''' <INCLUDE .\\vb\\zplay\\GetStreamInfo.txt>
Public Sub GetStreamInfo(ByRef info As TStreamInfo)
End Sub

''' <INCLUDE .\\vb\\zplay\\GetStatus.txt>
Public Sub GetStatus(ByRef status As TStreamStatus)
End Sub

''' <INCLUDE .\\vb\\zplay\\GetDynamicStreamLoad.txt>
Public Sub GetDynamicStreamLoad(ByRef StreamLoadInfo As TStreamLoadInfo)
End Sub

''' <INCLUDE .\\vb\\zplay\\EnumerateWaveOut.txt>
Public Function EnumerateWaveOut() As Integer
End Function

''' <INCLUDE .\\vb\\zplay\\GetWaveOutInfo.txt>
Public Function GetWaveOutInfo(ByVal Index As UInteger, ByRef Info As TWaveOutInfo) As Boolean
End Function

''' <INCLUDE .\\vb\\zplay\\SetWaveOutDevice.txt>
Public Function SetWaveOutDevice(ByVal Index As UInteger) As Boolean
End Function

''' <INCLUDE .\\vb\\zplay\\EnumerateWaveIn.txt>
Public Function EnumerateWaveIn() As Integer
End Function

''' <INCLUDE .\\vb\\zplay\\GetWaveInInfo.txt>
Public Function GetWaveInInfo(ByVal Index As UInteger, ByRef Info As TWaveInInfo) As Boolean
End Function

''' <INCLUDE .\\vb\\zplay\\SetWaveInDevice.txt>
Public Function SetWaveInDevice(ByVal Index As UInteger) As Boolean
End Function

''' <INCLUDE .\\vb\\zplay\\SetSettings.txt>
Public Function SetSettings(ByVal SettingID As TSettingID, ByVal Value As Integer) As Integer
End Function

''' <INCLUDE .\\vb\\zplay\\GetSettings.txt>
Public Function GetSettings(ByVal SettingID As TSettingID) As Integer
End Function


End Class

End Namespace