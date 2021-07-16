 
#include <stdio.h>
#include <string.h>
#include "asiosys.h"
#include "asio.h"
#include "asiodrivers.h"
#if WINDOWS
	#define ASIO_DRIVER_NAME    "ASIO Sample"
#elif MAC
	#define ASIO_DRIVER_NAME   	"ASIO Sample"
#endif
#define TEST_RUN_TIME  20.0		 
enum {
	 
	 
	kMaxInputChannels = 32,
	kMaxOutputChannels = 32
};
typedef struct DriverInfo
{
	 
	ASIODriverInfo driverInfo;
	 
	long           inputChannels;
	long           outputChannels;
	 
	long           minSize;
	long           maxSize;
	long           preferredSize;
	long           granularity;
	 
	ASIOSampleRate sampleRate;
	 
	bool           postOutput;
	 
	long           inputLatency;
	long           outputLatency;
	 
	long inputBuffers;	 
	long outputBuffers;	 
	ASIOBufferInfo bufferInfos[kMaxInputChannels + kMaxOutputChannels];  
	 
	ASIOChannelInfo channelInfos[kMaxInputChannels + kMaxOutputChannels];  
	 
	 
	 
	double         nanoSeconds;
	double         samples;
	double         tcSamples;	 
	 
	ASIOTime       tInfo;			 
	unsigned long  sysRefTime;       
	 
	bool           stopped;
} DriverInfo;
DriverInfo asioDriverInfo = {0};
ASIOCallbacks asioCallbacks;
extern AsioDrivers* asioDrivers;
bool loadAsioDriver(char *name);
int main(int argc, char* argv[]);
long init_asio_static_data (DriverInfo *asioDriverInfo);
ASIOError create_asio_buffers (DriverInfo *asioDriverInfo);
unsigned long get_sys_reference_time();
void bufferSwitch(long index, ASIOBool processNow);
ASIOTime *bufferSwitchTimeInfo(ASIOTime *timeInfo, long index, ASIOBool processNow);
void sampleRateChanged(ASIOSampleRate sRate);
long asioMessages(long selector, long value, void* message, double* opt);
long init_asio_static_data (DriverInfo *asioDriverInfo)
{	 
	 
	if(ASIOGetChannels(&asioDriverInfo->inputChannels, &asioDriverInfo->outputChannels) == ASE_OK)
	{
		printf ("ASIOGetChannels (inputs: %d, outputs: %d);\n", asioDriverInfo->inputChannels, asioDriverInfo->outputChannels);
		 
		if(ASIOGetBufferSize(&asioDriverInfo->minSize, &asioDriverInfo->maxSize, &asioDriverInfo->preferredSize, &asioDriverInfo->granularity) == ASE_OK)
		{
			printf ("ASIOGetBufferSize (min: %d, max: %d, preferred: %d, granularity: %d);\n",
					 asioDriverInfo->minSize, asioDriverInfo->maxSize,
					 asioDriverInfo->preferredSize, asioDriverInfo->granularity);
			 
			if(ASIOGetSampleRate(&asioDriverInfo->sampleRate) == ASE_OK)
			{
				printf ("ASIOGetSampleRate (sampleRate: %f);\n", asioDriverInfo->sampleRate);
				if (asioDriverInfo->sampleRate <= 0.0 || asioDriverInfo->sampleRate > 96000.0)
				{
					 
					 
					 
					if(ASIOSetSampleRate(44100.0) == ASE_OK)
					{
						if(ASIOGetSampleRate(&asioDriverInfo->sampleRate) == ASE_OK)
							printf ("ASIOGetSampleRate (sampleRate: %f);\n", asioDriverInfo->sampleRate);
						else
							return -6;
					}
					else
						return -5;
				}
				 
				 
				if(ASIOOutputReady() == ASE_OK)
					asioDriverInfo->postOutput = true;
				else
					asioDriverInfo->postOutput = false;
				printf ("ASIOOutputReady(); - %s\n", asioDriverInfo->postOutput ? "Supported" : "Not supported");
				return 0;
			}
			return -3;
		}
		return -2;
	}
	return -1;
}
#if NATIVE_INT64
	#define ASIO64toDouble(a)  (a)
