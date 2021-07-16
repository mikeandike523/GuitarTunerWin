 
#include "pa_mac_core_blocking.h"
#include "pa_mac_core_internal.h"
#include <assert.h>
#ifdef MOSX_USE_NON_ATOMIC_FLAG_BITS
# define OSAtomicOr32( a, b ) ( (*(b)) |= (a) )
# define OSAtomicAnd32( a, b ) ( (*(b)) &= (a) )
#else
# include <libkern/OSAtomic.h>
#endif
static size_t computeSampleSizeFromFormat( PaSampleFormat format )
{
switch( format & (~paNonInterleaved) ) {
case paFloat32: return 4;
case paInt32:   return 4;
case paInt24:   return 3;
case paInt16:   return 2;
case paInt8:
case paUInt8:   return 1;
default:        return 0;
}
}
static size_t computeSampleSizeFromFormatPow2( PaSampleFormat format )
{
switch( format & (~paNonInterleaved) ) {
case paFloat32: return 4;
case paInt32:   return 4;
case paInt24:   return 4;
case paInt16:   return 2;
case paInt8:
case paUInt8:   return 1;
default:        return 0;
}
}
PaError initializeBlioRingBuffers(
PaMacBlio *blio,
PaSampleFormat inputSampleFormat,
PaSampleFormat outputSampleFormat,
long ringBufferSizeInFrames,
int inChan,
int outChan )
{
void *data;
int result;
OSStatus err;
bzero( blio, sizeof( PaMacBlio ) );
blio->inputRingBuffer.buffer = NULL;
blio->outputRingBuffer.buffer = NULL;
blio->ringBufferFrames = ringBufferSizeInFrames;
blio->inputSampleFormat = inputSampleFormat;
blio->inputSampleSizeActual = computeSampleSizeFromFormat(inputSampleFormat);
blio->inputSampleSizePow2 = computeSampleSizeFromFormatPow2(inputSampleFormat);  
blio->outputSampleFormat = outputSampleFormat;
blio->outputSampleSizeActual = computeSampleSizeFromFormat(outputSampleFormat);
blio->outputSampleSizePow2 = computeSampleSizeFromFormatPow2(outputSampleFormat);
blio->inChan = inChan;
blio->outChan = outChan;
blio->statusFlags = 0;
blio->errors = paNoError;
#ifdef PA_MAC_BLIO_MUTEX
blio->isInputEmpty = false;
blio->isOutputFull = false;
#endif
#ifdef PA_MAC_BLIO_MUTEX
result = PaMacCore_SetUnixError( pthread_mutex_init(&(blio->inputMutex),NULL), 0 );
if( result )
goto error;
result = UNIX_ERR( pthread_cond_init( &(blio->inputCond), NULL ) );
if( result )
goto error;
result = UNIX_ERR( pthread_mutex_init(&(blio->outputMutex),NULL) );
if( result )
goto error;
result = UNIX_ERR( pthread_cond_init( &(blio->outputCond), NULL ) );
#endif
if( inChan ) {
data = calloc( ringBufferSizeInFrames, blio->inputSampleSizePow2 * inChan );
if( !data )
{
result = paInsufficientMemory;
goto error;
}
err = PaUtil_InitializeRingBuffer(
&blio->inputRingBuffer,
blio->inputSampleSizePow2 * inChan,
ringBufferSizeInFrames,
data );
assert( !err );
}
if( outChan ) {
data = calloc( ringBufferSizeInFrames, blio->outputSampleSizePow2 * outChan );
if( !data )
{
result = paInsufficientMemory;
goto error;
}
err = PaUtil_InitializeRingBuffer(
&blio->outputRingBuffer,
blio->outputSampleSizePow2 * outChan,
ringBufferSizeInFrames,
data );
assert( !err );
}
result = resetBlioRingBuffers( blio );
if( result )
goto error;
return 0;
error:
destroyBlioRingBuffers( blio );
return result;
}
#ifdef PA_MAC_BLIO_MUTEX
PaError blioSetIsInputEmpty( PaMacBlio *blio, bool isEmpty )
{
PaError result = paNoError;
if( isEmpty == blio->isInputEmpty )
goto done;
result = UNIX_ERR( pthread_mutex_lock( &blio->inputMutex ) );
if( result )
goto done;
blio->isInputEmpty = isEmpty;
result = UNIX_ERR( pthread_mutex_unlock( &blio->inputMutex ) );
if( result )
goto done;
result = UNIX_ERR( pthread_cond_broadcast( &blio->inputCond ) );
if( result )
goto done;
done:
return result;
}
PaError blioSetIsOutputFull( PaMacBlio *blio, bool isFull )
{
PaError result = paNoError;
if( isFull == blio->isOutputFull )
goto done;
result = UNIX_ERR( pthread_mutex_lock( &blio->outputMutex ) );
if( result )
goto done;
blio->isOutputFull = isFull;
result = UNIX_ERR( pthread_mutex_unlock( &blio->outputMutex ) );
if( result )
goto done;
result = UNIX_ERR( pthread_cond_broadcast( &blio->outputCond ) );
if( result )
goto done;
done:
return result;
}
#endif
PaError resetBlioRingBuffers( PaMacBlio *blio )
{
#ifdef PA_MAC__BLIO_MUTEX
int result;
#endif
blio->statusFlags = 0;
if( blio->outputRingBuffer.buffer ) {
PaUtil_FlushRingBuffer( &blio->outputRingBuffer );
bzero( blio->outputRingBuffer.buffer,
blio->outputRingBuffer.bufferSize * blio->outputRingBuffer.elementSizeBytes );
PaUtil_AdvanceRingBufferWriteIndex( &blio->outputRingBuffer, blio->ringBufferFrames );
#ifdef PA_MAC__BLIO_MUTEX
result = blioSetIsOutputFull( blio, toAdvance == blio->outputRingBuffer.bufferSize );
if( result )
goto error;
#endif
}
if( blio->inputRingBuffer.buffer ) {
PaUtil_FlushRingBuffer( &blio->inputRingBuffer );
bzero( blio->inputRingBuffer.buffer,
blio->inputRingBuffer.bufferSize * blio->inputRingBuffer.elementSizeBytes );
#ifdef PA_MAC__BLIO_MUTEX
result = blioSetIsInputEmpty( blio, true );
if( result )
goto error;
#endif
}
return paNoError;
#ifdef PA_MAC__BLIO_MUTEX
error:
return result;
#endif
}
PaError destroyBlioRingBuffers( PaMacBlio *blio )
{
PaError result = paNoError;
if( blio->inputRingBuffer.buffer ) {
free( blio->inputRingBuffer.buffer );
#ifdef PA_MAC__BLIO_MUTEX
result = UNIX_ERR( pthread_mutex_destroy( & blio->inputMutex ) );
if( result ) return result;
result = UNIX_ERR( pthread_cond_destroy( & blio->inputCond ) );
if( result ) return result;
#endif
}
blio->inputRingBuffer.buffer = NULL;
if( blio->outputRingBuffer.buffer ) {
free( blio->outputRingBuffer.buffer );
#ifdef PA_MAC__BLIO_MUTEX
result = UNIX_ERR( pthread_mutex_destroy( & blio->outputMutex ) );
if( result ) return result;
result = UNIX_ERR( pthread_cond_destroy( & blio->outputCond ) );
if( result ) return result;
#endif
}
blio->outputRingBuffer.buffer = NULL;
return result;
}
int BlioCallback( const void *input, void *output, unsigned long frameCount,
const PaStreamCallbackTimeInfo* timeInfo,
PaStreamCallbackFlags statusFlags,
void *userData )
{
PaMacBlio *blio = (PaMacBlio*)userData;
ring_buffer_size_t framesAvailable;
ring_buffer_size_t framesToTransfer;
ring_buffer_size_t framesTransferred;
OSAtomicOr32( statusFlags, &blio->statusFlags ) ;
if( blio->inChan ) {
framesAvailable = PaUtil_GetRingBufferWriteAvailable( &blio->inputRingBuffer );
if( framesAvailable < frameCount )
{
OSAtomicOr32( paInputOverflow, &blio->statusFlags );
framesToTransfer = framesAvailable;
}
else
{
framesToTransfer = (ring_buffer_size_t)frameCount;
}
framesTransferred = PaUtil_WriteRingBuffer( &blio->inputRingBuffer, input, framesToTransfer );
assert( framesToTransfer == framesTransferred );
#ifdef PA_MAC__BLIO_MUTEX
blioSetIsInputEmpty( blio, false );
#endif
}
if( blio->outChan ) {
framesAvailable = PaUtil_GetRingBufferReadAvailable( &blio->outputRingBuffer );
if( framesAvailable < frameCount )
{
framesToTransfer = framesAvailable;
size_t bytesPerFrame = blio->outputSampleSizeActual * blio->outChan;
size_t offsetInBytes = framesToTransfer * bytesPerFrame;
size_t countInBytes = (frameCount - framesToTransfer) * bytesPerFrame;
bzero( ((char *)output) + offsetInBytes, countInBytes );
OSAtomicOr32( paOutputUnderflow, &blio->statusFlags );
framesToTransfer = framesAvailable;
}
else
{
framesToTransfer = (ring_buffer_size_t)frameCount;
}
framesTransferred = PaUtil_ReadRingBuffer( &blio->outputRingBuffer, output, framesToTransfer );
assert( framesToTransfer == framesTransferred );
#ifdef PA_MAC__BLIO_MUTEX
blioSetIsOutputFull( blio, false );
#endif
}
return paContinue;
}
PaError ReadStream( PaStream* stream,
void *buffer,
unsigned long framesRequested )
{
PaMacBlio *blio = & ((PaMacCoreStream*)stream) -> blio;
char *cbuf = (char *) buffer;
PaError ret = paNoError;
VVDBUG(("ReadStream()\n"));
while( framesRequested > 0 ) {
ring_buffer_size_t framesAvailable;
ring_buffer_size_t framesToTransfer;
ring_buffer_size_t framesTransferred;
do {
framesAvailable = PaUtil_GetRingBufferReadAvailable( &blio->inputRingBuffer );
if( framesAvailable == 0 ) {
#ifdef PA_MAC_BLIO_MUTEX
ret = UNIX_ERR( pthread_mutex_lock( &blio->inputMutex ) );
if( ret )
return ret;
while( blio->isInputEmpty ) {
ret = UNIX_ERR( pthread_cond_wait( &blio->inputCond, &blio->inputMutex ) );
if( ret )
return ret;
}
ret = UNIX_ERR( pthread_mutex_unlock( &blio->inputMutex ) );
if( ret )
return ret;
#else
Pa_Sleep( PA_MAC_BLIO_BUSY_WAIT_SLEEP_INTERVAL );
#endif
}
} while( framesAvailable == 0 );
framesToTransfer = (ring_buffer_size_t) MIN( framesAvailable, framesRequested );
framesTransferred = PaUtil_ReadRingBuffer( &blio->inputRingBuffer, (void *)cbuf, framesToTransfer );
cbuf += framesTransferred * blio->inputSampleSizeActual * blio->inChan;
framesRequested -= framesTransferred;
if( framesToTransfer == framesAvailable ) {
#ifdef PA_MAC_BLIO_MUTEX
ret = blioSetIsInputEmpty( blio, true );
if( ret )
return ret;
if( PaUtil_GetRingBufferReadAvailable( &blio->inputRingBuffer ) ) {
blioSetIsInputEmpty( blio, false );
if( ret )
return ret;
}
#endif
}
}
ret = blio->statusFlags & paInputOverflow;
if( ret ) {
OSAtomicAnd32( (uint32_t)(~paInputOverflow), &blio->statusFlags );
ret = paInputOverflowed;
}
return ret;
}
PaError WriteStream( PaStream* stream,
const void *buffer,
unsigned long framesRequested )
{
PaMacCoreStream *macStream = (PaMacCoreStream*)stream;
PaMacBlio *blio = &macStream->blio;
char *cbuf = (char *) buffer;
PaError ret = paNoError;
VVDBUG(("WriteStream()\n"));
while( framesRequested > 0 && macStream->state != STOPPING ) {
ring_buffer_size_t framesAvailable;
ring_buffer_size_t framesToTransfer;
ring_buffer_size_t framesTransferred;
do {
framesAvailable = PaUtil_GetRingBufferWriteAvailable( &blio->outputRingBuffer );
if( framesAvailable == 0 ) {
#ifdef PA_MAC_BLIO_MUTEX
ret = UNIX_ERR( pthread_mutex_lock( &blio->outputMutex ) );
if( ret )
return ret;
while( blio->isOutputFull ) {
ret = UNIX_ERR( pthread_cond_wait( &blio->outputCond, &blio->outputMutex ) );
if( ret )
return ret;
}
ret = UNIX_ERR( pthread_mutex_unlock( &blio->outputMutex ) );
if( ret )
return ret;
#else
Pa_Sleep( PA_MAC_BLIO_BUSY_WAIT_SLEEP_INTERVAL );
#endif
}
} while( framesAvailable == 0 && macStream->state != STOPPING );
if( macStream->state == STOPPING )
{
break;
}
framesToTransfer = MIN( framesAvailable, framesRequested );
framesTransferred = PaUtil_WriteRingBuffer( &blio->outputRingBuffer, (void *)cbuf, framesToTransfer );
cbuf += framesTransferred * blio->outputSampleSizeActual * blio->outChan;
framesRequested -= framesTransferred;
#ifdef PA_MAC_BLIO_MUTEX
if( framesToTransfer == framesAvailable ) {
ret = blioSetIsOutputFull( blio, true );
if( ret )
return ret;
if( PaUtil_GetRingBufferWriteAvailable( &blio->outputRingBuffer ) ) {
blioSetIsOutputFull( blio, false );
if( ret )
return ret;
}
}
#endif
}
if ( macStream->state == STOPPING )
{
ret = paInternalError;
}
else if (ret == paNoError )
{
ret = blio->statusFlags & paOutputUnderflow;
if( ret )
{
OSAtomicAnd32( (uint32_t)(~paOutputUnderflow), &blio->statusFlags );
ret = paOutputUnderflowed;
}
}
return ret;
}
PaError waitUntilBlioWriteBufferIsEmpty( PaMacBlio *blio, double sampleRate,
size_t framesPerBuffer )
{
PaError result = paNoError;
if( blio->outputRingBuffer.buffer ) {
ring_buffer_size_t framesLeft = PaUtil_GetRingBufferReadAvailable( &blio->outputRingBuffer );
PaTime now = PaUtil_GetTime();
PaTime startTime = now;
PaTime timeoutTime = startTime + (framesLeft + (2 * framesPerBuffer)) / sampleRate;
long msecPerBuffer = 1 + (long)( 1000.0 * framesPerBuffer / sampleRate);
while( framesLeft > 0 && now < timeoutTime ) {
VDBUG(( "waitUntilBlioWriteBufferIsFlushed: framesLeft = %d, framesPerBuffer = %ld\n",
framesLeft, framesPerBuffer ));
Pa_Sleep( msecPerBuffer );
framesLeft = PaUtil_GetRingBufferReadAvailable( &blio->outputRingBuffer );
now = PaUtil_GetTime();
}
if( framesLeft > 0 )
{
VDBUG(( "waitUntilBlioWriteBufferIsFlushed: TIMED OUT - framesLeft = %d\n", framesLeft ));
result = paTimedOut;
}
}
return result;
}
signed long GetStreamReadAvailable( PaStream* stream )
{
PaMacBlio *blio = & ((PaMacCoreStream*)stream) -> blio;
VVDBUG(("GetStreamReadAvailable()\n"));
return PaUtil_GetRingBufferReadAvailable( &blio->inputRingBuffer );
}
signed long GetStreamWriteAvailable( PaStream* stream )
{
PaMacBlio *blio = & ((PaMacCoreStream*)stream) -> blio;
VVDBUG(("GetStreamWriteAvailable()\n"));
return PaUtil_GetRingBufferWriteAvailable( &blio->outputRingBuffer );
}
