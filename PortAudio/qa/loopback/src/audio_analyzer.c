
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "qa_tools.h"
#include "audio_analyzer.h"
#include "write_wav.h"
#define PAQA_POP_THRESHOLD  (0.04)
double PaQa_GetNthFrequency( double baseFrequency, int index )
{
return baseFrequency * pow( 2.0, index / 13.0 );
}
void PaQa_EraseBuffer( float *buffer, int numFrames, int samplesPerFrame )
{
int i;
int numSamples = numFrames * samplesPerFrame;
for( i=0; i<numSamples; i++ )
{
*buffer++ = 0.0;
}
}
void PaQa_SetupSineGenerator( PaQaSineGenerator *generator, double frequency, double amplitude, double frameRate )
{
generator->phase = 0.0;
generator->amplitude = amplitude;
generator->frequency = frequency;
generator->phaseIncrement = 2.0 * frequency * MATH_PI / frameRate;
}
void PaQa_MixSine( PaQaSineGenerator *generator, float *buffer, int numSamples, int stride )
{
int i;
for( i=0; i<numSamples; i++ )
{
float value = sinf( (float) generator->phase ) * generator->amplitude;
*buffer += value;  
buffer += stride;
generator->phase += generator->phaseIncrement;
if (generator->phase > MATH_TWO_PI)
{
generator->phase -= MATH_TWO_PI;
}
}
}
void PaQa_GenerateCrackDISABLED( float *buffer, int numSamples, int stride )
{
int i;
int offset = numSamples/2;
for( i=0; i<numSamples; i++ )
{
float phase = (MATH_TWO_PI * 0.5 * (i - offset)) / numSamples;
float cosp = cosf( phase );
float cos2 = cosp * cosp;
float value = (i < offset) ? cos2 : (0-cos2);
*buffer = value;
buffer += stride;
}
}
int PaQa_InitializeRecording( PaQaRecording *recording, int maxFrames, int frameRate )
{
int numBytes = maxFrames * sizeof(float);
recording->buffer = (float*)malloc(numBytes);
QA_ASSERT_TRUE( "Allocate recording buffer.", (recording->buffer != NULL) );
recording->maxFrames = maxFrames;    recording->sampleRate = frameRate;
recording->numFrames = 0;
return 0;
error:
return 1;
}
void PaQa_TerminateRecording( PaQaRecording *recording )
{
if (recording->buffer != NULL)
{
free( recording->buffer );
recording->buffer = NULL;
}
recording->maxFrames = 0;
}
int PaQa_WriteRecording( PaQaRecording *recording, float *buffer, int numFrames, int stride )
{
int i;
int framesToWrite;
float *data = &recording->buffer[recording->numFrames];
framesToWrite = numFrames;
if ((framesToWrite + recording->numFrames) > recording->maxFrames)
{
framesToWrite = recording->maxFrames - recording->numFrames;
}
for( i=0; i<framesToWrite; i++ )
{
*data++ = *buffer;
buffer += stride;
}
recording->numFrames += framesToWrite;
return (recording->numFrames >= recording->maxFrames);
}
int PaQa_WriteSilence( PaQaRecording *recording, int numFrames )
{
int i;
int framesToRecord;
float *data = &recording->buffer[recording->numFrames];
framesToRecord = numFrames;
if ((framesToRecord + recording->numFrames) > recording->maxFrames)
{
framesToRecord = recording->maxFrames - recording->numFrames;
}
for( i=0; i<framesToRecord; i++ )
{
*data++ = 0.0f;
}
recording->numFrames += framesToRecord;
return (recording->numFrames >= recording->maxFrames);
}
int PaQa_RecordFreeze( PaQaRecording *recording, int numFrames )
{
int i;
int framesToRecord;
float *data = &recording->buffer[recording->numFrames];
framesToRecord = numFrames;
if ((framesToRecord + recording->numFrames) > recording->maxFrames)
{
framesToRecord = recording->maxFrames - recording->numFrames;
}
for( i=0; i<framesToRecord; i++ )
{
data[i] = data[i-1];
}
recording->numFrames += framesToRecord;
return (recording->numFrames >= recording->maxFrames);
}
int PaQa_SaveRecordingToWaveFile( PaQaRecording *recording, const char *filename )
{
WAV_Writer writer;
int result = 0;
#define NUM_SAMPLES  (200)
short data[NUM_SAMPLES];
const int samplesPerFrame = 1;
int numLeft = recording->numFrames;
float *buffer = &recording->buffer[0];
result =  Audio_WAV_OpenWriter( &writer, filename, recording->sampleRate, samplesPerFrame );
if( result < 0 ) goto error;
while( numLeft > 0 )
{
int i;
int numToSave = (numLeft > NUM_SAMPLES) ? NUM_SAMPLES : numLeft;
for( i=0; i<numToSave; i++ )
{
double fval = *buffer++;
int ival = fval * 32768.0;
if( ival > 32767 ) ival = 32767;
else if( ival < -32768 ) ival = -32768;
data[i] = ival;
}
result =  Audio_WAV_WriteShorts( &writer, data, numToSave );
if( result < 0 ) goto error;
numLeft -= numToSave;
}
result =  Audio_WAV_CloseWriter( &writer );
if( result < 0 ) goto error;
return 0;
error:
printf("ERROR: result = %d\n", result );
return result;
#undef NUM_SAMPLES
}
double PaQa_MeasureCrossingSlope( float *buffer, int numFrames )
{
int i;
double slopeTotal = 0.0;
int slopeCount = 0;
float previous;
double averageSlope = 0.0;
previous = buffer[0];
for( i=1; i<numFrames; i++ )
{
float current = buffer[i];
if( (current > 0.0) && (previous < 0.0) )
{
double delta = current - previous;
slopeTotal += delta;
slopeCount += 1;
}
previous = current;
}
if( slopeCount > 0 )
{
averageSlope = slopeTotal / slopeCount;
}
return averageSlope;
}
double PaQa_MeasureSineAmplitudeBySlope( PaQaRecording *recording,
double frequency, double frameRate,
int startFrame, int numFrames )
{
float *buffer = &recording->buffer[startFrame];
double measuredSlope = PaQa_MeasureCrossingSlope( buffer, numFrames );
double unitySlope = sin( MATH_TWO_PI * frequency / frameRate );
double estimatedAmplitude = measuredSlope / unitySlope;
return estimatedAmplitude;
}
double PaQa_CorrelateSine( PaQaRecording *recording, double frequency, double frameRate,
int startFrame, int numFrames, double *phasePtr )
{
double magnitude = 0.0;
int numLeft = numFrames;
double phase = 0.0;
double phaseIncrement = 2.0 * MATH_PI * frequency / frameRate;
double sinAccumulator = 0.0;
double cosAccumulator = 0.0;
float *data = &recording->buffer[startFrame];
QA_ASSERT_TRUE( "startFrame out of bounds", (startFrame < recording->numFrames) );
QA_ASSERT_TRUE( "numFrames out of bounds", ((startFrame+numFrames) <= recording->numFrames) );
while( numLeft > 0 )
{
double sample = (double) *data++;
sinAccumulator += sample * sin( phase );
cosAccumulator += sample * cos( phase );
phase += phaseIncrement;
if (phase > MATH_TWO_PI)
{
phase -= MATH_TWO_PI;
}
numLeft -= 1;
}
sinAccumulator = sinAccumulator / numFrames;
cosAccumulator = cosAccumulator / numFrames;
magnitude = 2.0 * sqrt( (sinAccumulator * sinAccumulator) + (cosAccumulator * cosAccumulator ));
if( phasePtr != NULL )
{
double phase = atan2( cosAccumulator, sinAccumulator );
*phasePtr = phase;
}
return magnitude;
error:
return -1.0;
}
void PaQa_FilterRecording( PaQaRecording *input, PaQaRecording *output, BiquadFilter *filter )
{
int numToFilter = (input->numFrames > output->maxFrames) ? output->maxFrames : input->numFrames;
BiquadFilter_Filter( filter, &input->buffer[0], &output->buffer[0], numToFilter );
output->numFrames = numToFilter;
}
double PaQa_FindFirstMatch( PaQaRecording *recording, float *buffer, int numFrames, double threshold  )
{
int ic,is;
int maxCorrelations = recording->numFrames - numFrames;
double maxSum = 0.0;
int peakIndex = -1;
double inverseMaxSum = 0.0;
int inversePeakIndex = -1;
double location = -1.0;
QA_ASSERT_TRUE( "numFrames out of bounds", (numFrames < recording->numFrames) );
for( ic=0; ic<maxCorrelations; ic++ )
{
int pastPeak;
int inversePastPeak;
double sum = 0.0;
float *recorded = &recording->buffer[ ic ];
for( is=0; is<numFrames; is++ )
{
float s1 = buffer[is];
float s2 = *recorded++;
sum += s1 * s2;
}
if( (sum > maxSum) )
{
maxSum = sum;
peakIndex = ic;
}
if( ((-sum) > inverseMaxSum) )
{
inverseMaxSum = -sum;
inversePeakIndex = ic;
}
pastPeak = (maxSum > threshold) && (sum < 0.5*maxSum);
inversePastPeak = (inverseMaxSum > threshold) && ((-sum) < 0.5*inverseMaxSum);
if( pastPeak && inversePastPeak )
{
if( maxSum > inverseMaxSum )
{
location = peakIndex;
}
else
{
location = inversePeakIndex;
}
break;
}
}
return location;
error:
return -1.0;
}
double PaQa_MeasureArea( float *buffer, int numFrames, int stride  )
{
int is;
double area = 0.0;
for( is=0; is<numFrames; is++ )
{
area += fabs( *buffer );
buffer += stride;
}
return area;
}
double PaQa_MeasureRootMeanSquare( float *buffer, int numFrames )
{
int is;
double area = 0.0;
double root;
for( is=0; is<numFrames; is++ )
{
float value = *buffer++;
area += value * value;
}
root = sqrt( area );
return root / numFrames;
}
double PaQa_CompareAmplitudes( PaQaRecording *recording, int startAt, float *buffer, int numFrames )
{
QA_ASSERT_TRUE( "startAt+numFrames out of bounds", ((startAt+numFrames) < recording->numFrames) );
{
double recordedArea = PaQa_MeasureArea( &recording->buffer[startAt], numFrames, 1 );
double bufferArea = PaQa_MeasureArea( buffer, numFrames, 1 );
if( bufferArea == 0.0 ) return 100000000.0;
return recordedArea / bufferArea;
}
error:
return -1.0;
}
double PaQa_ComputePhaseDifference( double phase1, double phase2 )
{
double delta = phase1 - phase2;
while( delta > MATH_PI )
{
delta -= MATH_TWO_PI;
}
while( delta < -MATH_PI )
{
delta += MATH_TWO_PI;
}
return delta;
}
int PaQa_MeasureLatency( PaQaRecording *recording, PaQaTestTone *testTone, PaQaAnalysisResult *analysisResult )
{
double threshold;
PaQaSineGenerator generator;
#define MAX_BUFFER_SIZE 2048
float buffer[MAX_BUFFER_SIZE];
double period = testTone->sampleRate / testTone->frequency;
int cycleSize = (int) (period + 0.5);
analysisResult->latency = -1;
analysisResult->valid = (0);
QA_ASSERT_TRUE( "cycleSize out of bounds", (cycleSize < MAX_BUFFER_SIZE) );
PaQa_SetupSineGenerator( &generator, testTone->frequency, testTone->amplitude, testTone->sampleRate );
PaQa_EraseBuffer( buffer, cycleSize, testTone->samplesPerFrame );
PaQa_MixSine( &generator, buffer, cycleSize, testTone->samplesPerFrame );
threshold = cycleSize * 0.02;
analysisResult->latency = PaQa_FindFirstMatch( recording, buffer, cycleSize, threshold );
QA_ASSERT_TRUE( "Could not find the start of the signal.", (analysisResult->latency >= 0) );
analysisResult->amplitudeRatio = PaQa_CompareAmplitudes( recording, analysisResult->latency, buffer, cycleSize );
return 0;
error:
return -1;
}
void PaQa_FadeInRecording( PaQaRecording *recording, int startFrame, int count )
{
int is;
double phase = 0.5 * MATH_PI;
double phaseIncrement = 0.25 * 2.0 * MATH_PI / count;
assert( startFrame >= 0 );
assert( count > 0 );
for( is=0; is<startFrame; is++ )
{
recording->buffer[ is ] = 0.0f;
}
for( is=0; is<count; is++ )
{
double c = cos( phase );
double w = c * c;
float x = recording->buffer[ is + startFrame ];
float y = x * w;
recording->buffer[ is + startFrame ] = y;
phase += phaseIncrement;
}
}
int PaQa_DetectPop( PaQaRecording *recording, PaQaTestTone *testTone, PaQaAnalysisResult *analysisResult )
{
int result = 0;
int i;
double maxAmplitude;
int maxPosition;
PaQaRecording     notchOutput = { 0 };
BiquadFilter      notchFilter;
PaQaRecording     hipassOutput = { 0 };
BiquadFilter      hipassFilter;
int frameRate = (int) recording->sampleRate;
analysisResult->popPosition = -1;
analysisResult->popAmplitude = 0.0;
result = PaQa_InitializeRecording( &notchOutput, recording->numFrames, frameRate );
QA_ASSERT_EQUALS( "PaQa_InitializeRecording failed", 0, result );
result = PaQa_InitializeRecording( &hipassOutput, recording->numFrames, frameRate );
QA_ASSERT_EQUALS( "PaQa_InitializeRecording failed", 0, result );
BiquadFilter_SetupNotch( &notchFilter, testTone->frequency / frameRate, 0.5 );
PaQa_FilterRecording( recording, &notchOutput, &notchFilter );
PaQa_FadeInRecording( &notchOutput, (int) analysisResult->latency, 500 );
BiquadFilter_SetupHighPass( &hipassFilter, 2.0 * testTone->frequency / frameRate, 0.5 );
PaQa_FilterRecording( &notchOutput, &hipassOutput, &hipassFilter );
maxAmplitude = 0.0;
maxPosition = -1;
for( i=(int) analysisResult->latency; i<hipassOutput.numFrames; i++ )
{
float x = hipassOutput.buffer[i];
float mag = fabs( x );
if( mag > maxAmplitude )
{
maxAmplitude = mag;
maxPosition = i;
}
}
if( maxAmplitude > PAQA_POP_THRESHOLD )
{
analysisResult->popPosition = maxPosition;
analysisResult->popAmplitude = maxAmplitude;
}
PaQa_TerminateRecording( &notchOutput );
PaQa_TerminateRecording( &hipassOutput );
return 0;
error:
PaQa_TerminateRecording( &notchOutput );
PaQa_TerminateRecording( &hipassOutput );
return -1;
}
int PaQa_DetectPhaseError( PaQaRecording *recording, PaQaTestTone *testTone, PaQaAnalysisResult *analysisResult )
{
int i;
double period = testTone->sampleRate / testTone->frequency;
int cycleSize = (int) (period + 0.5);
double maxAddedFrames = 0.0;
double maxDroppedFrames = 0.0;
double previousPhase = 0.0;
double previousFrameError = 0;
int loopCount = 0;
int skip = cycleSize;
int windowSize = cycleSize;
analysisResult->numDroppedFrames = 0.0;
analysisResult->numAddedFrames = 0.0;
analysisResult->droppedFramesPosition = -1.0;
analysisResult->addedFramesPosition = -1.0;
for( i=analysisResult->latency; i<(recording->numFrames - windowSize); i += skip )
{
double expectedPhase = previousPhase + (skip * MATH_TWO_PI / period);
double expectedPhaseIncrement = PaQa_ComputePhaseDifference( expectedPhase, previousPhase );
double phase = 666.0;
double mag = PaQa_CorrelateSine( recording, testTone->frequency, testTone->sampleRate, i, windowSize, &phase );
if( (loopCount > 1) && (mag > 0.0) )
{
double phaseDelta = PaQa_ComputePhaseDifference( phase, previousPhase );
double phaseError = PaQa_ComputePhaseDifference( phaseDelta, expectedPhaseIncrement );
double frameError = period * phaseError / MATH_TWO_PI;
double consecutiveFrameError = frameError + previousFrameError;
if( consecutiveFrameError > 0.8 )
{
double droppedFrames = consecutiveFrameError;
if (droppedFrames > (maxDroppedFrames * 1.001))
{
analysisResult->numDroppedFrames = droppedFrames;
analysisResult->droppedFramesPosition = i + (windowSize/2);
maxDroppedFrames = droppedFrames;
}
}
else if( consecutiveFrameError < -0.8 )
{
double addedFrames = 0 - consecutiveFrameError;
if (addedFrames > (maxAddedFrames * 1.001))
{
analysisResult->numAddedFrames = addedFrames;
analysisResult->addedFramesPosition = i + (windowSize/2);
maxAddedFrames = addedFrames;
}
}
previousFrameError = frameError;
}
previousPhase = phase;
loopCount += 1;
}
return 0;
}
int PaQa_AnalyseRecording( PaQaRecording *recording, PaQaTestTone *testTone, PaQaAnalysisResult *analysisResult )
{
int result = 0;
memset( analysisResult, 0, sizeof(PaQaAnalysisResult) );
result = PaQa_MeasureLatency( recording, testTone, analysisResult );
QA_ASSERT_EQUALS( "latency measurement", 0, result );
if( (analysisResult->latency >= 0) && (analysisResult->amplitudeRatio > 0.1) )
{
analysisResult->valid = (1);
result = PaQa_DetectPop( recording, testTone, analysisResult );
QA_ASSERT_EQUALS( "detect pop", 0, result );
result = PaQa_DetectPhaseError( recording, testTone, analysisResult );
QA_ASSERT_EQUALS( "detect phase error", 0, result );
}
return 0;
error:
return -1;
}