#else
	const double twoRaisedTo32 = 4294967296.;
	#define ASIO64toDouble(a)  ((a).lo + (a).hi * twoRaisedTo32)
#endif
ASIOTime *bufferSwitchTimeInfo(ASIOTime *timeInfo, long index, ASIOBool processNow)
{	 
	 
	 
	static long processedSamples = 0;
	 
	asioDriverInfo.tInfo = *timeInfo;
	 
	 
	if (timeInfo->timeInfo.flags & kSystemTimeValid)
		asioDriverInfo.nanoSeconds = ASIO64toDouble(timeInfo->timeInfo.systemTime);
	else
		asioDriverInfo.nanoSeconds = 0;
	if (timeInfo->timeInfo.flags & kSamplePositionValid)
		asioDriverInfo.samples = ASIO64toDouble(timeInfo->timeInfo.samplePosition);
	else
		asioDriverInfo.samples = 0;
	if (timeInfo->timeCode.flags & kTcValid)
		asioDriverInfo.tcSamples = ASIO64toDouble(timeInfo->timeCode.timeCodeSamples);
	else
		asioDriverInfo.tcSamples = 0;
	 
	asioDriverInfo.sysRefTime = get_sys_reference_time();
#if WINDOWS && _DEBUG
	 
	 
	 
	static double last_samples = 0;
	char tmp[128];
	sprintf (tmp, "diff: %d / %d ms / %d ms / %d samples                 \n", asioDriverInfo.sysRefTime - (long)(asioDriverInfo.nanoSeconds / 1000000.0), asioDriverInfo.sysRefTime, (long)(asioDriverInfo.nanoSeconds / 1000000.0), (long)(asioDriverInfo.samples - last_samples));
	OutputDebugString (tmp);
	last_samples = asioDriverInfo.samples;
#endif
	 
	long buffSize = asioDriverInfo.preferredSize;
	 
	for (int i = 0; i < asioDriverInfo.inputBuffers + asioDriverInfo.outputBuffers; i++)
	{
		if (asioDriverInfo.bufferInfos[i].isInput == false)
		{
			 
			switch (asioDriverInfo.channelInfos[i].type)
			{
			case ASIOSTInt16LSB:
				memset (asioDriverInfo.bufferInfos[i].buffers[index], 0, buffSize * 2);
				break;
			case ASIOSTInt24LSB:		 
				memset (asioDriverInfo.bufferInfos[i].buffers[index], 0, buffSize * 3);
				break;
			case ASIOSTInt32LSB:
				memset (asioDriverInfo.bufferInfos[i].buffers[index], 0, buffSize * 4);
				break;
			case ASIOSTFloat32LSB:		 
				memset (asioDriverInfo.bufferInfos[i].buffers[index], 0, buffSize * 4);
				break;
			case ASIOSTFloat64LSB: 		 
				memset (asioDriverInfo.bufferInfos[i].buffers[index], 0, buffSize * 8);
				break;
				 
				 
			case ASIOSTInt32LSB16:		 
			case ASIOSTInt32LSB18:		 
			case ASIOSTInt32LSB20:		 
			case ASIOSTInt32LSB24:		 
				memset (asioDriverInfo.bufferInfos[i].buffers[index], 0, buffSize * 4);
				break;
			case ASIOSTInt16MSB:
				memset (asioDriverInfo.bufferInfos[i].buffers[index], 0, buffSize * 2);
				break;
			case ASIOSTInt24MSB:		 
				memset (asioDriverInfo.bufferInfos[i].buffers[index], 0, buffSize * 3);
				break;
			case ASIOSTInt32MSB:
				memset (asioDriverInfo.bufferInfos[i].buffers[index], 0, buffSize * 4);
				break;
			case ASIOSTFloat32MSB:		 
				memset (asioDriverInfo.bufferInfos[i].buffers[index], 0, buffSize * 4);
				break;
			case ASIOSTFloat64MSB: 		 
				memset (asioDriverInfo.bufferInfos[i].buffers[index], 0, buffSize * 8);
				break;
				 
				 
			case ASIOSTInt32MSB16:		 
			case ASIOSTInt32MSB18:		 
			case ASIOSTInt32MSB20:		 
			case ASIOSTInt32MSB24:		 
				memset (asioDriverInfo.bufferInfos[i].buffers[index], 0, buffSize * 4);
				break;
			}
		}
	}
	 
	if (asioDriverInfo.postOutput)
		ASIOOutputReady();
	if (processedSamples >= asioDriverInfo.sampleRate * TEST_RUN_TIME)	 
		asioDriverInfo.stopped = true;
	else
		processedSamples += buffSize;
	return 0L;
}
void bufferSwitch(long index, ASIOBool processNow)
{	 
	 
	 
	 
	 
	ASIOTime  timeInfo;
	memset (&timeInfo, 0, sizeof (timeInfo));
	 
	 
	if(ASIOGetSamplePosition(&timeInfo.timeInfo.samplePosition, &timeInfo.timeInfo.systemTime) == ASE_OK)
		timeInfo.timeInfo.flags = kSystemTimeValid | kSamplePositionValid;
	bufferSwitchTimeInfo (&timeInfo, index, processNow);
}
void sampleRateChanged(ASIOSampleRate sRate)
{
	 
	 
	 
	 
	 
	 
}
long asioMessages(long selector, long value, void* message, double* opt)
{
	 
	long ret = 0;
	switch(selector)
	{
		case kAsioSelectorSupported:
			if(value == kAsioResetRequest
			|| value == kAsioEngineVersion
			|| value == kAsioResyncRequest
			|| value == kAsioLatenciesChanged
			 
			|| value == kAsioSupportsTimeInfo
			|| value == kAsioSupportsTimeCode
			|| value == kAsioSupportsInputMonitor)
				ret = 1L;
			break;
		case kAsioResetRequest:
			 
			 
			 
			 
			asioDriverInfo.stopped;   
			ret = 1L;
			break;
		case kAsioResyncRequest:
			 
			 
			 
			 
			 
			 
			ret = 1L;
			break;
		case kAsioLatenciesChanged:
			 
			 
			 
			ret = 1L;
			break;
		case kAsioEngineVersion:
			 
			 
			 
			ret = 2L;
			break;
		case kAsioSupportsTimeInfo:
			 
			 
			 
			 
			ret = 1;
			break;
		case kAsioSupportsTimeCode:
			 
			 
			 
			ret = 0;
			break;
	}
	return ret;
}
ASIOError create_asio_buffers (DriverInfo *asioDriverInfo)
{	 
	 
	long i;
	ASIOError result;
	 
	ASIOBufferInfo *info = asioDriverInfo->bufferInfos;
	 
	if (asioDriverInfo->inputChannels > kMaxInputChannels)
		asioDriverInfo->inputBuffers = kMaxInputChannels;
	else
		asioDriverInfo->inputBuffers = asioDriverInfo->inputChannels;
	for(i = 0; i < asioDriverInfo->inputBuffers; i++, info++)
	{
		info->isInput = ASIOTrue;
		info->channelNum = i;
		info->buffers[0] = info->buffers[1] = 0;
	}
	 
	if (asioDriverInfo->outputChannels > kMaxOutputChannels)
		asioDriverInfo->outputBuffers = kMaxOutputChannels;
	else
		asioDriverInfo->outputBuffers = asioDriverInfo->outputChannels;
	for(i = 0; i < asioDriverInfo->outputBuffers; i++, info++)
	{
		info->isInput = ASIOFalse;
		info->channelNum = i;
		info->buffers[0] = info->buffers[1] = 0;
	}
	 
	result = ASIOCreateBuffers(asioDriverInfo->bufferInfos,
		asioDriverInfo->inputBuffers + asioDriverInfo->outputBuffers,
		asioDriverInfo->preferredSize, &asioCallbacks);
	if (result == ASE_OK)
	{
		 
		for (i = 0; i < asioDriverInfo->inputBuffers + asioDriverInfo->outputBuffers; i++)
		{
			asioDriverInfo->channelInfos[i].channel = asioDriverInfo->bufferInfos[i].channelNum;
			asioDriverInfo->channelInfos[i].isInput = asioDriverInfo->bufferInfos[i].isInput;
			result = ASIOGetChannelInfo(&asioDriverInfo->channelInfos[i]);
			if (result != ASE_OK)
				break;
		}
		if (result == ASE_OK)
		{
			 
			 
			 
			 
			result = ASIOGetLatencies(&asioDriverInfo->inputLatency, &asioDriverInfo->outputLatency);
			if (result == ASE_OK)
				printf ("ASIOGetLatencies (input: %d, output: %d);\n", asioDriverInfo->inputLatency, asioDriverInfo->outputLatency);
		}
	}
	return result;
}
int main(int argc, char* argv[])
{
	 
	if (loadAsioDriver (ASIO_DRIVER_NAME))
	{
		 
		if (ASIOInit (&asioDriverInfo.driverInfo) == ASE_OK)
		{
			printf ("asioVersion:   %d\n"
					"driverVersion: %d\n"
					"Name:          %s\n"
					"ErrorMessage:  %s\n",
					asioDriverInfo.driverInfo.asioVersion, asioDriverInfo.driverInfo.driverVersion,
					asioDriverInfo.driverInfo.name, asioDriverInfo.driverInfo.errorMessage);
			if (init_asio_static_data (&asioDriverInfo) == 0)
			{
				 
				 
				asioCallbacks.bufferSwitch = &bufferSwitch;
				asioCallbacks.sampleRateDidChange = &sampleRateChanged;
				asioCallbacks.asioMessage = &asioMessages;
				asioCallbacks.bufferSwitchTimeInfo = &bufferSwitchTimeInfo;
				if (create_asio_buffers (&asioDriverInfo) == ASE_OK)
				{
					if (ASIOStart() == ASE_OK)
					{
						 
						fprintf (stdout, "\nASIO Driver started succefully.\n\n");
						while (!asioDriverInfo.stopped)
						{
#if WINDOWS
							Sleep(100);	 
#elif MAC
							unsigned long dummy;
							Delay (6, &dummy);
#endif
							fprintf (stdout, "%d ms / %d ms / %d samples", asioDriverInfo.sysRefTime, (long)(asioDriverInfo.nanoSeconds / 1000000.0), (long)asioDriverInfo.samples);
							 
							double remainder = asioDriverInfo.tcSamples;
							long hours = (long)(remainder / (asioDriverInfo.sampleRate * 3600));
							remainder -= hours * asioDriverInfo.sampleRate * 3600;
							long minutes = (long)(remainder / (asioDriverInfo.sampleRate * 60));
							remainder -= minutes * asioDriverInfo.sampleRate * 60;
							long seconds = (long)(remainder / asioDriverInfo.sampleRate);
							remainder -= seconds * asioDriverInfo.sampleRate;
							fprintf (stdout, " / TC: %2.2d:%2.2d:%2.2d:%5.5d", (long)hours, (long)minutes, (long)seconds, (long)remainder);
							fprintf (stdout, "     \r");
							#if !MAC
							fflush (stdout);
							#endif
						}
						ASIOStop();
					}
					ASIODisposeBuffers();
				}
			}
			ASIOExit();
		}
		asioDrivers->removeCurrentDriver();
	}
	return 0;
}
unsigned long get_sys_reference_time()
{	 
#if WINDOWS
	return timeGetTime();
#elif MAC
static const double twoRaisedTo32 = 4294967296.;
	UnsignedWide ys;
	Microseconds(&ys);
	double r = ((double)ys.hi * twoRaisedTo32 + (double)ys.lo);
	return (unsigned long)(r / 1000.);
#endif
}
