 
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>
#include <string.h>  
#include <math.h>
#include <errno.h>
#if defined(__APPLE__) && !defined(HAVE_MACH_ABSOLUTE_TIME)
#define HAVE_MACH_ABSOLUTE_TIME
#endif
#ifdef HAVE_MACH_ABSOLUTE_TIME
#include <mach/mach_time.h>
#endif
#include "pa_util.h"
#include "pa_unix_util.h"
#include "pa_debugprint.h"
#if PA_TRACK_MEMORY
static int numAllocations_ = 0;
#endif
void *PaUtil_AllocateMemory( long size )
{
void *result = malloc( size );
#if PA_TRACK_MEMORY
if( result != NULL ) numAllocations_ += 1;
#endif
return result;
}
void PaUtil_FreeMemory( void *block )
{
if( block != NULL )
{
free( block );
#if PA_TRACK_MEMORY
numAllocations_ -= 1;
#endif
}
}
int PaUtil_CountCurrentlyAllocatedBlocks( void )
{
#if PA_TRACK_MEMORY
return numAllocations_;
#else
return 0;
#endif
}
void Pa_Sleep( long msec )
{
#ifdef HAVE_NANOSLEEP
struct timespec req = {0}, rem = {0};
PaTime time = msec / 1.e3;
req.tv_sec = (time_t)time;
assert(time - req.tv_sec < 1.0);
req.tv_nsec = (long)((time - req.tv_sec) * 1.e9);
nanosleep(&req, &rem);
#else
while( msec > 999 )      
{                    
usleep( 999000 );
msec -= 999;
}
usleep( msec * 1000 );
#endif
}
#ifdef HAVE_MACH_ABSOLUTE_TIME
static double machSecondsConversionScaler_ = 0.0;
#endif
void PaUtil_InitializeClock( void )
{
#ifdef HAVE_MACH_ABSOLUTE_TIME
mach_timebase_info_data_t info;
kern_return_t err = mach_timebase_info( &info );
if( err == 0  )
machSecondsConversionScaler_ = 1e-9 * (double) info.numer / (double) info.denom;
#endif
}
PaTime PaUtil_GetTime( void )
{
#ifdef HAVE_MACH_ABSOLUTE_TIME
return mach_absolute_time() * machSecondsConversionScaler_;
#elif defined(HAVE_CLOCK_GETTIME)
struct timespec tp;
#if defined(CLOCK_MONOTONIC)
clock_gettime(CLOCK_MONOTONIC, &tp);
#else
clock_gettime(CLOCK_REALTIME, &tp);
#endif
return (PaTime)(tp.tv_sec + tp.tv_nsec * 1e-9);
#else
struct timeval tv;
gettimeofday( &tv, NULL );
return (PaTime) tv.tv_usec * 1e-6 + tv.tv_sec;
#endif
}
PaError PaUtil_InitializeThreading( PaUtilThreading *threading )
{
(void) paUtilErr_;
return paNoError;
}
void PaUtil_TerminateThreading( PaUtilThreading *threading )
{
}
PaError PaUtil_StartThreading( PaUtilThreading *threading, void *(*threadRoutine)(void *), void *data )
{
pthread_create( &threading->callbackThread, NULL, threadRoutine, data );
return paNoError;
}
PaError PaUtil_CancelThreading( PaUtilThreading *threading, int wait, PaError *exitResult )
{
PaError result = paNoError;
void *pret;
if( exitResult )
*exitResult = paNoError;
#ifdef PTHREAD_CANCELED
if( !wait )
pthread_cancel( threading->callbackThread );    
#endif
pthread_join( threading->callbackThread, &pret );
#ifdef PTHREAD_CANCELED
if( pret && PTHREAD_CANCELED != pret )
#else
if( pret && wait )
#endif
{
if( exitResult )
*exitResult = *(PaError *) pret;
free( pret );
}
return result;
}
#ifdef __APPLE__
pthread_t paUnixMainThread = 0;
#else
pthread_t paUnixMainThread = 0;
#endif
PaError PaUnixThreading_Initialize( void )
{
paUnixMainThread = pthread_self();
return paNoError;
}
static PaError BoostPriority( PaUnixThread* self )
{
PaError result = paNoError;
struct sched_param spm = { 0 };
spm.sched_priority = 1;
assert( self );
if( pthread_setschedparam( self->thread, SCHED_FIFO, &spm ) != 0 )
{
PA_UNLESS( errno == EPERM, paInternalError );   
PA_DEBUG(( "Failed bumping priority\n" ));
result = 0;
}
else
{
result = 1;  
}
error:
return result;
}
PaError PaUnixThread_New( PaUnixThread* self, void* (*threadFunc)( void* ), void* threadArg, PaTime waitForChild,
int rtSched )
{
PaError result = paNoError;
pthread_attr_t attr;
pthread_condattr_t cattr;
int started = 0;
memset( self, 0, sizeof (PaUnixThread) );
PaUnixMutex_Initialize( &self->mtx );
PA_ASSERT_CALL( pthread_condattr_init( &cattr ), 0 );
#if defined(CLOCK_MONOTONIC) && !defined(__APPLE__)
PA_ASSERT_CALL( pthread_condattr_setclock( &cattr, CLOCK_MONOTONIC ), 0 );
#endif
PA_ASSERT_CALL( pthread_cond_init( &self->cond, &cattr), 0 );
self->parentWaiting = 0 != waitForChild;
#if 0
#if defined _POSIX_MEMLOCK && (_POSIX_MEMLOCK != -1)
if( rtSched )
{
if( mlockall( MCL_CURRENT | MCL_FUTURE ) < 0 )
{
int savedErrno = errno;              
assert( savedErrno != EINVAL );      
PA_UNLESS( (savedErrno == EPERM), paInternalError );
PA_DEBUG(( "%s: Failed locking memory\n", __FUNCTION__ ));
}
else
PA_DEBUG(( "%s: Successfully locked memory\n", __FUNCTION__ ));
}
#endif
#endif
PA_UNLESS( !pthread_attr_init( &attr ), paInternalError );
PA_UNLESS( !pthread_attr_setscope( &attr, PTHREAD_SCOPE_SYSTEM ), paInternalError );
PA_UNLESS( !pthread_create( &self->thread, &attr, threadFunc, threadArg ), paInternalError );
started = 1;
if( rtSched )
{
#if 0
if( self->useWatchdog )
{
int err;
struct sched_param wdSpm = { 0 };
int prio = PA_MIN( self->rtPrio + 4, sched_get_priority_max( SCHED_FIFO ) );
wdSpm.sched_priority = prio;
PA_UNLESS( !pthread_attr_init( &attr ), paInternalError );
PA_UNLESS( !pthread_attr_setinheritsched( &attr, PTHREAD_EXPLICIT_SCHED ), paInternalError );
PA_UNLESS( !pthread_attr_setscope( &attr, PTHREAD_SCOPE_SYSTEM ), paInternalError );
PA_UNLESS( !pthread_attr_setschedpolicy( &attr, SCHED_FIFO ), paInternalError );
PA_UNLESS( !pthread_attr_setschedparam( &attr, &wdSpm ), paInternalError );
if( (err = pthread_create( &self->watchdogThread, &attr, &WatchdogFunc, self )) )
{
PA_UNLESS( err == EPERM, paInternalError );
PA_DEBUG(( "Failed bumping priority\n" ));
}
else
{
int policy;
self->watchdogRunning = 1;
PA_ENSURE_SYSTEM( pthread_getschedparam( self->watchdogThread, &policy, &wdSpm ), 0 );
if( wdSpm.sched_priority != prio )
{
PA_DEBUG(( "Watchdog priority not set correctly (%d)\n", wdSpm.sched_priority ));
PA_ENSURE( paInternalError );
}
}
}
else
#endif
PA_ENSURE( BoostPriority( self ) );
{
int policy;
struct sched_param spm;
pthread_getschedparam(self->thread, &policy, &spm);
}
}
if( self->parentWaiting )
{
PaTime till;
struct timespec ts;
int res = 0;
PaTime now;
PA_ENSURE( PaUnixMutex_Lock( &self->mtx ) );
now = PaUtil_GetTime();
till = now + waitForChild;
while( self->parentWaiting && !res )
{
if( waitForChild > 0 )
{
ts.tv_sec = (time_t) floor( till );
ts.tv_nsec = (long) ((till - floor( till )) * 1e9);
res = pthread_cond_timedwait( &self->cond, &self->mtx.mtx, &ts );
}
else
{
res = pthread_cond_wait( &self->cond, &self->mtx.mtx );
}
}
PA_ENSURE( PaUnixMutex_Unlock( &self->mtx ) );
PA_UNLESS( !res || ETIMEDOUT == res, paInternalError );
PA_DEBUG(( "%s: Waited for %g seconds for stream to start\n", __FUNCTION__, PaUtil_GetTime() - now ));
if( ETIMEDOUT == res )
{
PA_ENSURE( paTimedOut );
}
}
end:
return result;
error:
if( started )
{
PaUnixThread_Terminate( self, 0, NULL );
}
goto end;
}
PaError PaUnixThread_Terminate( PaUnixThread* self, int wait, PaError* exitResult )
{
PaError result = paNoError;
void* pret;
if( exitResult )
{
*exitResult = paNoError;
}
#if 0
if( watchdogExitResult )
*watchdogExitResult = paNoError;
if( th->watchdogRunning )
{
pthread_cancel( th->watchdogThread );
PA_ENSURE_SYSTEM( pthread_join( th->watchdogThread, &pret ), 0 );
if( pret && pret != PTHREAD_CANCELED )
{
if( watchdogExitResult )
*watchdogExitResult = *(PaError *) pret;
free( pret );
}
}
#endif
self->stopRequested = wait;
if( !wait )
{
PA_DEBUG(( "%s: Canceling thread %d\n", __FUNCTION__, self->thread ));
#ifdef PTHREAD_CANCELED
pthread_cancel( self->thread );
#endif
}
PA_DEBUG(( "%s: Joining thread %d\n", __FUNCTION__, self->thread ));
PA_ENSURE_SYSTEM( pthread_join( self->thread, &pret ), 0 );
#ifdef PTHREAD_CANCELED
if( pret && PTHREAD_CANCELED != pret )
#else
if( pret && wait )
#endif
{
if( exitResult )
{
*exitResult = *(PaError*)pret;
}
free( pret );
}
error:
PA_ASSERT_CALL( PaUnixMutex_Terminate( &self->mtx ), paNoError );
PA_ASSERT_CALL( pthread_cond_destroy( &self->cond ), 0 );
return result;
}
PaError PaUnixThread_PrepareNotify( PaUnixThread* self )
{
PaError result = paNoError;
PA_UNLESS( self->parentWaiting, paInternalError );
PA_ENSURE( PaUnixMutex_Lock( &self->mtx ) );
self->locked = 1;
error:
return result;
}
PaError PaUnixThread_NotifyParent( PaUnixThread* self )
{
PaError result = paNoError;
PA_UNLESS( self->parentWaiting, paInternalError );
if( !self->locked )
{
PA_ENSURE( PaUnixMutex_Lock( &self->mtx ) );
self->locked = 1;
}
self->parentWaiting = 0;
pthread_cond_signal( &self->cond );
PA_ENSURE( PaUnixMutex_Unlock( &self->mtx ) );
self->locked = 0;
error:
return result;
}
int PaUnixThread_StopRequested( PaUnixThread* self )
{
return self->stopRequested;
}
PaError PaUnixMutex_Initialize( PaUnixMutex* self )
{
PaError result = paNoError;
PA_ASSERT_CALL( pthread_mutex_init( &self->mtx, NULL ), 0 );
return result;
}
PaError PaUnixMutex_Terminate( PaUnixMutex* self )
{
PaError result = paNoError;
PA_ASSERT_CALL( pthread_mutex_destroy( &self->mtx ), 0 );
return result;
}
PaError PaUnixMutex_Lock( PaUnixMutex* self )
{
PaError result = paNoError;
#ifdef PTHREAD_CANCEL
int oldState;
PA_ENSURE_SYSTEM( pthread_setcancelstate( PTHREAD_CANCEL_DISABLE, &oldState ), 0 );
#endif
PA_ENSURE_SYSTEM( pthread_mutex_lock( &self->mtx ), 0 );
error:
return result;
}
PaError PaUnixMutex_Unlock( PaUnixMutex* self )
{
PaError result = paNoError;
PA_ENSURE_SYSTEM( pthread_mutex_unlock( &self->mtx ), 0 );
#ifdef PTHREAD_CANCEL
int oldState;
PA_ENSURE_SYSTEM( pthread_setcancelstate( PTHREAD_CANCEL_ENABLE, &oldState ), 0 );
#endif
error:
return result;
}
#if 0
static void OnWatchdogExit( void *userData )
{
PaAlsaThreading *th = (PaAlsaThreading *) userData;
struct sched_param spm = { 0 };
assert( th );
PA_ASSERT_CALL( pthread_setschedparam( th->callbackThread, SCHED_OTHER, &spm ), 0 );     
PA_DEBUG(( "Watchdog exiting\n" ));
}
static void *WatchdogFunc( void *userData )
{
PaError result = paNoError, *pres = NULL;
int err;
PaAlsaThreading *th = (PaAlsaThreading *) userData;
unsigned intervalMsec = 500;
const PaTime maxSeconds = 3.;    
PaTime timeThen = PaUtil_GetTime(), timeNow, timeElapsed, cpuTimeThen, cpuTimeNow, cpuTimeElapsed;
double cpuLoad, avgCpuLoad = 0.;
int throttled = 0;
assert( th );
pthread_cleanup_push( &OnWatchdogExit, th );
PA_ENSURE( result = BoostPriority( th ) );
if( !result )
{
pthread_exit( NULL );
}
cpuTimeThen = th->callbackCpuTime;
{
int policy;
struct sched_param spm = { 0 };
pthread_getschedparam( pthread_self(), &policy, &spm );
PA_DEBUG(( "%s: Watchdog priority is %d\n", __FUNCTION__, spm.sched_priority ));
}
while( 1 )
{
double lowpassCoeff = 0.9, lowpassCoeff1 = 0.99999 - lowpassCoeff;
pthread_testcancel();
Pa_Sleep( intervalMsec );
pthread_testcancel();
if( PaUtil_GetTime() - th->callbackTime > maxSeconds )
{
PA_DEBUG(( "Watchdog: Terminating callback thread\n" ));
err = pthread_kill( th->callbackThread, SIGKILL );
pthread_exit( NULL );
}
PA_DEBUG(( "%s: PortAudio reports CPU load: %g\n", __FUNCTION__, PaUtil_GetCpuLoad( th->cpuLoadMeasurer ) ));
cpuTimeNow = th->callbackCpuTime;
cpuTimeElapsed = cpuTimeNow - cpuTimeThen;
cpuTimeThen = cpuTimeNow;
timeNow = PaUtil_GetTime();
timeElapsed = timeNow - timeThen;
timeThen = timeNow;
cpuLoad = cpuTimeElapsed / timeElapsed;
avgCpuLoad = avgCpuLoad * lowpassCoeff + cpuLoad * lowpassCoeff1;
if( PaUtil_GetCpuLoad( th->cpuLoadMeasurer ) > .925 )
{
static int policy;
static struct sched_param spm = { 0 };
static const struct sched_param defaultSpm = { 0 };
PA_DEBUG(( "%s: Throttling audio thread, priority %d\n", __FUNCTION__, spm.sched_priority ));
pthread_getschedparam( th->callbackThread, &policy, &spm );
if( !pthread_setschedparam( th->callbackThread, SCHED_OTHER, &defaultSpm ) )
{
throttled = 1;
}
else
PA_DEBUG(( "Watchdog: Couldn't lower priority of audio thread: %s\n", strerror( errno ) ));
PA_DEBUG(( "%s: Watchdog sleeping for %lu msecs before unthrottling\n", __FUNCTION__, th->throttledSleepTime ));
Pa_Sleep( th->throttledSleepTime );
if( pthread_setschedparam( th->callbackThread, SCHED_FIFO, &spm ) != 0 )
{
PA_DEBUG(( "%s: Couldn't raise priority of audio thread: %s\n", __FUNCTION__, strerror( errno ) ));
}
if( PaUtil_GetCpuLoad( th->cpuLoadMeasurer ) >= .99 )
intervalMsec = 50;
else
intervalMsec = 100;
}
else if( throttled && avgCpuLoad < .8 )
{
intervalMsec = 500;
throttled = 0;
}
}
pthread_cleanup_pop( 1 );    
error:
pres = malloc( sizeof (PaError) );
*pres = result;
pthread_exit( pres );
}
static void CallbackUpdate( PaAlsaThreading *th )
{
th->callbackTime = PaUtil_GetTime();
th->callbackCpuTime = PaUtil_GetCpuLoad( th->cpuLoadMeasurer );
}
#endif
