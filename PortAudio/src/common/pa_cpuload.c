 
#include "pa_cpuload.h"
#include <assert.h>
#include "pa_util.h"    
void PaUtil_InitializeCpuLoadMeasurer( PaUtilCpuLoadMeasurer* measurer, double sampleRate )
{
assert( sampleRate > 0 );
measurer->samplingPeriod = 1. / sampleRate;
measurer->averageLoad = 0.;
}
void PaUtil_ResetCpuLoadMeasurer( PaUtilCpuLoadMeasurer* measurer )
{
measurer->averageLoad = 0.;
}
void PaUtil_BeginCpuLoadMeasurement( PaUtilCpuLoadMeasurer* measurer )
{
measurer->measurementStartTime = PaUtil_GetTime();
}
void PaUtil_EndCpuLoadMeasurement( PaUtilCpuLoadMeasurer* measurer, unsigned long framesProcessed )
{
double measurementEndTime, secondsFor100Percent, measuredLoad;
if( framesProcessed > 0 ){
measurementEndTime = PaUtil_GetTime();
assert( framesProcessed > 0 );
secondsFor100Percent = framesProcessed * measurer->samplingPeriod;
measuredLoad = (measurementEndTime - measurer->measurementStartTime) / secondsFor100Percent;
#define LOWPASS_COEFFICIENT_0   (0.9)
#define LOWPASS_COEFFICIENT_1   (0.99999 - LOWPASS_COEFFICIENT_0)
measurer->averageLoad = (LOWPASS_COEFFICIENT_0 * measurer->averageLoad) +
(LOWPASS_COEFFICIENT_1 * measuredLoad);
}
}
double PaUtil_GetCpuLoad( PaUtilCpuLoadMeasurer* measurer )
{
return measurer->averageLoad;
}
