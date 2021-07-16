 
#if defined(__APPLE__)
#include <JavaVM/jni.h>
#else
#include <jni.h>
#endif
#ifndef _Included_com_portaudio_BlockingStream
#define _Included_com_portaudio_BlockingStream
#ifdef __cplusplus
extern "C" {
#endif
JNIEXPORT jint JNICALL Java_com_portaudio_BlockingStream_getReadAvailable
(JNIEnv *, jobject);
JNIEXPORT jint JNICALL Java_com_portaudio_BlockingStream_getWriteAvailable
(JNIEnv *, jobject);
JNIEXPORT jboolean JNICALL Java_com_portaudio_BlockingStream_readFloats
(JNIEnv *, jobject, jfloatArray, jint);
JNIEXPORT jboolean JNICALL Java_com_portaudio_BlockingStream_writeFloats
(JNIEnv *, jobject, jfloatArray, jint);
JNIEXPORT jboolean JNICALL Java_com_portaudio_BlockingStream_readShorts
(JNIEnv *, jobject, jshortArray, jint);
JNIEXPORT jboolean JNICALL Java_com_portaudio_BlockingStream_writeShorts
(JNIEnv *, jobject, jshortArray, jint);
JNIEXPORT void JNICALL Java_com_portaudio_BlockingStream_start
(JNIEnv *, jobject);
JNIEXPORT void JNICALL Java_com_portaudio_BlockingStream_stop
(JNIEnv *, jobject);
JNIEXPORT void JNICALL Java_com_portaudio_BlockingStream_abort
(JNIEnv *, jobject);
JNIEXPORT void JNICALL Java_com_portaudio_BlockingStream_close
(JNIEnv *, jobject);
JNIEXPORT jboolean JNICALL Java_com_portaudio_BlockingStream_isStopped
(JNIEnv *, jobject);
JNIEXPORT jboolean JNICALL Java_com_portaudio_BlockingStream_isActive
(JNIEnv *, jobject);
JNIEXPORT jdouble JNICALL Java_com_portaudio_BlockingStream_getTime
(JNIEnv *, jobject);
JNIEXPORT void JNICALL Java_com_portaudio_BlockingStream_getInfo
(JNIEnv *, jobject, jobject);
#ifdef __cplusplus
}
#endif
#endif
