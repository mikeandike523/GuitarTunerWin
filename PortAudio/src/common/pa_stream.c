 
#include "pa_stream.h"
void PaUtil_InitializeStreamInterface( PaUtilStreamInterface *streamInterface,
PaError (*Close)( PaStream* ),
PaError (*Start)( PaStream* ),
PaError (*Stop)( PaStream* ),
PaError (*Abort)( PaStream* ),
PaError (*IsStopped)( PaStream* ),
PaError (*IsActive)( PaStream* ),
PaTime (*GetTime)( PaStream* ),
double (*GetCpuLoad)( PaStream* ),
PaError (*Read)( PaStream*, void *, unsigned long ),
PaError (*Write)( PaStream*, const void *, unsigned long ),
signed long (*GetReadAvailable)( PaStream* ),
signed long (*GetWriteAvailable)( PaStream* )  )
{
streamInterface->Close = Close;
streamInterface->Start = Start;
streamInterface->Stop = Stop;
streamInterface->Abort = Abort;
streamInterface->IsStopped = IsStopped;
streamInterface->IsActive = IsActive;
streamInterface->GetTime = GetTime;
streamInterface->GetCpuLoad = GetCpuLoad;
streamInterface->Read = Read;
streamInterface->Write = Write;
streamInterface->GetReadAvailable = GetReadAvailable;
streamInterface->GetWriteAvailable = GetWriteAvailable;
}
void PaUtil_InitializeStreamRepresentation( PaUtilStreamRepresentation *streamRepresentation,
PaUtilStreamInterface *streamInterface,
PaStreamCallback *streamCallback,
void *userData )
{
streamRepresentation->magic = PA_STREAM_MAGIC;
streamRepresentation->nextOpenStream = 0;
streamRepresentation->streamInterface = streamInterface;
streamRepresentation->streamCallback = streamCallback;
streamRepresentation->streamFinishedCallback = 0;
streamRepresentation->userData = userData;
streamRepresentation->streamInfo.inputLatency = 0.;
streamRepresentation->streamInfo.outputLatency = 0.;
streamRepresentation->streamInfo.sampleRate = 0.;
}
void PaUtil_TerminateStreamRepresentation( PaUtilStreamRepresentation *streamRepresentation )
{
streamRepresentation->magic = 0;
}
PaError PaUtil_DummyRead( PaStream* stream,
void *buffer,
unsigned long frames )
{
(void)stream;  
(void)buffer;  
(void)frames;  
return paCanNotReadFromACallbackStream;
}
PaError PaUtil_DummyWrite( PaStream* stream,
const void *buffer,
unsigned long frames )
{
(void)stream;  
(void)buffer;  
(void)frames;  
return paCanNotWriteToACallbackStream;
}
signed long PaUtil_DummyGetReadAvailable( PaStream* stream )
{
(void)stream;  
return paCanNotReadFromACallbackStream;
}
signed long PaUtil_DummyGetWriteAvailable( PaStream* stream )
{
(void)stream;  
return paCanNotWriteToACallbackStream;
}
double PaUtil_DummyGetCpuLoad( PaStream* stream )
{
(void)stream;  
return 0.0;
}
