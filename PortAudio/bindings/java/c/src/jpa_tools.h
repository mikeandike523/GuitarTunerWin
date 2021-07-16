 
#include "com_portaudio_PortAudio.h"
#include "portaudio.h"
#ifndef JPA_TOOLS_H
#define JPA_TOOLS_H
jint jpa_GetIntField( JNIEnv *env, jclass cls, jobject obj, const char *fieldName );
void jpa_SetIntField( JNIEnv *env, jclass cls, jobject obj, const char *fieldName, jint value );
jlong jpa_GetLongField( JNIEnv *env, jclass cls, jobject obj, const char *fieldName );
void jpa_SetLongField( JNIEnv *env, jclass cls, jobject obj, const char *fieldName, jlong value );
jdouble jpa_GetDoubleField( JNIEnv *env, jclass cls, jobject obj, const char *fieldName );
void jpa_SetDoubleField( JNIEnv *env, jclass cls, jobject obj, const char *fieldName, jdouble value );
void jpa_SetStringField( JNIEnv *env, jclass cls, jobject obj, const char *fieldName, const char *value );
PaStreamParameters *jpa_FillStreamParameters( JNIEnv *env, jobject jstreamParam, PaStreamParameters *myParams );
jint jpa_CheckError( JNIEnv *env, PaError err );
jint jpa_ThrowError( JNIEnv *env, const char *message );
PaStream *jpa_GetStreamPointer( JNIEnv *env, jobject blockingStream );
#endif  
