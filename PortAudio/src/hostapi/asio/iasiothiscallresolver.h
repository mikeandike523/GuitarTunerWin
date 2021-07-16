 
#ifndef included_iasiothiscallresolver_h
#define included_iasiothiscallresolver_h
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__)) && !defined(_WIN64)
#if !defined(_MSC_VER)
#if !defined(iasiothiscallresolver_sourcefile)
	#if !defined(__ASIO_H)
	#error iasiothiscallresolver.h must be included AFTER asio.h
	#endif
#endif
#include <windows.h>
#include <asiodrvr.h>  
class IASIOThiscallResolver : public IASIO {
private:
	IASIO* that_;  
	static IASIOThiscallResolver instance;  
	 
IASIOThiscallResolver();
	IASIOThiscallResolver(IASIO* that);
public:
virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppv);
virtual ULONG STDMETHODCALLTYPE AddRef();
virtual ULONG STDMETHODCALLTYPE Release();
	virtual ASIOBool init(void *sysHandle);
	virtual void getDriverName(char *name);
	virtual long getDriverVersion();
	virtual void getErrorMessage(char *string);
	virtual ASIOError start();
	virtual ASIOError stop();
	virtual ASIOError getChannels(long *numInputChannels, long *numOutputChannels);
	virtual ASIOError getLatencies(long *inputLatency, long *outputLatency);
	virtual ASIOError getBufferSize(long *minSize, long *maxSize, long *preferredSize, long *granularity);
	virtual ASIOError canSampleRate(ASIOSampleRate sampleRate);
	virtual ASIOError getSampleRate(ASIOSampleRate *sampleRate);
	virtual ASIOError setSampleRate(ASIOSampleRate sampleRate);
	virtual ASIOError getClockSources(ASIOClockSource *clocks, long *numSources);
	virtual ASIOError setClockSource(long reference);
	virtual ASIOError getSamplePosition(ASIOSamples *sPos, ASIOTimeStamp *tStamp);
	virtual ASIOError getChannelInfo(ASIOChannelInfo *info);
	virtual ASIOError createBuffers(ASIOBufferInfo *bufferInfos, long numChannels, long bufferSize, ASIOCallbacks *callbacks);
	virtual ASIOError disposeBuffers();
	virtual ASIOError controlPanel();
	virtual ASIOError future(long selector,void *opt);
	virtual ASIOError outputReady();
static ASIOError ASIOInit(ASIODriverInfo *info);  
};
#define ASIOInit(name) IASIOThiscallResolver::ASIOInit((name))
#endif  
#endif  
#endif  
