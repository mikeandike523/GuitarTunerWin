 
#if defined(__APPLE__)
#include <JavaVM/jni.h>
#else
#include <jni.h>
#endif
#ifndef _Included_com_portaudio_PortAudio
#define _Included_com_portaudio_PortAudio
#ifdef __cplusplus
extern "C" {
#endif
#undef com_portaudio_PortAudio_FLAG_CLIP_OFF
#define com_portaudio_PortAudio_FLAG_CLIP_OFF 1L
#undef com_portaudio_PortAudio_FLAG_DITHER_OFF
#define com_portaudio_PortAudio_FLAG_DITHER_OFF 2L
#undef com_portaudio_PortAudio_FORMAT_FLOAT_32
#define com_portaudio_PortAudio_FORMAT_FLOAT_32 1L
#undef com_portaudio_PortAudio_FORMAT_INT_32
#define com_portaudio_PortAudio_FORMAT_INT_32 2L
#undef com_portaudio_PortAudio_FORMAT_INT_24
#define com_portaudio_PortAudio_FORMAT_INT_24 4L
#undef com_portaudio_PortAudio_FORMAT_INT_16
#define com_portaudio_PortAudio_FORMAT_INT_16 8L
#undef com_portaudio_PortAudio_FORMAT_INT_8
#define com_portaudio_PortAudio_FORMAT_INT_8 16L
#undef com_portaudio_PortAudio_FORMAT_UINT_8
#define com_portaudio_PortAudio_FORMAT_UINT_8 32L
#undef com_portaudio_PortAudio_HOST_API_TYPE_DEV
#define com_portaudio_PortAudio_HOST_API_TYPE_DEV 0L
#undef com_portaudio_PortAudio_HOST_API_TYPE_DIRECTSOUND
#define com_portaudio_PortAudio_HOST_API_TYPE_DIRECTSOUND 1L
#undef com_portaudio_PortAudio_HOST_API_TYPE_MME
#define com_portaudio_PortAudio_HOST_API_TYPE_MME 2L
#undef com_portaudio_PortAudio_HOST_API_TYPE_ASIO
#define com_portaudio_PortAudio_HOST_API_TYPE_ASIO 3L
#undef com_portaudio_PortAudio_HOST_API_TYPE_SOUNDMANAGER
#define com_portaudio_PortAudio_HOST_API_TYPE_SOUNDMANAGER 4L
#undef com_portaudio_PortAudio_HOST_API_TYPE_COREAUDIO
#define com_portaudio_PortAudio_HOST_API_TYPE_COREAUDIO 5L
#undef com_portaudio_PortAudio_HOST_API_TYPE_OSS
#define com_portaudio_PortAudio_HOST_API_TYPE_OSS 7L
#undef com_portaudio_PortAudio_HOST_API_TYPE_ALSA
#define com_portaudio_PortAudio_HOST_API_TYPE_ALSA 8L
#undef com_portaudio_PortAudio_HOST_API_TYPE_AL
#define com_portaudio_PortAudio_HOST_API_TYPE_AL 9L
#undef com_portaudio_PortAudio_HOST_API_TYPE_BEOS
#define com_portaudio_PortAudio_HOST_API_TYPE_BEOS 10L
#undef com_portaudio_PortAudio_HOST_API_TYPE_WDMKS
#define com_portaudio_PortAudio_HOST_API_TYPE_WDMKS 11L
#undef com_portaudio_PortAudio_HOST_API_TYPE_JACK
#define com_portaudio_PortAudio_HOST_API_TYPE_JACK 12L
#undef com_portaudio_PortAudio_HOST_API_TYPE_WASAPI
#define com_portaudio_PortAudio_HOST_API_TYPE_WASAPI 13L
#undef com_portaudio_PortAudio_HOST_API_TYPE_AUDIOSCIENCE
#define com_portaudio_PortAudio_HOST_API_TYPE_AUDIOSCIENCE 14L
#undef com_portaudio_PortAudio_HOST_API_TYPE_COUNT
#define com_portaudio_PortAudio_HOST_API_TYPE_COUNT 15L
JNIEXPORT jint JNICALL Java_com_portaudio_PortAudio_getVersion
(JNIEnv *, jclass);
JNIEXPORT jstring JNICALL Java_com_portaudio_PortAudio_getVersionText
(JNIEnv *, jclass);
JNIEXPORT void JNICALL Java_com_portaudio_PortAudio_initialize
(JNIEnv *, jclass);
JNIEXPORT void JNICALL Java_com_portaudio_PortAudio_terminate
(JNIEnv *, jclass);
JNIEXPORT jint JNICALL Java_com_portaudio_PortAudio_getDeviceCount
(JNIEnv *, jclass);
JNIEXPORT void JNICALL Java_com_portaudio_PortAudio_getDeviceInfo
(JNIEnv *, jclass, jint, jobject);
JNIEXPORT jint JNICALL Java_com_portaudio_PortAudio_getHostApiCount
(JNIEnv *, jclass);
JNIEXPORT void JNICALL Java_com_portaudio_PortAudio_getHostApiInfo
(JNIEnv *, jclass, jint, jobject);
JNIEXPORT jint JNICALL Java_com_portaudio_PortAudio_hostApiTypeIdToHostApiIndex
(JNIEnv *, jclass, jint);
JNIEXPORT jint JNICALL Java_com_portaudio_PortAudio_hostApiDeviceIndexToDeviceIndex
(JNIEnv *, jclass, jint, jint);
JNIEXPORT jint JNICALL Java_com_portaudio_PortAudio_getDefaultInputDevice
(JNIEnv *, jclass);
JNIEXPORT jint JNICALL Java_com_portaudio_PortAudio_getDefaultOutputDevice
(JNIEnv *, jclass);
JNIEXPORT jint JNICALL Java_com_portaudio_PortAudio_getDefaultHostApi
(JNIEnv *, jclass);
JNIEXPORT jint JNICALL Java_com_portaudio_PortAudio_isFormatSupported
(JNIEnv *, jclass, jobject, jobject, jint);
JNIEXPORT void JNICALL Java_com_portaudio_PortAudio_openStream
(JNIEnv *, jclass, jobject, jobject, jobject, jint, jint, jint);
#ifdef __cplusplus
}
#endif
#endif
