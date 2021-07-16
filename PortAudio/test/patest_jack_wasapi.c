 
#include <stdio.h>
#include "portaudio.h"
#include "pa_win_wasapi.h"
static int IsInMask(int val, int val2)
{
return ((val & val2) == val2);
}
static void EnumIJackChannels(int channelMapping)
{
printf("Channel Mapping: ");
if(channelMapping == PAWIN_SPEAKER_DIRECTOUT)
{
printf("DIRECTOUT\n");
return;
}
if(IsInMask(channelMapping, PAWIN_SPEAKER_FRONT_LEFT))
printf("FRONT_LEFT, ");
if(IsInMask(channelMapping, PAWIN_SPEAKER_FRONT_RIGHT))
printf("FRONT_RIGHT, ");
if(IsInMask(channelMapping, PAWIN_SPEAKER_FRONT_CENTER))
printf("FRONT_CENTER, ");
if(IsInMask(channelMapping, PAWIN_SPEAKER_LOW_FREQUENCY))
printf("LOW_FREQUENCY, ");
if(IsInMask(channelMapping, PAWIN_SPEAKER_BACK_LEFT))
printf("BACK_LEFT, ");
if(IsInMask(channelMapping,PAWIN_SPEAKER_BACK_RIGHT))
printf("BACK_RIGHT, ");
if(IsInMask(channelMapping, PAWIN_SPEAKER_FRONT_LEFT_OF_CENTER))
printf("FRONT_LEFT_OF_CENTER, ");
if(IsInMask(channelMapping, PAWIN_SPEAKER_FRONT_RIGHT_OF_CENTER))
printf("FRONT_RIGHT_OF_CENTER, ");
if(IsInMask(channelMapping, PAWIN_SPEAKER_BACK_CENTER))
printf("BACK_CENTER, ");
if(IsInMask(channelMapping,PAWIN_SPEAKER_SIDE_LEFT))
printf("SIDE_LEFT, ");
if(IsInMask(channelMapping,PAWIN_SPEAKER_SIDE_RIGHT))
printf("SIDE_RIGHT, ");
if(IsInMask(channelMapping, PAWIN_SPEAKER_TOP_CENTER))
printf("TOP_CENTER, ");
if(IsInMask(channelMapping, PAWIN_SPEAKER_TOP_FRONT_LEFT))
printf("TOP_FRONT_LEFT, ");
if(IsInMask(channelMapping, PAWIN_SPEAKER_TOP_FRONT_CENTER))
printf("TOP_FRONT_CENTER, ");
if(IsInMask(channelMapping, PAWIN_SPEAKER_TOP_FRONT_RIGHT))
printf("TOP_FRONT_RIGHT, ");
if(IsInMask(channelMapping, PAWIN_SPEAKER_TOP_BACK_LEFT))
printf("TOP_BACK_LEFT, ");
if(IsInMask(channelMapping, PAWIN_SPEAKER_TOP_BACK_CENTER))
printf("TOP_BACK_CENTER, ");
if(IsInMask(channelMapping, PAWIN_SPEAKER_TOP_BACK_RIGHT))
printf("TOP_BACK_RIGHT, ");
printf("\n");
}
static void EnumIJackConnectionType(int cType)
{
printf("Connection Type: ");
switch(cType)
{
case eJackConnTypeUnknown:
printf("eJackConnTypeUnknown");
break;
case eJackConnType3Point5mm:
printf("eJackConnType3Point5mm");
break;
case eJackConnTypeQuarter:
printf("eJackConnTypeQuarter");
break;
case eJackConnTypeAtapiInternal:
printf("eJackConnTypeAtapiInternal");
break;
case eJackConnTypeRCA:
printf("eJackConnTypeRCA");
break;
case eJackConnTypeOptical:
printf("eJackConnTypeOptical");
break;
case eJackConnTypeOtherDigital:
printf("eJackConnTypeOtherDigital");
break;
case eJackConnTypeOtherAnalog:
printf("eJackConnTypeOtherAnalog");
break;
case eJackConnTypeMultichannelAnalogDIN:
printf("eJackConnTypeMultichannelAnalogDIN");
break;
case eJackConnTypeXlrProfessional:
printf("eJackConnTypeXlrProfessional");
break;
case eJackConnTypeRJ11Modem:
printf("eJackConnTypeRJ11Modem");
break;
case eJackConnTypeCombination:
printf("eJackConnTypeCombination");
break;
}
printf("\n");
}
static void EnumIJackGeoLocation(int iVal)
{
printf("Geometric Location: ");
switch(iVal)
{
case eJackGeoLocRear:
printf("eJackGeoLocRear");
break;
case eJackGeoLocFront:
printf("eJackGeoLocFront");
break;
case eJackGeoLocLeft:
printf("eJackGeoLocLeft");
break;
case eJackGeoLocRight:
printf("eJackGeoLocRight");
break;
case eJackGeoLocTop:
printf("eJackGeoLocTop");
break;
case eJackGeoLocBottom:
printf("eJackGeoLocBottom");
break;
case eJackGeoLocRearPanel:
printf("eJackGeoLocRearPanel");
break;
case eJackGeoLocRiser:
printf("eJackGeoLocRiser");
break;
case eJackGeoLocInsideMobileLid:
printf("eJackGeoLocInsideMobileLid");
break;
case eJackGeoLocDrivebay:
printf("eJackGeoLocDrivebay");
break;
case eJackGeoLocHDMI:
printf("eJackGeoLocHDMI");
break;
case eJackGeoLocOutsideMobileLid:
printf("eJackGeoLocOutsideMobileLid");
break;
case eJackGeoLocATAPI:
printf("eJackGeoLocATAPI");
break;
}
printf("\n");
}
static void EnumIJackGenLocation(int iVal)
{
printf("General Location: ");
switch(iVal)
{
case eJackGenLocPrimaryBox:
printf("eJackGenLocPrimaryBox");
break;
case eJackGenLocInternal:
printf("eJackGenLocInternal");
break;
case eJackGenLocSeparate:
printf("eJackGenLocSeparate");
break;
case eJackGenLocOther:
printf("eJackGenLocOther");
break;
}
printf("\n");
}
static void EnumIJackPortConnection(int iVal)
{
printf("Port Type: ");
switch(iVal)
{
case eJackPortConnJack:
printf("eJackPortConnJack");
break;
case eJackPortConnIntegratedDevice:
printf("eJackPortConnIntegratedDevice");
break;
case eJackPortConnBothIntegratedAndJack:
printf("eJackPortConnBothIntegratedAndJack");
break;
case eJackPortConnUnknown:
printf("eJackPortConnUnknown");
break;
}
printf("\n");
}
static PaError GetJackInformation(int deviceId)
{
PaError err;
int i;
int jackCount = 0;
PaWasapiJackDescription jackDesc;
err = PaWasapi_GetJackCount(deviceId, &jackCount);
if( err != paNoError ) return err;
fprintf( stderr,"Number of Jacks: %d \n", jackCount );
for( i = 0; i<jackCount; i++ )
{
fprintf( stderr,"Jack #%d:\n", i );
err = PaWasapi_GetJackDescription(deviceId, i, &jackDesc);
if( err != paNoError )
{
fprintf( stderr,"Failed getting description." );
continue;
}
else
{
printf("Is connected: %s\n",(jackDesc.isConnected)?"true":"false");
EnumIJackChannels(jackDesc.channelMapping);
EnumIJackConnectionType(jackDesc.connectionType);
EnumIJackGeoLocation(jackDesc.geoLocation);
EnumIJackGenLocation(jackDesc.genLocation);
EnumIJackPortConnection(jackDesc.portConnection);
printf("Jack Color: 0x%06X\n", jackDesc.color);
printf("\n");
}
}
return 0;
}
int main(void);
int main(void)
{
PaError err;
const PaDeviceInfo *device;
int i;
int jackCount = 0;
int isInput = 0;
printf("PortAudio Test: WASAPI Jack Configuration");
err = Pa_Initialize();
if( err != paNoError ) goto error;
for( i = 0; i < Pa_GetDeviceCount(); ++i )
{
device = Pa_GetDeviceInfo(i);
if( Pa_GetDeviceInfo(i)->hostApi == Pa_HostApiTypeIdToHostApiIndex(paWASAPI) )
{
if( device->maxOutputChannels == 0 )
{
isInput = 1;
}
printf("------------------------------------------\n");
printf("Device: %s",device->name);
if(isInput)
printf("  (Input) %d Channels\n",device->maxInputChannels);
else
printf("  (Output) %d Channels\n",device->maxOutputChannels);
err = GetJackInformation(i);
if( err != paNoError ) goto error;
}
}
Pa_Terminate();
printf("Test finished.\n");
return err;
error:
Pa_Terminate();
fprintf( stderr, "An error occurred while using the portaudio stream\n" );
fprintf( stderr, "Error number: %d\n", err );
fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
return err;
}
