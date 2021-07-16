#ifndef PA_DITHER_H
#define PA_DITHER_H
#include "pa_types.h"
#ifdef __cplusplus
extern "C"
{
#endif  
typedef struct PaUtilTriangularDitherGenerator{
PaUint32 previous;
PaUint32 randSeed1;
PaUint32 randSeed2;
} PaUtilTriangularDitherGenerator;
void PaUtil_InitializeTriangularDitherState( PaUtilTriangularDitherGenerator *ditherState );
PaInt32 PaUtil_Generate16BitTriangularDither( PaUtilTriangularDitherGenerator *ditherState );
float PaUtil_GenerateFloatTriangularDither( PaUtilTriangularDitherGenerator *ditherState );
#ifdef __cplusplus
}
#endif  
#endif  
