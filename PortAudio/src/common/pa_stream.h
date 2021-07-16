#ifndef PA_STREAM_H
#define PA_STREAM_H
#include "portaudio.h"
#ifdef __cplusplus
extern "C"
{
#endif  
#define PA_STREAM_MAGIC (0x18273645)
typedef struct {
PaError (*Close)( PaStream* stream );
PaError (*Start)( PaStream *stream );
PaError (*Stop)( PaStream *stream );
PaError (*Abort)( PaStream *stream );
PaError (*IsStopped)( PaStream *stream );
PaError (*IsActive)( PaStream *stream );
PaTime (*GetTime)( PaStream *stream );
double (*GetCpuLoad)( PaStream* stream );
PaError (*Read)( PaStream* stream, void *buffer, unsigned long frames );
PaError (*Write)( PaStream* stream, const void *buffer, unsigned long frames );
signed long (*GetReadAvailable)( PaStream* stream );
signed long (*GetWriteAvailable)( PaStream* stream );
} PaUtilStreamInterface;
void PaUtil_InitializeStreamInterface( PaUtilStreamInterface *streamInterface,
PaError (*Close)( PaStream* ),
PaError (*Start)( PaStream* ),
PaError (*Stop)( PaStream* ),
PaError (*Abort)( PaStream* ),
PaError (*IsStopped)( PaStream* ),
PaError (*IsActive)( PaStream* ),
PaTime (*GetTime)( PaStream* ),
double (*GetCpuLoad)( PaStream* ),
PaError (*Read)( PaStream* stream, void *buffer, unsigned long frames ),
PaError (*Write)( PaStream* stream, const void *buffer, unsigned long frames ),
signed long (*GetReadAvailable)( PaStream* stream ),
signed long (*GetWriteAvailable)( PaStream* stream ) );
PaError PaUtil_DummyRead( PaStream* stream,
void *buffer,
unsigned long frames );
PaError PaUtil_DummyWrite( PaStream* stream,
const void *buffer,
unsigned long frames );
signed long PaUtil_DummyGetReadAvailable( PaStream* stream );
signed long PaUtil_DummyGetWriteAvailable( PaStream* stream );
double PaUtil_DummyGetCpuLoad( PaStream* stream );
typedef struct PaUtilStreamRepresentation {
unsigned long magic;     
struct PaUtilStreamRepresentation *nextOpenStream;  
PaUtilStreamInterface *streamInterface;
PaStreamCallback *streamCallback;
PaStreamFinishedCallback *streamFinishedCallback;
void *userData;
PaStreamInfo streamInfo;
} PaUtilStreamRepresentation;
void PaUtil_InitializeStreamRepresentation(
PaUtilStreamRepresentation *streamRepresentation,
PaUtilStreamInterface *streamInterface,
PaStreamCallback *streamCallback,
void *userData );
void PaUtil_TerminateStreamRepresentation( PaUtilStreamRepresentation *streamRepresentation );
PaError PaUtil_ValidateStreamPointer( PaStream *stream );
#define PA_STREAM_REP( stream )\
((PaUtilStreamRepresentation*) (stream) )
#define PA_STREAM_INTERFACE( stream )\
PA_STREAM_REP( (stream) )->streamInterface
#ifdef __cplusplus
}
#endif  
#endif  
