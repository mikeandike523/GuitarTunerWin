 
#ifdef _WIN32
#if defined(PA_USE_ASIO) || defined(PA_USE_DS) || defined(PA_USE_WMME) || defined(PA_USE_WASAPI) || defined(PA_USE_WDMKS)
#error "This header needs to be included before pa_hostapi.h!!"
#endif
#define PA_USE_ASIO 0
#define PA_USE_DS 1
#define PA_USE_WMME 1
#define PA_USE_WASAPI 1
#define PA_USE_WDMKS 1
#else
#error "Platform currently not supported by CMake script"
#endif
