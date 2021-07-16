 
#ifndef __ASIO_H
#define __ASIO_H
#if defined(_MSC_VER) && !defined(__MWERKS__) 
#pragma pack(push,4)
#elif PRAGMA_ALIGN_SUPPORTED
#pragma options align = native
#endif
#if NATIVE_INT64
	typedef long long int ASIOSamples;
#else
	typedef struct ASIOSamples {
		unsigned long hi;
		unsigned long lo;
	} ASIOSamples;
#endif
#if NATIVE_INT64
	typedef long long int ASIOTimeStamp ;
#else
	typedef struct ASIOTimeStamp {
		unsigned long hi;
		unsigned long lo;
	} ASIOTimeStamp;
#endif
#if IEEE754_64FLOAT
	typedef double ASIOSampleRate;
#else
	typedef struct ASIOSampleRate {
		char ieee[8];
	} ASIOSampleRate;
#endif
typedef long ASIOBool;
enum {
	ASIOFalse = 0,
	ASIOTrue = 1
};
typedef long ASIOSampleType;
enum {
	ASIOSTInt16MSB   = 0,
	ASIOSTInt24MSB   = 1,		 
	ASIOSTInt32MSB   = 2,
	ASIOSTFloat32MSB = 3,		 
	ASIOSTFloat64MSB = 4,		 
	 
	 
	ASIOSTInt32MSB16 = 8,		 
	ASIOSTInt32MSB18 = 9,		 
	ASIOSTInt32MSB20 = 10,		 
	ASIOSTInt32MSB24 = 11,		 
	
	ASIOSTInt16LSB   = 16,
	ASIOSTInt24LSB   = 17,		 
	ASIOSTInt32LSB   = 18,
	ASIOSTFloat32LSB = 19,		 
	ASIOSTFloat64LSB = 20, 		 
	 
	 
	ASIOSTInt32LSB16 = 24,		 
	ASIOSTInt32LSB18 = 25,		 
	ASIOSTInt32LSB20 = 26,		 
	ASIOSTInt32LSB24 = 27,		 
	 
	ASIOSTDSDInt8LSB1   = 32,		 
	ASIOSTDSDInt8MSB1   = 33,		 
	ASIOSTDSDInt8NER8	= 40,		 
	ASIOSTLastEntry
};
typedef long ASIOError;
enum {
	ASE_OK = 0,              
	ASE_SUCCESS = 0x3f4847a0,	 
	ASE_NotPresent = -1000,  
	ASE_HWMalfunction,       
	ASE_InvalidParameter,    
	ASE_InvalidMode,         
	ASE_SPNotAdvancing,      
	ASE_NoClock,             
	ASE_NoMemory             
};
typedef struct ASIOTimeCode
{       
	double          speed;                   
	                                         
	ASIOSamples     timeCodeSamples;         
	unsigned long   flags;                   
	char future[64];
} ASIOTimeCode;
typedef enum ASIOTimeCodeFlags
{
	kTcValid                = 1,
	kTcRunning              = 1 << 1,
	kTcReverse              = 1 << 2,
	kTcOnspeed              = 1 << 3,
	kTcStill                = 1 << 4,
	
	kTcSpeedValid           = 1 << 8
}  ASIOTimeCodeFlags;
typedef struct AsioTimeInfo
{
	double          speed;                   
	ASIOTimeStamp   systemTime;              
	                                         
	                                         
	ASIOSamples     samplePosition;
	ASIOSampleRate  sampleRate;              
	unsigned long flags;                     
	char reserved[12];
} AsioTimeInfo;
typedef enum AsioTimeInfoFlags
{
	kSystemTimeValid        = 1,             
	kSamplePositionValid    = 1 << 1,        
	kSampleRateValid        = 1 << 2,
	kSpeedValid             = 1 << 3,
	
	kSampleRateChanged      = 1 << 4,
	kClockSourceChanged     = 1 << 5
} AsioTimeInfoFlags;
typedef struct ASIOTime                           
{
	long reserved[4];                        
	struct AsioTimeInfo     timeInfo;        
	struct ASIOTimeCode     timeCode;        
} ASIOTime;
typedef struct ASIOCallbacks
{
	void (*bufferSwitch) (long doubleBufferIndex, ASIOBool directProcess);
		 
		 
		 
		 
		 
		 
		 
		 
		 
		 
		 
		 
		 
		 
		 
	void (*sampleRateDidChange) (ASIOSampleRate sRate);
		 
		 
		 
	long (*asioMessage) (long selector, long value, void* message, double* opt);
		 
		 
	ASIOTime* (*bufferSwitchTimeInfo) (ASIOTime* params, long doubleBufferIndex, ASIOBool directProcess);
		 
		 
		 
		 
} ASIOCallbacks;
enum
{
	kAsioSelectorSupported = 1,	 
								 
kAsioEngineVersion,			 
								 
	kAsioResetRequest,			 
								 
								 
								 
								 
								 
								 
								 
								 
								 
	kAsioBufferSizeChange,		 
								 
								 
								 
	kAsioResyncRequest,			 
								 
								 
								 
								 
	kAsioLatenciesChanged, 		 
								 
