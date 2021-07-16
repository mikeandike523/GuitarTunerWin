 
#include "pa_x86_plain_converters.h"
#include "pa_converters.h"
#include "pa_dither.h"
#if defined(_WIN64) || defined(_WIN32_WCE)
void PaUtil_InitializeX86PlainConverters( void )
{
}
#else
static const short fpuControlWord_ = 0x033F;  
static const double int32Scaler_ = 0x7FFFFFFF;
static const double ditheredInt32Scaler_ = 0x7FFFFFFE;
static const double int24Scaler_ = 0x7FFFFF;
static const double ditheredInt24Scaler_ = 0x7FFFFE;
static const double int16Scaler_ = 0x7FFF;
static const double ditheredInt16Scaler_ = 0x7FFE;
#define PA_DITHER_BITS_   (15)
#define PA_FLOAT_DITHER_SCALE_  (1.0F / ((1<<PA_DITHER_BITS_)-1))
static const float const_float_dither_scale_ = PA_FLOAT_DITHER_SCALE_;
#define PA_DITHER_SHIFT_  ((32 - PA_DITHER_BITS_) + 1)
static void Float32_To_Int32(
void *destinationBuffer, signed int destinationStride,
void *sourceBuffer, signed int sourceStride,
unsigned int count, PaUtilTriangularDitherGenerator *ditherGenerator )
{
short savedFpuControlWord;
(void) ditherGenerator;  
__asm{
mov     esi, sourceBuffer
mov     edx, 4                   
mov     eax, sourceStride
imul    eax, edx
mov     ecx, count
imul    ecx, eax
add     ecx, esi
mov     edi, destinationBuffer
mov     ebx, destinationStride
imul    ebx, edx
fwait
fstcw   savedFpuControlWord
fldcw   fpuControlWord_
fld     int32Scaler_             
Float32_To_Int32_loop:
fld     dword ptr [esi]          
add     esi, eax                 
fmul    st(0), st(1)             
fistp   dword ptr [edi]          
add     edi, ebx                 
cmp     esi, ecx                 
jne     Float32_To_Int32_loop
ffree   st(0)
fincstp
fwait
fnclex
fldcw   savedFpuControlWord
}
}
static void Float32_To_Int32_Clip(
void *destinationBuffer, signed int destinationStride,
void *sourceBuffer, signed int sourceStride,
unsigned int count, PaUtilTriangularDitherGenerator *ditherGenerator )
{
short savedFpuControlWord;
(void) ditherGenerator;  
__asm{
mov     esi, sourceBuffer
mov     edx, 4                   
mov     eax, sourceStride
imul    eax, edx
mov     ecx, count
imul    ecx, eax
add     ecx, esi
mov     edi, destinationBuffer
mov     ebx, destinationStride
imul    ebx, edx
fwait
fstcw   savedFpuControlWord
fldcw   fpuControlWord_
fld     int32Scaler_             
Float32_To_Int32_Clip_loop:
mov     edx, dword ptr [esi]     
and     edx, 0x7FFFFFFF          
cmp     edx, 0x3F800000          
jg      Float32_To_Int32_Clip_clamp
fld     dword ptr [esi]          
add     esi, eax                 
fmul    st(0), st(1)             
fistp   dword ptr [edi]          
jmp     Float32_To_Int32_Clip_stored
Float32_To_Int32_Clip_clamp:
mov     edx, dword ptr [esi]     
shr     edx, 31                  
add     esi, eax                 
add     edx, 0x7FFFFFFF          
mov     dword ptr [edi], edx
Float32_To_Int32_Clip_stored:
lea     edi, [edi+ebx]
cmp     esi, ecx                 
jne     Float32_To_Int32_Clip_loop
ffree   st(0)
fincstp
fwait
fnclex
fldcw   savedFpuControlWord
}
}
static void Float32_To_Int32_DitherClip(
void *destinationBuffer, signed int destinationStride,
void *sourceBuffer, signed int sourceStride,
unsigned int count, PaUtilTriangularDitherGenerator *ditherGenerator )
{
short savedFpuControlWord;
signed long sourceByteStride;
signed long highpassedDither;
unsigned long ditherPrevious = ditherGenerator->previous;
unsigned long ditherRandSeed1 = ditherGenerator->randSeed1;
unsigned long ditherRandSeed2 = ditherGenerator->randSeed2;
__asm{
mov     esi, sourceBuffer
mov     edx, 4                   
mov     eax, sourceStride
imul    eax, edx
mov     ecx, count
imul    ecx, eax
add     ecx, esi
mov     edi, destinationBuffer
mov     ebx, destinationStride
imul    ebx, edx
fwait
fstcw   savedFpuControlWord
fldcw   fpuControlWord_
fld     ditheredInt32Scaler_     
Float32_To_Int32_DitherClip_loop:
mov     edx, dword ptr [esi]     
and     edx, 0x7FFFFFFF          
cmp     edx, 0x3F800000          
jg      Float32_To_Int32_DitherClip_clamp
fld     dword ptr [esi]          
add     esi, eax                 
fmul    st(0), st(1)             
mov     sourceByteStride, eax    
mov     edx, 196314165
mov     eax, ditherRandSeed1
mul     edx                      
lea     eax, [eax+907633515]
mov     ditherRandSeed1, eax
mov     edx, 196314165
mov     eax, ditherRandSeed2
mul     edx                      
lea     eax, [eax+907633515]
mov     edx, ditherRandSeed1
shr     edx, PA_DITHER_SHIFT_
mov     ditherRandSeed2, eax
shr     eax, PA_DITHER_SHIFT_
lea     eax, [eax+edx]
mov     edx, ditherPrevious
neg     edx
lea     edx, [eax+edx]           
mov     highpassedDither, edx
mov     ditherPrevious, eax      
mov     eax, sourceByteStride    
fild    highpassedDither
fmul    const_float_dither_scale_
faddp   st(1), st(0)             
fistp   dword ptr [edi]          
jmp     Float32_To_Int32_DitherClip_stored
Float32_To_Int32_DitherClip_clamp:
mov     edx, dword ptr [esi]     
shr     edx, 31                  
add     esi, eax                 
add     edx, 0x7FFFFFFF          
mov     dword ptr [edi], edx
Float32_To_Int32_DitherClip_stored:
lea     edi, [edi+ebx]
cmp     esi, ecx                 
jne     Float32_To_Int32_DitherClip_loop
ffree   st(0)
fincstp
fwait
fnclex
fldcw   savedFpuControlWord
}
ditherGenerator->previous = ditherPrevious;
ditherGenerator->randSeed1 = ditherRandSeed1;
ditherGenerator->randSeed2 = ditherRandSeed2;
}
static void Float32_To_Int24(
void *destinationBuffer, signed int destinationStride,
void *sourceBuffer, signed int sourceStride,
unsigned int count, PaUtilTriangularDitherGenerator *ditherGenerator )
{
short savedFpuControlWord;
signed long tempInt32;
(void) ditherGenerator;  
__asm{
mov     esi, sourceBuffer
mov     edx, 4                   
mov     eax, sourceStride
imul    eax, edx
mov     ecx, count
imul    ecx, eax
add     ecx, esi
mov     edi, destinationBuffer
mov     edx, 3                   
mov     ebx, destinationStride
imul    ebx, edx
fwait
fstcw   savedFpuControlWord
fldcw   fpuControlWord_
fld     int24Scaler_             
Float32_To_Int24_loop:
fld     dword ptr [esi]          
add     esi, eax                 
fmul    st(0), st(1)             
fistp   tempInt32                
mov     edx, tempInt32
mov     byte ptr [edi], DL
shr     edx, 8
mov     word ptr [edi+1], DX
lea     edi, [edi+ebx]
cmp     esi, ecx                 
jne     Float32_To_Int24_loop
ffree   st(0)
fincstp
fwait
fnclex
fldcw   savedFpuControlWord
}
}
static void Float32_To_Int24_Clip(
void *destinationBuffer, signed int destinationStride,
void *sourceBuffer, signed int sourceStride,
unsigned int count, PaUtilTriangularDitherGenerator *ditherGenerator )
{
short savedFpuControlWord;
signed long tempInt32;
(void) ditherGenerator;  
__asm{
mov     esi, sourceBuffer
mov     edx, 4                   
mov     eax, sourceStride
imul    eax, edx
mov     ecx, count
imul    ecx, eax
add     ecx, esi
mov     edi, destinationBuffer
mov     edx, 3                   
mov     ebx, destinationStride
imul    ebx, edx
fwait
fstcw   savedFpuControlWord
fldcw   fpuControlWord_
fld     int24Scaler_             
Float32_To_Int24_Clip_loop:
mov     edx, dword ptr [esi]     
and     edx, 0x7FFFFFFF          
cmp     edx, 0x3F800000          
jg      Float32_To_Int24_Clip_clamp
fld     dword ptr [esi]          
add     esi, eax                 
fmul    st(0), st(1)             
fistp   tempInt32                
mov     edx, tempInt32
jmp     Float32_To_Int24_Clip_store
Float32_To_Int24_Clip_clamp:
mov     edx, dword ptr [esi]     
shr     edx, 31                  
add     esi, eax                 
add     edx, 0x7FFFFF            
Float32_To_Int24_Clip_store:
mov     byte ptr [edi], DL
shr     edx, 8
mov     word ptr [edi+1], DX
lea     edi, [edi+ebx]
cmp     esi, ecx                 
jne     Float32_To_Int24_Clip_loop
ffree   st(0)
fincstp
fwait
fnclex
fldcw   savedFpuControlWord
}
}
static void Float32_To_Int24_DitherClip(
void *destinationBuffer, signed int destinationStride,
void *sourceBuffer, signed int sourceStride,
unsigned int count, PaUtilTriangularDitherGenerator *ditherGenerator )
{
short savedFpuControlWord;
signed long sourceByteStride;
signed long highpassedDither;
unsigned long ditherPrevious = ditherGenerator->previous;
unsigned long ditherRandSeed1 = ditherGenerator->randSeed1;
unsigned long ditherRandSeed2 = ditherGenerator->randSeed2;
signed long tempInt32;
__asm{
mov     esi, sourceBuffer
mov     edx, 4                   
mov     eax, sourceStride
imul    eax, edx
mov     ecx, count
imul    ecx, eax
add     ecx, esi
mov     edi, destinationBuffer
mov     edx, 3                   
mov     ebx, destinationStride
imul    ebx, edx
fwait
fstcw   savedFpuControlWord
fldcw   fpuControlWord_
fld     ditheredInt24Scaler_     
Float32_To_Int24_DitherClip_loop:
mov     edx, dword ptr [esi]     
and     edx, 0x7FFFFFFF          
cmp     edx, 0x3F800000          
jg      Float32_To_Int24_DitherClip_clamp
fld     dword ptr [esi]          
add     esi, eax                 
fmul    st(0), st(1)             
mov     sourceByteStride, eax    
mov     edx, 196314165
mov     eax, ditherRandSeed1
mul     edx                      
lea     eax, [eax+907633515]
mov     ditherRandSeed1, eax
mov     edx, 196314165
mov     eax, ditherRandSeed2
mul     edx                      
lea     eax, [eax+907633515]
mov     edx, ditherRandSeed1
shr     edx, PA_DITHER_SHIFT_
mov     ditherRandSeed2, eax
shr     eax, PA_DITHER_SHIFT_
lea     eax, [eax+edx]
mov     edx, ditherPrevious
neg     edx
lea     edx, [eax+edx]           
mov     highpassedDither, edx
mov     ditherPrevious, eax      
mov     eax, sourceByteStride    
fild    highpassedDither
fmul    const_float_dither_scale_
faddp   st(1), st(0)             
fistp   tempInt32                
mov     edx, tempInt32
jmp     Float32_To_Int24_DitherClip_store
Float32_To_Int24_DitherClip_clamp:
mov     edx, dword ptr [esi]     
shr     edx, 31                  
add     esi, eax                 
add     edx, 0x7FFFFF            
Float32_To_Int24_DitherClip_store:
mov     byte ptr [edi], DL
shr     edx, 8
mov     word ptr [edi+1], DX
lea     edi, [edi+ebx]
cmp     esi, ecx                 
jne     Float32_To_Int24_DitherClip_loop
ffree   st(0)
fincstp
fwait
fnclex
fldcw   savedFpuControlWord
}
ditherGenerator->previous = ditherPrevious;
ditherGenerator->randSeed1 = ditherRandSeed1;
ditherGenerator->randSeed2 = ditherRandSeed2;
}
static void Float32_To_Int16(
void *destinationBuffer, signed int destinationStride,
void *sourceBuffer, signed int sourceStride,
unsigned int count, PaUtilTriangularDitherGenerator *ditherGenerator )
{
short savedFpuControlWord;
(void) ditherGenerator;  
__asm{
mov     esi, sourceBuffer
mov     edx, 4                   
mov     eax, sourceStride
imul    eax, edx                 
mov     ecx, count
imul    ecx, eax
add     ecx, esi                 
mov     edi, destinationBuffer
mov     edx, 2                   
mov     ebx, destinationStride
imul    ebx, edx                 
fwait
fstcw   savedFpuControlWord
fldcw   fpuControlWord_
fld     int16Scaler_             
Float32_To_Int16_loop:
fld     dword ptr [esi]          
add     esi, eax                 
fmul    st(0), st(1)             
fistp   word ptr [edi]           
add     edi, ebx                 
cmp     esi, ecx                 
jne     Float32_To_Int16_loop
ffree   st(0)
fincstp
fwait
fnclex
fldcw   savedFpuControlWord
}
}
static void Float32_To_Int16_Clip(
void *destinationBuffer, signed int destinationStride,
void *sourceBuffer, signed int sourceStride,
unsigned int count, PaUtilTriangularDitherGenerator *ditherGenerator )
{
short savedFpuControlWord;
(void) ditherGenerator;  
__asm{
mov     esi, sourceBuffer
mov     edx, 4                   
mov     eax, sourceStride
imul    eax, edx                 
mov     ecx, count
imul    ecx, eax
add     ecx, esi                 
mov     edi, destinationBuffer
mov     edx, 2                   
mov     ebx, destinationStride
imul    ebx, edx                 
fwait
fstcw   savedFpuControlWord
fldcw   fpuControlWord_
fld     int16Scaler_             
Float32_To_Int16_Clip_loop:
mov     edx, dword ptr [esi]     
and     edx, 0x7FFFFFFF          
cmp     edx, 0x3F800000          
jg      Float32_To_Int16_Clip_clamp
fld     dword ptr [esi]          
add     esi, eax                 
fmul    st(0), st(1)             
fistp   word ptr [edi]           
jmp     Float32_To_Int16_Clip_stored
Float32_To_Int16_Clip_clamp:
mov     edx, dword ptr [esi]     
shr     edx, 31                  
add     esi, eax                 
add     dx, 0x7FFF               
mov     word ptr [edi], dx       
Float32_To_Int16_Clip_stored:
add     edi, ebx                 
cmp     esi, ecx                 
jne     Float32_To_Int16_Clip_loop
ffree   st(0)
fincstp
fwait
fnclex
fldcw   savedFpuControlWord
}
}
static void Float32_To_Int16_DitherClip(
void *destinationBuffer, signed int destinationStride,
void *sourceBuffer, signed int sourceStride,
unsigned int count, PaUtilTriangularDitherGenerator *ditherGenerator )
{
short savedFpuControlWord;
signed long sourceByteStride;
signed long highpassedDither;
unsigned long ditherPrevious = ditherGenerator->previous;
unsigned long ditherRandSeed1 = ditherGenerator->randSeed1;
unsigned long ditherRandSeed2 = ditherGenerator->randSeed2;
__asm{
mov     esi, sourceBuffer
mov     edx, 4                   
mov     eax, sourceStride
imul    eax, edx                 
mov     ecx, count
imul    ecx, eax
add     ecx, esi                 
mov     edi, destinationBuffer
mov     edx, 2                   
mov     ebx, destinationStride
imul    ebx, edx                 
fwait
fstcw   savedFpuControlWord
fldcw   fpuControlWord_
fld     ditheredInt16Scaler_     
Float32_To_Int16_DitherClip_loop:
mov     edx, dword ptr [esi]     
and     edx, 0x7FFFFFFF          
cmp     edx, 0x3F800000          
jg      Float32_To_Int16_DitherClip_clamp
fld     dword ptr [esi]          
add     esi, eax                 
fmul    st(0), st(1)             
mov     sourceByteStride, eax    
mov     edx, 196314165
mov     eax, ditherRandSeed1
mul     edx                      
lea     eax, [eax+907633515]
mov     ditherRandSeed1, eax
mov     edx, 196314165
mov     eax, ditherRandSeed2
mul     edx                      
lea     eax, [eax+907633515]
mov     edx, ditherRandSeed1
shr     edx, PA_DITHER_SHIFT_
mov     ditherRandSeed2, eax
shr     eax, PA_DITHER_SHIFT_
lea     eax, [eax+edx]           
mov     edx, ditherPrevious
neg     edx
lea     edx, [eax+edx]           
mov     highpassedDither, edx
mov     ditherPrevious, eax      
mov     eax, sourceByteStride    
fild    highpassedDither
fmul    const_float_dither_scale_
faddp   st(1), st(0)             
fistp   word ptr [edi]           
jmp     Float32_To_Int16_DitherClip_stored
Float32_To_Int16_DitherClip_clamp:
mov     edx, dword ptr [esi]     
shr     edx, 31                  
add     esi, eax                 
add     dx, 0x7FFF               
mov     word ptr [edi], dx       
Float32_To_Int16_DitherClip_stored:
add     edi, ebx                 
cmp     esi, ecx                 
jne     Float32_To_Int16_DitherClip_loop
ffree   st(0)
fincstp
fwait
fnclex
fldcw   savedFpuControlWord
}
ditherGenerator->previous = ditherPrevious;
ditherGenerator->randSeed1 = ditherRandSeed1;
ditherGenerator->randSeed2 = ditherRandSeed2;
}
void PaUtil_InitializeX86PlainConverters( void )
{
paConverters.Float32_To_Int32 = Float32_To_Int32;
paConverters.Float32_To_Int32_Clip = Float32_To_Int32_Clip;
paConverters.Float32_To_Int32_DitherClip = Float32_To_Int32_DitherClip;
paConverters.Float32_To_Int24 = Float32_To_Int24;
paConverters.Float32_To_Int24_Clip = Float32_To_Int24_Clip;
paConverters.Float32_To_Int24_DitherClip = Float32_To_Int24_DitherClip;
paConverters.Float32_To_Int16 = Float32_To_Int16;
paConverters.Float32_To_Int16_Clip = Float32_To_Int16_Clip;
paConverters.Float32_To_Int16_DitherClip = Float32_To_Int16_DitherClip;
}
#endif
