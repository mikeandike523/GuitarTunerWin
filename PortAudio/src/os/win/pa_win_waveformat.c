 
#include <windows.h>
#include <mmsystem.h>
#if defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_APP)
#include <mmreg.h>  
#endif
#include "portaudio.h"
#include "pa_win_waveformat.h"
#if !defined(WAVE_FORMAT_EXTENSIBLE)
#define  WAVE_FORMAT_EXTENSIBLE         0xFFFE
#endif
static GUID pawin_ksDataFormatSubtypeGuidBase =
{ (USHORT)(WAVE_FORMAT_PCM), 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 };
int PaWin_SampleFormatToLinearWaveFormatTag( PaSampleFormat sampleFormat )
{
if( sampleFormat == paFloat32 )
return PAWIN_WAVE_FORMAT_IEEE_FLOAT;
return PAWIN_WAVE_FORMAT_PCM;
}
void PaWin_InitializeWaveFormatEx( PaWinWaveFormat *waveFormat,
int numChannels, PaSampleFormat sampleFormat, int waveFormatTag, double sampleRate )
{
WAVEFORMATEX *waveFormatEx = (WAVEFORMATEX*)waveFormat;
int bytesPerSample = Pa_GetSampleSize(sampleFormat);
unsigned long bytesPerFrame = numChannels * bytesPerSample;
waveFormatEx->wFormatTag = waveFormatTag;
waveFormatEx->nChannels = (WORD)numChannels;
waveFormatEx->nSamplesPerSec = (DWORD)sampleRate;
waveFormatEx->nAvgBytesPerSec = waveFormatEx->nSamplesPerSec * bytesPerFrame;
waveFormatEx->nBlockAlign = (WORD)bytesPerFrame;
waveFormatEx->wBitsPerSample = bytesPerSample * 8;
waveFormatEx->cbSize = 0;
}
void PaWin_InitializeWaveFormatExtensible( PaWinWaveFormat *waveFormat,
int numChannels, PaSampleFormat sampleFormat, int waveFormatTag, double sampleRate,
PaWinWaveFormatChannelMask channelMask )
{
WAVEFORMATEX *waveFormatEx = (WAVEFORMATEX*)waveFormat;
int bytesPerSample = Pa_GetSampleSize(sampleFormat);
unsigned long bytesPerFrame = numChannels * bytesPerSample;
GUID guid;
waveFormatEx->wFormatTag = WAVE_FORMAT_EXTENSIBLE;
waveFormatEx->nChannels = (WORD)numChannels;
waveFormatEx->nSamplesPerSec = (DWORD)sampleRate;
waveFormatEx->nAvgBytesPerSec = waveFormatEx->nSamplesPerSec * bytesPerFrame;
waveFormatEx->nBlockAlign = (WORD)bytesPerFrame;
waveFormatEx->wBitsPerSample = bytesPerSample * 8;
waveFormatEx->cbSize = 22;
memcpy(&waveFormat->fields[PAWIN_INDEXOF_WVALIDBITSPERSAMPLE],
&waveFormatEx->wBitsPerSample, sizeof(WORD));
memcpy(&waveFormat->fields[PAWIN_INDEXOF_DWCHANNELMASK],
&channelMask, sizeof(DWORD));
guid = pawin_ksDataFormatSubtypeGuidBase;
guid.Data1 = (USHORT)waveFormatTag;
memcpy(&waveFormat->fields[PAWIN_INDEXOF_SUBFORMAT], &guid, sizeof(GUID));
}
PaWinWaveFormatChannelMask PaWin_DefaultChannelMask( int numChannels )
{
switch( numChannels ){
case 1:
return PAWIN_SPEAKER_MONO;
case 2:
return PAWIN_SPEAKER_STEREO;
case 3:
return PAWIN_SPEAKER_FRONT_LEFT | PAWIN_SPEAKER_FRONT_CENTER | PAWIN_SPEAKER_FRONT_RIGHT;
case 4:
return PAWIN_SPEAKER_QUAD;
case 5:
return PAWIN_SPEAKER_QUAD | PAWIN_SPEAKER_FRONT_CENTER;
case 6:
return PAWIN_SPEAKER_5POINT1;
case 8:
return PAWIN_SPEAKER_7POINT1_SURROUND;
}
return  PAWIN_SPEAKER_DIRECTOUT;
}
