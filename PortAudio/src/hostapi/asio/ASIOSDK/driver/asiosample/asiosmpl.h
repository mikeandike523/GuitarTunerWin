 
#ifndef _asiosmpl_
#define _asiosmpl_
#include "asiosys.h"
#define TESTWAVES 1
enum
{
	kBlockFrames = 256,
	kNumInputs = 16,
	kNumOutputs = 16
};
#if WINDOWS
#include "rpc.h"
#include "rpcndr.h"
#ifndef COM_NO_WINDOWS_H
#include <windows.h>
#include "ole2.h"
#endif
#include "combase.h"
#include "iasiodrv.h"
class AsioSample : public IASIO, public CUnknown
{
public:
	AsioSample(LPUNKNOWN pUnk, HRESULT *phr);
	~AsioSample();
	DECLARE_IUNKNOWN
	 
	static CUnknown *CreateInstance(LPUNKNOWN pUnk, HRESULT *phr);
	 
	virtual HRESULT STDMETHODCALLTYPE NonDelegatingQueryInterface(REFIID riid,void **ppvObject);
#else
#include "asiodrvr.h"
class AsioSample : public AsioDriver
{
public:
	AsioSample ();
	~AsioSample ();
#endif
	ASIOBool init (void* sysRef);
	void getDriverName (char *name);	 
	long getDriverVersion ();
	void getErrorMessage (char *string);	 
	ASIOError start ();
	ASIOError stop ();
	ASIOError getChannels (long *numInputChannels, long *numOutputChannels);
	ASIOError getLatencies (long *inputLatency, long *outputLatency);
	ASIOError getBufferSize (long *minSize, long *maxSize,
		long *preferredSize, long *granularity);
	ASIOError canSampleRate (ASIOSampleRate sampleRate);
	ASIOError getSampleRate (ASIOSampleRate *sampleRate);
	ASIOError setSampleRate (ASIOSampleRate sampleRate);
	ASIOError getClockSources (ASIOClockSource *clocks, long *numSources);
	ASIOError setClockSource (long index);
	ASIOError getSamplePosition (ASIOSamples *sPos, ASIOTimeStamp *tStamp);
	ASIOError getChannelInfo (ASIOChannelInfo *info);
	ASIOError createBuffers (ASIOBufferInfo *bufferInfos, long numChannels,
		long bufferSize, ASIOCallbacks *callbacks);
	ASIOError disposeBuffers ();
	ASIOError controlPanel ();
	ASIOError future (long selector, void *opt);
	ASIOError outputReady ();
	void bufferSwitch ();
	long getMilliSeconds () {return milliSeconds;}
private:
friend void myTimer();
	bool inputOpen ();
#if TESTWAVES
	void makeSine (short *wave);
	void makeSaw (short *wave);
#endif
	void inputClose ();
	void input ();
	bool outputOpen ();
	void outputClose ();
	void output ();
	void timerOn ();
	void timerOff ();
	void bufferSwitchX ();
	double samplePosition;
	double sampleRate;
	ASIOCallbacks *callbacks;
	ASIOTime asioTime;
	ASIOTimeStamp theSystemTime;
	short *inputBuffers[kNumInputs * 2];
	short *outputBuffers[kNumOutputs * 2];
#if TESTWAVES
	short *sineWave, *sawTooth;
#endif
	long inMap[kNumInputs];
	long outMap[kNumOutputs];
	long blockFrames;
	long inputLatency;
	long outputLatency;
	long activeInputs;
	long activeOutputs;
	long toggle;
	long milliSeconds;
	bool active, started;
	bool timeInfoMode, tcRead;
	char errorMessage[128];
};
#endif
