 
#ifndef PA_WIN_COINITIALIZE_H
#define PA_WIN_COINITIALIZE_H
#ifdef __cplusplus
extern "C"
{
#endif  
typedef struct PaWinUtilComInitializationResult{
int state;
DWORD initializingThreadId;
} PaWinUtilComInitializationResult;
PaError PaWinUtil_CoInitialize( PaHostApiTypeId hostApiType, PaWinUtilComInitializationResult *comInitializationResult );
void PaWinUtil_CoUninitialize( PaHostApiTypeId hostApiType, PaWinUtilComInitializationResult *comInitializationResult );
#ifdef __cplusplus
}
#endif  
#endif  
