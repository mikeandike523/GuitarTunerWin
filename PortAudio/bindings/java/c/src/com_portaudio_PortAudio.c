 
#include "com_portaudio_PortAudio.h"
#include "portaudio.h"
#include "jpa_tools.h"
JNIEXPORT jint JNICALL Java_com_portaudio_PortAudio_getVersion
(JNIEnv *env, jclass clazz)
{
return Pa_GetVersion();
}
JNIEXPORT jstring JNICALL Java_com_portaudio_PortAudio_getVersionText
(JNIEnv *env, jclass clazz)
{
return (*env)->NewStringUTF(env, Pa_GetVersionText() );
}
JNIEXPORT void JNICALL Java_com_portaudio_PortAudio_initialize
(JNIEnv *env, jclass clazz)
{
PaError err = Pa_Initialize();
jpa_CheckError( env, err );
}
JNIEXPORT void JNICALL Java_com_portaudio_PortAudio_terminate
(JNIEnv *env, jclass clazz)
{
PaError err = Pa_Terminate();
jpa_CheckError( env, err );
}
JNIEXPORT jint JNICALL Java_com_portaudio_PortAudio_getDeviceCount
(JNIEnv *env, jclass clazz)
{
jint count = Pa_GetDeviceCount();
return jpa_CheckError( env, count );
}
JNIEXPORT void JNICALL Java_com_portaudio_PortAudio_getDeviceInfo
(JNIEnv *env, jclass clazz, jint index, jobject deviceInfo)
{
const PaDeviceInfo *info;
jclass cls = (*env)->GetObjectClass(env, deviceInfo);
info = Pa_GetDeviceInfo( index );
if( info == NULL )
{
jpa_ThrowError( env, "Pa_GetDeviceInfo returned NULL." );
}
else
{
jpa_SetStringField( env, cls, deviceInfo, "name", info->name );
jpa_SetIntField( env, cls, deviceInfo, "maxInputChannels", info->maxInputChannels );
jpa_SetIntField( env, cls, deviceInfo, "maxOutputChannels", info->maxOutputChannels );
jpa_SetIntField( env, cls, deviceInfo, "hostApi", info->hostApi );
jpa_SetDoubleField( env, cls, deviceInfo, "defaultSampleRate", info->defaultSampleRate );
jpa_SetDoubleField( env, cls, deviceInfo, "defaultLowInputLatency", info->defaultLowInputLatency );
jpa_SetDoubleField( env, cls, deviceInfo, "defaultLowInputLatency", info->defaultHighInputLatency );
jpa_SetDoubleField( env, cls, deviceInfo, "defaultLowOutputLatency", info->defaultLowOutputLatency );
jpa_SetDoubleField( env, cls, deviceInfo, "defaultHighOutputLatency", info->defaultHighOutputLatency );
}
}
JNIEXPORT jint JNICALL Java_com_portaudio_PortAudio_getHostApiCount
(JNIEnv *env, jclass clazz)
{
jint count = Pa_GetHostApiCount();
return jpa_CheckError( env, count );
}
JNIEXPORT jint JNICALL Java_com_portaudio_PortAudio_hostApiTypeIdToHostApiIndex
(JNIEnv *env, jclass clazz, jint hostApiType)
{
return Pa_HostApiTypeIdToHostApiIndex( (PaHostApiTypeId) hostApiType );
}
JNIEXPORT jint JNICALL Java_com_portaudio_PortAudio_hostApiDeviceIndexToDeviceIndex
(JNIEnv *env, jclass clazz, jint hostApiIndex, jint apiDeviceIndex)
{
return Pa_HostApiDeviceIndexToDeviceIndex( hostApiIndex, apiDeviceIndex );
}
JNIEXPORT void JNICALL Java_com_portaudio_PortAudio_getHostApiInfo
(JNIEnv *env, jclass clazz, jint index, jobject hostApiInfo)
{
const PaHostApiInfo *info;
jclass cls = (*env)->GetObjectClass(env, hostApiInfo);
info = Pa_GetHostApiInfo( index );
if( info == NULL )
{
jpa_ThrowError( env, "Pa_GetHostApiInfo returned NULL." );
}
else
{
jpa_SetIntField( env, cls, hostApiInfo, "version", info->structVersion );
jpa_SetIntField( env, cls, hostApiInfo, "type", info->type );
jpa_SetStringField( env, cls, hostApiInfo, "name", info->name );
jpa_SetIntField( env, cls, hostApiInfo, "deviceCount", info->deviceCount );
jpa_SetIntField( env, cls, hostApiInfo, "defaultInputDevice", info->defaultInputDevice );
jpa_SetIntField( env, cls, hostApiInfo, "defaultOutputDevice", info->defaultOutputDevice );
}
}
JNIEXPORT jint JNICALL Java_com_portaudio_PortAudio_getDefaultInputDevice
(JNIEnv *env, jclass clazz)
{
jint deviceId = Pa_GetDefaultInputDevice();
return jpa_CheckError( env, deviceId );
}
JNIEXPORT jint JNICALL Java_com_portaudio_PortAudio_getDefaultOutputDevice
(JNIEnv *env, jclass clazz)
{
jint deviceId = Pa_GetDefaultOutputDevice();
return jpa_CheckError( env, deviceId );
}
JNIEXPORT jint JNICALL Java_com_portaudio_PortAudio_getDefaultHostApi
(JNIEnv *env, jclass clazz)
{
jint deviceId = Pa_GetDefaultHostApi();
return jpa_CheckError( env, deviceId );
}
JNIEXPORT jint JNICALL Java_com_portaudio_PortAudio_isFormatSupported
(JNIEnv *env, jclass clazz, jobject inParams, jobject outParams, jint sampleRate )
{
PaStreamParameters myInParams, *paInParams;
PaStreamParameters myOutParams, *paOutParams;
paInParams = jpa_FillStreamParameters(  env, inParams, &myInParams );
paOutParams = jpa_FillStreamParameters(  env, outParams, &myOutParams );
return Pa_IsFormatSupported( paInParams, paOutParams, sampleRate );
}
JNIEXPORT void JNICALL Java_com_portaudio_PortAudio_openStream
(JNIEnv *env, jclass clazz, jobject blockingStream,  jobject inParams, jobject outParams, jint sampleRate, jint framesPerBuffer, jint flags )
{
int err;
PaStreamParameters myInParams, *paInParams;
PaStreamParameters myOutParams, *paOutParams;
PaStream *stream;
paInParams = jpa_FillStreamParameters(  env, inParams, &myInParams );
paOutParams = jpa_FillStreamParameters(  env, outParams, &myOutParams );
err = Pa_OpenStream( &stream, paInParams, paOutParams, sampleRate, framesPerBuffer, flags, NULL, NULL );
if( jpa_CheckError( env, err ) == 0 )
{
jclass cls = (*env)->GetObjectClass(env, blockingStream);
jpa_SetLongField( env, cls, blockingStream, "nativeStream", (jlong) stream );
if( paInParams != NULL )
{
jpa_SetIntField( env, cls, blockingStream, "inputFormat", paInParams->sampleFormat );
}
if( paOutParams != NULL )
{
jpa_SetIntField( env, cls, blockingStream, "outputFormat", paOutParams->sampleFormat );
}
}
}
