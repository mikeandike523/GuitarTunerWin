#ifndef PA_LOG_H
#define PA_LOG_H
#ifdef __cplusplus
extern "C"
{
#endif  
void PaUtil_DebugPrint( const char *format, ... );
#ifdef PA_ENABLE_DEBUG_OUTPUT
#define PA_DEBUG(x) PaUtil_DebugPrint x ;
#else
#define PA_DEBUG(x)
#endif
#ifdef PA_LOG_API_CALLS
#define PA_LOGAPI(x) PaUtil_DebugPrint x
#define PA_LOGAPI_ENTER(functionName) PaUtil_DebugPrint( functionName " called.\n" )
#define PA_LOGAPI_ENTER_PARAMS(functionName) PaUtil_DebugPrint( functionName " called:\n" )
#define PA_LOGAPI_EXIT(functionName) PaUtil_DebugPrint( functionName " returned.\n" )
#define PA_LOGAPI_EXIT_PAERROR( functionName, result ) \
PaUtil_DebugPrint( functionName " returned:\n" ); \
PaUtil_DebugPrint("\tPaError: %d ( %s )\n", result, Pa_GetErrorText( result ) )
#define PA_LOGAPI_EXIT_T( functionName, resultFormatString, result ) \
PaUtil_DebugPrint( functionName " returned:\n" ); \
PaUtil_DebugPrint("\t" resultFormatString "\n", result )
#define PA_LOGAPI_EXIT_PAERROR_OR_T_RESULT( functionName, positiveResultFormatString, result ) \
PaUtil_DebugPrint( functionName " returned:\n" ); \
if( result > 0 ) \
PaUtil_DebugPrint("\t" positiveResultFormatString "\n", result ); \
else \
PaUtil_DebugPrint("\tPaError: %d ( %s )\n", result, Pa_GetErrorText( result ) )
#else
#define PA_LOGAPI(x)
#define PA_LOGAPI_ENTER(functionName)
#define PA_LOGAPI_ENTER_PARAMS(functionName)
#define PA_LOGAPI_EXIT(functionName)
#define PA_LOGAPI_EXIT_PAERROR( functionName, result )
#define PA_LOGAPI_EXIT_T( functionName, resultFormatString, result )
#define PA_LOGAPI_EXIT_PAERROR_OR_T_RESULT( functionName, positiveResultFormatString, result )
#endif
typedef void (*PaUtilLogCallback ) (const char *log);
void PaUtil_SetDebugPrintFunction(PaUtilLogCallback  cb);
#ifdef __cplusplus
}
#endif  
#endif  
