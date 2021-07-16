 
#include "com_portaudio_BlockingStream.h"
#include "portaudio.h"
#include "jpa_tools.h"
#ifndef FALSE
#define FALSE  (0)
#endif
#ifndef TRUE
#define TRUE  (!FALSE)
#endif
JNIEXPORT jint JNICALL Java_com_portaudio_BlockingStream_getReadAvailable
(JNIEnv *env, jobject blockingStream)
{
PaStream *stream =jpa_GetStreamPointer( env, blockingStream );
if( stream == NULL ) return 0;
return Pa_GetStreamReadAvailable( stream );
}
JNIEXPORT jint JNICALL Java_com_portaudio_BlockingStream_getWriteAvailable
(JNIEnv *env, jobject blockingStream)
{
PaStream *stream =jpa_GetStreamPointer( env, blockingStream );
if( stream == NULL ) return 0;
return Pa_GetStreamWriteAvailable( stream );
}
JNIEXPORT jboolean JNICALL Java_com_portaudio_BlockingStream_writeFloats
(JNIEnv *env, jobject blockingStream, jfloatArray buffer, jint numFrames)
{
jfloat *carr;
jint err;
PaStream *stream =jpa_GetStreamPointer( env, blockingStream );
if( buffer == NULL )
{
(*env)->ThrowNew( env, (*env)->FindClass(env,"java/lang/RuntimeException"),
"null stream buffer");
return FALSE;
}
carr = (*env)->GetFloatArrayElements(env, buffer, NULL);
if (carr == NULL)
{
(*env)->ThrowNew( env, (*env)->FindClass(env,"java/lang/RuntimeException"),
"invalid stream buffer");
return FALSE;
}
err = Pa_WriteStream( stream, carr, numFrames );
(*env)->ReleaseFloatArrayElements(env, buffer, carr, 0);
if( err == paOutputUnderflowed )
{
return TRUE;
}
else
{
jpa_CheckError( env, err );
return FALSE;
}
}
JNIEXPORT jboolean JNICALL Java_com_portaudio_BlockingStream_readFloats
(JNIEnv *env, jobject blockingStream, jfloatArray buffer, jint numFrames)
{
jfloat *carr;
jint err;
PaStream *stream =jpa_GetStreamPointer( env, blockingStream );
if( buffer == NULL )
{
(*env)->ThrowNew( env, (*env)->FindClass(env,"java/lang/RuntimeException"),
"null stream buffer");
return FALSE;
}
carr = (*env)->GetFloatArrayElements(env, buffer, NULL);
if (carr == NULL)
{
(*env)->ThrowNew( env, (*env)->FindClass(env,"java/lang/RuntimeException"),
"invalid stream buffer");
return FALSE;
}
err = Pa_ReadStream( stream, carr, numFrames );
(*env)->ReleaseFloatArrayElements(env, buffer, carr, 0);
if( err == paInputOverflowed )
{
return TRUE;
}
else
{
jpa_CheckError( env, err );
return FALSE;
}
}
JNIEXPORT jboolean JNICALL Java_com_portaudio_BlockingStream_writeShorts
(JNIEnv *env, jobject blockingStream, jfloatArray buffer, jint numFrames)
{
jshort *carr;
jint err;
PaStream *stream =jpa_GetStreamPointer( env, blockingStream );
if( buffer == NULL )
{
(*env)->ThrowNew( env, (*env)->FindClass(env,"java/lang/RuntimeException"),
"null stream buffer");
return FALSE;
}
carr = (*env)->GetShortArrayElements(env, buffer, NULL);
if (carr == NULL)
{
(*env)->ThrowNew( env, (*env)->FindClass(env,"java/lang/RuntimeException"),
"invalid stream buffer");
return FALSE;
}
err = Pa_WriteStream( stream, carr, numFrames );
(*env)->ReleaseShortArrayElements(env, buffer, carr, 0);
if( err == paOutputUnderflowed )
{
return TRUE;
}
else
{
jpa_CheckError( env, err );
return FALSE;
}
}
JNIEXPORT jboolean JNICALL Java_com_portaudio_BlockingStream_readShorts
(JNIEnv *env, jobject blockingStream, jfloatArray buffer, jint numFrames)
{
jshort *carr;
jint err;
PaStream *stream =jpa_GetStreamPointer( env, blockingStream );
if( buffer == NULL )
{
(*env)->ThrowNew( env, (*env)->FindClass(env,"java/lang/RuntimeException"),
"null stream buffer");
return FALSE;
}
carr = (*env)->GetShortArrayElements(env, buffer, NULL);
if (carr == NULL)
{
(*env)->ThrowNew( env, (*env)->FindClass(env,"java/lang/RuntimeException"),
"invalid stream buffer");
return FALSE;
}
err = Pa_ReadStream( stream, carr, numFrames );
(*env)->ReleaseShortArrayElements(env, buffer, carr, 0);
if( err == paInputOverflowed )
{
return TRUE;
}
else
{
jpa_CheckError( env, err );
return FALSE;
}
}
JNIEXPORT void JNICALL Java_com_portaudio_BlockingStream_start
(JNIEnv *env, jobject blockingStream )
{
PaStream *stream = jpa_GetStreamPointer( env, blockingStream );
int err = Pa_StartStream( stream );
jpa_CheckError( env, err );
}
JNIEXPORT void JNICALL Java_com_portaudio_BlockingStream_stop
(JNIEnv *env, jobject blockingStream )
{
PaStream *stream =jpa_GetStreamPointer( env, blockingStream );
int err = Pa_StopStream( stream );
jpa_CheckError( env, err );
}
JNIEXPORT void JNICALL Java_com_portaudio_BlockingStream_abort
(JNIEnv *env, jobject blockingStream )
{
PaStream *stream =jpa_GetStreamPointer( env, blockingStream );
int err = Pa_AbortStream( stream );
jpa_CheckError( env, err );
}
JNIEXPORT void JNICALL Java_com_portaudio_BlockingStream_close
(JNIEnv *env, jobject blockingStream )
{
jclass cls;
PaStream *stream =jpa_GetStreamPointer( env, blockingStream );
if( stream != NULL )
{
int err = Pa_CloseStream( stream );
jpa_CheckError( env, err );
cls = (*env)->GetObjectClass(env, blockingStream);
jpa_SetLongField( env, cls, blockingStream, "nativeStream", (jlong) 0 );
}
}
JNIEXPORT jboolean JNICALL Java_com_portaudio_BlockingStream_isStopped
(JNIEnv *env, jobject blockingStream )
{
int err;
PaStream *stream =jpa_GetStreamPointer( env, blockingStream );
if( stream == NULL ) return 1;
err = Pa_IsStreamStopped( stream );
return (jpa_CheckError( env, err ) > 0);
}
JNIEXPORT jboolean JNICALL Java_com_portaudio_BlockingStream_isActive
(JNIEnv *env, jobject blockingStream )
{
int err;
PaStream *stream =jpa_GetStreamPointer( env, blockingStream );
if( stream == NULL ) return 0;
err = Pa_IsStreamActive( stream );
return (jpa_CheckError( env, err ) > 0);
}
JNIEXPORT jdouble JNICALL Java_com_portaudio_BlockingStream_getTime
(JNIEnv *env, jobject blockingStream )
{
PaStream *stream =jpa_GetStreamPointer( env, blockingStream );
if( stream == NULL ) return 0.0;
return Pa_GetStreamTime( stream );
}
JNIEXPORT void JNICALL Java_com_portaudio_BlockingStream_getInfo
(JNIEnv *env, jobject blockingStream, jobject streamInfo)
{
PaStream *stream =jpa_GetStreamPointer( env, blockingStream );
const PaStreamInfo *info = Pa_GetStreamInfo( stream );
if( streamInfo == NULL )
{
jpa_ThrowError( env, "Invalid stream." );
}
else
{
jclass cls = (*env)->GetObjectClass(env, streamInfo);
jpa_SetIntField( env, cls, streamInfo, "structVersion", info->structVersion );
jpa_SetDoubleField( env, cls, streamInfo, "inputLatency", info->inputLatency );
jpa_SetDoubleField( env, cls, streamInfo, "outputLatency", info->outputLatency );
jpa_SetDoubleField( env, cls, streamInfo, "sampleRate", info->sampleRate );
}
}
