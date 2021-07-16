
#ifndef _AUDIO_ANALYZER_H
#define _AUDIO_ANALYZER_H
#include "biquad_filter.h"
#define MATH_PI  (3.141592653589793238462643)
#define MATH_TWO_PI  (2.0 * MATH_PI)
typedef struct PaQaSineGenerator_s
{
double    phase;
double    phaseIncrement;
double    frequency;
double    amplitude;
} PaQaSineGenerator;
typedef struct PaQaRecording_s
{
int       maxFrames;
float    *buffer;
int       numFrames;
int       sampleRate;
} PaQaRecording;
typedef struct PaQaTestTone_s
{
int       samplesPerFrame;
int       startDelay;
double    sampleRate;
double    frequency;
double    amplitude;
} PaQaTestTone;
typedef struct PaQaAnalysisResult_s
{
int       valid;
double    latency;
double    amplitudeRatio;
double    popAmplitude;
double    popPosition;
double    numDroppedFrames;
double    droppedFramesPosition;
double    numAddedFrames;
double    addedFramesPosition;
} PaQaAnalysisResult;
double PaQa_GetNthFrequency( double baseFrequency, int index );
void PaQa_EraseBuffer( float *buffer, int numFrames, int samplesPerFrame );
void PaQa_MixSine( PaQaSineGenerator *generator, float *buffer, int numSamples, int stride );
void PaQa_WriteSine( float *buffer, int numSamples, int stride,
double frequency, double amplitude );
void PaQa_GenerateCrack( float *buffer, int numSamples, int stride );
double PaQa_ComputePhaseDifference( double phase1, double phase2 );
double PaQa_MeasureArea( float *buffer, int numFrames, int stride  );
double PaQa_MeasureCrossingSlope( float *buffer, int numFrames );
void PaQa_SetupSineGenerator( PaQaSineGenerator *generator, double frequency, double amplitude, double frameRate );
int PaQa_InitializeRecording( PaQaRecording *recording, int maxSamples, int sampleRate );
void PaQa_TerminateRecording( PaQaRecording *recording );
void PaQa_FilterRecording( PaQaRecording *input, PaQaRecording *output, BiquadFilter *filter );
int PaQa_SaveRecordingToWaveFile( PaQaRecording *recording, const char *filename );
int PaQa_WriteRecording( PaQaRecording *recording, float *buffer, int numSamples, int stride );
int PaQa_WriteSilence( PaQaRecording *recording, int numSamples );
int PaQa_RecordFreeze( PaQaRecording *recording, int numSamples );
double PaQa_CorrelateSine( PaQaRecording *recording, double frequency, double frameRate,
int startFrame, int numSamples, double *phasePtr );
double PaQa_FindFirstMatch( PaQaRecording *recording, float *buffer, int numSamples, double tolerance  );
double PaQa_MeasureSineAmplitudeBySlope( PaQaRecording *recording,
double frequency, double frameRate,
int startFrame, int numFrames );
double PaQa_MeasureRootMeanSquare( float *buffer, int numFrames );
double PaQa_CompareAmplitudes( PaQaRecording *recording, int startAt, float *buffer, int numSamples );
int PaQa_AnalyseRecording( PaQaRecording *recording, PaQaTestTone *testTone, PaQaAnalysisResult *analysisResult );
#endif  
