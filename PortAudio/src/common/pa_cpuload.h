#ifndef PA_CPULOAD_H
#define PA_CPULOAD_H
#ifdef __cplusplus
extern "C"
{
#endif  
typedef struct {
double samplingPeriod;
double measurementStartTime;
double averageLoad;
} PaUtilCpuLoadMeasurer;  
void PaUtil_InitializeCpuLoadMeasurer( PaUtilCpuLoadMeasurer* measurer, double sampleRate );
void PaUtil_BeginCpuLoadMeasurement( PaUtilCpuLoadMeasurer* measurer );
void PaUtil_EndCpuLoadMeasurement( PaUtilCpuLoadMeasurer* measurer, unsigned long framesProcessed );
void PaUtil_ResetCpuLoadMeasurer( PaUtilCpuLoadMeasurer* measurer );
double PaUtil_GetCpuLoad( PaUtilCpuLoadMeasurer* measurer );
#ifdef __cplusplus
}
#endif  
#endif  
