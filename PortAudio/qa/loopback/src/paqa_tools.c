
#include "paqa_tools.h"
void PaQa_ListAudioDevices(void)
{
int     i, numDevices;
const   PaDeviceInfo *deviceInfo;
numDevices = Pa_GetDeviceCount();
for( i=0; i<numDevices; i++ )
{
deviceInfo = Pa_GetDeviceInfo( i );
printf( "#%d: ", i );
printf( "%2d in", deviceInfo->maxInputChannels  );
printf( ", %2d out", deviceInfo->maxOutputChannels  );
printf( ",  %s", deviceInfo->name );
printf( ", on %s\n",  Pa_GetHostApiInfo( deviceInfo->hostApi )->name );
}
}
void PaQa_ConvertToFloat( const void *input, int numSamples, PaSampleFormat inFormat, float *output )
{
int i;
switch( inFormat )
{
case paUInt8:
{
unsigned char *data = (unsigned char *)input;
for( i=0; i<numSamples; i++ )
{
int value = *data++;
value -= 128;
*output++ = value / 128.0f;
}
}
break;
case paInt8:
{
char *data = (char *)input;
for( i=0; i<numSamples; i++ )
{
int value = *data++;
*output++ = value / 128.0f;
}
}
break;
case paInt16:
{
short *data = (short *)input;
for( i=0; i<numSamples; i++ )
{
*output++ = *data++ / 32768.0f;
}
}
break;
case paInt32:
{
int *data = (int *)input;
for( i=0; i<numSamples; i++ )
{
int value = (*data++) >> 8;
float fval = (float) (value / ((double) 0x00800000));
*output++ = fval;
}
}
break;
}
}
void PaQa_ConvertFromFloat( const float *input, int numSamples, PaSampleFormat outFormat, void *output )
{
int i;
switch( outFormat )
{
case paUInt8:
{
unsigned char *data = (unsigned char *)output;
for( i=0; i<numSamples; i++ )
{
float value = *input++;
int byte = ((int) (value * 127)) + 128;
*data++ = (unsigned char) byte;
}
}
break;
case paInt8:
{
char *data = (char *)output;
for( i=0; i<numSamples; i++ )
{
float value = *input++;
int byte = (int) (value * 127);
*data++ = (char) byte;
}
}
break;
case paInt16:
{
short *data = (short *)output;
for( i=0; i<numSamples; i++ )
{
float value = *input++;
short sval = value * 32767.0;
*data++ = sval;
}
}
break;
case paInt32:
{
int *data = (int *)output;
for( i=0; i<numSamples; i++ )
{
float value = *input++;
int ival = value * ((double) 0x007FFFF0);
ival = ival << 8;
*data++ = ival;
}
}
break;
}
}
