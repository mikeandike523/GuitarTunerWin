
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "qa_tools.h"
#include "audio_analyzer.h"
#include "test_audio_analyzer.h"
#include "write_wav.h"
#include "biquad_filter.h"
#define FRAMES_PER_BLOCK  (64)
#define PRINT_REPORTS  0
#define TEST_SAVED_WAVE  (0)
static int TestSingleMonoTone( void )
{
int result = 0;
PaQaSineGenerator generator;
PaQaRecording     recording;
float buffer[FRAMES_PER_BLOCK];
double sampleRate = 44100.0;
int maxFrames = ((int)sampleRate) * 1;
int samplesPerFrame = 1;
int stride = 1;
int done = 0;
double freq = 234.5;
double amp = 0.5;
double mag1, mag2;
PaQa_SetupSineGenerator( &generator, freq, amp, sampleRate );
result = PaQa_InitializeRecording( &recording, maxFrames, (int) sampleRate );
QA_ASSERT_EQUALS( "PaQa_InitializeRecording failed", 0, result );
done = 0;
while (!done)
{
PaQa_EraseBuffer( buffer, FRAMES_PER_BLOCK, samplesPerFrame );
PaQa_MixSine( &generator, buffer, FRAMES_PER_BLOCK, stride );
done = PaQa_WriteRecording( &recording, buffer, FRAMES_PER_BLOCK, samplesPerFrame );
}
mag1 = PaQa_CorrelateSine( &recording, freq, sampleRate, 0, recording.numFrames, NULL );
QA_ASSERT_CLOSE( "exact frequency match", amp, mag1, 0.01 );
mag2 = PaQa_CorrelateSine( &recording, freq * 1.23, sampleRate, 0, recording.numFrames, NULL );
QA_ASSERT_CLOSE( "wrong frequency", 0.0, mag2, 0.01 );
PaQa_TerminateRecording( &recording );
return 0;
error:
PaQa_TerminateRecording( &recording);
return 1;
}
static int TestMixedMonoTones( void )
{
int i;
int result = 0;
#define NUM_TONES (5)
PaQaSineGenerator generators[NUM_TONES];
PaQaRecording     recording;
float buffer[FRAMES_PER_BLOCK];
double sampleRate = 44100.0;
int maxFrames = ((int)sampleRate) * 1;
int samplesPerFrame = 1;
double baseFreq = 234.5;
double amp = 0.1;
double mag2;
int stride = samplesPerFrame;
int done = 0;
for( i=0; i<NUM_TONES; i++ )
{
PaQa_SetupSineGenerator( &generators[i], PaQa_GetNthFrequency( baseFreq, i ), amp, sampleRate );
}
result = PaQa_InitializeRecording( &recording, maxFrames, (int) sampleRate );
QA_ASSERT_EQUALS( "PaQa_InitializeRecording failed", 0, result );
done = 0;
while (!done)
{
PaQa_EraseBuffer( buffer, FRAMES_PER_BLOCK, samplesPerFrame );
for( i=0; i<NUM_TONES; i++ )
{
PaQa_MixSine( &generators[i], buffer, FRAMES_PER_BLOCK, stride );
}
done = PaQa_WriteRecording( &recording, buffer, FRAMES_PER_BLOCK, samplesPerFrame );
}
for( i=0; i<NUM_TONES; i++ )
{
double mag = PaQa_CorrelateSine( &recording, PaQa_GetNthFrequency( baseFreq, i), sampleRate, 0, recording.numFrames, NULL );
QA_ASSERT_CLOSE( "exact frequency match", amp, mag, 0.01 );
}
mag2 = PaQa_CorrelateSine( &recording, baseFreq * 0.87, sampleRate, 0, recording.numFrames, NULL );
QA_ASSERT_CLOSE( "wrong frequency", 0.0, mag2, 0.01 );
PaQa_TerminateRecording( &recording );
return 0;
error:
PaQa_TerminateRecording( &recording);
return 1;
}
static void MakeRecordingWithAddedFrames( PaQaRecording *recording, PaQaTestTone *testTone, int glitchPosition, int framesToAdd )
{
PaQaSineGenerator generator;
#define BUFFER_SIZE 512
float buffer[BUFFER_SIZE];
int frameCounter = testTone->startDelay;
int stride = 1;
int done = PaQa_WriteSilence( recording, testTone->startDelay );
PaQa_SetupSineGenerator( &generator, testTone->frequency, testTone->amplitude, testTone->sampleRate );
while (!done)
{
int framesThisLoop = BUFFER_SIZE;
if( frameCounter == glitchPosition )
{
if( framesToAdd > 0 )
{
done = PaQa_RecordFreeze( recording, framesToAdd );
frameCounter += framesToAdd;
}
else if( framesToAdd < 0 )
{
PaQa_MixSine( &generator, buffer, 0 - framesToAdd, stride );
}
}
else if( (frameCounter < glitchPosition) && ((frameCounter + framesThisLoop) > glitchPosition) )
{
framesThisLoop = glitchPosition - frameCounter;
}
if( framesThisLoop > 0 )
{
PaQa_EraseBuffer( buffer, framesThisLoop, testTone->samplesPerFrame );
PaQa_MixSine( &generator, buffer, framesThisLoop, stride );
done = PaQa_WriteRecording( recording, buffer, framesThisLoop, testTone->samplesPerFrame );
}
frameCounter += framesThisLoop;
}
}
static void MakeCleanRecording( PaQaRecording *recording, PaQaTestTone *testTone )
{
PaQaSineGenerator generator;
#define BUFFER_SIZE 512
float buffer[BUFFER_SIZE];
int stride = 1;
int done = PaQa_WriteSilence( recording, testTone->startDelay );
PaQa_SetupSineGenerator( &generator, testTone->frequency, testTone->amplitude, testTone->sampleRate );
while (!done)
{
PaQa_EraseBuffer( buffer, BUFFER_SIZE, testTone->samplesPerFrame );
PaQa_MixSine( &generator, buffer, BUFFER_SIZE, stride );
done = PaQa_WriteRecording( recording, buffer, BUFFER_SIZE, testTone->samplesPerFrame );
}
}
static void MakeRecordingWithPop( PaQaRecording *recording, PaQaTestTone *testTone, int popPosition, int popWidth, double popAmplitude )
{
int i;
MakeCleanRecording( recording, testTone );
if( (popPosition + popWidth) >= recording->numFrames )
{
popWidth = (recording->numFrames - popPosition) - 1;
}
for( i=0; i<popWidth; i++ )
{
float good = recording->buffer[i+popPosition];
float bad = (good > 0.0) ? (good - popAmplitude) : (good + popAmplitude);
recording->buffer[i+popPosition] = bad;
}
}
static int TestDetectSinglePhaseError( double sampleRate, int cycleSize, int latencyFrames, int glitchPosition, int framesAdded )
{
int result = 0;
PaQaRecording     recording;
PaQaTestTone testTone;
PaQaAnalysisResult analysisResult = { 0.0 };
int framesDropped = 0;
int maxFrames = ((int)sampleRate) * 2;
testTone.samplesPerFrame = 1;
testTone.sampleRate = sampleRate;
testTone.frequency = sampleRate / cycleSize;
testTone.amplitude = 0.5;
testTone.startDelay = latencyFrames;
result = PaQa_InitializeRecording( &recording, maxFrames, (int) sampleRate );
QA_ASSERT_EQUALS( "PaQa_InitializeRecording failed", 0, result );
MakeRecordingWithAddedFrames( &recording, &testTone, glitchPosition, framesAdded );
PaQa_AnalyseRecording( &recording, &testTone, &analysisResult );
if( framesAdded < 0 )
{
framesDropped = -framesAdded;
framesAdded = 0;
}
#if PRINT_REPORTS
printf("\n=== Dropped Frame Analysis ===================\n");
printf("                        expected      actual\n");
printf("             latency: %10.3f  %10.3f\n", (double)latencyFrames, analysisResult.latency );
printf("    num added frames: %10.3f  %10.3f\n", (double)framesAdded, analysisResult.numAddedFrames );
printf("     added frames at: %10.3f  %10.3f\n", (double)glitchPosition, analysisResult.addedFramesPosition );
printf("  num dropped frames: %10.3f  %10.3f\n", (double)framesDropped, analysisResult.numDroppedFrames );
printf("   dropped frames at: %10.3f  %10.3f\n", (double)glitchPosition, analysisResult.droppedFramesPosition );
#endif
QA_ASSERT_CLOSE( "PaQa_AnalyseRecording latency", latencyFrames, analysisResult.latency, 0.5 );
QA_ASSERT_CLOSE( "PaQa_AnalyseRecording framesAdded", framesAdded, analysisResult.numAddedFrames, 1.0 );
QA_ASSERT_CLOSE( "PaQa_AnalyseRecording framesDropped", framesDropped, analysisResult.numDroppedFrames, 1.0 );
PaQa_TerminateRecording( &recording );
return 0;
error:
PaQa_TerminateRecording( &recording);
return 1;
}
static int TestDetectPhaseErrors( void )
{
int result;
result = TestDetectSinglePhaseError( 44100, 200, 477, -1, 0 );
if( result < 0 ) return result;
return 0;
}
static int TestDetectSinglePop( double sampleRate, int cycleSize, int latencyFrames, int popPosition, int popWidth, double popAmplitude )
{
int result = 0;
PaQaRecording     recording;
PaQaTestTone testTone;
PaQaAnalysisResult analysisResult = { 0.0 };
int maxFrames = ((int)sampleRate) * 2;
testTone.samplesPerFrame = 1;
testTone.sampleRate = sampleRate;
testTone.frequency = sampleRate / cycleSize;
testTone.amplitude = 0.5;
testTone.startDelay = latencyFrames;
result = PaQa_InitializeRecording( &recording, maxFrames, (int) sampleRate );
QA_ASSERT_EQUALS( "PaQa_InitializeRecording failed", 0, result );
MakeRecordingWithPop( &recording, &testTone, popPosition, popWidth, popAmplitude );
PaQa_AnalyseRecording( &recording, &testTone, &analysisResult );
#if PRINT_REPORTS
printf("\n=== Pop Analysis ===================\n");
printf("                        expected      actual\n");
printf("             latency: %10.3f  %10.3f\n", (double)latencyFrames, analysisResult.latency );
printf("         popPosition: %10.3f  %10.3f\n", (double)popPosition, analysisResult.popPosition );
printf("        popAmplitude: %10.3f  %10.3f\n", popAmplitude, analysisResult.popAmplitude );
printf("           cycleSize: %6d\n", cycleSize );
printf("    num added frames: %10.3f\n", analysisResult.numAddedFrames );
printf("     added frames at: %10.3f\n", analysisResult.addedFramesPosition );
printf("  num dropped frames: %10.3f\n", analysisResult.numDroppedFrames );
printf("   dropped frames at: %10.3f\n", analysisResult.droppedFramesPosition );
#endif
QA_ASSERT_CLOSE( "PaQa_AnalyseRecording latency", latencyFrames, analysisResult.latency, 0.5 );
QA_ASSERT_CLOSE( "PaQa_AnalyseRecording popPosition", popPosition, analysisResult.popPosition, 10 );
if( popWidth > 0 )
{
QA_ASSERT_CLOSE( "PaQa_AnalyseRecording popAmplitude", popAmplitude, analysisResult.popAmplitude, 0.1 * popAmplitude  );
}
PaQa_TerminateRecording( &recording );
return 0;
error:
PaQa_SaveRecordingToWaveFile( &recording, "bad_recording.wav" );
PaQa_TerminateRecording( &recording);
return 1;
}
static int TestSingleInitialSpike( double sampleRate, int stepPosition, int cycleSize, int latencyFrames, double stepAmplitude )
{
int i;
int result = 0;
int expectedLatency = latencyFrames + 1;
PaQaRecording     recording;
PaQaRecording     hipassOutput = { 0 };
BiquadFilter      hipassFilter;
PaQaTestTone testTone;
PaQaAnalysisResult analysisResult = { 0.0 };
int maxFrames = ((int)sampleRate) * 2;
testTone.samplesPerFrame = 1;
testTone.sampleRate = sampleRate;
testTone.frequency = sampleRate / cycleSize;
testTone.amplitude = -0.5;
testTone.startDelay = latencyFrames;
result = PaQa_InitializeRecording( &recording, maxFrames, (int) sampleRate );
QA_ASSERT_EQUALS( "PaQa_InitializeRecording failed", 0, result );
result = PaQa_InitializeRecording( &hipassOutput, maxFrames, (int) sampleRate );
QA_ASSERT_EQUALS( "PaQa_InitializeRecording failed", 0, result );
MakeCleanRecording( &recording, &testTone );
for( i=stepPosition; i<recording.numFrames; i++ )
{
recording.buffer[i] += stepAmplitude;
}
BiquadFilter_SetupHighPass( &hipassFilter, 10.0 / sampleRate, 0.5 );
PaQa_FilterRecording( &recording, &hipassOutput, &hipassFilter );
testTone.amplitude = 0.5;
PaQa_AnalyseRecording( &hipassOutput, &testTone, &analysisResult );
#if PRINT_REPORTS
printf("\n=== InitialSpike Analysis ===================\n");
printf("                        expected      actual\n");
printf("             latency: %10.3f  %10.3f\n", (double)expectedLatency, analysisResult.latency );
printf("         popPosition: %10.3f\n", analysisResult.popPosition );
printf("        popAmplitude: %10.3f\n", analysisResult.popAmplitude );
printf("      amplitudeRatio: %10.3f\n", analysisResult.amplitudeRatio );
printf("           cycleSize: %6d\n", cycleSize );
printf("    num added frames: %10.3f\n", analysisResult.numAddedFrames );
printf("     added frames at: %10.3f\n", analysisResult.addedFramesPosition );
printf("  num dropped frames: %10.3f\n", analysisResult.numDroppedFrames );
printf("   dropped frames at: %10.3f\n", analysisResult.droppedFramesPosition );
#endif
QA_ASSERT_CLOSE( "PaQa_AnalyseRecording latency", expectedLatency, analysisResult.latency, 4.0 );
QA_ASSERT_EQUALS( "PaQa_AnalyseRecording no pop from step", -1, (int) analysisResult.popPosition );
PaQa_TerminateRecording( &recording );
PaQa_TerminateRecording( &hipassOutput );
return 0;
error:
PaQa_SaveRecordingToWaveFile( &recording, "bad_step_original.wav" );
PaQa_SaveRecordingToWaveFile( &hipassOutput, "bad_step_hipass.wav" );
PaQa_TerminateRecording( &recording);
PaQa_TerminateRecording( &hipassOutput );
return 1;
}
static int TestDetectPops( void )
{
int result;
result = TestDetectSinglePop( 44100, 200, 477, -1, 0, 0.0 );
if( result < 0 ) return result;
result = TestDetectSinglePop( 44100, 300, 810, 3987, 1, 0.5 );
if( result < 0 ) return result;
result = TestDetectSinglePop( 44100, 300, 810, 9876, 5, 0.5 );
if( result < 0 ) return result;
result = TestDetectSinglePop( 44100, 250, 810, 5672, 1, 0.05 );
if( result < 0 ) return result;
return 0;
}
static int TestInitialSpike( void )
{
int result;
result = TestSingleInitialSpike( 44100, 32, 100, 537, 0.0 );
if( result < 0 ) return result;
result = TestSingleInitialSpike( 44100, 32, 100, 537, 0.1 );
if( result < 0 ) return result;
result = TestSingleInitialSpike( 8000, 32, 42, 2000, 0.1 );
if( result < 0 ) return result;
result = TestSingleInitialSpike( 44100, 40, 190, 3000, 0.5 );
if( result < 0 ) return result;
if( result < 0 ) return result;
return 0;
}
#if TEST_SAVED_WAVE
static int TestSavedWave()
{
int i,j;
WAV_Writer writer;
int result = 0;
#define NUM_SAMPLES  (200)
short data[NUM_SAMPLES];
short saw = 0;
result =  Audio_WAV_OpenWriter( &writer, "test_sawtooth.wav", 44100, 1 );
if( result < 0 ) goto error;
for( i=0; i<15; i++ )
{
for( j=0; j<NUM_SAMPLES; j++ )
{
data[j] = saw;
saw += 293;
}
result =  Audio_WAV_WriteShorts( &writer, data, NUM_SAMPLES );
if( result < 0 ) goto error;
}
result =  Audio_WAV_CloseWriter( &writer );
if( result < 0 ) goto error;
return 0;
error:
printf("ERROR: result = %d\n", result );
return result;
}
#endif  
void PaQa_FillWithSine( PaQaRecording *recording, double sampleRate, double freq, double amp )
{
PaQaSineGenerator generator;
float buffer[FRAMES_PER_BLOCK];
int samplesPerFrame = 1;
int stride = 1;
int done = 0;
PaQa_SetupSineGenerator( &generator, freq, amp, sampleRate );
done = 0;
while (!done)
{
PaQa_EraseBuffer( buffer, FRAMES_PER_BLOCK, samplesPerFrame );
PaQa_MixSine( &generator, buffer, FRAMES_PER_BLOCK, stride );
done = PaQa_WriteRecording( recording, buffer, FRAMES_PER_BLOCK, samplesPerFrame );
}
}
static int TestNotchFilter( void )
{
int result = 0;
PaQaRecording     original = { 0 };
PaQaRecording     filtered = { 0 };
BiquadFilter      notchFilter;
double sampleRate = 44100.0;
int maxFrames = ((int)sampleRate) * 1;
double freq = 234.5;
double amp = 0.5;
double mag1, mag2, mag3;
result = PaQa_InitializeRecording( &original, maxFrames, (int) sampleRate );
QA_ASSERT_EQUALS( "PaQa_InitializeRecording failed", 0, result );
PaQa_FillWithSine( &original, sampleRate, freq, amp );
mag1 = PaQa_CorrelateSine( &original, freq, sampleRate, 0, original.numFrames, NULL );
QA_ASSERT_CLOSE( "exact frequency match", amp, mag1, 0.01 );
result = PaQa_InitializeRecording( &filtered, maxFrames, (int) sampleRate );
QA_ASSERT_EQUALS( "PaQa_InitializeRecording failed", 0, result );
BiquadFilter_SetupNotch( &notchFilter, freq / sampleRate, 0.5 );
PaQa_FilterRecording( &original, &filtered, &notchFilter );
result = PaQa_SaveRecordingToWaveFile( &filtered, "filtered1.wav" );
QA_ASSERT_EQUALS( "PaQa_SaveRecordingToWaveFile failed", 0, result );
mag2 = PaQa_CorrelateSine( &filtered, freq, sampleRate, 0, filtered.numFrames, NULL );
QA_ASSERT_CLOSE( "should eliminate tone", 0.0, mag2, 0.01 );
BiquadFilter_SetupNotch( &notchFilter, 1.07 * freq / sampleRate, 2.0 );
PaQa_FilterRecording( &original, &filtered, &notchFilter );
mag3 = PaQa_CorrelateSine( &filtered, freq, sampleRate, 0, filtered.numFrames, NULL );
QA_ASSERT_CLOSE( "should eliminate tone", amp*0.26, mag3, 0.01 );
PaQa_TerminateRecording( &original );
PaQa_TerminateRecording( &filtered );
return 0;
error:
PaQa_TerminateRecording( &original);
PaQa_TerminateRecording( &filtered );
return 1;
}
int PaQa_TestAnalyzer( void )
{
int result;
#if TEST_SAVED_WAVE
if ((result = TestSavedWave()) != 0) return result;
#endif  
if ((result = TestSingleMonoTone()) != 0) return result;
if ((result = TestMixedMonoTones()) != 0) return result;
if ((result = TestDetectPhaseErrors()) != 0) return result;
if ((result = TestNotchFilter()) != 0) return result;
if ((result = TestDetectPops()) != 0) return result;
if ((result = TestInitialSpike()) != 0) return result;
return 0;
}
