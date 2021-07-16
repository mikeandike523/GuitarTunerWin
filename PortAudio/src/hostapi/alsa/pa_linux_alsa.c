 
#define ALSA_PCM_NEW_HW_PARAMS_API
#define ALSA_PCM_NEW_SW_PARAMS_API
#include <alsa/asoundlib.h>
#undef ALSA_PCM_NEW_HW_PARAMS_API
#undef ALSA_PCM_NEW_SW_PARAMS_API
#include <sys/poll.h>
#include <string.h>  
#include <limits.h>
#include <math.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <sys/mman.h>
#include <signal.h>  
#ifdef PA_ALSA_DYNAMIC
#include <dlfcn.h>  
#endif
#include "portaudio.h"
#include "pa_util.h"
#include "pa_unix_util.h"
#include "pa_allocation.h"
#include "pa_hostapi.h"
#include "pa_stream.h"
#include "pa_cpuload.h"
#include "pa_process.h"
#include "pa_endianness.h"
#include "pa_debugprint.h"
#include "pa_linux_alsa.h"
#ifndef SND_PCM_TSTAMP_ENABLE
#define SND_PCM_TSTAMP_ENABLE SND_PCM_TSTAMP_MMAP
#endif
#define ALSA_VERSION_INT(major, minor, subminor)  ((major << 16) | (minor << 8) | subminor)
#define RATE_MAX_DEVIATE_RATIO 100
#define _PA_DEFINE_FUNC(x)  typedef typeof(x) x##_ft; static x##_ft *alsa_##x = 0
#define __alsa_snd_alloca(ptr,type) do { size_t __alsa_alloca_size = alsa_##type##_sizeof(); (*ptr) = (type##_t *) alloca(__alsa_alloca_size); memset(*ptr, 0, __alsa_alloca_size); } while (0)
_PA_DEFINE_FUNC(snd_pcm_open);
_PA_DEFINE_FUNC(snd_pcm_close);
_PA_DEFINE_FUNC(snd_pcm_nonblock);
_PA_DEFINE_FUNC(snd_pcm_frames_to_bytes);
_PA_DEFINE_FUNC(snd_pcm_prepare);
_PA_DEFINE_FUNC(snd_pcm_start);
_PA_DEFINE_FUNC(snd_pcm_resume);
_PA_DEFINE_FUNC(snd_pcm_wait);
_PA_DEFINE_FUNC(snd_pcm_state);
_PA_DEFINE_FUNC(snd_pcm_avail_update);
_PA_DEFINE_FUNC(snd_pcm_areas_silence);
_PA_DEFINE_FUNC(snd_pcm_mmap_begin);
_PA_DEFINE_FUNC(snd_pcm_mmap_commit);
_PA_DEFINE_FUNC(snd_pcm_readi);
_PA_DEFINE_FUNC(snd_pcm_readn);
_PA_DEFINE_FUNC(snd_pcm_writei);
_PA_DEFINE_FUNC(snd_pcm_writen);
_PA_DEFINE_FUNC(snd_pcm_drain);
_PA_DEFINE_FUNC(snd_pcm_recover);
_PA_DEFINE_FUNC(snd_pcm_drop);
_PA_DEFINE_FUNC(snd_pcm_area_copy);
_PA_DEFINE_FUNC(snd_pcm_poll_descriptors);
_PA_DEFINE_FUNC(snd_pcm_poll_descriptors_count);
_PA_DEFINE_FUNC(snd_pcm_poll_descriptors_revents);
_PA_DEFINE_FUNC(snd_pcm_format_size);
_PA_DEFINE_FUNC(snd_pcm_link);
_PA_DEFINE_FUNC(snd_pcm_delay);
_PA_DEFINE_FUNC(snd_pcm_hw_params_sizeof);
_PA_DEFINE_FUNC(snd_pcm_hw_params_malloc);
_PA_DEFINE_FUNC(snd_pcm_hw_params_free);
_PA_DEFINE_FUNC(snd_pcm_hw_params_any);
_PA_DEFINE_FUNC(snd_pcm_hw_params_set_access);
_PA_DEFINE_FUNC(snd_pcm_hw_params_set_format);
_PA_DEFINE_FUNC(snd_pcm_hw_params_set_channels);
_PA_DEFINE_FUNC(snd_pcm_hw_params_set_rate_near);  
_PA_DEFINE_FUNC(snd_pcm_hw_params_set_rate);
_PA_DEFINE_FUNC(snd_pcm_hw_params_set_rate_resample);
_PA_DEFINE_FUNC(snd_pcm_hw_params_set_buffer_size);
_PA_DEFINE_FUNC(snd_pcm_hw_params_set_buffer_size_near);  
_PA_DEFINE_FUNC(snd_pcm_hw_params_set_buffer_size_min);
_PA_DEFINE_FUNC(snd_pcm_hw_params_set_period_size_near);
_PA_DEFINE_FUNC(snd_pcm_hw_params_set_periods_integer);
_PA_DEFINE_FUNC(snd_pcm_hw_params_set_periods_min);
_PA_DEFINE_FUNC(snd_pcm_hw_params_get_buffer_size);
_PA_DEFINE_FUNC(snd_pcm_hw_params_get_channels_min);
_PA_DEFINE_FUNC(snd_pcm_hw_params_get_channels_max);
_PA_DEFINE_FUNC(snd_pcm_hw_params_test_period_size);
_PA_DEFINE_FUNC(snd_pcm_hw_params_test_format);
_PA_DEFINE_FUNC(snd_pcm_hw_params_test_access);
_PA_DEFINE_FUNC(snd_pcm_hw_params_dump);
_PA_DEFINE_FUNC(snd_pcm_hw_params);
_PA_DEFINE_FUNC(snd_pcm_hw_params_get_periods_min);
_PA_DEFINE_FUNC(snd_pcm_hw_params_get_periods_max);
_PA_DEFINE_FUNC(snd_pcm_hw_params_set_period_size);
_PA_DEFINE_FUNC(snd_pcm_hw_params_get_period_size_min);
_PA_DEFINE_FUNC(snd_pcm_hw_params_get_period_size_max);
_PA_DEFINE_FUNC(snd_pcm_hw_params_get_buffer_size_max);
_PA_DEFINE_FUNC(snd_pcm_hw_params_get_rate_min);
_PA_DEFINE_FUNC(snd_pcm_hw_params_get_rate_max);
_PA_DEFINE_FUNC(snd_pcm_hw_params_get_rate_numden);
#define alsa_snd_pcm_hw_params_alloca(ptr) __alsa_snd_alloca(ptr, snd_pcm_hw_params)
_PA_DEFINE_FUNC(snd_pcm_sw_params_sizeof);
_PA_DEFINE_FUNC(snd_pcm_sw_params_malloc);
_PA_DEFINE_FUNC(snd_pcm_sw_params_current);
_PA_DEFINE_FUNC(snd_pcm_sw_params_set_avail_min);
_PA_DEFINE_FUNC(snd_pcm_sw_params);
_PA_DEFINE_FUNC(snd_pcm_sw_params_free);
_PA_DEFINE_FUNC(snd_pcm_sw_params_set_start_threshold);
_PA_DEFINE_FUNC(snd_pcm_sw_params_set_stop_threshold);
_PA_DEFINE_FUNC(snd_pcm_sw_params_get_boundary);
_PA_DEFINE_FUNC(snd_pcm_sw_params_set_silence_threshold);
_PA_DEFINE_FUNC(snd_pcm_sw_params_set_silence_size);
_PA_DEFINE_FUNC(snd_pcm_sw_params_set_xfer_align);
_PA_DEFINE_FUNC(snd_pcm_sw_params_set_tstamp_mode);
#define alsa_snd_pcm_sw_params_alloca(ptr) __alsa_snd_alloca(ptr, snd_pcm_sw_params)
_PA_DEFINE_FUNC(snd_pcm_info);
_PA_DEFINE_FUNC(snd_pcm_info_sizeof);
_PA_DEFINE_FUNC(snd_pcm_info_malloc);
_PA_DEFINE_FUNC(snd_pcm_info_free);
_PA_DEFINE_FUNC(snd_pcm_info_set_device);
_PA_DEFINE_FUNC(snd_pcm_info_set_subdevice);
_PA_DEFINE_FUNC(snd_pcm_info_set_stream);
_PA_DEFINE_FUNC(snd_pcm_info_get_name);
_PA_DEFINE_FUNC(snd_pcm_info_get_card);
#define alsa_snd_pcm_info_alloca(ptr) __alsa_snd_alloca(ptr, snd_pcm_info)
_PA_DEFINE_FUNC(snd_ctl_pcm_next_device);
_PA_DEFINE_FUNC(snd_ctl_pcm_info);
_PA_DEFINE_FUNC(snd_ctl_open);
_PA_DEFINE_FUNC(snd_ctl_close);
_PA_DEFINE_FUNC(snd_ctl_card_info_malloc);
_PA_DEFINE_FUNC(snd_ctl_card_info_free);
_PA_DEFINE_FUNC(snd_ctl_card_info);
_PA_DEFINE_FUNC(snd_ctl_card_info_sizeof);
_PA_DEFINE_FUNC(snd_ctl_card_info_get_name);
#define alsa_snd_ctl_card_info_alloca(ptr) __alsa_snd_alloca(ptr, snd_ctl_card_info)
_PA_DEFINE_FUNC(snd_config);
_PA_DEFINE_FUNC(snd_config_update);
_PA_DEFINE_FUNC(snd_config_search);
_PA_DEFINE_FUNC(snd_config_iterator_entry);
_PA_DEFINE_FUNC(snd_config_iterator_first);
_PA_DEFINE_FUNC(snd_config_iterator_end);
_PA_DEFINE_FUNC(snd_config_iterator_next);
_PA_DEFINE_FUNC(snd_config_get_string);
_PA_DEFINE_FUNC(snd_config_get_id);
_PA_DEFINE_FUNC(snd_config_update_free_global);
_PA_DEFINE_FUNC(snd_pcm_status);
_PA_DEFINE_FUNC(snd_pcm_status_sizeof);
_PA_DEFINE_FUNC(snd_pcm_status_get_tstamp);
_PA_DEFINE_FUNC(snd_pcm_status_get_htstamp);
_PA_DEFINE_FUNC(snd_pcm_status_get_state);
_PA_DEFINE_FUNC(snd_pcm_status_get_trigger_tstamp);
_PA_DEFINE_FUNC(snd_pcm_status_get_trigger_htstamp);
_PA_DEFINE_FUNC(snd_pcm_status_get_delay);
#define alsa_snd_pcm_status_alloca(ptr) __alsa_snd_alloca(ptr, snd_pcm_status)
_PA_DEFINE_FUNC(snd_card_next);
_PA_DEFINE_FUNC(snd_asoundlib_version);
_PA_DEFINE_FUNC(snd_strerror);
_PA_DEFINE_FUNC(snd_output_stdio_attach);
#define alsa_snd_config_for_each(pos, next, node)\
for (pos = alsa_snd_config_iterator_first(node),\
next = alsa_snd_config_iterator_next(pos);\
pos != alsa_snd_config_iterator_end(node); pos = next, next = alsa_snd_config_iterator_next(pos))
#undef _PA_DEFINE_FUNC
#ifndef PA_ALSA_PATHNAME
#define PA_ALSA_PATHNAME "libasound.so"
#endif
static const char *g_AlsaLibName = PA_ALSA_PATHNAME;
static void *g_AlsaLib = NULL;
#ifdef PA_ALSA_DYNAMIC
#define _PA_LOCAL_IMPL(x) __pa_local_##x
int _PA_LOCAL_IMPL(snd_pcm_hw_params_set_rate_near) (snd_pcm_t *pcm, snd_pcm_hw_params_t *params, unsigned int *val, int *dir)
{
int ret;
if(( ret = alsa_snd_pcm_hw_params_set_rate(pcm, params, (*val), (*dir)) ) < 0 )
return ret;
return 0;
}
int _PA_LOCAL_IMPL(snd_pcm_hw_params_set_buffer_size_near) (snd_pcm_t *pcm, snd_pcm_hw_params_t *params, snd_pcm_uframes_t *val)
{
int ret;
if(( ret = alsa_snd_pcm_hw_params_set_buffer_size(pcm, params, (*val)) ) < 0 )
return ret;
return 0;
}
int _PA_LOCAL_IMPL(snd_pcm_hw_params_set_period_size_near) (snd_pcm_t *pcm, snd_pcm_hw_params_t *params, snd_pcm_uframes_t *val, int *dir)
{
int ret;
if(( ret = alsa_snd_pcm_hw_params_set_period_size(pcm, params, (*val), (*dir)) ) < 0 )
return ret;
return 0;
}
int _PA_LOCAL_IMPL(snd_pcm_hw_params_get_channels_min) (const snd_pcm_hw_params_t *params, unsigned int *val)
{
(*val) = 1;
return 0;
}
int _PA_LOCAL_IMPL(snd_pcm_hw_params_get_channels_max) (const snd_pcm_hw_params_t *params, unsigned int *val)
{
(*val) = 2;
return 0;
}
int _PA_LOCAL_IMPL(snd_pcm_hw_params_get_periods_min) (const snd_pcm_hw_params_t *params, unsigned int *val, int *dir)
{
(*val) = 2;
return 0;
}
int _PA_LOCAL_IMPL(snd_pcm_hw_params_get_periods_max) (const snd_pcm_hw_params_t *params, unsigned int *val, int *dir)
{
(*val) = 8;
return 0;
}
int _PA_LOCAL_IMPL(snd_pcm_hw_params_get_period_size_min) (const snd_pcm_hw_params_t *params, snd_pcm_uframes_t *frames, int *dir)
{
(*frames) = 64;
return 0;
}
int _PA_LOCAL_IMPL(snd_pcm_hw_params_get_period_size_max) (const snd_pcm_hw_params_t *params, snd_pcm_uframes_t *frames, int *dir)
{
(*frames) = 512;
return 0;
}
int _PA_LOCAL_IMPL(snd_pcm_hw_params_get_buffer_size_max) (const snd_pcm_hw_params_t *params, snd_pcm_uframes_t *val)
{
int ret;
int dir                = 0;
snd_pcm_uframes_t pmax = 0;
unsigned int      pcnt = 0;
dir = 0;
if(( ret = _PA_LOCAL_IMPL(snd_pcm_hw_params_get_period_size_max)(params, &pmax, &dir) ) < 0 )
return ret;
dir = 0;
if(( ret = _PA_LOCAL_IMPL(snd_pcm_hw_params_get_periods_max)(params, &pcnt, &dir) ) < 0 )
return ret;
(*val) = pmax * pcnt;
return 0;
}
int _PA_LOCAL_IMPL(snd_pcm_hw_params_get_rate_min) (const snd_pcm_hw_params_t *params, unsigned int *val, int *dir)
{
(*val) = 44100;
return 0;
}
int _PA_LOCAL_IMPL(snd_pcm_hw_params_get_rate_max) (const snd_pcm_hw_params_t *params, unsigned int *val, int *dir)
{
(*val) = 44100;
return 0;
}
#endif  
static int PaAlsa_LoadLibrary()
{
#ifdef PA_ALSA_DYNAMIC
PA_DEBUG(( "%s: loading ALSA library file - %s\n", __FUNCTION__, g_AlsaLibName ));
dlerror();
g_AlsaLib = dlopen(g_AlsaLibName, (RTLD_NOW|RTLD_GLOBAL) );
if (g_AlsaLib == NULL)
{
PA_DEBUG(( "%s: failed dlopen() ALSA library file - %s, error: %s\n", __FUNCTION__, g_AlsaLibName, dlerror() ));
return 0;
}
PA_DEBUG(( "%s: loading ALSA API\n", __FUNCTION__ ));
#define _PA_LOAD_FUNC(x) do {             \
alsa_##x = dlsym( g_AlsaLib, #x );      \
if( alsa_##x == NULL ) {               \
PA_DEBUG(( "%s: symbol [%s] not found in - %s, error: %s\n", __FUNCTION__, #x, g_AlsaLibName, dlerror() )); }\
} while(0)
#else
#define _PA_LOAD_FUNC(x) alsa_##x = &x
#endif
_PA_LOAD_FUNC(snd_pcm_open);
_PA_LOAD_FUNC(snd_pcm_close);
_PA_LOAD_FUNC(snd_pcm_nonblock);
_PA_LOAD_FUNC(snd_pcm_frames_to_bytes);
_PA_LOAD_FUNC(snd_pcm_prepare);
_PA_LOAD_FUNC(snd_pcm_start);
_PA_LOAD_FUNC(snd_pcm_resume);
_PA_LOAD_FUNC(snd_pcm_wait);
_PA_LOAD_FUNC(snd_pcm_state);
_PA_LOAD_FUNC(snd_pcm_avail_update);
_PA_LOAD_FUNC(snd_pcm_areas_silence);
_PA_LOAD_FUNC(snd_pcm_mmap_begin);
_PA_LOAD_FUNC(snd_pcm_mmap_commit);
_PA_LOAD_FUNC(snd_pcm_readi);
_PA_LOAD_FUNC(snd_pcm_readn);
_PA_LOAD_FUNC(snd_pcm_writei);
_PA_LOAD_FUNC(snd_pcm_writen);
_PA_LOAD_FUNC(snd_pcm_drain);
_PA_LOAD_FUNC(snd_pcm_recover);
_PA_LOAD_FUNC(snd_pcm_drop);
_PA_LOAD_FUNC(snd_pcm_area_copy);
_PA_LOAD_FUNC(snd_pcm_poll_descriptors);
_PA_LOAD_FUNC(snd_pcm_poll_descriptors_count);
_PA_LOAD_FUNC(snd_pcm_poll_descriptors_revents);
_PA_LOAD_FUNC(snd_pcm_format_size);
_PA_LOAD_FUNC(snd_pcm_link);
_PA_LOAD_FUNC(snd_pcm_delay);
_PA_LOAD_FUNC(snd_pcm_hw_params_sizeof);
_PA_LOAD_FUNC(snd_pcm_hw_params_malloc);
_PA_LOAD_FUNC(snd_pcm_hw_params_free);
_PA_LOAD_FUNC(snd_pcm_hw_params_any);
_PA_LOAD_FUNC(snd_pcm_hw_params_set_access);
_PA_LOAD_FUNC(snd_pcm_hw_params_set_format);
_PA_LOAD_FUNC(snd_pcm_hw_params_set_channels);
_PA_LOAD_FUNC(snd_pcm_hw_params_set_rate_near);
_PA_LOAD_FUNC(snd_pcm_hw_params_set_rate);
_PA_LOAD_FUNC(snd_pcm_hw_params_set_rate_resample);
_PA_LOAD_FUNC(snd_pcm_hw_params_set_buffer_size);
_PA_LOAD_FUNC(snd_pcm_hw_params_set_buffer_size_near);
_PA_LOAD_FUNC(snd_pcm_hw_params_set_buffer_size_min);
_PA_LOAD_FUNC(snd_pcm_hw_params_set_period_size_near);
_PA_LOAD_FUNC(snd_pcm_hw_params_set_periods_integer);
_PA_LOAD_FUNC(snd_pcm_hw_params_set_periods_min);
_PA_LOAD_FUNC(snd_pcm_hw_params_get_buffer_size);
_PA_LOAD_FUNC(snd_pcm_hw_params_get_channels_min);
_PA_LOAD_FUNC(snd_pcm_hw_params_get_channels_max);
_PA_LOAD_FUNC(snd_pcm_hw_params_test_period_size);
_PA_LOAD_FUNC(snd_pcm_hw_params_test_format);
_PA_LOAD_FUNC(snd_pcm_hw_params_test_access);
_PA_LOAD_FUNC(snd_pcm_hw_params_dump);
_PA_LOAD_FUNC(snd_pcm_hw_params);
_PA_LOAD_FUNC(snd_pcm_hw_params_get_periods_min);
_PA_LOAD_FUNC(snd_pcm_hw_params_get_periods_max);
_PA_LOAD_FUNC(snd_pcm_hw_params_set_period_size);
_PA_LOAD_FUNC(snd_pcm_hw_params_get_period_size_min);
_PA_LOAD_FUNC(snd_pcm_hw_params_get_period_size_max);
_PA_LOAD_FUNC(snd_pcm_hw_params_get_buffer_size_max);
_PA_LOAD_FUNC(snd_pcm_hw_params_get_rate_min);
_PA_LOAD_FUNC(snd_pcm_hw_params_get_rate_max);
_PA_LOAD_FUNC(snd_pcm_hw_params_get_rate_numden);
_PA_LOAD_FUNC(snd_pcm_sw_params_sizeof);
_PA_LOAD_FUNC(snd_pcm_sw_params_malloc);
_PA_LOAD_FUNC(snd_pcm_sw_params_current);
_PA_LOAD_FUNC(snd_pcm_sw_params_set_avail_min);
_PA_LOAD_FUNC(snd_pcm_sw_params);
_PA_LOAD_FUNC(snd_pcm_sw_params_free);
_PA_LOAD_FUNC(snd_pcm_sw_params_set_start_threshold);
_PA_LOAD_FUNC(snd_pcm_sw_params_set_stop_threshold);
_PA_LOAD_FUNC(snd_pcm_sw_params_get_boundary);
_PA_LOAD_FUNC(snd_pcm_sw_params_set_silence_threshold);
_PA_LOAD_FUNC(snd_pcm_sw_params_set_silence_size);
_PA_LOAD_FUNC(snd_pcm_sw_params_set_xfer_align);
_PA_LOAD_FUNC(snd_pcm_sw_params_set_tstamp_mode);
_PA_LOAD_FUNC(snd_pcm_info);
_PA_LOAD_FUNC(snd_pcm_info_sizeof);
_PA_LOAD_FUNC(snd_pcm_info_malloc);
_PA_LOAD_FUNC(snd_pcm_info_free);
_PA_LOAD_FUNC(snd_pcm_info_set_device);
_PA_LOAD_FUNC(snd_pcm_info_set_subdevice);
_PA_LOAD_FUNC(snd_pcm_info_set_stream);
_PA_LOAD_FUNC(snd_pcm_info_get_name);
_PA_LOAD_FUNC(snd_pcm_info_get_card);
_PA_LOAD_FUNC(snd_ctl_pcm_next_device);
_PA_LOAD_FUNC(snd_ctl_pcm_info);
_PA_LOAD_FUNC(snd_ctl_open);
_PA_LOAD_FUNC(snd_ctl_close);
_PA_LOAD_FUNC(snd_ctl_card_info_malloc);
_PA_LOAD_FUNC(snd_ctl_card_info_free);
_PA_LOAD_FUNC(snd_ctl_card_info);
_PA_LOAD_FUNC(snd_ctl_card_info_sizeof);
_PA_LOAD_FUNC(snd_ctl_card_info_get_name);
_PA_LOAD_FUNC(snd_config);
_PA_LOAD_FUNC(snd_config_update);
_PA_LOAD_FUNC(snd_config_search);
_PA_LOAD_FUNC(snd_config_iterator_entry);
_PA_LOAD_FUNC(snd_config_iterator_first);
_PA_LOAD_FUNC(snd_config_iterator_end);
_PA_LOAD_FUNC(snd_config_iterator_next);
_PA_LOAD_FUNC(snd_config_get_string);
_PA_LOAD_FUNC(snd_config_get_id);
_PA_LOAD_FUNC(snd_config_update_free_global);
_PA_LOAD_FUNC(snd_pcm_status);
_PA_LOAD_FUNC(snd_pcm_status_sizeof);
_PA_LOAD_FUNC(snd_pcm_status_get_tstamp);
_PA_LOAD_FUNC(snd_pcm_status_get_htstamp);
_PA_LOAD_FUNC(snd_pcm_status_get_state);
_PA_LOAD_FUNC(snd_pcm_status_get_trigger_tstamp);
_PA_LOAD_FUNC(snd_pcm_status_get_trigger_htstamp);
_PA_LOAD_FUNC(snd_pcm_status_get_delay);
_PA_LOAD_FUNC(snd_card_next);
_PA_LOAD_FUNC(snd_asoundlib_version);
_PA_LOAD_FUNC(snd_strerror);
_PA_LOAD_FUNC(snd_output_stdio_attach);
#undef _PA_LOAD_FUNC
#ifdef PA_ALSA_DYNAMIC
PA_DEBUG(( "%s: loaded ALSA API - ok\n", __FUNCTION__ ));
#define _PA_VALIDATE_LOAD_REPLACEMENT(x)\
do {\
if( alsa_##x == NULL )\
{\
alsa_##x = &_PA_LOCAL_IMPL(x);\
PA_DEBUG(( "%s: replacing [%s] with local implementation\n", __FUNCTION__, #x ));\
}\
} while (0)
_PA_VALIDATE_LOAD_REPLACEMENT(snd_pcm_hw_params_set_rate_near);
_PA_VALIDATE_LOAD_REPLACEMENT(snd_pcm_hw_params_set_buffer_size_near);
_PA_VALIDATE_LOAD_REPLACEMENT(snd_pcm_hw_params_set_period_size_near);
_PA_VALIDATE_LOAD_REPLACEMENT(snd_pcm_hw_params_get_channels_min);
_PA_VALIDATE_LOAD_REPLACEMENT(snd_pcm_hw_params_get_channels_max);
_PA_VALIDATE_LOAD_REPLACEMENT(snd_pcm_hw_params_get_periods_min);
_PA_VALIDATE_LOAD_REPLACEMENT(snd_pcm_hw_params_get_periods_max);
_PA_VALIDATE_LOAD_REPLACEMENT(snd_pcm_hw_params_get_period_size_min);
_PA_VALIDATE_LOAD_REPLACEMENT(snd_pcm_hw_params_get_period_size_max);
_PA_VALIDATE_LOAD_REPLACEMENT(snd_pcm_hw_params_get_buffer_size_max);
_PA_VALIDATE_LOAD_REPLACEMENT(snd_pcm_hw_params_get_rate_min);
_PA_VALIDATE_LOAD_REPLACEMENT(snd_pcm_hw_params_get_rate_max);
#undef _PA_LOCAL_IMPL
#undef _PA_VALIDATE_LOAD_REPLACEMENT
#endif  
return 1;
}
void PaAlsa_SetLibraryPathName( const char *pathName )
{
#ifdef PA_ALSA_DYNAMIC
g_AlsaLibName = pathName;
#else
(void)pathName;
#endif
}
static void PaAlsa_CloseLibrary()
{
#ifdef PA_ALSA_DYNAMIC
dlclose(g_AlsaLib);
g_AlsaLib = NULL;
#endif
}
#define ENSURE_(expr, code) \
do { \
int __pa_unsure_error_id;\
if( UNLIKELY( (__pa_unsure_error_id = (expr)) < 0 ) ) \
{ \
\
if( (code) == paUnanticipatedHostError && pthread_equal( pthread_self(), paUnixMainThread) ) \
{ \
PaUtil_SetLastHostErrorInfo( paALSA, __pa_unsure_error_id, alsa_snd_strerror( __pa_unsure_error_id ) ); \
} \
PaUtil_DebugPrint( "Expression '" #expr "' failed in '" __FILE__ "', line: " STRINGIZE( __LINE__ ) "\n" ); \
if( (code) == paUnanticipatedHostError ) \
PA_DEBUG(( "Host error description: %s\n", alsa_snd_strerror( __pa_unsure_error_id ) )); \
result = (code); \
goto error; \
} \
} while (0)
#define ASSERT_CALL_(expr, success) \
do {\
int __pa_assert_error_id;\
__pa_assert_error_id = (expr);\
assert( success == __pa_assert_error_id );\
} while (0)
static int numPeriods_ = 4;
static int busyRetries_ = 100;
int PaAlsa_SetNumPeriods( int numPeriods )
{
numPeriods_ = numPeriods;
return paNoError;
}
typedef enum
{
StreamDirection_In,
StreamDirection_Out
} StreamDirection;
typedef struct
{
PaSampleFormat hostSampleFormat;
int numUserChannels, numHostChannels;
int userInterleaved, hostInterleaved;
int canMmap;
void *nonMmapBuffer;
unsigned int nonMmapBufferSize;
PaDeviceIndex device;      
int deviceIsPlug;  
int useReventFix;  
snd_pcm_t *pcm;
snd_pcm_uframes_t framesPerPeriod, alsaBufferSize;
snd_pcm_format_t nativeFormat;
unsigned int nfds;
int ready;   
void **userBuffers;
snd_pcm_uframes_t offset;
StreamDirection streamDir;
snd_pcm_channel_area_t *channelAreas;   
} PaAlsaStreamComponent;
typedef struct PaAlsaStream
{
PaUtilStreamRepresentation streamRepresentation;
PaUtilCpuLoadMeasurer cpuLoadMeasurer;
PaUtilBufferProcessor bufferProcessor;
PaUnixThread thread;
unsigned long framesPerUserBuffer, maxFramesPerHostBuffer;
int primeBuffers;
int callbackMode;               
int pcmsSynced;                 
int rtSched;
struct pollfd* pfds;
int pollTimeout;
volatile sig_atomic_t callback_finished;  
volatile sig_atomic_t callbackAbort;     
volatile sig_atomic_t isActive;          
PaUnixMutex stateMtx;                    
int neverDropInput;
PaTime underrun;
PaTime overrun;
PaAlsaStreamComponent capture, playback;
}
PaAlsaStream;
typedef struct PaAlsaHostApiRepresentation
{
PaUtilHostApiRepresentation baseHostApiRep;
PaUtilStreamInterface callbackStreamInterface;
PaUtilStreamInterface blockingStreamInterface;
PaUtilAllocationGroup *allocations;
PaHostApiIndex hostApiIndex;
PaUint32 alsaLibVersion;  
}
PaAlsaHostApiRepresentation;
typedef struct PaAlsaDeviceInfo
{
PaDeviceInfo baseDeviceInfo;
char *alsaName;
int isPlug;
int minInputChannels;
int minOutputChannels;
}
PaAlsaDeviceInfo;
static void Terminate( struct PaUtilHostApiRepresentation *hostApi );
static PaError IsFormatSupported( struct PaUtilHostApiRepresentation *hostApi,
const PaStreamParameters *inputParameters,
const PaStreamParameters *outputParameters,
double sampleRate );
static PaError OpenStream( struct PaUtilHostApiRepresentation *hostApi,
PaStream** s,
const PaStreamParameters *inputParameters,
const PaStreamParameters *outputParameters,
double sampleRate,
unsigned long framesPerBuffer,
PaStreamFlags streamFlags,
PaStreamCallback *callback,
void *userData );
static PaError CloseStream( PaStream* stream );
static PaError StartStream( PaStream *stream );
static PaError StopStream( PaStream *stream );
static PaError AbortStream( PaStream *stream );
static PaError IsStreamStopped( PaStream *s );
static PaError IsStreamActive( PaStream *stream );
static PaTime GetStreamTime( PaStream *stream );
static double GetStreamCpuLoad( PaStream* stream );
static PaError BuildDeviceList( PaAlsaHostApiRepresentation *hostApi );
static int SetApproximateSampleRate( snd_pcm_t *pcm, snd_pcm_hw_params_t *hwParams, double sampleRate );
static int GetExactSampleRate( snd_pcm_hw_params_t *hwParams, double *sampleRate );
static PaUint32 PaAlsaVersionNum(void);
static void *CallbackThreadFunc( void *userData );
static signed long GetStreamReadAvailable( PaStream* s );
static signed long GetStreamWriteAvailable( PaStream* s );
static PaError ReadStream( PaStream* stream, void *buffer, unsigned long frames );
static PaError WriteStream( PaStream* stream, const void *buffer, unsigned long frames );
static const PaAlsaDeviceInfo *GetDeviceInfo( const PaUtilHostApiRepresentation *hostApi, int device )
{
return (const PaAlsaDeviceInfo *)hostApi->deviceInfos[device];
}
PaError PaAlsa_Initialize( PaUtilHostApiRepresentation **hostApi, PaHostApiIndex hostApiIndex )
{
PaError result = paNoError;
PaAlsaHostApiRepresentation *alsaHostApi = NULL;
if (!PaAlsa_LoadLibrary())
return paHostApiNotFound;
PA_UNLESS( alsaHostApi = (PaAlsaHostApiRepresentation*) PaUtil_AllocateMemory(
sizeof(PaAlsaHostApiRepresentation) ), paInsufficientMemory );
PA_UNLESS( alsaHostApi->allocations = PaUtil_CreateAllocationGroup(), paInsufficientMemory );
alsaHostApi->hostApiIndex = hostApiIndex;
alsaHostApi->alsaLibVersion = PaAlsaVersionNum();
*hostApi = (PaUtilHostApiRepresentation*)alsaHostApi;
(*hostApi)->info.structVersion = 1;
(*hostApi)->info.type = paALSA;
(*hostApi)->info.name = "ALSA";
(*hostApi)->Terminate = Terminate;
(*hostApi)->OpenStream = OpenStream;
(*hostApi)->IsFormatSupported = IsFormatSupported;
PA_ENSURE( BuildDeviceList( alsaHostApi ) );
PaUtil_InitializeStreamInterface( &alsaHostApi->callbackStreamInterface,
CloseStream, StartStream,
StopStream, AbortStream,
IsStreamStopped, IsStreamActive,
GetStreamTime, GetStreamCpuLoad,
PaUtil_DummyRead, PaUtil_DummyWrite,
PaUtil_DummyGetReadAvailable,
PaUtil_DummyGetWriteAvailable );
PaUtil_InitializeStreamInterface( &alsaHostApi->blockingStreamInterface,
CloseStream, StartStream,
StopStream, AbortStream,
IsStreamStopped, IsStreamActive,
GetStreamTime, PaUtil_DummyGetCpuLoad,
ReadStream, WriteStream,
GetStreamReadAvailable,
GetStreamWriteAvailable );
PA_ENSURE( PaUnixThreading_Initialize() );
return result;
error:
if( alsaHostApi )
{
if( alsaHostApi->allocations )
{
PaUtil_FreeAllAllocations( alsaHostApi->allocations );
PaUtil_DestroyAllocationGroup( alsaHostApi->allocations );
}
PaUtil_FreeMemory( alsaHostApi );
}
return result;
}
static void Terminate( struct PaUtilHostApiRepresentation *hostApi )
{
PaAlsaHostApiRepresentation *alsaHostApi = (PaAlsaHostApiRepresentation*)hostApi;
assert( hostApi );
if( alsaHostApi->allocations )
{
PaUtil_FreeAllAllocations( alsaHostApi->allocations );
PaUtil_DestroyAllocationGroup( alsaHostApi->allocations );
}
PaUtil_FreeMemory( alsaHostApi );
alsa_snd_config_update_free_global();
PaAlsa_CloseLibrary();
}
static PaError GropeDevice( snd_pcm_t* pcm, int isPlug, StreamDirection mode, int openBlocking,
PaAlsaDeviceInfo* devInfo )
{
PaError result = paNoError;
snd_pcm_hw_params_t *hwParams;
snd_pcm_uframes_t alsaBufferFrames, alsaPeriodFrames;
unsigned int minChans, maxChans;
int* minChannels, * maxChannels;
double * defaultLowLatency, * defaultHighLatency, * defaultSampleRate =
&devInfo->baseDeviceInfo.defaultSampleRate;
double defaultSr = *defaultSampleRate;
assert( pcm );
PA_DEBUG(( "%s: collecting info ..\n", __FUNCTION__ ));
if( StreamDirection_In == mode )
{
minChannels = &devInfo->minInputChannels;
maxChannels = &devInfo->baseDeviceInfo.maxInputChannels;
defaultLowLatency = &devInfo->baseDeviceInfo.defaultLowInputLatency;
defaultHighLatency = &devInfo->baseDeviceInfo.defaultHighInputLatency;
}
else
{
minChannels = &devInfo->minOutputChannels;
maxChannels = &devInfo->baseDeviceInfo.maxOutputChannels;
defaultLowLatency = &devInfo->baseDeviceInfo.defaultLowOutputLatency;
defaultHighLatency = &devInfo->baseDeviceInfo.defaultHighOutputLatency;
}
ENSURE_( alsa_snd_pcm_nonblock( pcm, 0 ), paUnanticipatedHostError );
alsa_snd_pcm_hw_params_alloca( &hwParams );
alsa_snd_pcm_hw_params_any( pcm, hwParams );
if( defaultSr >= 0 )
{
if( SetApproximateSampleRate( pcm, hwParams, defaultSr ) < 0 )
{
defaultSr = -1.;
alsa_snd_pcm_hw_params_any( pcm, hwParams );  
PA_DEBUG(( "%s: Original default samplerate failed, trying again ..\n", __FUNCTION__ ));
}
}
if( defaultSr < 0. )            
{
unsigned int sampleRate = 44100;         
alsa_snd_pcm_hw_params_set_rate_resample( pcm, hwParams, 0 );
if( alsa_snd_pcm_hw_params_set_rate_near( pcm, hwParams, &sampleRate, NULL ) < 0 )
{
result = paUnanticipatedHostError;
goto error;
}
ENSURE_( GetExactSampleRate( hwParams, &defaultSr ), paUnanticipatedHostError );
}
ENSURE_( alsa_snd_pcm_hw_params_get_channels_min( hwParams, &minChans ), paUnanticipatedHostError );
ENSURE_( alsa_snd_pcm_hw_params_get_channels_max( hwParams, &maxChans ), paUnanticipatedHostError );
assert( maxChans <= INT_MAX );
assert( maxChans > 0 );     
if( isPlug && maxChans > 128 )
{
maxChans = 128;
PA_DEBUG(( "%s: Limiting number of plugin channels to %u\n", __FUNCTION__, maxChans ));
}
alsaBufferFrames = 512;
alsaPeriodFrames = 128;
ENSURE_( alsa_snd_pcm_hw_params_set_buffer_size_near( pcm, hwParams, &alsaBufferFrames ), paUnanticipatedHostError );
ENSURE_( alsa_snd_pcm_hw_params_set_period_size_near( pcm, hwParams, &alsaPeriodFrames, NULL ), paUnanticipatedHostError );
*defaultLowLatency = (double) (alsaBufferFrames - alsaPeriodFrames) / defaultSr;
alsaBufferFrames = 2048;
alsaPeriodFrames = 512;
ENSURE_( alsa_snd_pcm_hw_params_any( pcm, hwParams ), paUnanticipatedHostError );
ENSURE_( SetApproximateSampleRate( pcm, hwParams, defaultSr ), paUnanticipatedHostError );
ENSURE_( alsa_snd_pcm_hw_params_set_buffer_size_near( pcm, hwParams, &alsaBufferFrames ), paUnanticipatedHostError );
ENSURE_( alsa_snd_pcm_hw_params_set_period_size_near( pcm, hwParams, &alsaPeriodFrames, NULL ), paUnanticipatedHostError );
*defaultHighLatency = (double) (alsaBufferFrames - alsaPeriodFrames) / defaultSr;
*minChannels = (int)minChans;
*maxChannels = (int)maxChans;
*defaultSampleRate = defaultSr;
end:
alsa_snd_pcm_close( pcm );
return result;
error:
goto end;
}
static void InitializeDeviceInfo( PaDeviceInfo *deviceInfo )
{
deviceInfo->structVersion = -1;
deviceInfo->name = NULL;
deviceInfo->hostApi = -1;
deviceInfo->maxInputChannels = 0;
deviceInfo->maxOutputChannels = 0;
deviceInfo->defaultLowInputLatency = -1.;
deviceInfo->defaultLowOutputLatency = -1.;
deviceInfo->defaultHighInputLatency = -1.;
deviceInfo->defaultHighOutputLatency = -1.;
deviceInfo->defaultSampleRate = -1.;
}
static PaUint32 PaAlsaVersionNum(void)
{
char* verStr;
PaUint32 verNum;
verStr = (char*) alsa_snd_asoundlib_version();
verNum = ALSA_VERSION_INT( atoi(verStr), atoi(verStr + 2), atoi(verStr + 4) );
PA_DEBUG(( "ALSA version (build): " SND_LIB_VERSION_STR "\nALSA version (runtime): %s\n", verStr ));
return verNum;
}
typedef struct
{
char *alsaName;
char *name;
int isPlug;
int hasPlayback;
int hasCapture;
} HwDevInfo;
HwDevInfo predefinedNames[] = {
{ "center_lfe", NULL, 0, 1, 0 },
{ "dmix", NULL, 0, 1, 0 },
{ "front", NULL, 0, 1, 0 },
{ "iec958", NULL, 0, 1, 0 },
{ "rear", NULL, 0, 1, 0 },
{ "side", NULL, 0, 1, 0 },
{ "surround40", NULL, 0, 1, 0 },
{ "surround41", NULL, 0, 1, 0 },
{ "surround50", NULL, 0, 1, 0 },
{ "surround51", NULL, 0, 1, 0 },
{ "surround71", NULL, 0, 1, 0 },
{ "AndroidPlayback_Earpiece_normal",         NULL, 0, 1, 0 },
{ "AndroidPlayback_Speaker_normal",          NULL, 0, 1, 0 },
{ "AndroidPlayback_Bluetooth_normal",        NULL, 0, 1, 0 },
{ "AndroidPlayback_Headset_normal",          NULL, 0, 1, 0 },
{ "AndroidPlayback_Speaker_Headset_normal",  NULL, 0, 1, 0 },
{ "AndroidPlayback_Bluetooth-A2DP_normal",   NULL, 0, 1, 0 },
{ "AndroidPlayback_ExtraDockSpeaker_normal", NULL, 0, 1, 0 },
{ "AndroidPlayback_TvOut_normal",            NULL, 0, 1, 0 },
{ "AndroidRecord_Microphone",                NULL, 0, 0, 1 },
{ "AndroidRecord_Earpiece_normal",           NULL, 0, 0, 1 },
{ "AndroidRecord_Speaker_normal",            NULL, 0, 0, 1 },
{ "AndroidRecord_Headset_normal",            NULL, 0, 0, 1 },
{ "AndroidRecord_Bluetooth_normal",          NULL, 0, 0, 1 },
{ "AndroidRecord_Speaker_Headset_normal",    NULL, 0, 0, 1 },
{ NULL, NULL, 0, 1, 0 }
};
static const HwDevInfo *FindDeviceName( const char *name )
{
int i;
for( i = 0; predefinedNames[i].alsaName; i++ )
{
if( strcmp( name, predefinedNames[i].alsaName ) == 0 )
{
return &predefinedNames[i];
}
}
return NULL;
}
static PaError PaAlsa_StrDup( PaAlsaHostApiRepresentation *alsaApi,
char **dst,
const char *src)
{
PaError result = paNoError;
int len = strlen( src ) + 1;
PA_UNLESS( *dst = (char *)PaUtil_GroupAllocateMemory( alsaApi->allocations, len ),
paInsufficientMemory );
strncpy( *dst, src, len );
error:
return result;
}
static int IgnorePlugin( const char *pluginId )
{
static const char *ignoredPlugins[] = {"hw", "plughw", "plug", "dsnoop", "tee",
"file", "null", "shm", "cards", "rate_convert", NULL};
int i = 0;
if( getenv( "PA_ALSA_IGNORE_ALL_PLUGINS" ) && atoi( getenv( "PA_ALSA_IGNORE_ALL_PLUGINS") ) )
return 1;
while( ignoredPlugins[i] )
{
if( !strcmp( pluginId, ignoredPlugins[i] ) )
{
return 1;
}
++i;
}
return 0;
}
static char *SkipCardDetailsInName( char *infoSkipName, char *cardRefName )
{
char *lastSpacePosn = infoSkipName;
while( *cardRefName )
{
while( *infoSkipName && *cardRefName && *infoSkipName == *cardRefName)
{
infoSkipName++;
cardRefName++;
if( *infoSkipName == ' ' || *infoSkipName == '\0' )
lastSpacePosn = infoSkipName;
}
infoSkipName = lastSpacePosn;
while( *cardRefName && ( *cardRefName++ != ' ' ));
}
if( *infoSkipName == '\0' )
return "-";  
while( *lastSpacePosn && *lastSpacePosn == ' ' )
lastSpacePosn++;
if(( *lastSpacePosn == '-' || *lastSpacePosn == ':' ) && *(lastSpacePosn + 1) == ' ' )
lastSpacePosn += 2;
return lastSpacePosn;
}
static int OpenPcm( snd_pcm_t **pcmp, const char *name, snd_pcm_stream_t stream, int mode, int waitOnBusy )
{
int ret, tries = 0, maxTries = waitOnBusy ? busyRetries_ : 0;
ret = alsa_snd_pcm_open( pcmp, name, stream, mode );
for( tries = 0; tries < maxTries && -EBUSY == ret; ++tries )
{
Pa_Sleep( 10 );
ret = alsa_snd_pcm_open( pcmp, name, stream, mode );
if( -EBUSY != ret )
{
PA_DEBUG(( "%s: Successfully opened initially busy device after %d tries\n", __FUNCTION__, tries ));
}
}
if( -EBUSY == ret )
{
PA_DEBUG(( "%s: Failed to open busy device '%s'\n", __FUNCTION__, name ));
}
else
{
if( ret < 0 )
PA_DEBUG(( "%s: Opened device '%s' ptr[%p] - result: [%d:%s]\n", __FUNCTION__, name, *pcmp, ret, alsa_snd_strerror(ret) ));
}
return ret;
}
static PaError FillInDevInfo( PaAlsaHostApiRepresentation *alsaApi, HwDevInfo* deviceHwInfo, int blocking,
PaAlsaDeviceInfo* devInfo, int* devIdx )
{
PaError result = 0;
PaDeviceInfo *baseDeviceInfo = &devInfo->baseDeviceInfo;
snd_pcm_t *pcm = NULL;
PaUtilHostApiRepresentation *baseApi = &alsaApi->baseHostApiRep;
PA_DEBUG(( "%s: Filling device info for: %s\n", __FUNCTION__, deviceHwInfo->name ));
InitializeDeviceInfo( baseDeviceInfo );
if( deviceHwInfo->hasCapture &&
OpenPcm( &pcm, deviceHwInfo->alsaName, SND_PCM_STREAM_CAPTURE, blocking, 0 ) >= 0 )
{
if( GropeDevice( pcm, deviceHwInfo->isPlug, StreamDirection_In, blocking, devInfo ) != paNoError )
{
PA_DEBUG(( "%s: Failed groping %s for capture\n", __FUNCTION__, deviceHwInfo->alsaName ));
goto end;
}
}
if( deviceHwInfo->hasPlayback &&
OpenPcm( &pcm, deviceHwInfo->alsaName, SND_PCM_STREAM_PLAYBACK, blocking, 0 ) >= 0 )
{
if( GropeDevice( pcm, deviceHwInfo->isPlug, StreamDirection_Out, blocking, devInfo ) != paNoError )
{
PA_DEBUG(( "%s: Failed groping %s for playback\n", __FUNCTION__, deviceHwInfo->alsaName ));
goto end;
}
}
baseDeviceInfo->structVersion = 2;
baseDeviceInfo->hostApi = alsaApi->hostApiIndex;
baseDeviceInfo->name = deviceHwInfo->name;
devInfo->alsaName = deviceHwInfo->alsaName;
devInfo->isPlug = deviceHwInfo->isPlug;
if( baseDeviceInfo->maxInputChannels > 0 || baseDeviceInfo->maxOutputChannels > 0 )
{
if( ( baseApi->info.defaultInputDevice == paNoDevice ||
!strcmp( deviceHwInfo->alsaName, "default" ) ) && baseDeviceInfo->maxInputChannels > 0 )
{
baseApi->info.defaultInputDevice = *devIdx;
PA_DEBUG(( "Default input device: %s\n", deviceHwInfo->name ));
}
if( ( baseApi->info.defaultOutputDevice == paNoDevice ||
!strcmp( deviceHwInfo->alsaName, "default" ) ) && baseDeviceInfo->maxOutputChannels > 0 )
{
baseApi->info.defaultOutputDevice = *devIdx;
PA_DEBUG(( "Default output device: %s\n", deviceHwInfo->name ));
}
PA_DEBUG(( "%s: Adding device %s: %d\n", __FUNCTION__, deviceHwInfo->name, *devIdx ));
baseApi->deviceInfos[*devIdx] = (PaDeviceInfo *) devInfo;
(*devIdx) += 1;
}
else
{
PA_DEBUG(( "%s: Skipped device: %s, all channels == 0\n", __FUNCTION__, deviceHwInfo->name ));
}
end:
return result;
}
static PaError BuildDeviceList( PaAlsaHostApiRepresentation *alsaApi )
{
PaUtilHostApiRepresentation *baseApi = &alsaApi->baseHostApiRep;
PaAlsaDeviceInfo *deviceInfoArray;
int cardIdx = -1, devIdx = 0;
snd_ctl_card_info_t *cardInfo;
PaError result = paNoError;
size_t numDeviceNames = 0, maxDeviceNames = 1, i;
HwDevInfo *hwDevInfos = NULL;
snd_config_t *topNode = NULL;
snd_pcm_info_t *pcmInfo;
int res;
int blocking = SND_PCM_NONBLOCK;
int usePlughw = 0;
char *hwPrefix = "";
char alsaCardName[50];
#ifdef PA_ENABLE_DEBUG_OUTPUT
PaTime startTime = PaUtil_GetTime();
#endif
if( getenv( "PA_ALSA_INITIALIZE_BLOCK" ) && atoi( getenv( "PA_ALSA_INITIALIZE_BLOCK" ) ) )
blocking = 0;
if( getenv( "PA_ALSA_PLUGHW" ) && atoi( getenv( "PA_ALSA_PLUGHW" ) ) )
{
usePlughw = 1;
hwPrefix = "plug";
PA_DEBUG(( "%s: Using Plughw\n", __FUNCTION__ ));
}
baseApi->info.defaultInputDevice = paNoDevice;
baseApi->info.defaultOutputDevice = paNoDevice;
cardIdx = -1;
alsa_snd_ctl_card_info_alloca( &cardInfo );
alsa_snd_pcm_info_alloca( &pcmInfo );
while( alsa_snd_card_next( &cardIdx ) == 0 && cardIdx >= 0 )
{
char *cardName;
int devIdx = -1;
snd_ctl_t *ctl;
char buf[66];
snprintf( alsaCardName, sizeof (alsaCardName), "hw:%d", cardIdx );
if( alsa_snd_ctl_open( &ctl, alsaCardName, 0 ) < 0 )
{
PA_DEBUG(( "%s: Unable to open device %s\n", __FUNCTION__, alsaCardName ));
continue;
}
alsa_snd_ctl_card_info( ctl, cardInfo );
PA_ENSURE( PaAlsa_StrDup( alsaApi, &cardName, alsa_snd_ctl_card_info_get_name( cardInfo )) );
while( alsa_snd_ctl_pcm_next_device( ctl, &devIdx ) == 0 && devIdx >= 0 )
{
char *alsaDeviceName, *deviceName, *infoName;
size_t len;
int hasPlayback = 0, hasCapture = 0;
snprintf( buf, sizeof (buf), "%s%s,%d", hwPrefix, alsaCardName, devIdx );
alsa_snd_pcm_info_set_device( pcmInfo, devIdx );
alsa_snd_pcm_info_set_subdevice( pcmInfo, 0 );
alsa_snd_pcm_info_set_stream( pcmInfo, SND_PCM_STREAM_CAPTURE );
if( alsa_snd_ctl_pcm_info( ctl, pcmInfo ) >= 0 )
{
hasCapture = 1;
}
alsa_snd_pcm_info_set_stream( pcmInfo, SND_PCM_STREAM_PLAYBACK );
if( alsa_snd_ctl_pcm_info( ctl, pcmInfo ) >= 0 )
{
hasPlayback = 1;
}
if( !hasPlayback && !hasCapture )
{
continue;
}
infoName = SkipCardDetailsInName( (char *)alsa_snd_pcm_info_get_name( pcmInfo ), cardName );
len = snprintf( NULL, 0, "%s: %s (%s)", cardName, infoName, buf ) + 1;
PA_UNLESS( deviceName = (char *)PaUtil_GroupAllocateMemory( alsaApi->allocations, len ),
paInsufficientMemory );
snprintf( deviceName, len, "%s: %s (%s)", cardName, infoName, buf );
PA_DEBUG(( "%s: Found device [%d]: %s\n", __FUNCTION__, numDeviceNames, deviceName ));
++numDeviceNames;
if( !hwDevInfos || numDeviceNames > maxDeviceNames )
{
maxDeviceNames *= 2;
PA_UNLESS( hwDevInfos = (HwDevInfo *) realloc( hwDevInfos, maxDeviceNames * sizeof (HwDevInfo) ),
paInsufficientMemory );
}
PA_ENSURE( PaAlsa_StrDup( alsaApi, &alsaDeviceName, buf ) );
hwDevInfos[ numDeviceNames - 1 ].alsaName = alsaDeviceName;
hwDevInfos[ numDeviceNames - 1 ].name = deviceName;
hwDevInfos[ numDeviceNames - 1 ].isPlug = usePlughw;
hwDevInfos[ numDeviceNames - 1 ].hasPlayback = hasPlayback;
hwDevInfos[ numDeviceNames - 1 ].hasCapture = hasCapture;
}
alsa_snd_ctl_close( ctl );
}
if( NULL == (*alsa_snd_config) )
{
ENSURE_( alsa_snd_config_update(), paUnanticipatedHostError );
PA_DEBUG(( "Updating snd_config\n" ));
}
assert( *alsa_snd_config );
if( ( res = alsa_snd_config_search( *alsa_snd_config, "pcm", &topNode ) ) >= 0 )
{
snd_config_iterator_t i, next;
alsa_snd_config_for_each( i, next, topNode )
{
const char *tpStr = "unknown", *idStr = NULL;
int err = 0;
char *alsaDeviceName, *deviceName;
const HwDevInfo *predefined = NULL;
snd_config_t *n = alsa_snd_config_iterator_entry( i ), * tp = NULL;;
if( (err = alsa_snd_config_search( n, "type", &tp )) < 0 )
{
if( -ENOENT != err )
{
ENSURE_(err, paUnanticipatedHostError);
}
}
else
{
ENSURE_( alsa_snd_config_get_string( tp, &tpStr ), paUnanticipatedHostError );
}
ENSURE_( alsa_snd_config_get_id( n, &idStr ), paUnanticipatedHostError );
if( IgnorePlugin( idStr ) )
{
PA_DEBUG(( "%s: Ignoring ALSA plugin device [%s] of type [%s]\n", __FUNCTION__, idStr, tpStr ));
continue;
}
PA_DEBUG(( "%s: Found plugin [%s] of type [%s]\n", __FUNCTION__, idStr, tpStr ));
PA_UNLESS( alsaDeviceName = (char*)PaUtil_GroupAllocateMemory( alsaApi->allocations,
strlen(idStr) + 6 ), paInsufficientMemory );
strcpy( alsaDeviceName, idStr );
PA_UNLESS( deviceName = (char*)PaUtil_GroupAllocateMemory( alsaApi->allocations,
strlen(idStr) + 1 ), paInsufficientMemory );
strcpy( deviceName, idStr );
++numDeviceNames;
if( !hwDevInfos || numDeviceNames > maxDeviceNames )
{
maxDeviceNames *= 2;
PA_UNLESS( hwDevInfos = (HwDevInfo *) realloc( hwDevInfos, maxDeviceNames * sizeof (HwDevInfo) ),
paInsufficientMemory );
}
predefined = FindDeviceName( alsaDeviceName );
hwDevInfos[numDeviceNames - 1].alsaName = alsaDeviceName;
hwDevInfos[numDeviceNames - 1].name     = deviceName;
hwDevInfos[numDeviceNames - 1].isPlug   = 1;
if( predefined )
{
hwDevInfos[numDeviceNames - 1].hasPlayback = predefined->hasPlayback;
hwDevInfos[numDeviceNames - 1].hasCapture  = predefined->hasCapture;
}
else
{
hwDevInfos[numDeviceNames - 1].hasPlayback = 1;
hwDevInfos[numDeviceNames - 1].hasCapture  = 1;
}
}
}
else
PA_DEBUG(( "%s: Iterating over ALSA plugins failed: %s\n", __FUNCTION__, alsa_snd_strerror( res ) ));
PA_UNLESS( baseApi->deviceInfos = (PaDeviceInfo**)PaUtil_GroupAllocateMemory(
alsaApi->allocations, sizeof(PaDeviceInfo*) * (numDeviceNames) ), paInsufficientMemory );
PA_UNLESS( deviceInfoArray = (PaAlsaDeviceInfo*)PaUtil_GroupAllocateMemory(
alsaApi->allocations, sizeof(PaAlsaDeviceInfo) * numDeviceNames ), paInsufficientMemory );
PA_DEBUG(( "%s: Filling device info for %d devices\n", __FUNCTION__, numDeviceNames ));
for( i = 0, devIdx = 0; i < numDeviceNames; ++i )
{
PaAlsaDeviceInfo* devInfo = &deviceInfoArray[i];
HwDevInfo* hwInfo = &hwDevInfos[i];
if( !strcmp( hwInfo->name, "dmix" ) || !strcmp( hwInfo->name, "default" ) )
{
continue;
}
PA_ENSURE( FillInDevInfo( alsaApi, hwInfo, blocking, devInfo, &devIdx ) );
}
assert( devIdx <= numDeviceNames );
for( i = 0; i < numDeviceNames; ++i )
{
PaAlsaDeviceInfo* devInfo = &deviceInfoArray[i];
HwDevInfo* hwInfo = &hwDevInfos[i];
if( strcmp( hwInfo->name, "dmix" ) && strcmp( hwInfo->name, "default" ) )
{
continue;
}
PA_ENSURE( FillInDevInfo( alsaApi, hwInfo, blocking, devInfo, &devIdx ) );
}
free( hwDevInfos );
baseApi->info.deviceCount = devIdx;    
#ifdef PA_ENABLE_DEBUG_OUTPUT
PA_DEBUG(( "%s: Building device list took %f seconds\n", __FUNCTION__, PaUtil_GetTime() - startTime ));
#endif
end:
return result;
error:
goto end;
}
static PaError ValidateParameters( const PaStreamParameters *parameters, PaUtilHostApiRepresentation *hostApi, StreamDirection mode )
{
PaError result = paNoError;
int maxChans;
const PaAlsaDeviceInfo *deviceInfo = NULL;
assert( parameters );
if( parameters->device != paUseHostApiSpecificDeviceSpecification )
{
assert( parameters->device < hostApi->info.deviceCount );
PA_UNLESS( parameters->hostApiSpecificStreamInfo == NULL, paBadIODeviceCombination );
deviceInfo = GetDeviceInfo( hostApi, parameters->device );
}
else
{
const PaAlsaStreamInfo *streamInfo = parameters->hostApiSpecificStreamInfo;
PA_UNLESS( parameters->device == paUseHostApiSpecificDeviceSpecification, paInvalidDevice );
PA_UNLESS( streamInfo->size == sizeof (PaAlsaStreamInfo) && streamInfo->version == 1,
paIncompatibleHostApiSpecificStreamInfo );
PA_UNLESS( streamInfo->deviceString != NULL, paInvalidDevice );
return paNoError;
}
assert( deviceInfo );
assert( parameters->hostApiSpecificStreamInfo == NULL );
maxChans = ( StreamDirection_In == mode ? deviceInfo->baseDeviceInfo.maxInputChannels :
deviceInfo->baseDeviceInfo.maxOutputChannels );
PA_UNLESS( parameters->channelCount <= maxChans, paInvalidChannelCount );
error:
return result;
}
static PaSampleFormat GetAvailableFormats( snd_pcm_t *pcm )
{
PaSampleFormat available = 0;
snd_pcm_hw_params_t *hwParams;
alsa_snd_pcm_hw_params_alloca( &hwParams );
alsa_snd_pcm_hw_params_any( pcm, hwParams );
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_FLOAT ) >= 0)
available |= paFloat32;
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_S32 ) >= 0)
available |= paInt32;
#ifdef PA_LITTLE_ENDIAN
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_S24_3LE ) >= 0)
available |= paInt24;
#elif defined PA_BIG_ENDIAN
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_S24_3BE ) >= 0)
available |= paInt24;
#endif
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_S16 ) >= 0)
available |= paInt16;
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_U8 ) >= 0)
available |= paUInt8;
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_S8 ) >= 0)
available |= paInt8;
return available;
}
static void LogAllAvailableFormats( snd_pcm_t *pcm )
{
PaSampleFormat available = 0;
snd_pcm_hw_params_t *hwParams;
alsa_snd_pcm_hw_params_alloca( &hwParams );
alsa_snd_pcm_hw_params_any( pcm, hwParams );
PA_DEBUG(( " --- Supported Formats ---\n" ));
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_S8 ) >= 0)
PA_DEBUG(( "SND_PCM_FORMAT_S8\n" ));
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_U8 ) >= 0)
PA_DEBUG(( "SND_PCM_FORMAT_U8\n" ));
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_S16_LE ) >= 0)
PA_DEBUG(( "SND_PCM_FORMAT_S16_LE\n" ));
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_S16_BE ) >= 0)
PA_DEBUG(( "SND_PCM_FORMAT_S16_BE\n" ));
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_U16_LE ) >= 0)
PA_DEBUG(( "SND_PCM_FORMAT_U16_LE\n" ));
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_U16_BE ) >= 0)
PA_DEBUG(( "SND_PCM_FORMAT_U16_BE\n" ));
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_S24_LE ) >= 0)
PA_DEBUG(( "SND_PCM_FORMAT_S24_LE\n" ));
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_S24_BE ) >= 0)
PA_DEBUG(( "SND_PCM_FORMAT_S24_BE\n" ));
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_U24_LE ) >= 0)
PA_DEBUG(( "SND_PCM_FORMAT_U24_LE\n" ));
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_U24_BE ) >= 0)
PA_DEBUG(( "SND_PCM_FORMAT_U24_BE\n" ));
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_FLOAT_LE ) >= 0)
PA_DEBUG(( "SND_PCM_FORMAT_FLOAT_LE\n" ));
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_FLOAT_BE ) >= 0)
PA_DEBUG(( "SND_PCM_FORMAT_FLOAT_BE\n" ));
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_FLOAT64_LE ) >= 0)
PA_DEBUG(( "SND_PCM_FORMAT_FLOAT64_LE\n" ));
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_FLOAT64_BE ) >= 0)
PA_DEBUG(( "SND_PCM_FORMAT_FLOAT64_BE\n" ));
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_IEC958_SUBFRAME_LE ) >= 0)
PA_DEBUG(( "SND_PCM_FORMAT_IEC958_SUBFRAME_LE\n" ));
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_IEC958_SUBFRAME_BE ) >= 0)
PA_DEBUG(( "SND_PCM_FORMAT_IEC958_SUBFRAME_BE\n" ));
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_MU_LAW ) >= 0)
PA_DEBUG(( "SND_PCM_FORMAT_MU_LAW\n" ));
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_A_LAW ) >= 0)
PA_DEBUG(( "SND_PCM_FORMAT_A_LAW\n" ));
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_IMA_ADPCM ) >= 0)
PA_DEBUG(( "SND_PCM_FORMAT_IMA_ADPCM\n" ));
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_MPEG ) >= 0)
PA_DEBUG(( "SND_PCM_FORMAT_MPEG\n" ));
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_GSM ) >= 0)
PA_DEBUG(( "SND_PCM_FORMAT_GSM\n" ));
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_SPECIAL ) >= 0)
PA_DEBUG(( "SND_PCM_FORMAT_SPECIAL\n" ));
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_S24_3LE ) >= 0)
PA_DEBUG(( "SND_PCM_FORMAT_S24_3LE\n" ));
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_S24_3BE ) >= 0)
PA_DEBUG(( "SND_PCM_FORMAT_S24_3BE\n" ));
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_U24_3LE ) >= 0)
PA_DEBUG(( "SND_PCM_FORMAT_U24_3LE\n" ));
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_U24_3BE ) >= 0)
PA_DEBUG(( "SND_PCM_FORMAT_U24_3BE\n" ));
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_S20_3LE ) >= 0)
PA_DEBUG(( "SND_PCM_FORMAT_S20_3LE\n" ));
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_S20_3BE ) >= 0)
PA_DEBUG(( "SND_PCM_FORMAT_S20_3BE\n" ));
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_U20_3LE ) >= 0)
PA_DEBUG(( "SND_PCM_FORMAT_U20_3LE\n" ));
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_U20_3BE ) >= 0)
PA_DEBUG(( "SND_PCM_FORMAT_U20_3BE\n" ));
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_S18_3LE ) >= 0)
PA_DEBUG(( "SND_PCM_FORMAT_S18_3LE\n" ));
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_S18_3BE ) >= 0)
PA_DEBUG(( "SND_PCM_FORMAT_S18_3BE\n" ));
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_U18_3LE ) >= 0)
PA_DEBUG(( "SND_PCM_FORMAT_U18_3LE\n" ));
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_U18_3BE ) >= 0)
PA_DEBUG(( "SND_PCM_FORMAT_U18_3BE\n" ));
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_S16 ) >= 0)
PA_DEBUG(( "SND_PCM_FORMAT_S16\n" ));
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_U16 ) >= 0)
PA_DEBUG(( "SND_PCM_FORMAT_U16\n" ));
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_S24 ) >= 0)
PA_DEBUG(( "SND_PCM_FORMAT_S24\n" ));
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_U24 ) >= 0)
PA_DEBUG(( "SND_PCM_FORMAT_U24\n" ));
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_S32 ) >= 0)
PA_DEBUG(( "SND_PCM_FORMAT_S32\n" ));
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_U32 ) >= 0)
PA_DEBUG(( "SND_PCM_FORMAT_U32\n" ));
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_FLOAT ) >= 0)
PA_DEBUG(( "SND_PCM_FORMAT_FLOAT\n" ));
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_FLOAT64 ) >= 0)
PA_DEBUG(( "SND_PCM_FORMAT_FLOAT64\n" ));
if( alsa_snd_pcm_hw_params_test_format( pcm, hwParams, SND_PCM_FORMAT_IEC958_SUBFRAME ) >= 0)
PA_DEBUG(( "SND_PCM_FORMAT_IEC958_SUBFRAME\n" ));
PA_DEBUG(( " -------------------------\n" ));
}
static snd_pcm_format_t Pa2AlsaFormat( PaSampleFormat paFormat )
{
switch( paFormat )
{
case paFloat32:
return SND_PCM_FORMAT_FLOAT;
case paInt16:
return SND_PCM_FORMAT_S16;
case paInt24:
#ifdef PA_LITTLE_ENDIAN
return SND_PCM_FORMAT_S24_3LE;
#elif defined PA_BIG_ENDIAN
return SND_PCM_FORMAT_S24_3BE;
#endif
case paInt32:
return SND_PCM_FORMAT_S32;
case paInt8:
return SND_PCM_FORMAT_S8;
case paUInt8:
return SND_PCM_FORMAT_U8;
default:
return SND_PCM_FORMAT_UNKNOWN;
}
}
static PaError AlsaOpen( const PaUtilHostApiRepresentation *hostApi, const PaStreamParameters *params, StreamDirection
streamDir, snd_pcm_t **pcm )
{
PaError result = paNoError;
int ret;
const char* deviceName = "";
const PaAlsaDeviceInfo *deviceInfo = NULL;
PaAlsaStreamInfo *streamInfo = (PaAlsaStreamInfo *)params->hostApiSpecificStreamInfo;
if( !streamInfo )
{
deviceInfo = GetDeviceInfo( hostApi, params->device );
deviceName = deviceInfo->alsaName;
}
else
deviceName = streamInfo->deviceString;
PA_DEBUG(( "%s: Opening device %s\n", __FUNCTION__, deviceName ));
if( (ret = OpenPcm( pcm, deviceName, streamDir == StreamDirection_In ? SND_PCM_STREAM_CAPTURE : SND_PCM_STREAM_PLAYBACK,
SND_PCM_NONBLOCK, 1 )) < 0 )
{
*pcm = NULL;
ENSURE_( ret, -EBUSY == ret ? paDeviceUnavailable : paBadIODeviceCombination );
}
ENSURE_( alsa_snd_pcm_nonblock( *pcm, 0 ), paUnanticipatedHostError );
end:
return result;
error:
goto end;
}
static PaError TestParameters( const PaUtilHostApiRepresentation *hostApi, const PaStreamParameters *parameters,
double sampleRate, StreamDirection streamDir )
{
PaError result = paNoError;
snd_pcm_t *pcm = NULL;
PaSampleFormat availableFormats;
unsigned int numHostChannels;
PaSampleFormat hostFormat;
snd_pcm_hw_params_t *hwParams;
alsa_snd_pcm_hw_params_alloca( &hwParams );
if( !parameters->hostApiSpecificStreamInfo )
{
const PaAlsaDeviceInfo *devInfo = GetDeviceInfo( hostApi, parameters->device );
numHostChannels = PA_MAX( parameters->channelCount, StreamDirection_In == streamDir ?
devInfo->minInputChannels : devInfo->minOutputChannels );
}
else
numHostChannels = parameters->channelCount;
PA_ENSURE( AlsaOpen( hostApi, parameters, streamDir, &pcm ) );
alsa_snd_pcm_hw_params_any( pcm, hwParams );
if( SetApproximateSampleRate( pcm, hwParams, sampleRate ) < 0 )
{
result = paInvalidSampleRate;
goto error;
}
if( alsa_snd_pcm_hw_params_set_channels( pcm, hwParams, numHostChannels ) < 0 )
{
result = paInvalidChannelCount;
goto error;
}
availableFormats = GetAvailableFormats( pcm );
PA_ENSURE( hostFormat = PaUtil_SelectClosestAvailableFormat( availableFormats, parameters->sampleFormat ) );
ENSURE_( alsa_snd_pcm_hw_params_set_format( pcm, hwParams, Pa2AlsaFormat( hostFormat ) ), paUnanticipatedHostError );
{
int ret = 0;
if( ( ret = alsa_snd_pcm_hw_params( pcm, hwParams ) ) < 0 )
{
if( -EINVAL == ret )
{
result = paBadIODeviceCombination;
goto error;
}
else if( -EBUSY == ret )
{
result = paDeviceUnavailable;
PA_DEBUG(( "%s: Device is busy\n", __FUNCTION__ ));
}
else
{
result = paUnanticipatedHostError;
}
ENSURE_( ret, result );
}
}
end:
if( pcm )
{
alsa_snd_pcm_close( pcm );
}
return result;
error:
goto end;
}
static PaError IsFormatSupported( struct PaUtilHostApiRepresentation *hostApi,
const PaStreamParameters *inputParameters,
const PaStreamParameters *outputParameters,
double sampleRate )
{
int inputChannelCount = 0, outputChannelCount = 0;
PaSampleFormat inputSampleFormat, outputSampleFormat;
PaError result = paFormatIsSupported;
if( inputParameters )
{
PA_ENSURE( ValidateParameters( inputParameters, hostApi, StreamDirection_In ) );
inputChannelCount = inputParameters->channelCount;
inputSampleFormat = inputParameters->sampleFormat;
}
if( outputParameters )
{
PA_ENSURE( ValidateParameters( outputParameters, hostApi, StreamDirection_Out ) );
outputChannelCount = outputParameters->channelCount;
outputSampleFormat = outputParameters->sampleFormat;
}
if( inputChannelCount )
{
if( ( result = TestParameters( hostApi, inputParameters, sampleRate, StreamDirection_In ) )
!= paNoError )
goto error;
}
if ( outputChannelCount )
{
if( ( result = TestParameters( hostApi, outputParameters, sampleRate, StreamDirection_Out ) )
!= paNoError )
goto error;
}
return paFormatIsSupported;
error:
return result;
}
static PaError PaAlsaStreamComponent_Initialize( PaAlsaStreamComponent *self, PaAlsaHostApiRepresentation *alsaApi,
const PaStreamParameters *params, StreamDirection streamDir, int callbackMode )
{
PaError result = paNoError;
PaSampleFormat userSampleFormat = params->sampleFormat, hostSampleFormat = paNoError;
assert( params->channelCount > 0 );
memset( self, 0, sizeof (PaAlsaStreamComponent) );
if( NULL == params->hostApiSpecificStreamInfo )
{
const PaAlsaDeviceInfo *devInfo = GetDeviceInfo( &alsaApi->baseHostApiRep, params->device );
self->numHostChannels = PA_MAX( params->channelCount, StreamDirection_In == streamDir ? devInfo->minInputChannels
: devInfo->minOutputChannels );
self->deviceIsPlug = devInfo->isPlug;
PA_DEBUG(( "%s: Host Chans %c %i\n", __FUNCTION__, streamDir == StreamDirection_In ? 'C' : 'P', self->numHostChannels ));
}
else
{
self->numHostChannels = params->channelCount;
if( strncmp( "hw:", ((PaAlsaStreamInfo *)params->hostApiSpecificStreamInfo)->deviceString, 3 ) != 0  )
self->deviceIsPlug = 1;  
}
if( self->deviceIsPlug && alsaApi->alsaLibVersion < ALSA_VERSION_INT( 1, 0, 16 ) )
self->useReventFix = 1;  
self->device = params->device;
PA_ENSURE( AlsaOpen( &alsaApi->baseHostApiRep, params, streamDir, &self->pcm ) );
self->nfds = alsa_snd_pcm_poll_descriptors_count( self->pcm );
PA_ENSURE( hostSampleFormat = PaUtil_SelectClosestAvailableFormat( GetAvailableFormats( self->pcm ), userSampleFormat ) );
self->hostSampleFormat = hostSampleFormat;
self->nativeFormat = Pa2AlsaFormat( hostSampleFormat );
self->hostInterleaved = self->userInterleaved = !( userSampleFormat & paNonInterleaved );
self->numUserChannels = params->channelCount;
self->streamDir = streamDir;
self->canMmap = 0;
self->nonMmapBuffer = NULL;
self->nonMmapBufferSize = 0;
if( !callbackMode && !self->userInterleaved )
{
PA_UNLESS( self->userBuffers = PaUtil_AllocateMemory( sizeof (void *) * self->numUserChannels ),
paInsufficientMemory );
}
error:
if ( hostSampleFormat == paSampleFormatNotSupported )
{
LogAllAvailableFormats( self->pcm );
PA_DEBUG(( "%s: Please provide the log output to PortAudio developers, your hardware does not have any sample format implemented yet.\n", __FUNCTION__ ));
}
return result;
}
static void PaAlsaStreamComponent_Terminate( PaAlsaStreamComponent *self )
{
alsa_snd_pcm_close( self->pcm );
PaUtil_FreeMemory( self->userBuffers );  
PaUtil_FreeMemory( self->nonMmapBuffer );
}
static PaError PaAlsaStreamComponent_InitialConfigure( PaAlsaStreamComponent *self, const PaStreamParameters *params,
int primeBuffers, snd_pcm_hw_params_t *hwParams, double *sampleRate )
{
PaError result = paNoError;
snd_pcm_access_t accessMode, alternateAccessMode;
int dir = 0;
snd_pcm_t *pcm = self->pcm;
double sr = *sampleRate;
unsigned int minPeriods = 2;
ENSURE_( alsa_snd_pcm_hw_params_any( pcm, hwParams ), paUnanticipatedHostError );
ENSURE_( alsa_snd_pcm_hw_params_set_periods_integer( pcm, hwParams ), paUnanticipatedHostError );
dir = 0;
ENSURE_( alsa_snd_pcm_hw_params_set_periods_min( pcm, hwParams, &minPeriods, &dir ), paUnanticipatedHostError );
if( self->userInterleaved )
{
accessMode          = SND_PCM_ACCESS_MMAP_INTERLEAVED;
alternateAccessMode = SND_PCM_ACCESS_MMAP_NONINTERLEAVED;
self->canMmap = alsa_snd_pcm_hw_params_test_access( pcm, hwParams, accessMode ) >= 0 ||
alsa_snd_pcm_hw_params_test_access( pcm, hwParams, alternateAccessMode ) >= 0;
PA_DEBUG(( "%s: device MMAP SND_PCM_ACCESS_MMAP_INTERLEAVED: %s\n", __FUNCTION__, ( alsa_snd_pcm_hw_params_test_access( pcm, hwParams, accessMode ) >= 0 ? "YES" : "NO" ) ));
PA_DEBUG(( "%s: device MMAP SND_PCM_ACCESS_MMAP_NONINTERLEAVED: %s\n", __FUNCTION__, ( alsa_snd_pcm_hw_params_test_access( pcm, hwParams, alternateAccessMode ) >= 0 ? "YES" : "NO" ) ));
if( !self->canMmap )
{
accessMode          = SND_PCM_ACCESS_RW_INTERLEAVED;
alternateAccessMode = SND_PCM_ACCESS_RW_NONINTERLEAVED;
}
}
else
{
accessMode          = SND_PCM_ACCESS_MMAP_NONINTERLEAVED;
alternateAccessMode = SND_PCM_ACCESS_MMAP_INTERLEAVED;
self->canMmap = alsa_snd_pcm_hw_params_test_access( pcm, hwParams, accessMode ) >= 0 ||
alsa_snd_pcm_hw_params_test_access( pcm, hwParams, alternateAccessMode ) >= 0;
PA_DEBUG((" %s: device MMAP SND_PCM_ACCESS_MMAP_NONINTERLEAVED: %s\n", __FUNCTION__, ( alsa_snd_pcm_hw_params_test_access( pcm, hwParams, accessMode ) >= 0 ? "YES" : "NO" ) ));
PA_DEBUG(( "%s: device MMAP SND_PCM_ACCESS_MMAP_INTERLEAVED: %s\n", __FUNCTION__, ( alsa_snd_pcm_hw_params_test_access( pcm, hwParams, alternateAccessMode ) >= 0 ? "YES" : "NO" ) ));
if( !self->canMmap )
{
accessMode          = SND_PCM_ACCESS_RW_NONINTERLEAVED;
alternateAccessMode = SND_PCM_ACCESS_RW_INTERLEAVED;
}
}
PA_DEBUG(( "%s: device can MMAP: %s\n", __FUNCTION__, ( self->canMmap ? "YES" : "NO" ) ));
if( alsa_snd_pcm_hw_params_set_access( pcm, hwParams, accessMode ) < 0 )
{
int err = 0;
if( ( err = alsa_snd_pcm_hw_params_set_access( pcm, hwParams, alternateAccessMode )) < 0 )
{
result = paUnanticipatedHostError;
PaUtil_SetLastHostErrorInfo( paALSA, err, alsa_snd_strerror( err ) );
goto error;
}
self->hostInterleaved = !self->userInterleaved;
}
ENSURE_( alsa_snd_pcm_hw_params_set_format( pcm, hwParams, self->nativeFormat ), paUnanticipatedHostError );
if( ( result = SetApproximateSampleRate( pcm, hwParams, sr )) != paUnanticipatedHostError )
{
ENSURE_( GetExactSampleRate( hwParams, &sr ), paUnanticipatedHostError );
if( result == paInvalidSampleRate )  
{  
PA_DEBUG(( "%s: Wanted %.3f, closest sample rate was %.3f\n", __FUNCTION__, sampleRate, sr ));
PA_ENSURE( paInvalidSampleRate );
}
}
else
{
PA_ENSURE( paUnanticipatedHostError );
}
ENSURE_( alsa_snd_pcm_hw_params_set_channels( pcm, hwParams, self->numHostChannels ), paInvalidChannelCount );
*sampleRate = sr;
end:
return result;
error:
goto end;
}
static PaError PaAlsaStreamComponent_FinishConfigure( PaAlsaStreamComponent *self, snd_pcm_hw_params_t* hwParams,
const PaStreamParameters *params, int primeBuffers, double sampleRate, PaTime* latency )
{
PaError result = paNoError;
snd_pcm_sw_params_t* swParams;
snd_pcm_uframes_t bufSz = 0;
*latency = -1.;
alsa_snd_pcm_sw_params_alloca( &swParams );
bufSz = params->suggestedLatency * sampleRate + self->framesPerPeriod;
ENSURE_( alsa_snd_pcm_hw_params_set_buffer_size_near( self->pcm, hwParams, &bufSz ), paUnanticipatedHostError );
{
int r = alsa_snd_pcm_hw_params( self->pcm, hwParams );
#ifdef PA_ENABLE_DEBUG_OUTPUT
if( r < 0 )
{
snd_output_t *output = NULL;
alsa_snd_output_stdio_attach( &output, stderr, 0 );
alsa_snd_pcm_hw_params_dump( hwParams, output );
}
#endif
ENSURE_( r, paUnanticipatedHostError );
}
if( alsa_snd_pcm_hw_params_get_buffer_size != NULL )
{
ENSURE_( alsa_snd_pcm_hw_params_get_buffer_size( hwParams, &self->alsaBufferSize ), paUnanticipatedHostError );
}
else
{
self->alsaBufferSize = bufSz;
}
*latency = (self->alsaBufferSize - self->framesPerPeriod) / sampleRate;
ENSURE_( alsa_snd_pcm_sw_params_current( self->pcm, swParams ), paUnanticipatedHostError );
ENSURE_( alsa_snd_pcm_sw_params_set_start_threshold( self->pcm, swParams, self->framesPerPeriod ), paUnanticipatedHostError );
ENSURE_( alsa_snd_pcm_sw_params_set_stop_threshold( self->pcm, swParams, self->alsaBufferSize ), paUnanticipatedHostError );
if( !primeBuffers )  
{
snd_pcm_uframes_t boundary;
ENSURE_( alsa_snd_pcm_sw_params_get_boundary( swParams, &boundary ), paUnanticipatedHostError );
ENSURE_( alsa_snd_pcm_sw_params_set_silence_threshold( self->pcm, swParams, 0 ), paUnanticipatedHostError );
ENSURE_( alsa_snd_pcm_sw_params_set_silence_size( self->pcm, swParams, boundary ), paUnanticipatedHostError );
}
ENSURE_( alsa_snd_pcm_sw_params_set_avail_min( self->pcm, swParams, self->framesPerPeriod ), paUnanticipatedHostError );
ENSURE_( alsa_snd_pcm_sw_params_set_xfer_align( self->pcm, swParams, 1 ), paUnanticipatedHostError );
ENSURE_( alsa_snd_pcm_sw_params_set_tstamp_mode( self->pcm, swParams, SND_PCM_TSTAMP_ENABLE ), paUnanticipatedHostError );
ENSURE_( alsa_snd_pcm_sw_params( self->pcm, swParams ), paUnanticipatedHostError );
error:
return result;
}
static PaError PaAlsaStream_Initialize( PaAlsaStream *self, PaAlsaHostApiRepresentation *alsaApi, const PaStreamParameters *inParams,
const PaStreamParameters *outParams, double sampleRate, unsigned long framesPerUserBuffer, PaStreamCallback callback,
PaStreamFlags streamFlags, void *userData )
{
PaError result = paNoError;
assert( self );
memset( self, 0, sizeof( PaAlsaStream ) );
if( NULL != callback )
{
PaUtil_InitializeStreamRepresentation( &self->streamRepresentation,
&alsaApi->callbackStreamInterface,
callback, userData );
self->callbackMode = 1;
}
else
{
PaUtil_InitializeStreamRepresentation( &self->streamRepresentation,
&alsaApi->blockingStreamInterface,
NULL, userData );
}
self->framesPerUserBuffer = framesPerUserBuffer;
self->neverDropInput = streamFlags & paNeverDropInput;
memset( &self->capture, 0, sizeof (PaAlsaStreamComponent) );
memset( &self->playback, 0, sizeof (PaAlsaStreamComponent) );
if( inParams )
{
PA_ENSURE( PaAlsaStreamComponent_Initialize( &self->capture, alsaApi, inParams, StreamDirection_In, NULL != callback ) );
}
if( outParams )
{
PA_ENSURE( PaAlsaStreamComponent_Initialize( &self->playback, alsaApi, outParams, StreamDirection_Out, NULL != callback ) );
}
assert( self->capture.nfds || self->playback.nfds );
PA_UNLESS( self->pfds = (struct pollfd*)PaUtil_AllocateMemory( ( self->capture.nfds +
self->playback.nfds ) * sizeof( struct pollfd ) ), paInsufficientMemory );
PaUtil_InitializeCpuLoadMeasurer( &self->cpuLoadMeasurer, sampleRate );
ASSERT_CALL_( PaUnixMutex_Initialize( &self->stateMtx ), paNoError );
error:
return result;
}
static void PaAlsaStream_Terminate( PaAlsaStream *self )
{
assert( self );
if( self->capture.pcm )
{
PaAlsaStreamComponent_Terminate( &self->capture );
}
if( self->playback.pcm )
{
PaAlsaStreamComponent_Terminate( &self->playback );
}
PaUtil_FreeMemory( self->pfds );
ASSERT_CALL_( PaUnixMutex_Terminate( &self->stateMtx ), paNoError );
PaUtil_FreeMemory( self );
}
static int CalculatePollTimeout( const PaAlsaStream *stream, unsigned long frames )
{
assert( stream->streamRepresentation.streamInfo.sampleRate > 0.0 );
return (int)ceil( 1000 * frames / stream->streamRepresentation.streamInfo.sampleRate );
}
static unsigned long PaAlsa_AlignBackward(unsigned long v, unsigned long align)
{
return ( v - ( align ? v % align : 0 ) );
}
static unsigned long PaAlsa_AlignForward(unsigned long v, unsigned long align)
{
unsigned long remainder = ( align ? ( v % align ) : 0);
return ( remainder != 0 ? v + ( align - remainder ) : v );
}
static unsigned long PaAlsa_GetFramesPerHostBuffer(unsigned long userFramesPerBuffer, PaTime suggestedLatency, double sampleRate)
{
unsigned long frames = userFramesPerBuffer + PA_MAX( userFramesPerBuffer, (unsigned long)( suggestedLatency * sampleRate ) );
return frames;
}
static PaError PaAlsaStreamComponent_DetermineFramesPerBuffer( PaAlsaStreamComponent* self, const PaStreamParameters* params,
unsigned long framesPerUserBuffer, double sampleRate, snd_pcm_hw_params_t* hwParams, int* accurate )
{
PaError result = paNoError;
unsigned long bufferSize, framesPerHostBuffer;
int dir = 0;
bufferSize = PaAlsa_GetFramesPerHostBuffer(framesPerUserBuffer, params->suggestedLatency, sampleRate);
PA_DEBUG(( "%s: user-buffer (frames)           = %lu\n", __FUNCTION__, framesPerUserBuffer ));
PA_DEBUG(( "%s: user-buffer (sec)              = %f\n",  __FUNCTION__, (double)(framesPerUserBuffer / sampleRate) ));
PA_DEBUG(( "%s: suggested latency (sec)        = %f\n",  __FUNCTION__, params->suggestedLatency ));
PA_DEBUG(( "%s: suggested host buffer (frames) = %lu\n", __FUNCTION__, bufferSize ));
PA_DEBUG(( "%s: suggested host buffer (sec)    = %f\n",  __FUNCTION__, (double)(bufferSize / sampleRate) ));
#ifdef PA_ALSA_USE_OBSOLETE_HOST_BUFFER_CALC
if( framesPerUserBuffer != paFramesPerBufferUnspecified )
{
if( bufferSize > framesPerUserBuffer )
{
snd_pcm_uframes_t remainder = bufferSize % framesPerUserBuffer;
if( remainder > framesPerUserBuffer / 2. )
bufferSize += framesPerUserBuffer - remainder;
else
bufferSize -= remainder;
assert( bufferSize % framesPerUserBuffer == 0 );
}
else if( framesPerUserBuffer % bufferSize != 0 )
{
if( bufferSize > framesPerUserBuffer * .75 )
{
bufferSize = framesPerUserBuffer;
}
else
{
snd_pcm_uframes_t newSz = framesPerUserBuffer;
while( newSz / 2 >= bufferSize )
{
if( framesPerUserBuffer % (newSz / 2) != 0 )
{
break;
}
newSz /= 2;
}
bufferSize = newSz;
}
assert( framesPerUserBuffer % bufferSize == 0 );
}
}
#endif
{
unsigned numPeriods = numPeriods_, maxPeriods = 0, minPeriods = numPeriods_;
dir = 0;
ENSURE_( alsa_snd_pcm_hw_params_get_periods_min( hwParams, &minPeriods, &dir ), paUnanticipatedHostError );
dir = 0;
ENSURE_( alsa_snd_pcm_hw_params_get_periods_max( hwParams, &maxPeriods, &dir ), paUnanticipatedHostError );
assert( maxPeriods > 1 );
numPeriods = PA_MIN(maxPeriods, PA_MAX(minPeriods, numPeriods));
PA_DEBUG(( "%s: periods min = %lu, max = %lu, req = %lu \n", __FUNCTION__, minPeriods, maxPeriods, numPeriods ));
#ifndef PA_ALSA_USE_OBSOLETE_HOST_BUFFER_CALC
framesPerHostBuffer = (bufferSize / numPeriods);
if( framesPerUserBuffer != paFramesPerBufferUnspecified )
{
framesPerHostBuffer = PaAlsa_AlignForward(framesPerHostBuffer, framesPerUserBuffer);
if( framesPerHostBuffer < framesPerUserBuffer )
{
assert( framesPerUserBuffer % framesPerHostBuffer == 0 );
if( alsa_snd_pcm_hw_params_test_period_size( self->pcm, hwParams, framesPerHostBuffer, 0 ) < 0 )
{
if( alsa_snd_pcm_hw_params_test_period_size( self->pcm, hwParams, framesPerHostBuffer * 2, 0 ) == 0 )
framesPerHostBuffer *= 2;
else if( alsa_snd_pcm_hw_params_test_period_size( self->pcm, hwParams, framesPerHostBuffer / 2, 0 ) == 0 )
framesPerHostBuffer /= 2;
}
}
else
{
assert( framesPerHostBuffer % framesPerUserBuffer == 0 );
if( alsa_snd_pcm_hw_params_test_period_size( self->pcm, hwParams, framesPerHostBuffer, 0 ) < 0 )
{
if( alsa_snd_pcm_hw_params_test_period_size( self->pcm, hwParams, framesPerHostBuffer + framesPerUserBuffer, 0 ) == 0 )
framesPerHostBuffer += framesPerUserBuffer;
else if( alsa_snd_pcm_hw_params_test_period_size( self->pcm, hwParams, framesPerHostBuffer - framesPerUserBuffer, 0 ) == 0 )
framesPerHostBuffer -= framesPerUserBuffer;
}
}
}
#endif
#ifdef PA_ALSA_USE_OBSOLETE_HOST_BUFFER_CALC
if( framesPerUserBuffer != paFramesPerBufferUnspecified )
{
framesPerHostBuffer = framesPerUserBuffer;
if( framesPerHostBuffer < bufferSize )
{
while( bufferSize / framesPerHostBuffer > numPeriods )
{
framesPerHostBuffer *= 2;
}
if( bufferSize / framesPerHostBuffer < numPeriods )
{
framesPerHostBuffer /= 2;
}
}
else
{
while( bufferSize / framesPerHostBuffer < numPeriods )
{
if( framesPerUserBuffer % ( framesPerHostBuffer / 2 ) != 0 )
{
break;
}
framesPerHostBuffer /= 2;
}
}
if( framesPerHostBuffer < framesPerUserBuffer )
{
assert( framesPerUserBuffer % framesPerHostBuffer == 0 );
if( alsa_snd_pcm_hw_params_test_period_size( self->pcm, hwParams, framesPerHostBuffer, 0 ) < 0 )
{
if( alsa_snd_pcm_hw_params_test_period_size( self->pcm, hwParams, framesPerHostBuffer * 2, 0 ) == 0 )
framesPerHostBuffer *= 2;
else if( alsa_snd_pcm_hw_params_test_period_size( self->pcm, hwParams, framesPerHostBuffer / 2, 0 ) == 0 )
framesPerHostBuffer /= 2;
}
}
else
{
assert( framesPerHostBuffer % framesPerUserBuffer == 0 );
if( alsa_snd_pcm_hw_params_test_period_size( self->pcm, hwParams, framesPerHostBuffer, 0 ) < 0 )
{
if( alsa_snd_pcm_hw_params_test_period_size( self->pcm, hwParams, framesPerHostBuffer + framesPerUserBuffer, 0 ) == 0 )
framesPerHostBuffer += framesPerUserBuffer;
else if( alsa_snd_pcm_hw_params_test_period_size( self->pcm, hwParams, framesPerHostBuffer - framesPerUserBuffer, 0 ) == 0 )
framesPerHostBuffer -= framesPerUserBuffer;
}
}
}
else
{
framesPerHostBuffer = bufferSize / numPeriods;
}
if( !self->canMmap && framesPerHostBuffer < 2048 )
framesPerHostBuffer = 2048;
#endif
PA_DEBUG(( "%s: suggested host buffer period   = %lu \n", __FUNCTION__, framesPerHostBuffer ));
}
{
snd_pcm_uframes_t min = 0, max = 0, minmax_diff;
ENSURE_( alsa_snd_pcm_hw_params_get_period_size_min( hwParams, &min, NULL ), paUnanticipatedHostError );
ENSURE_( alsa_snd_pcm_hw_params_get_period_size_max( hwParams, &max, NULL ), paUnanticipatedHostError );
minmax_diff = max - min;
if( framesPerHostBuffer < min )
{
PA_DEBUG(( "%s: The determined period size (%lu) is less than minimum (%lu)\n", __FUNCTION__, framesPerHostBuffer, min ));
framesPerHostBuffer = (( minmax_diff == 2 ) ? min + 1 : min );
}
else if( framesPerHostBuffer > max )
{
PA_DEBUG(( "%s: The determined period size (%lu) is greater than maximum (%lu)\n", __FUNCTION__, framesPerHostBuffer, max ));
framesPerHostBuffer = (( minmax_diff == 2 ) ? max - 1 : max );
}
PA_DEBUG(( "%s: device period minimum          = %lu\n", __FUNCTION__, min ));
PA_DEBUG(( "%s: device period maximum          = %lu\n", __FUNCTION__, max ));
PA_DEBUG(( "%s: host buffer period             = %lu\n", __FUNCTION__, framesPerHostBuffer ));
PA_DEBUG(( "%s: host buffer period latency     = %f\n", __FUNCTION__, (double)( framesPerHostBuffer / sampleRate ) ));
dir = 0;
ENSURE_( alsa_snd_pcm_hw_params_set_period_size_near( self->pcm, hwParams, &framesPerHostBuffer, &dir ), paUnanticipatedHostError );
if( dir != 0 )
{
PA_DEBUG(( "%s: The configured period size is non-integer.\n", __FUNCTION__, dir ));
*accurate = 0;
}
}
self->framesPerPeriod = framesPerHostBuffer;
error:
return result;
}
static PaError PaAlsaStream_DetermineFramesPerBuffer( PaAlsaStream* self, double sampleRate, const PaStreamParameters* inputParameters,
const PaStreamParameters* outputParameters, unsigned long framesPerUserBuffer, snd_pcm_hw_params_t* hwParamsCapture,
snd_pcm_hw_params_t* hwParamsPlayback, PaUtilHostBufferSizeMode* hostBufferSizeMode )
{
PaError result = paNoError;
unsigned long framesPerHostBuffer = 0;
int dir = 0;
int accurate = 1;
unsigned numPeriods = numPeriods_;
if( self->capture.pcm && self->playback.pcm )
{
if( framesPerUserBuffer == paFramesPerBufferUnspecified )
{
snd_pcm_uframes_t desiredBufSz, e, minPeriodSize, maxPeriodSize, optimalPeriodSize, periodSize,
minCapture, minPlayback, maxCapture, maxPlayback;
dir = 0;
ENSURE_( alsa_snd_pcm_hw_params_get_period_size_min( hwParamsCapture, &minCapture, &dir ), paUnanticipatedHostError );
dir = 0;
ENSURE_( alsa_snd_pcm_hw_params_get_period_size_min( hwParamsPlayback, &minPlayback, &dir ), paUnanticipatedHostError );
dir = 0;
ENSURE_( alsa_snd_pcm_hw_params_get_period_size_max( hwParamsCapture, &maxCapture, &dir ), paUnanticipatedHostError );
dir = 0;
ENSURE_( alsa_snd_pcm_hw_params_get_period_size_max( hwParamsPlayback, &maxPlayback, &dir ), paUnanticipatedHostError );
minPeriodSize = PA_MAX( minPlayback, minCapture );
maxPeriodSize = PA_MIN( maxPlayback, maxCapture );
PA_UNLESS( minPeriodSize <= maxPeriodSize, paBadIODeviceCombination );
desiredBufSz = (snd_pcm_uframes_t)( PA_MIN( outputParameters->suggestedLatency, inputParameters->suggestedLatency )
* sampleRate );
{
snd_pcm_uframes_t maxBufferSize;
snd_pcm_uframes_t maxBufferSizeCapture, maxBufferSizePlayback;
ENSURE_( alsa_snd_pcm_hw_params_get_buffer_size_max( hwParamsCapture, &maxBufferSizeCapture ), paUnanticipatedHostError );
ENSURE_( alsa_snd_pcm_hw_params_get_buffer_size_max( hwParamsPlayback, &maxBufferSizePlayback ), paUnanticipatedHostError );
maxBufferSize = PA_MIN( maxBufferSizeCapture, maxBufferSizePlayback );
desiredBufSz = PA_MIN( desiredBufSz, maxBufferSize );
}
e = ilogb( minPeriodSize );
if( minPeriodSize & ( minPeriodSize - 1 ) )
e += 1;
periodSize = (snd_pcm_uframes_t)pow( 2, e );
while( periodSize <= maxPeriodSize )
{
if( alsa_snd_pcm_hw_params_test_period_size( self->playback.pcm, hwParamsPlayback, periodSize, 0 ) >= 0 &&
alsa_snd_pcm_hw_params_test_period_size( self->capture.pcm, hwParamsCapture, periodSize, 0 ) >= 0 )
{
break;
}
periodSize *= 2;
}
optimalPeriodSize = PA_MAX( desiredBufSz / numPeriods, minPeriodSize );
optimalPeriodSize = PA_MIN( optimalPeriodSize, maxPeriodSize );
e = ilogb( optimalPeriodSize );
if( optimalPeriodSize & (optimalPeriodSize - 1) )
e += 1;
optimalPeriodSize = (snd_pcm_uframes_t)pow( 2, e );
while( optimalPeriodSize >= periodSize )
{
if( alsa_snd_pcm_hw_params_test_period_size( self->capture.pcm, hwParamsCapture, optimalPeriodSize, 0 )
>= 0 && alsa_snd_pcm_hw_params_test_period_size( self->playback.pcm, hwParamsPlayback,
optimalPeriodSize, 0 ) >= 0 )
{
break;
}
optimalPeriodSize /= 2;
}
if( optimalPeriodSize > periodSize )
periodSize = optimalPeriodSize;
if( periodSize <= maxPeriodSize )
{
ENSURE_( alsa_snd_pcm_hw_params_set_period_size( self->capture.pcm, hwParamsCapture, periodSize, 0 ),
paUnanticipatedHostError );
ENSURE_( alsa_snd_pcm_hw_params_set_period_size( self->playback.pcm, hwParamsPlayback, periodSize, 0 ),
paUnanticipatedHostError );
self->capture.framesPerPeriod = self->playback.framesPerPeriod = periodSize;
framesPerHostBuffer = periodSize;
}
else
{
optimalPeriodSize = PA_MAX( desiredBufSz / numPeriods, minPeriodSize );
optimalPeriodSize = PA_MIN( optimalPeriodSize, maxPeriodSize );
self->capture.framesPerPeriod = optimalPeriodSize;
dir = 0;
ENSURE_( alsa_snd_pcm_hw_params_set_period_size_near( self->capture.pcm, hwParamsCapture, &self->capture.framesPerPeriod, &dir ),
paUnanticipatedHostError );
self->playback.framesPerPeriod = optimalPeriodSize;
dir = 0;
ENSURE_( alsa_snd_pcm_hw_params_set_period_size_near( self->playback.pcm, hwParamsPlayback, &self->playback.framesPerPeriod, &dir ),
paUnanticipatedHostError );
framesPerHostBuffer = PA_MAX( self->capture.framesPerPeriod, self->playback.framesPerPeriod );
*hostBufferSizeMode = paUtilBoundedHostBufferSize;
}
}
else
{
unsigned maxPeriods = 0;
PaAlsaStreamComponent* first = &self->capture, * second = &self->playback;
const PaStreamParameters* firstStreamParams = inputParameters;
snd_pcm_hw_params_t* firstHwParams = hwParamsCapture, * secondHwParams = hwParamsPlayback;
dir = 0;
ENSURE_( alsa_snd_pcm_hw_params_get_periods_max( hwParamsPlayback, &maxPeriods, &dir ), paUnanticipatedHostError );
if( maxPeriods < numPeriods )
{
first = &self->playback;
second = &self->capture;
firstStreamParams = outputParameters;
firstHwParams = hwParamsPlayback;
secondHwParams = hwParamsCapture;
}
PA_ENSURE( PaAlsaStreamComponent_DetermineFramesPerBuffer( first, firstStreamParams, framesPerUserBuffer,
sampleRate, firstHwParams, &accurate ) );
second->framesPerPeriod = first->framesPerPeriod;
dir = 0;
ENSURE_( alsa_snd_pcm_hw_params_set_period_size_near( second->pcm, secondHwParams, &second->framesPerPeriod, &dir ),
paUnanticipatedHostError );
if( self->capture.framesPerPeriod == self->playback.framesPerPeriod )
{
framesPerHostBuffer = self->capture.framesPerPeriod;
}
else
{
framesPerHostBuffer = PA_MAX( self->capture.framesPerPeriod, self->playback.framesPerPeriod );
*hostBufferSizeMode = paUtilBoundedHostBufferSize;
}
}
}
else     
{
if( self->capture.pcm )
{
PA_ENSURE( PaAlsaStreamComponent_DetermineFramesPerBuffer( &self->capture, inputParameters, framesPerUserBuffer,
sampleRate, hwParamsCapture, &accurate) );
framesPerHostBuffer = self->capture.framesPerPeriod;
}
else
{
assert( self->playback.pcm );
PA_ENSURE( PaAlsaStreamComponent_DetermineFramesPerBuffer( &self->playback, outputParameters, framesPerUserBuffer,
sampleRate, hwParamsPlayback, &accurate ) );
framesPerHostBuffer = self->playback.framesPerPeriod;
}
}
PA_UNLESS( framesPerHostBuffer != 0, paInternalError );
self->maxFramesPerHostBuffer = framesPerHostBuffer;
if( !self->playback.canMmap || !accurate )
{
*hostBufferSizeMode = paUtilBoundedHostBufferSize;
if( !accurate )
++self->maxFramesPerHostBuffer;
}
error:
return result;
}
static PaError PaAlsaStream_Configure( PaAlsaStream *self, const PaStreamParameters *inParams, const PaStreamParameters*
outParams, double sampleRate, unsigned long framesPerUserBuffer, double* inputLatency, double* outputLatency,
PaUtilHostBufferSizeMode* hostBufferSizeMode )
{
PaError result = paNoError;
double realSr = sampleRate;
snd_pcm_hw_params_t* hwParamsCapture, * hwParamsPlayback;
alsa_snd_pcm_hw_params_alloca( &hwParamsCapture );
alsa_snd_pcm_hw_params_alloca( &hwParamsPlayback );
if( self->capture.pcm )
PA_ENSURE( PaAlsaStreamComponent_InitialConfigure( &self->capture, inParams, self->primeBuffers, hwParamsCapture,
&realSr ) );
if( self->playback.pcm )
PA_ENSURE( PaAlsaStreamComponent_InitialConfigure( &self->playback, outParams, self->primeBuffers, hwParamsPlayback,
&realSr ) );
PA_ENSURE( PaAlsaStream_DetermineFramesPerBuffer( self, realSr, inParams, outParams, framesPerUserBuffer,
hwParamsCapture, hwParamsPlayback, hostBufferSizeMode ) );
if( self->capture.pcm )
{
assert( self->capture.framesPerPeriod != 0 );
PA_ENSURE( PaAlsaStreamComponent_FinishConfigure( &self->capture, hwParamsCapture, inParams, self->primeBuffers, realSr,
inputLatency ) );
PA_DEBUG(( "%s: Capture period size: %lu, latency: %f\n", __FUNCTION__, self->capture.framesPerPeriod, *inputLatency ));
}
if( self->playback.pcm )
{
assert( self->playback.framesPerPeriod != 0 );
PA_ENSURE( PaAlsaStreamComponent_FinishConfigure( &self->playback, hwParamsPlayback, outParams, self->primeBuffers, realSr,
outputLatency ) );
PA_DEBUG(( "%s: Playback period size: %lu, latency: %f\n", __FUNCTION__, self->playback.framesPerPeriod, *outputLatency ));
}
self->streamRepresentation.streamInfo.sampleRate = realSr;
if( self->callbackMode && self->capture.pcm && self->playback.pcm )
{
int err = alsa_snd_pcm_link( self->capture.pcm, self->playback.pcm );
if( err == 0 )
self->pcmsSynced = 1;
else
PA_DEBUG(( "%s: Unable to sync pcms: %s\n", __FUNCTION__, alsa_snd_strerror( err ) ));
}
{
unsigned long minFramesPerHostBuffer = PA_MIN( self->capture.pcm ? self->capture.framesPerPeriod : ULONG_MAX,
self->playback.pcm ? self->playback.framesPerPeriod : ULONG_MAX );
self->pollTimeout = CalculatePollTimeout( self, minFramesPerHostBuffer );     
}
if( self->callbackMode )
{
if( self->framesPerUserBuffer != paFramesPerBufferUnspecified )
{
}
}
error:
return result;
}
static PaError OpenStream( struct PaUtilHostApiRepresentation *hostApi,
PaStream** s,
const PaStreamParameters *inputParameters,
const PaStreamParameters *outputParameters,
double sampleRate,
unsigned long framesPerBuffer,
PaStreamFlags streamFlags,
PaStreamCallback* callback,
void *userData )
{
PaError result = paNoError;
PaAlsaHostApiRepresentation *alsaHostApi = (PaAlsaHostApiRepresentation*)hostApi;
PaAlsaStream *stream = NULL;
PaSampleFormat hostInputSampleFormat = 0, hostOutputSampleFormat = 0;
PaSampleFormat inputSampleFormat = 0, outputSampleFormat = 0;
int numInputChannels = 0, numOutputChannels = 0;
PaTime inputLatency, outputLatency;
PaUtilHostBufferSizeMode hostBufferSizeMode = paUtilFixedHostBufferSize;
if( ( streamFlags & paPlatformSpecificFlags ) != 0 )
return paInvalidFlag;
if( inputParameters )
{
PA_ENSURE( ValidateParameters( inputParameters, hostApi, StreamDirection_In ) );
numInputChannels = inputParameters->channelCount;
inputSampleFormat = inputParameters->sampleFormat;
}
if( outputParameters )
{
PA_ENSURE( ValidateParameters( outputParameters, hostApi, StreamDirection_Out ) );
numOutputChannels = outputParameters->channelCount;
outputSampleFormat = outputParameters->sampleFormat;
}
if( framesPerBuffer == paFramesPerBufferUnspecified && getenv( "PA_ALSA_PERIODSIZE" ) != NULL )
{
PA_DEBUG(( "%s: Getting framesPerBuffer (Alsa period-size) from environment\n", __FUNCTION__ ));
framesPerBuffer = atoi( getenv("PA_ALSA_PERIODSIZE") );
}
PA_UNLESS( stream = (PaAlsaStream*)PaUtil_AllocateMemory( sizeof(PaAlsaStream) ), paInsufficientMemory );
PA_ENSURE( PaAlsaStream_Initialize( stream, alsaHostApi, inputParameters, outputParameters, sampleRate,
framesPerBuffer, callback, streamFlags, userData ) );
PA_ENSURE( PaAlsaStream_Configure( stream, inputParameters, outputParameters, sampleRate, framesPerBuffer,
&inputLatency, &outputLatency, &hostBufferSizeMode ) );
hostInputSampleFormat = stream->capture.hostSampleFormat | (!stream->capture.hostInterleaved ? paNonInterleaved : 0);
hostOutputSampleFormat = stream->playback.hostSampleFormat | (!stream->playback.hostInterleaved ? paNonInterleaved : 0);
PA_ENSURE( PaUtil_InitializeBufferProcessor( &stream->bufferProcessor,
numInputChannels, inputSampleFormat, hostInputSampleFormat,
numOutputChannels, outputSampleFormat, hostOutputSampleFormat,
sampleRate, streamFlags, framesPerBuffer, stream->maxFramesPerHostBuffer,
hostBufferSizeMode, callback, userData ) );
if( numInputChannels > 0 )
stream->streamRepresentation.streamInfo.inputLatency = inputLatency + (PaTime)(
PaUtil_GetBufferProcessorInputLatencyFrames( &stream->bufferProcessor ) / sampleRate);
if( numOutputChannels > 0 )
stream->streamRepresentation.streamInfo.outputLatency = outputLatency + (PaTime)(
PaUtil_GetBufferProcessorOutputLatencyFrames( &stream->bufferProcessor ) / sampleRate);
PA_DEBUG(( "%s: Stream: framesPerBuffer = %lu, maxFramesPerHostBuffer = %lu, latency i=%f, o=%f\n", __FUNCTION__, framesPerBuffer, stream->maxFramesPerHostBuffer, stream->streamRepresentation.streamInfo.inputLatency, stream->streamRepresentation.streamInfo.outputLatency));
*s = (PaStream*)stream;
return result;
error:
if( stream )
{
PA_DEBUG(( "%s: Stream in error, terminating\n", __FUNCTION__ ));
PaAlsaStream_Terminate( stream );
}
return result;
}
static PaError CloseStream( PaStream* s )
{
PaError result = paNoError;
PaAlsaStream *stream = (PaAlsaStream*)s;
PaUtil_TerminateBufferProcessor( &stream->bufferProcessor );
PaUtil_TerminateStreamRepresentation( &stream->streamRepresentation );
PaAlsaStream_Terminate( stream );
return result;
}
static void SilenceBuffer( PaAlsaStream *stream )
{
const snd_pcm_channel_area_t *areas;
snd_pcm_uframes_t frames = (snd_pcm_uframes_t)alsa_snd_pcm_avail_update( stream->playback.pcm ), offset;
alsa_snd_pcm_mmap_begin( stream->playback.pcm, &areas, &offset, &frames );
alsa_snd_pcm_areas_silence( areas, offset, stream->playback.numHostChannels, frames, stream->playback.nativeFormat );
alsa_snd_pcm_mmap_commit( stream->playback.pcm, offset, frames );
}
static PaError AlsaStart( PaAlsaStream *stream, int priming )
{
PaError result = paNoError;
if( stream->playback.pcm )
{
if( stream->callbackMode )
{
if( !priming )
{
ENSURE_( alsa_snd_pcm_prepare( stream->playback.pcm ), paUnanticipatedHostError );
if( stream->playback.canMmap )
SilenceBuffer( stream );
}
if( stream->playback.canMmap )
ENSURE_( alsa_snd_pcm_start( stream->playback.pcm ), paUnanticipatedHostError );
}
else
ENSURE_( alsa_snd_pcm_prepare( stream->playback.pcm ), paUnanticipatedHostError );
}
if( stream->capture.pcm && !stream->pcmsSynced )
{
ENSURE_( alsa_snd_pcm_prepare( stream->capture.pcm ), paUnanticipatedHostError );
ENSURE_( alsa_snd_pcm_start( stream->capture.pcm ), paUnanticipatedHostError );
}
end:
return result;
error:
goto end;
}
#if 0
static int IsRunning( PaAlsaStream *stream )
{
int result = 0;
PA_ENSURE( PaUnixMutex_Lock( &stream->stateMtx ) );
if( stream->capture.pcm )
{
snd_pcm_state_t capture_state = alsa_snd_pcm_state( stream->capture.pcm );
if( capture_state == SND_PCM_STATE_RUNNING || capture_state == SND_PCM_STATE_XRUN
|| capture_state == SND_PCM_STATE_DRAINING )
{
result = 1;
goto end;
}
}
if( stream->playback.pcm )
{
snd_pcm_state_t playback_state = alsa_snd_pcm_state( stream->playback.pcm );
if( playback_state == SND_PCM_STATE_RUNNING || playback_state == SND_PCM_STATE_XRUN
|| playback_state == SND_PCM_STATE_DRAINING )
{
result = 1;
goto end;
}
}
end:
ASSERT_CALL_( PaUnixMutex_Unlock( &stream->stateMtx ), paNoError );
return result;
error:
goto error;
}
#endif
static PaError StartStream( PaStream *s )
{
PaError result = paNoError;
PaAlsaStream* stream = (PaAlsaStream*)s;
int streamStarted = 0;   
PaUtil_ResetBufferProcessor( &stream->bufferProcessor );
stream->isActive = 1;
if( stream->callbackMode )
{
PA_ENSURE( PaUnixThread_New( &stream->thread, &CallbackThreadFunc, stream, 1., stream->rtSched ) );
}
else
{
PA_ENSURE( AlsaStart( stream, 0 ) );
streamStarted = 1;
}
end:
return result;
error:
if( streamStarted )
{
AbortStream( stream );
}
stream->isActive = 0;
goto end;
}
static PaError AlsaStop( PaAlsaStream *stream, int abort )
{
PaError result = paNoError;
abort = 1;
if( abort )
{
if( stream->playback.pcm )
{
ENSURE_( alsa_snd_pcm_drop( stream->playback.pcm ), paUnanticipatedHostError );
}
if( stream->capture.pcm && !stream->pcmsSynced )
{
ENSURE_( alsa_snd_pcm_drop( stream->capture.pcm ), paUnanticipatedHostError );
}
PA_DEBUG(( "%s: Dropped frames\n", __FUNCTION__ ));
}
else
{
if( stream->playback.pcm )
{
ENSURE_( alsa_snd_pcm_nonblock( stream->playback.pcm, 0 ), paUnanticipatedHostError );
if( alsa_snd_pcm_drain( stream->playback.pcm ) < 0 )
{
PA_DEBUG(( "%s: Draining playback handle failed!\n", __FUNCTION__ ));
}
}
if( stream->capture.pcm && !stream->pcmsSynced )
{
if( alsa_snd_pcm_drain( stream->capture.pcm ) < 0 )
{
PA_DEBUG(( "%s: Draining capture handle failed!\n", __FUNCTION__ ));
}
}
}
end:
return result;
error:
goto end;
}
static PaError RealStop( PaAlsaStream *stream, int abort )
{
PaError result = paNoError;
if( stream->callbackMode )
{
PaError threadRes;
stream->callbackAbort = abort;
if( !abort )
{
PA_DEBUG(( "Stopping callback\n" ));
}
PA_ENSURE( PaUnixThread_Terminate( &stream->thread, !abort, &threadRes ) );
if( threadRes != paNoError )
{
PA_DEBUG(( "Callback thread returned: %d\n", threadRes ));
}
#if 0
if( watchdogRes != paNoError )
PA_DEBUG(( "Watchdog thread returned: %d\n", watchdogRes ));
#endif
stream->callback_finished = 0;
}
else
{
PA_ENSURE( AlsaStop( stream, abort ) );
}
stream->isActive = 0;
end:
return result;
error:
goto end;
}
static PaError StopStream( PaStream *s )
{
return RealStop( (PaAlsaStream *) s, 0 );
}
static PaError AbortStream( PaStream *s )
{
return RealStop( (PaAlsaStream * ) s, 1 );
}
static PaError IsStreamStopped( PaStream *s )
{
PaAlsaStream *stream = (PaAlsaStream *)s;
return !IsStreamActive( s ) && !stream->callback_finished;
}
static PaError IsStreamActive( PaStream *s )
{
PaAlsaStream *stream = (PaAlsaStream*)s;
return stream->isActive;
}
static PaTime StatusToTime( const snd_pcm_status_t *status, int trigger, snd_pcm_uframes_t* delay )
{
snd_htimestamp_t timestamp;
if ( trigger )
{
alsa_snd_pcm_status_get_trigger_htstamp( status, &timestamp );
}
else
{
alsa_snd_pcm_status_get_htstamp( status, &timestamp );
}
if ( delay )
{
*delay = alsa_snd_pcm_status_get_delay( status );
}
return timestamp.tv_sec + ( (PaTime)timestamp.tv_nsec * 1e-9 );
}
static PaTime GetStreamTime( PaStream *s )
{
PaAlsaStream *stream = (PaAlsaStream*)s;
snd_timestamp_t timestamp;
snd_pcm_status_t* status;
alsa_snd_pcm_status_alloca( &status );
if( stream->capture.pcm )
{
alsa_snd_pcm_status( stream->capture.pcm, status );
}
else if( stream->playback.pcm )
{
alsa_snd_pcm_status( stream->playback.pcm, status );
}
return StatusToTime( status, 0, NULL );
}
static double GetStreamCpuLoad( PaStream* s )
{
PaAlsaStream *stream = (PaAlsaStream*)s;
return PaUtil_GetCpuLoad( &stream->cpuLoadMeasurer );
}
static int SetApproximateSampleRate( snd_pcm_t *pcm, snd_pcm_hw_params_t *hwParams, double sampleRate )
{
PaError result = paNoError;
unsigned int reqRate, setRate, deviation;
assert( pcm && hwParams );
reqRate = setRate = (unsigned int) sampleRate;
ENSURE_( alsa_snd_pcm_hw_params_set_rate_near( pcm, hwParams, &setRate, NULL ), paUnanticipatedHostError );
deviation = abs( (int)setRate - (int)reqRate );
if( deviation > 0 && deviation * RATE_MAX_DEVIATE_RATIO > reqRate )
result = paInvalidSampleRate;
end:
return result;
error:
{
unsigned int _min = 0, _max = 0;
int _dir = 0;
ENSURE_( alsa_snd_pcm_hw_params_get_rate_min( hwParams, &_min, &_dir ), paUnanticipatedHostError );
_dir = 0;
ENSURE_( alsa_snd_pcm_hw_params_get_rate_max( hwParams, &_max, &_dir ), paUnanticipatedHostError );
PA_DEBUG(( "%s: SR min = %u, max = %u, req = %u\n", __FUNCTION__, _min, _max, reqRate ));
}
goto end;
}
static int GetExactSampleRate( snd_pcm_hw_params_t *hwParams, double *sampleRate )
{
unsigned int num, den = 1;
int err;
assert( hwParams );
err = alsa_snd_pcm_hw_params_get_rate_numden( hwParams, &num, &den );
*sampleRate = (double) num / den;
return err;
}
static PaError AlsaRestart( PaAlsaStream *stream )
{
PaError result = paNoError;
PA_ENSURE( PaUnixMutex_Lock( &stream->stateMtx ) );
PA_ENSURE( AlsaStop( stream, 0 ) );
PA_ENSURE( AlsaStart( stream, 0 ) );
PA_DEBUG(( "%s: Restarted audio\n", __FUNCTION__ ));
error:
PA_ENSURE( PaUnixMutex_Unlock( &stream->stateMtx ) );
return result;
}
static PaError PaAlsaStream_HandleXrun( PaAlsaStream *self )
{
PaError result = paNoError;
snd_pcm_status_t *st;
PaTime now = PaUtil_GetTime();
snd_timestamp_t t;
int restartAlsa = 0;  
alsa_snd_pcm_status_alloca( &st );
if( self->playback.pcm )
{
alsa_snd_pcm_status( self->playback.pcm, st );
if( alsa_snd_pcm_status_get_state( st ) == SND_PCM_STATE_XRUN )
{
alsa_snd_pcm_status_get_trigger_tstamp( st, &t );
self->underrun = ( now - StatusToTime( st, 1, NULL ) ) * 1000;
if( !self->playback.canMmap )
{
if( alsa_snd_pcm_recover( self->playback.pcm, -EPIPE, 0 ) < 0 )
{
PA_DEBUG(( "%s: [playback] non-MMAP-PCM failed recovering from XRUN, will restart Alsa\n", __FUNCTION__ ));
++ restartAlsa;  
}
}
else
++ restartAlsa;  
}
}
if( self->capture.pcm )
{
alsa_snd_pcm_status( self->capture.pcm, st );
if( alsa_snd_pcm_status_get_state( st ) == SND_PCM_STATE_XRUN )
{
self->overrun = ( now - StatusToTime( st, 1, NULL ) ) * 1000;
if (!self->capture.canMmap)
{
if (alsa_snd_pcm_recover( self->capture.pcm, -EPIPE, 0 ) < 0)
{
PA_DEBUG(( "%s: [capture] non-MMAP-PCM failed recovering from XRUN, will restart Alsa\n", __FUNCTION__ ));
++ restartAlsa;  
}
}
else
++ restartAlsa;  
}
}
if( restartAlsa )
{
PA_DEBUG(( "%s: restarting Alsa to recover from XRUN\n", __FUNCTION__ ));
PA_ENSURE( AlsaRestart( self ) );
}
end:
return result;
error:
goto end;
}
static PaError ContinuePoll( const PaAlsaStream *stream, StreamDirection streamDir, int *pollTimeout, int *continuePoll )
{
PaError result = paNoError;
snd_pcm_sframes_t delay, margin;
int err;
const PaAlsaStreamComponent *component = NULL, *otherComponent = NULL;
*continuePoll = 1;
if( StreamDirection_In == streamDir )
{
component = &stream->capture;
otherComponent = &stream->playback;
}
else
{
component = &stream->playback;
otherComponent = &stream->capture;
}
if( ( err = alsa_snd_pcm_delay( otherComponent->pcm, &delay ) ) < 0 )
{
if( err == -EPIPE )
{
*continuePoll = 0;
goto error;
}
ENSURE_( err, paUnanticipatedHostError );
}
if( StreamDirection_Out == streamDir )
{
delay = otherComponent->alsaBufferSize - delay;
}
margin = delay - otherComponent->framesPerPeriod / 2;
if( margin < 0 )
{
PA_DEBUG(( "%s: Stopping poll for %s\n", __FUNCTION__, StreamDirection_In == streamDir ? "capture" : "playback" ));
*continuePoll = 0;
}
else if( margin < otherComponent->framesPerPeriod )
{
*pollTimeout = CalculatePollTimeout( stream, margin );
PA_DEBUG(( "%s: Trying to poll again for %s frames, pollTimeout: %d\n",
__FUNCTION__, StreamDirection_In == streamDir ? "capture" : "playback", *pollTimeout ));
}
error:
return result;
}
static void OnExit( void *data )
{
PaAlsaStream *stream = (PaAlsaStream *) data;
assert( data );
PaUtil_ResetCpuLoadMeasurer( &stream->cpuLoadMeasurer );
stream->callback_finished = 1;   
PA_DEBUG(( "%s: Stopping ALSA handles\n", __FUNCTION__ ));
AlsaStop( stream, stream->callbackAbort );
PA_DEBUG(( "%s: Stoppage\n", __FUNCTION__ ));
if( stream->streamRepresentation.streamFinishedCallback )
{
stream->streamRepresentation.streamFinishedCallback( stream->streamRepresentation.userData );
}
stream->isActive = 0;
}
static void CalculateTimeInfo( PaAlsaStream *stream, PaStreamCallbackTimeInfo *timeInfo )
{
snd_pcm_status_t *status;
PaTime capture_time = 0., playback_time = 0.;
alsa_snd_pcm_status_alloca( &status );
if( stream->capture.pcm )
{
snd_pcm_sframes_t capture_delay;
alsa_snd_pcm_status( stream->capture.pcm, status );
capture_time = StatusToTime( status, 0, &capture_delay );
timeInfo->currentTime = capture_time;
timeInfo->inputBufferAdcTime = capture_time -
(PaTime)capture_delay / stream->streamRepresentation.streamInfo.sampleRate;
}
if( stream->playback.pcm )
{
snd_pcm_sframes_t playback_delay;
PaTime playback_time;
alsa_snd_pcm_status( stream->playback.pcm, status );
playback_time = StatusToTime( status, 0, &playback_delay );
if( stream->capture.pcm )  
{
if( fabs( capture_time - playback_time ) > 0.01 )
PA_DEBUG(( "Capture time and playback time differ by %f\n", fabs( capture_time-playback_time ) ));
}
else
timeInfo->currentTime = playback_time;
timeInfo->outputBufferDacTime = timeInfo->currentTime +
(PaTime)playback_delay / stream->streamRepresentation.streamInfo.sampleRate;
}
}
static PaError PaAlsaStreamComponent_EndProcessing( PaAlsaStreamComponent *self, unsigned long numFrames, int *xrun )
{
PaError result = paNoError;
int res = 0;
if( !self->ready )
goto end;
if( !self->canMmap && StreamDirection_Out == self->streamDir )
{
if( self->hostInterleaved )
res = alsa_snd_pcm_writei( self->pcm, self->nonMmapBuffer, numFrames );
else
{
void *bufs[self->numHostChannels];
int bufsize = alsa_snd_pcm_format_size( self->nativeFormat, self->framesPerPeriod + 1 );
unsigned char *buffer = self->nonMmapBuffer;
int i;
for( i = 0; i < self->numHostChannels; ++i )
{
bufs[i] = buffer;
buffer += bufsize;
}
res = alsa_snd_pcm_writen( self->pcm, bufs, numFrames );
}
}
if( self->canMmap )
res = alsa_snd_pcm_mmap_commit( self->pcm, self->offset, numFrames );
if( res == -EPIPE || res == -ESTRPIPE )
{
*xrun = 1;
}
else
{
ENSURE_( res, paUnanticipatedHostError );
}
end:
error:
return result;
}
static unsigned char *ExtractAddress( const snd_pcm_channel_area_t *area, snd_pcm_uframes_t offset )
{
return (unsigned char *) area->addr + ( area->first + offset * area->step ) / 8;
}
static PaError PaAlsaStreamComponent_DoChannelAdaption( PaAlsaStreamComponent *self, PaUtilBufferProcessor *bp, int numFrames )
{
PaError result = paNoError;
unsigned char *p;
int i;
int unusedChans = self->numHostChannels - self->numUserChannels;
unsigned char *src, *dst;
int convertMono = ( self->numHostChannels % 2 ) == 0 && ( self->numUserChannels % 2 ) != 0;
assert( StreamDirection_Out == self->streamDir );
if( self->hostInterleaved )
{
int swidth = alsa_snd_pcm_format_size( self->nativeFormat, 1 );
unsigned char *buffer = self->canMmap ? ExtractAddress( self->channelAreas, self->offset ) : self->nonMmapBuffer;
p = buffer + self->numUserChannels * swidth;
if( convertMono )
{
src = buffer + ( self->numUserChannels - 1 ) * swidth;
for( i = 0; i < numFrames; ++i )
{
dst = src + swidth;
memcpy( dst, src, swidth );
src += self->numHostChannels * swidth;
}
p += swidth;
--unusedChans;
}
if( unusedChans > 0 )
{
for( i = 0; i < numFrames; ++i )
{
memset( p, 0, swidth * unusedChans );
p += self->numHostChannels * swidth;
}
}
}
else
{
if( convertMono )
{
ENSURE_( alsa_snd_pcm_area_copy( self->channelAreas + self->numUserChannels, self->offset, self->channelAreas +
( self->numUserChannels - 1 ), self->offset, numFrames, self->nativeFormat ), paUnanticipatedHostError );
--unusedChans;
}
if( unusedChans > 0 )
{
alsa_snd_pcm_areas_silence( self->channelAreas + ( self->numHostChannels - unusedChans ), self->offset, unusedChans, numFrames,
self->nativeFormat );
}
}
error:
return result;
}
static PaError PaAlsaStream_EndProcessing( PaAlsaStream *self, unsigned long numFrames, int *xrunOccurred )
{
PaError result = paNoError;
int xrun = 0;
if( self->capture.pcm )
{
PA_ENSURE( PaAlsaStreamComponent_EndProcessing( &self->capture, numFrames, &xrun ) );
}
if( self->playback.pcm )
{
if( self->playback.numHostChannels > self->playback.numUserChannels )
{
PA_ENSURE( PaAlsaStreamComponent_DoChannelAdaption( &self->playback, &self->bufferProcessor, numFrames ) );
}
PA_ENSURE( PaAlsaStreamComponent_EndProcessing( &self->playback, numFrames, &xrun ) );
}
error:
*xrunOccurred = xrun;
return result;
}
static PaError PaAlsaStreamComponent_GetAvailableFrames( PaAlsaStreamComponent *self, unsigned long *numFrames, int *xrunOccurred )
{
PaError result = paNoError;
snd_pcm_sframes_t framesAvail = alsa_snd_pcm_avail_update( self->pcm );
*xrunOccurred = 0;
if( -EPIPE == framesAvail )
{
*xrunOccurred = 1;
framesAvail = 0;
}
else
{
ENSURE_( framesAvail, paUnanticipatedHostError );
}
*numFrames = framesAvail;
error:
return result;
}
static PaError PaAlsaStreamComponent_BeginPolling( PaAlsaStreamComponent* self, struct pollfd* pfds )
{
int nfds = alsa_snd_pcm_poll_descriptors( self->pcm, pfds, self->nfds );
if( nfds != self->nfds )
{
return paUnanticipatedHostError;
}
self->ready = 0;
return paNoError;
}
static PaError PaAlsaStreamComponent_EndPolling( PaAlsaStreamComponent* self, struct pollfd* pfds, int* shouldPoll, int* xrun )
{
PaError result = paNoError;
unsigned short revents;
ENSURE_( alsa_snd_pcm_poll_descriptors_revents( self->pcm, pfds, self->nfds, &revents ), paUnanticipatedHostError );
if( revents != 0 )
{
if( revents & POLLERR )
{
*xrun = 1;
}
else if( revents & POLLHUP )
{
*xrun = 1;
PA_DEBUG(( "%s: revents has POLLHUP, processing as XRUN\n", __FUNCTION__ ));
}
else
self->ready = 1;
*shouldPoll = 0;
}
else  
if( self->useReventFix )
{
self->ready = 1;
*shouldPoll = 0;
}
error:
return result;
}
static PaError PaAlsaStream_GetAvailableFrames( PaAlsaStream *self, int queryCapture, int queryPlayback, unsigned long
*available, int *xrunOccurred )
{
PaError result = paNoError;
unsigned long captureFrames, playbackFrames;
*xrunOccurred = 0;
assert( queryCapture || queryPlayback );
if( queryCapture )
{
assert( self->capture.pcm );
PA_ENSURE( PaAlsaStreamComponent_GetAvailableFrames( &self->capture, &captureFrames, xrunOccurred ) );
if( *xrunOccurred )
{
goto end;
}
}
if( queryPlayback )
{
assert( self->playback.pcm );
PA_ENSURE( PaAlsaStreamComponent_GetAvailableFrames( &self->playback, &playbackFrames, xrunOccurred ) );
if( *xrunOccurred )
{
goto end;
}
}
if( queryCapture && queryPlayback )
{
*available = PA_MIN( captureFrames, playbackFrames );
}
else if( queryCapture )
{
*available = captureFrames;
}
else
{
*available = playbackFrames;
}
end:
error:
return result;
}
static PaError PaAlsaStream_WaitForFrames( PaAlsaStream *self, unsigned long *framesAvail, int *xrunOccurred )
{
PaError result = paNoError;
int pollPlayback = self->playback.pcm != NULL, pollCapture = self->capture.pcm != NULL;
int pollTimeout = self->pollTimeout;
int xrun = 0, timeouts = 0;
int pollResults;
assert( self );
assert( framesAvail );
if( !self->callbackMode )
{
PA_ENSURE( PaAlsaStream_GetAvailableFrames( self, self->capture.pcm != NULL, self->playback.pcm != NULL,
framesAvail, &xrun ) );
if( xrun )
{
goto end;
}
if( *framesAvail > 0 )
{
if( self->capture.pcm )
self->capture.ready = 1;
if( self->playback.pcm )
self->playback.ready = 1;
goto end;
}
}
while( pollPlayback || pollCapture )
{
int totalFds = 0;
struct pollfd *capturePfds = NULL, *playbackPfds = NULL;
#ifdef PTHREAD_CANCELED
pthread_testcancel();
#endif
if( pollCapture )
{
capturePfds = self->pfds;
PaError res = PaAlsaStreamComponent_BeginPolling( &self->capture, capturePfds );
if( res != paNoError)
{
xrun = 1;
goto end;
}
totalFds += self->capture.nfds;
}
if( pollPlayback )
{
playbackPfds = self->pfds + (pollCapture ? self->capture.nfds : 0);
PaError res = PaAlsaStreamComponent_BeginPolling( &self->playback, playbackPfds );
if( res != paNoError)
{
xrun = 1;
goto end;
}
totalFds += self->playback.nfds;
}
#ifdef PTHREAD_CANCELED
if( self->callbackMode )
{
pthread_setcancelstate( PTHREAD_CANCEL_ENABLE, NULL );
}
#endif
pollResults = poll( self->pfds, totalFds, pollTimeout );
#ifdef PTHREAD_CANCELED
if( self->callbackMode )
{
pthread_setcancelstate( PTHREAD_CANCEL_DISABLE, NULL );
}
#endif
if( pollResults < 0 )
{
if( errno == EINTR )
{
Pa_Sleep( 1 );  
continue;
}
PA_ENSURE( paInternalError );
}
else if( pollResults == 0 )
{
++ timeouts;
if( timeouts > 1 )  
{
Pa_Sleep( 1 );  
}
if( timeouts >= 2048 )  
{
*framesAvail = 0;  
xrun = 1;  
PA_DEBUG(( "%s: poll timed out\n", __FUNCTION__, timeouts ));
goto end; 
}
}
else if( pollResults > 0 )
{
timeouts = 0;
if( pollCapture )
{
PA_ENSURE( PaAlsaStreamComponent_EndPolling( &self->capture, capturePfds, &pollCapture, &xrun ) );
}
if( pollPlayback )
{
PA_ENSURE( PaAlsaStreamComponent_EndPolling( &self->playback, playbackPfds, &pollPlayback, &xrun ) );
}
if( xrun )
{
break;
}
}
if( self->capture.pcm && self->playback.pcm )
{
if( pollCapture && !pollPlayback )
{
PA_ENSURE( ContinuePoll( self, StreamDirection_In, &pollTimeout, &pollCapture ) );
}
else if( pollPlayback && !pollCapture )
{
PA_ENSURE( ContinuePoll( self, StreamDirection_Out, &pollTimeout, &pollPlayback ) );
}
}
}
if( !xrun )
{
int captureReady = self->capture.pcm ? self->capture.ready : 0,
playbackReady = self->playback.pcm ? self->playback.ready : 0;
PA_ENSURE( PaAlsaStream_GetAvailableFrames( self, captureReady, playbackReady, framesAvail, &xrun ) );
if( self->capture.pcm && self->playback.pcm )
{
if( !self->playback.ready && !self->neverDropInput )
{
assert( self->capture.ready );
PaAlsaStreamComponent_EndProcessing( &self->capture, PA_MIN( self->capture.framesPerPeriod,
*framesAvail ), &xrun );
*framesAvail = 0;
self->capture.ready = 0;
}
}
else if( self->capture.pcm )
assert( self->capture.ready );
else
assert( self->playback.ready );
}
end:
error:
if( xrun )
{
PA_ENSURE( PaAlsaStream_HandleXrun( self ) );
*framesAvail = 0;
}
else
{
if( 0 != *framesAvail )
{
PA_UNLESS( self->capture.ready || self->playback.ready, paInternalError );
}
}
*xrunOccurred = xrun;
return result;
}
static PaError PaAlsaStreamComponent_RegisterChannels( PaAlsaStreamComponent* self, PaUtilBufferProcessor* bp,
unsigned long* numFrames, int* xrun )
{
PaError result = paNoError;
const snd_pcm_channel_area_t *areas, *area;
void (*setChannel)(PaUtilBufferProcessor *, unsigned int, void *, unsigned int) =
StreamDirection_In == self->streamDir ? PaUtil_SetInputChannel : PaUtil_SetOutputChannel;
unsigned char *buffer, *p;
int i;
unsigned long framesAvail;
PA_ENSURE( PaAlsaStreamComponent_GetAvailableFrames( self, &framesAvail, xrun ) );
if( *xrun )
{
*numFrames = 0;
goto end;
}
if( self->canMmap )
{
ENSURE_( alsa_snd_pcm_mmap_begin( self->pcm, &areas, &self->offset, numFrames ), paUnanticipatedHostError );
self->channelAreas = (snd_pcm_channel_area_t *)areas;
}
else
{
unsigned int bufferSize = self->numHostChannels * alsa_snd_pcm_format_size( self->nativeFormat, *numFrames );
if( bufferSize > self->nonMmapBufferSize )
{
self->nonMmapBuffer = realloc( self->nonMmapBuffer, ( self->nonMmapBufferSize = bufferSize ) );
if( !self->nonMmapBuffer )
{
result = paInsufficientMemory;
goto error;
}
}
}
if( self->hostInterleaved )
{
int swidth = alsa_snd_pcm_format_size( self->nativeFormat, 1 );
p = buffer = self->canMmap ? ExtractAddress( areas, self->offset ) : self->nonMmapBuffer;
for( i = 0; i < self->numUserChannels; ++i )
{
setChannel( bp, i, p, self->numHostChannels );
p += swidth;
}
}
else
{
if( self->canMmap )
{
for( i = 0; i < self->numUserChannels; ++i )
{
area = areas + i;
buffer = ExtractAddress( area, self->offset );
setChannel( bp, i, buffer, 1 );
}
}
else
{
unsigned int buf_per_ch_size = self->nonMmapBufferSize / self->numHostChannels;
buffer = self->nonMmapBuffer;
for( i = 0; i < self->numUserChannels; ++i )
{
setChannel( bp, i, buffer, 1 );
buffer += buf_per_ch_size;
}
}
}
if( !self->canMmap && StreamDirection_In == self->streamDir )
{
int res;
if( self->hostInterleaved )
res = alsa_snd_pcm_readi( self->pcm, self->nonMmapBuffer, *numFrames );
else
{
void *bufs[self->numHostChannels];
unsigned int buf_per_ch_size = self->nonMmapBufferSize / self->numHostChannels;
unsigned char *buffer = self->nonMmapBuffer;
int i;
for( i = 0; i < self->numHostChannels; ++i )
{
bufs[i] = buffer;
buffer += buf_per_ch_size;
}
res = alsa_snd_pcm_readn( self->pcm, bufs, *numFrames );
}
if( res == -EPIPE || res == -ESTRPIPE )
{
*xrun = 1;
*numFrames = 0;
}
}
end:
error:
return result;
}
static PaError PaAlsaStream_SetUpBuffers( PaAlsaStream* self, unsigned long* numFrames, int* xrunOccurred )
{
PaError result = paNoError;
unsigned long captureFrames = ULONG_MAX, playbackFrames = ULONG_MAX, commonFrames = 0;
int xrun = 0;
if( *xrunOccurred )
{
*numFrames = 0;
return result;
}
PA_UNLESS( self->capture.ready || self->playback.ready, paInternalError );
if( self->capture.pcm && self->capture.ready )
{
captureFrames = *numFrames;
PA_ENSURE( PaAlsaStreamComponent_RegisterChannels( &self->capture, &self->bufferProcessor, &captureFrames,
&xrun ) );
}
if( self->playback.pcm && self->playback.ready )
{
playbackFrames = *numFrames;
PA_ENSURE( PaAlsaStreamComponent_RegisterChannels( &self->playback, &self->bufferProcessor, &playbackFrames,
&xrun ) );
}
if( xrun )
{
assert( 0 == commonFrames );
goto end;
}
commonFrames = PA_MIN( captureFrames, playbackFrames );
if( commonFrames > *numFrames )
{
PA_DEBUG(( "%s: Common available frames are reported to be more than number requested: %lu, %lu, callbackMode: %d\n", __FUNCTION__,
commonFrames, *numFrames, self->callbackMode ));
if( self->capture.pcm )
{
PA_DEBUG(( "%s: captureFrames: %lu, capture.ready: %d\n", __FUNCTION__, captureFrames, self->capture.ready ));
}
if( self->playback.pcm )
{
PA_DEBUG(( "%s: playbackFrames: %lu, playback.ready: %d\n", __FUNCTION__, playbackFrames, self->playback.ready ));
}
commonFrames = 0;
goto end;
}
if( self->capture.pcm )
{
if( self->capture.ready )
{
PaUtil_SetInputFrameCount( &self->bufferProcessor, commonFrames );
}
else
{
PaUtil_SetNoInput( &self->bufferProcessor );
}
}
if( self->playback.pcm )
{
if( self->playback.ready )
{
PaUtil_SetOutputFrameCount( &self->bufferProcessor, commonFrames );
}
else
{
assert( self->neverDropInput );
assert( self->capture.pcm != NULL );
PA_DEBUG(( "%s: Setting output buffers to NULL\n", __FUNCTION__ ));
PaUtil_SetNoOutput( &self->bufferProcessor );
}
}
end:
*numFrames = commonFrames;
error:
if( xrun )
{
PA_ENSURE( PaAlsaStream_HandleXrun( self ) );
*numFrames = 0;
}
*xrunOccurred = xrun;
return result;
}
static void *CallbackThreadFunc( void *userData )
{
PaError result = paNoError;
PaAlsaStream *stream = (PaAlsaStream*) userData;
PaStreamCallbackTimeInfo timeInfo = {0, 0, 0};
snd_pcm_sframes_t startThreshold = 0;
int callbackResult = paContinue;
PaStreamCallbackFlags cbFlags = 0;   
int streamStarted = 0;
assert( stream );
assert( !stream->primeBuffers );
pthread_cleanup_push( &OnExit, stream );
#ifdef PTHREAD_CANCELED
pthread_testcancel();
pthread_setcancelstate( PTHREAD_CANCEL_DISABLE, NULL );
#endif
if( stream->primeBuffers )
{
snd_pcm_sframes_t avail;
if( stream->playback.pcm )
ENSURE_( alsa_snd_pcm_prepare( stream->playback.pcm ), paUnanticipatedHostError );
if( stream->capture.pcm && !stream->pcmsSynced )
ENSURE_( alsa_snd_pcm_prepare( stream->capture.pcm ), paUnanticipatedHostError );
avail = alsa_snd_pcm_avail_update( stream->playback.pcm );
startThreshold = avail - (avail % stream->playback.framesPerPeriod);
assert( startThreshold >= stream->playback.framesPerPeriod );
}
else
{
PA_ENSURE( PaUnixThread_PrepareNotify( &stream->thread ) );
PA_ENSURE( AlsaStart( stream, 0 ) );
PA_ENSURE( PaUnixThread_NotifyParent( &stream->thread ) );
streamStarted = 1;
}
while( 1 )
{
unsigned long framesAvail, framesGot;
int xrun = 0;
#ifdef PTHREAD_CANCELED
pthread_testcancel();
#endif
if( PaUnixThread_StopRequested( &stream->thread ) && paContinue == callbackResult )
{
PA_DEBUG(( "Setting callbackResult to paComplete\n" ));
callbackResult = paComplete;
}
if( paContinue != callbackResult )
{
stream->callbackAbort = ( paAbort == callbackResult );
if( stream->callbackAbort ||
PaUtil_IsBufferProcessorOutputEmpty( &stream->bufferProcessor ) )
{
goto end;
}
PA_DEBUG(( "%s: Flushing buffer processor\n", __FUNCTION__ ));
}
PA_ENSURE( PaAlsaStream_WaitForFrames( stream, &framesAvail, &xrun ) );
if( xrun )
{
assert( 0 == framesAvail );
continue;
}
while( framesAvail > 0 )
{
xrun = 0;
if( stream->underrun > 0.0 )
{
cbFlags |= paOutputUnderflow;
stream->underrun = 0.0;
}
if( stream->overrun > 0.0 )
{
cbFlags |= paInputOverflow;
stream->overrun = 0.0;
}
if( stream->capture.pcm && stream->playback.pcm )
{
if( !stream->capture.ready )
{
cbFlags |= paInputUnderflow;
PA_DEBUG(( "%s: Input underflow\n", __FUNCTION__ ));
}
else if( !stream->playback.ready )
{
cbFlags |= paOutputOverflow;
PA_DEBUG(( "%s: Output overflow\n", __FUNCTION__ ));
}
}
#if 0
CallbackUpdate( &stream->threading );
#endif
CalculateTimeInfo( stream, &timeInfo );
PaUtil_BeginBufferProcessing( &stream->bufferProcessor, &timeInfo, cbFlags );
cbFlags = 0;
PaUtil_BeginCpuLoadMeasurement( &stream->cpuLoadMeasurer );
framesGot = framesAvail;
if( paUtilFixedHostBufferSize == stream->bufferProcessor.hostBufferSizeMode )
{
framesGot = framesGot >= stream->maxFramesPerHostBuffer ? stream->maxFramesPerHostBuffer : 0;
}
else
{
assert( paUtilBoundedHostBufferSize == stream->bufferProcessor.hostBufferSizeMode );
framesGot = PA_MIN( framesGot, stream->maxFramesPerHostBuffer );
}
PA_ENSURE( PaAlsaStream_SetUpBuffers( stream, &framesGot, &xrun ) );
framesAvail -= framesGot;
if( framesGot > 0 )
{
assert( !xrun );
PaUtil_EndBufferProcessing( &stream->bufferProcessor, &callbackResult );
PA_ENSURE( PaAlsaStream_EndProcessing( stream, framesGot, &xrun ) );
}
PaUtil_EndCpuLoadMeasurement( &stream->cpuLoadMeasurer, framesGot );
if( 0 == framesGot )
{
break;
}
if( paContinue != callbackResult )
break;
}
}
end:
;  
pthread_cleanup_pop( 1 );
PA_DEBUG(( "%s: Thread %d exiting\n ", __FUNCTION__, pthread_self() ));
PaUnixThreading_EXIT( result );
error:
PA_DEBUG(( "%s: Thread %d is canceled due to error %d\n ", __FUNCTION__, pthread_self(), result ));
goto end;
}
static PaError ReadStream( PaStream* s, void *buffer, unsigned long frames )
{
PaError result = paNoError;
PaAlsaStream *stream = (PaAlsaStream*)s;
unsigned long framesGot, framesAvail;
void *userBuffer;
snd_pcm_t *save = stream->playback.pcm;
assert( stream );
PA_UNLESS( stream->capture.pcm, paCanNotReadFromAnOutputOnlyStream );
stream->playback.pcm = NULL;
if( stream->overrun > 0. )
{
result = paInputOverflowed;
stream->overrun = 0.0;
}
if( stream->capture.userInterleaved )
{
userBuffer = buffer;
}
else
{
userBuffer = stream->capture.userBuffers;
memcpy( userBuffer, buffer, sizeof (void *) * stream->capture.numUserChannels );
}
if( alsa_snd_pcm_state( stream->capture.pcm ) == SND_PCM_STATE_PREPARED )
{
ENSURE_( alsa_snd_pcm_start( stream->capture.pcm ), paUnanticipatedHostError );
}
while( frames > 0 )
{
int xrun = 0;
PA_ENSURE( PaAlsaStream_WaitForFrames( stream, &framesAvail, &xrun ) );
framesGot = PA_MIN( framesAvail, frames );
PA_ENSURE( PaAlsaStream_SetUpBuffers( stream, &framesGot, &xrun ) );
if( framesGot > 0 )
{
framesGot = PaUtil_CopyInput( &stream->bufferProcessor, &userBuffer, framesGot );
PA_ENSURE( PaAlsaStream_EndProcessing( stream, framesGot, &xrun ) );
frames -= framesGot;
}
}
end:
stream->playback.pcm = save;
return result;
error:
goto end;
}
static PaError WriteStream( PaStream* s, const void *buffer, unsigned long frames )
{
PaError result = paNoError;
signed long err;
PaAlsaStream *stream = (PaAlsaStream*)s;
snd_pcm_uframes_t framesGot, framesAvail;
const void *userBuffer;
snd_pcm_t *save = stream->capture.pcm;
assert( stream );
PA_UNLESS( stream->playback.pcm, paCanNotWriteToAnInputOnlyStream );
stream->capture.pcm = NULL;
if( stream->underrun > 0. )
{
result = paOutputUnderflowed;
stream->underrun = 0.0;
}
if( stream->playback.userInterleaved )
userBuffer = buffer;
else  
{
userBuffer = stream->playback.userBuffers;
memcpy( (void *)userBuffer, buffer, sizeof (void *) * stream->playback.numUserChannels );
}
while( frames > 0 )
{
int xrun = 0;
snd_pcm_uframes_t hwAvail;
PA_ENSURE( PaAlsaStream_WaitForFrames( stream, &framesAvail, &xrun ) );
framesGot = PA_MIN( framesAvail, frames );
PA_ENSURE( PaAlsaStream_SetUpBuffers( stream, &framesGot, &xrun ) );
if( framesGot > 0 )
{
framesGot = PaUtil_CopyOutput( &stream->bufferProcessor, &userBuffer, framesGot );
PA_ENSURE( PaAlsaStream_EndProcessing( stream, framesGot, &xrun ) );
frames -= framesGot;
}
PA_ENSURE( err = GetStreamWriteAvailable( stream ) );
framesAvail = err;
hwAvail = stream->playback.alsaBufferSize - framesAvail;
if( alsa_snd_pcm_state( stream->playback.pcm ) == SND_PCM_STATE_PREPARED &&
hwAvail >= stream->playback.framesPerPeriod )
{
ENSURE_( alsa_snd_pcm_start( stream->playback.pcm ), paUnanticipatedHostError );
}
}
end:
stream->capture.pcm = save;
return result;
error:
goto end;
}
static signed long GetStreamReadAvailable( PaStream* s )
{
PaError result = paNoError;
PaAlsaStream *stream = (PaAlsaStream*)s;
unsigned long avail;
int xrun;
PA_ENSURE( PaAlsaStreamComponent_GetAvailableFrames( &stream->capture, &avail, &xrun ) );
if( xrun )
{
PA_ENSURE( PaAlsaStream_HandleXrun( stream ) );
PA_ENSURE( PaAlsaStreamComponent_GetAvailableFrames( &stream->capture, &avail, &xrun ) );
if( xrun )
PA_ENSURE( paInputOverflowed );
}
return (signed long)avail;
error:
return result;
}
static signed long GetStreamWriteAvailable( PaStream* s )
{
PaError result = paNoError;
PaAlsaStream *stream = (PaAlsaStream*)s;
unsigned long avail;
int xrun;
PA_ENSURE( PaAlsaStreamComponent_GetAvailableFrames( &stream->playback, &avail, &xrun ) );
if( xrun )
{
snd_pcm_sframes_t savail;
PA_ENSURE( PaAlsaStream_HandleXrun( stream ) );
savail = alsa_snd_pcm_avail_update( stream->playback.pcm );
ENSURE_( savail, paUnanticipatedHostError );
avail = (unsigned long) savail;
}
return (signed long)avail;
error:
return result;
}
void PaAlsa_InitializeStreamInfo( PaAlsaStreamInfo *info )
{
info->size = sizeof (PaAlsaStreamInfo);
info->hostApiType = paALSA;
info->version = 1;
info->deviceString = NULL;
}
void PaAlsa_EnableRealtimeScheduling( PaStream *s, int enable )
{
PaAlsaStream *stream = (PaAlsaStream *) s;
stream->rtSched = enable;
}
#if 0
void PaAlsa_EnableWatchdog( PaStream *s, int enable )
{
PaAlsaStream *stream = (PaAlsaStream *) s;
stream->thread.useWatchdog = enable;
}
#endif
static PaError GetAlsaStreamPointer( PaStream* s, PaAlsaStream** stream )
{
PaError result = paNoError;
PaUtilHostApiRepresentation* hostApi;
PaAlsaHostApiRepresentation* alsaHostApi;
PA_ENSURE( PaUtil_ValidateStreamPointer( s ) );
PA_ENSURE( PaUtil_GetHostApiRepresentation( &hostApi, paALSA ) );
alsaHostApi = (PaAlsaHostApiRepresentation*)hostApi;
PA_UNLESS( PA_STREAM_REP( s )->streamInterface == &alsaHostApi->callbackStreamInterface
|| PA_STREAM_REP( s )->streamInterface == &alsaHostApi->blockingStreamInterface,
paIncompatibleStreamHostApi );
*stream = (PaAlsaStream*)s;
error:
return paNoError;
}
PaError PaAlsa_GetStreamInputCard( PaStream* s, int* card )
{
PaAlsaStream *stream;
PaError result = paNoError;
snd_pcm_info_t* pcmInfo;
stream = NULL;
PA_ENSURE( GetAlsaStreamPointer( s, &stream ) );
PA_UNLESS( stream->capture.pcm, paDeviceUnavailable );
alsa_snd_pcm_info_alloca( &pcmInfo );
PA_ENSURE( alsa_snd_pcm_info( stream->capture.pcm, pcmInfo ) );
*card = alsa_snd_pcm_info_get_card( pcmInfo );
error:
return result;
}
PaError PaAlsa_GetStreamOutputCard( PaStream* s, int* card )
{
PaAlsaStream *stream;
PaError result = paNoError;
snd_pcm_info_t* pcmInfo;
stream = NULL;
PA_ENSURE( GetAlsaStreamPointer( s, &stream ) );
PA_UNLESS( stream->playback.pcm, paDeviceUnavailable );
alsa_snd_pcm_info_alloca( &pcmInfo );
PA_ENSURE( alsa_snd_pcm_info( stream->playback.pcm, pcmInfo ) );
*card = alsa_snd_pcm_info_get_card( pcmInfo );
error:
return result;
}
PaError PaAlsa_SetRetriesBusy( int retries )
{
busyRetries_ = retries;
return paNoError;
}
