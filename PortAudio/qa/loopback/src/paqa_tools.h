
#ifndef _PAQA_TOOLS_H
#define _PAQA_TOOLS_H
#include <stdio.h>
#include "portaudio.h"
void PaQa_ListAudioDevices(void);
void PaQa_ConvertToFloat( const void *input, int numSamples, PaSampleFormat inFormat, float *output );
void PaQa_ConvertFromFloat( const float *input, int numSamples, PaSampleFormat outFormat, void *output );
#endif  
