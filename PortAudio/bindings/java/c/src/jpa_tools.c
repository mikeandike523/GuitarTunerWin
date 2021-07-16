 
#include "com_portaudio_PortAudio.h"
#include "portaudio.h"
#include "jpa_tools.h"
jint jpa_GetIntField( JNIEnv *env, jclass cls, jobject obj, const char *fieldName )
{
jfieldID fid = (*env)->GetFieldID(env, cls, fieldName, "I");
if (fid == NULL)
{
jpa_ThrowError( env, "Cannot find integer JNI field." );
return 0;
}
else
{
return (*env)->GetIntField(env, obj, fid );
}
}
void jpa_SetIntField( JNIEnv *env, jclass cls, jobject obj, const char *fieldName, jint value )
{
jfieldID fid = (*env)->GetFieldID(env, cls, fieldName, "I");
if (fid == NULL)
{
jpa_ThrowError( env, "Cannot find integer JNI field." );
}
else
{
(*env)->SetIntField(env, obj, fid, value );
}
}
jlong jpa_GetLongField( JNIEnv *env, jclass cls, jobject obj, const char *fieldName )
{
jfieldID fid = (*env)->GetFieldID(env, cls, fieldName, "J");
if (fid == NULL)
{
jpa_ThrowError( env, "Cannot find long JNI field." );
return 0L;
}
else
{
return (*env)->GetLongField(env, obj, fid );
}
}
void jpa_SetLongField( JNIEnv *env, jclass cls, jobject obj, const char *fieldName, jlong value )
{
jfieldID fid = (*env)->GetFieldID(env, cls, fieldName, "J");
if (fid == NULL)
{
jpa_ThrowError( env, "Cannot find long JNI field." );
}
else
{
(*env)->SetLongField(env, obj, fid, value );
}
}
void jpa_SetDoubleField( JNIEnv *env, jclass cls, jobject obj, const char *fieldName, jdouble value )
{
jfieldID fid = (*env)->GetFieldID(env, cls, fieldName, "D");
if (fid == NULL)
{
jpa_ThrowError( env, "Cannot find double JNI field." );
}
else
{
(*env)->SetDoubleField(env, obj, fid, value );
}
}
jdouble jpa_GetDoubleField( JNIEnv *env, jclass cls, jobject obj, const char *fieldName )
{
jfieldID fid = (*env)->GetFieldID(env, cls, fieldName, "D");
if (fid == NULL)
{
jpa_ThrowError( env, "Cannot find double JNI field." );
return 0;
}
else
{
return (*env)->GetDoubleField(env, obj, fid );
}
}
void jpa_SetStringField( JNIEnv *env, jclass cls, jobject obj, const char *fieldName, const char *value )
{
jfieldID fid = (*env)->GetFieldID(env, cls, fieldName, "Ljava/lang/String;");
if (fid == NULL)
{
jpa_ThrowError( env, "Cannot find String JNI field." );
}
else
{
jstring jstr = (*env)->NewStringUTF(env, value);
if (jstr == NULL)
{
jpa_ThrowError( env, "Cannot create new String." );
}
else
{
(*env)->SetObjectField(env, obj, fid, jstr );
}
}
}
PaStreamParameters *jpa_FillStreamParameters( JNIEnv *env, jobject jstreamParam, PaStreamParameters *myParams )
{
jclass cls;
if( jstreamParam == NULL ) return NULL;  
cls = (*env)->GetObjectClass(env, jstreamParam);
myParams->channelCount = jpa_GetIntField( env, cls, jstreamParam, "channelCount" );
myParams->device = jpa_GetIntField( env, cls, jstreamParam, "device" );
myParams->sampleFormat = jpa_GetIntField( env, cls, jstreamParam, "sampleFormat" );
myParams->suggestedLatency = jpa_GetDoubleField( env, cls, jstreamParam, "suggestedLatency" );
myParams->hostApiSpecificStreamInfo = NULL;
return myParams;
}
jint jpa_ThrowError( JNIEnv *env, const char *message )
{
return (*env)->ThrowNew(env, (*env)->FindClass( env, "java/lang/RuntimeException"),
message );
}
jint jpa_CheckError( JNIEnv *env, PaError err )
{
if( err == -1 )
{
return jpa_ThrowError( env, "-1, possibly no available default device" );
}
else if( err < 0 )
{
if( err == paUnanticipatedHostError )
{
const PaHostErrorInfo *hostErrorInfo = Pa_GetLastHostErrorInfo();
return jpa_ThrowError( env, hostErrorInfo->errorText );
}
else
{
return jpa_ThrowError( env, Pa_GetErrorText( err ) );
}
}
else
{
return err;
}
}
PaStream *jpa_GetStreamPointer( JNIEnv *env, jobject blockingStream )
{
jclass cls = (*env)->GetObjectClass(env, blockingStream);
return (PaStream *) jpa_GetLongField( env, cls, blockingStream, "nativeStream" );
}
