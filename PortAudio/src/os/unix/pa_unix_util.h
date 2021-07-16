 
#ifndef PA_UNIX_UTIL_H
#define PA_UNIX_UTIL_H
#include "pa_cpuload.h"
#include <assert.h>
#include <pthread.h>
#include <signal.h>
#ifdef __cplusplus
extern "C"
{
#endif  
#define PA_MIN(x,y) ( (x) < (y) ? (x) : (y) )
#define PA_MAX(x,y) ( (x) > (y) ? (x) : (y) )
#if defined __GNUC__ && __GNUC__ >= 3
#define UNLIKELY(expr) __builtin_expect( (expr), 0 )
#else
#define UNLIKELY(expr) (expr)
#endif
#define STRINGIZE_HELPER(expr) #expr
#define STRINGIZE(expr) STRINGIZE_HELPER(expr)
#define PA_UNLESS(expr, code) \
do { \
if( UNLIKELY( (expr) == 0 ) ) \
{ \
PaUtil_DebugPrint(( "Expression '" #expr "' failed in '" __FILE__ "', line: " STRINGIZE( __LINE__ ) "\n" )); \
result = (code); \
goto error; \
} \
} while (0);
static PaError paUtilErr_;           
#define PA_ENSURE(expr) \
do { \
if( UNLIKELY( (paUtilErr_ = (expr)) < paNoError ) ) \
{ \
PaUtil_DebugPrint(( "Expression '" #expr "' failed in '" __FILE__ "', line: " STRINGIZE( __LINE__ ) "\n" )); \
result = paUtilErr_; \
goto error; \
} \
} while (0);
#define PA_ASSERT_CALL(expr, success) \
paUtilErr_ = (expr); \
assert( success == paUtilErr_ );
#define PA_ENSURE_SYSTEM(expr, success) \
do { \
if( UNLIKELY( (paUtilErr_ = (expr)) != success ) ) \
{ \
\
if( pthread_equal(pthread_self(), paUnixMainThread) ) \
{ \
PaUtil_SetLastHostErrorInfo( paALSA, paUtilErr_, strerror( paUtilErr_ ) ); \
} \
PaUtil_DebugPrint( "Expression '" #expr "' failed in '" __FILE__ "', line: " STRINGIZE( __LINE__ ) "\n" ); \
result = paUnanticipatedHostError; \
goto error; \
} \
} while( 0 );
typedef struct {
pthread_t callbackThread;
} PaUtilThreading;
PaError PaUtil_InitializeThreading( PaUtilThreading *threading );
void PaUtil_TerminateThreading( PaUtilThreading *threading );
PaError PaUtil_StartThreading( PaUtilThreading *threading, void *(*threadRoutine)(void *), void *data );
PaError PaUtil_CancelThreading( PaUtilThreading *threading, int wait, PaError *exitResult );
extern pthread_t paUnixMainThread;
typedef struct
{
pthread_mutex_t mtx;
} PaUnixMutex;
PaError PaUnixMutex_Initialize( PaUnixMutex* self );
PaError PaUnixMutex_Terminate( PaUnixMutex* self );
PaError PaUnixMutex_Lock( PaUnixMutex* self );
PaError PaUnixMutex_Unlock( PaUnixMutex* self );
typedef struct
{
pthread_t thread;
int parentWaiting;
int stopRequested;
int locked;
PaUnixMutex mtx;
pthread_cond_t cond;
volatile sig_atomic_t stopRequest;
} PaUnixThread;
PaError PaUnixThreading_Initialize( void );
#define PaUnixThreading_EXIT(result) \
do { \
PaError* pres = NULL; \
if( paNoError != (result) ) \
{ \
pres = malloc( sizeof (PaError) ); \
*pres = (result); \
} \
pthread_exit( pres ); \
} while (0);
PaError PaUnixThread_New( PaUnixThread* self, void* (*threadFunc)( void* ), void* threadArg, PaTime waitForChild,
int rtSched );
PaError PaUnixThread_Terminate( PaUnixThread* self, int wait, PaError* exitResult );
PaError PaUnixThread_PrepareNotify( PaUnixThread* self );
PaError PaUnixThread_NotifyParent( PaUnixThread* self );
int PaUnixThread_StopRequested( PaUnixThread* self );
#ifdef __cplusplus
}
#endif  
#endif
