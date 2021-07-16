#ifndef PA_CONVERTERS_H
#define PA_CONVERTERS_H
#include "portaudio.h"   
#ifdef __cplusplus
extern "C"
{
#endif  
struct PaUtilTriangularDitherGenerator;
PaSampleFormat PaUtil_SelectClosestAvailableFormat(
PaSampleFormat availableFormats, PaSampleFormat format );
typedef void PaUtilConverter(
void *destinationBuffer, signed int destinationStride,
void *sourceBuffer, signed int sourceStride,
unsigned int count, struct PaUtilTriangularDitherGenerator *ditherGenerator );
PaUtilConverter* PaUtil_SelectConverter( PaSampleFormat sourceFormat,
PaSampleFormat destinationFormat, PaStreamFlags flags );
typedef void PaUtilZeroer(
void *destinationBuffer, signed int destinationStride, unsigned int count );
PaUtilZeroer* PaUtil_SelectZeroer( PaSampleFormat destinationFormat );
typedef struct{
PaUtilConverter *Float32_To_Int32;
PaUtilConverter *Float32_To_Int32_Dither;
PaUtilConverter *Float32_To_Int32_Clip;
PaUtilConverter *Float32_To_Int32_DitherClip;
PaUtilConverter *Float32_To_Int24;
PaUtilConverter *Float32_To_Int24_Dither;
PaUtilConverter *Float32_To_Int24_Clip;
PaUtilConverter *Float32_To_Int24_DitherClip;
PaUtilConverter *Float32_To_Int16;
PaUtilConverter *Float32_To_Int16_Dither;
PaUtilConverter *Float32_To_Int16_Clip;
PaUtilConverter *Float32_To_Int16_DitherClip;
PaUtilConverter *Float32_To_Int8;
PaUtilConverter *Float32_To_Int8_Dither;
PaUtilConverter *Float32_To_Int8_Clip;
PaUtilConverter *Float32_To_Int8_DitherClip;
PaUtilConverter *Float32_To_UInt8;
PaUtilConverter *Float32_To_UInt8_Dither;
PaUtilConverter *Float32_To_UInt8_Clip;
PaUtilConverter *Float32_To_UInt8_DitherClip;
PaUtilConverter *Int32_To_Float32;
PaUtilConverter *Int32_To_Int24;
PaUtilConverter *Int32_To_Int24_Dither;
PaUtilConverter *Int32_To_Int16;
PaUtilConverter *Int32_To_Int16_Dither;
PaUtilConverter *Int32_To_Int8;
PaUtilConverter *Int32_To_Int8_Dither;
PaUtilConverter *Int32_To_UInt8;
PaUtilConverter *Int32_To_UInt8_Dither;
PaUtilConverter *Int24_To_Float32;
PaUtilConverter *Int24_To_Int32;
PaUtilConverter *Int24_To_Int16;
PaUtilConverter *Int24_To_Int16_Dither;
PaUtilConverter *Int24_To_Int8;
PaUtilConverter *Int24_To_Int8_Dither;
PaUtilConverter *Int24_To_UInt8;
PaUtilConverter *Int24_To_UInt8_Dither;
PaUtilConverter *Int16_To_Float32;
PaUtilConverter *Int16_To_Int32;
PaUtilConverter *Int16_To_Int24;
PaUtilConverter *Int16_To_Int8;
PaUtilConverter *Int16_To_Int8_Dither;
PaUtilConverter *Int16_To_UInt8;
PaUtilConverter *Int16_To_UInt8_Dither;
PaUtilConverter *Int8_To_Float32;
PaUtilConverter *Int8_To_Int32;
PaUtilConverter *Int8_To_Int24;
PaUtilConverter *Int8_To_Int16;
PaUtilConverter *Int8_To_UInt8;
PaUtilConverter *UInt8_To_Float32;
PaUtilConverter *UInt8_To_Int32;
PaUtilConverter *UInt8_To_Int24;
PaUtilConverter *UInt8_To_Int16;
PaUtilConverter *UInt8_To_Int8;
PaUtilConverter *Copy_8_To_8;        
PaUtilConverter *Copy_16_To_16;      
PaUtilConverter *Copy_24_To_24;      
PaUtilConverter *Copy_32_To_32;      
} PaUtilConverterTable;
extern PaUtilConverterTable paConverters;
typedef struct{
PaUtilZeroer *ZeroU8;  
PaUtilZeroer *Zero8;
PaUtilZeroer *Zero16;
PaUtilZeroer *Zero24;
PaUtilZeroer *Zero32;
} PaUtilZeroerTable;
extern PaUtilZeroerTable paZeroers;
#ifdef __cplusplus
}
#endif  
#endif  