	kAsioSupportsTimeInfo,		 
								 
								 
	kAsioSupportsTimeCode,		 
	kAsioMMCCommand,			 
	kAsioSupportsInputMonitor,	 
	kAsioSupportsInputGain,      
	kAsioSupportsInputMeter,     
	kAsioSupportsOutputGain,     
	kAsioSupportsOutputMeter,    
	kAsioOverload,               
	kAsioNumMessageSelectors
};
typedef struct ASIODriverInfo
{
	long asioVersion;		 
	long driverVersion;		 
	char name[32];
	char errorMessage[124];
	void *sysRef;			 
							 
} ASIODriverInfo;
ASIOError ASIOInit(ASIODriverInfo *info);
ASIOError ASIOExit(void);
ASIOError ASIOStart(void);
ASIOError ASIOStop(void);
ASIOError ASIOGetChannels(long *numInputChannels, long *numOutputChannels);
ASIOError ASIOGetLatencies(long *inputLatency, long *outputLatency);
ASIOError ASIOGetBufferSize(long *minSize, long *maxSize, long *preferredSize, long *granularity);
ASIOError ASIOCanSampleRate(ASIOSampleRate sampleRate);
ASIOError ASIOGetSampleRate(ASIOSampleRate *currentRate);
ASIOError ASIOSetSampleRate(ASIOSampleRate sampleRate);
typedef struct ASIOClockSource
{
	long index;					 
	long associatedChannel;		 
	long associatedGroup;		 
	ASIOBool isCurrentSource;	 
	char name[32];				 
} ASIOClockSource;
ASIOError ASIOGetClockSources(ASIOClockSource *clocks, long *numSources);
ASIOError ASIOSetClockSource(long index);
ASIOError ASIOGetSamplePosition (ASIOSamples *sPos, ASIOTimeStamp *tStamp);
typedef struct ASIOChannelInfo
{
	long channel;			 
	ASIOBool isInput;		 
	ASIOBool isActive;		 
	long channelGroup;		 
	ASIOSampleType type;	 
	char name[32];			 
} ASIOChannelInfo;
ASIOError ASIOGetChannelInfo(ASIOChannelInfo *info);
typedef struct ASIOBufferInfo
{
	ASIOBool isInput;			 
	long channelNum;			 
	void *buffers[2];			 
} ASIOBufferInfo;
ASIOError ASIOCreateBuffers(ASIOBufferInfo *bufferInfos, long numChannels,
	long bufferSize, ASIOCallbacks *callbacks);
ASIOError ASIODisposeBuffers(void);
ASIOError ASIOControlPanel(void);
ASIOError ASIOFuture(long selector, void *params);
enum
{
	kAsioEnableTimeCodeRead = 1,	 
	kAsioDisableTimeCodeRead,		 
	kAsioSetInputMonitor,			 
	kAsioTransport,					 
	kAsioSetInputGain,				 
	kAsioGetInputMeter,				 
	kAsioSetOutputGain,				 
	kAsioGetOutputMeter,			 
	kAsioCanInputMonitor,			 
	kAsioCanTimeInfo,
	kAsioCanTimeCode,
	kAsioCanTransport,
	kAsioCanInputGain,
	kAsioCanInputMeter,
	kAsioCanOutputGain,
	kAsioCanOutputMeter,
	kAsioOptionalOne,
	
	 
	 
	 
	kAsioSetIoFormat			= 0x23111961,		 
	kAsioGetIoFormat			= 0x23111983,		 
	kAsioCanDoIoFormat			= 0x23112004,		 
	
	 
	kAsioCanReportOverload			= 0x24042012,	 
	
	kAsioGetInternalBufferSamples	= 0x25042012	 
};
typedef struct ASIOInputMonitor
{
	long input;		 
	long output;	 
	long gain;		 
	ASIOBool state;	 
	long pan;		 
} ASIOInputMonitor;
typedef struct ASIOChannelControls
{
	long channel;			 
	ASIOBool isInput;		 
	long gain;				 
	long meter;				 
	char future[32];
} ASIOChannelControls;
typedef struct ASIOTransportParameters
{
	long command;		 
	ASIOSamples samplePosition;
	long track;
	long trackSwitches[16];		 
	char future[64];
} ASIOTransportParameters;
enum
{
	kTransStart = 1,
	kTransStop,
	kTransLocate,		 
	kTransPunchIn,
	kTransPunchOut,
	kTransArmOn,		 
	kTransArmOff,		 
	kTransMonitorOn,	 
	kTransMonitorOff,	 
	kTransArm,			 
	kTransMonitor		 
};
typedef long int ASIOIoFormatType;
enum ASIOIoFormatType_e
{
	kASIOFormatInvalid = -1,
	kASIOPCMFormat = 0,
	kASIODSDFormat = 1,
};
typedef struct ASIOIoFormat_s
{
	ASIOIoFormatType	FormatType;
	char				future[512-sizeof(ASIOIoFormatType)];
} ASIOIoFormat;
typedef struct ASIOInternalBufferInfo
{
	long inputSamples;			 
	long outputSamples;			 
} ASIOInternalBufferInfo;
ASIOError ASIOOutputReady(void);
#if defined(_MSC_VER) && !defined(__MWERKS__) 
#pragma pack(pop)
#elif PRAGMA_ALIGN_SUPPORTED
#pragma options align = reset
#endif
#endif
