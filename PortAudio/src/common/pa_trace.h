#ifndef PA_TRACE_H
#define PA_TRACE_H
#ifndef PA_TRACE_REALTIME_EVENTS
#define PA_TRACE_REALTIME_EVENTS     (0)    
#endif
#ifndef PA_MAX_TRACE_RECORDS
#define PA_MAX_TRACE_RECORDS      (2048)    
#endif
#ifdef __cplusplus
extern "C"
{
#endif  
#if PA_TRACE_REALTIME_EVENTS
void PaUtil_ResetTraceMessages();
void PaUtil_AddTraceMessage( const char *msg, int data );
void PaUtil_DumpTraceMessages();
typedef void* LogHandle;
int PaUtil_InitializeHighSpeedLog(LogHandle* phLog, unsigned maxSizeInBytes);
void PaUtil_ResetHighSpeedLogTimeRef(LogHandle hLog);
int PaUtil_AddHighSpeedLogMessage(LogHandle hLog, const char* fmt, ...);
void PaUtil_DumpHighSpeedLog(LogHandle hLog, const char* fileName);
void PaUtil_DiscardHighSpeedLog(LogHandle hLog);
#else
#define PaUtil_ResetTraceMessages()  
#define PaUtil_AddTraceMessage(msg,data)  
#define PaUtil_DumpTraceMessages()  
#define PaUtil_InitializeHighSpeedLog(phLog, maxSizeInBytes)  (0)
#define PaUtil_ResetHighSpeedLogTimeRef(hLog)
#define PaUtil_AddHighSpeedLogMessage(...)   (0)
#define PaUtil_DumpHighSpeedLog(hLog, fileName)
#define PaUtil_DiscardHighSpeedLog(hLog)
#endif
#ifdef __cplusplus
}
#endif  
#endif  
