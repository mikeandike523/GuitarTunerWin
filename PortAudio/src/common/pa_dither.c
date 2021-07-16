 
#include "pa_types.h"
#include "pa_dither.h"
#define PA_DITHER_BITS_   (15)
void PaUtil_InitializeTriangularDitherState( PaUtilTriangularDitherGenerator *state )
{
state->previous = 0;
state->randSeed1 = 22222;
state->randSeed2 = 5555555;
}
PaInt32 PaUtil_Generate16BitTriangularDither( PaUtilTriangularDitherGenerator *state )
{
PaInt32 current, highPass;
state->randSeed1 = (state->randSeed1 * 196314165) + 907633515;
state->randSeed2 = (state->randSeed2 * 196314165) + 907633515;
#define DITHER_SHIFT_  ((sizeof(PaInt32)*8 - PA_DITHER_BITS_) + 1)
current = (((PaInt32)state->randSeed1)>>DITHER_SHIFT_) +
(((PaInt32)state->randSeed2)>>DITHER_SHIFT_);
highPass = current - state->previous;
state->previous = current;
return highPass;
}
#define PA_FLOAT_DITHER_SCALE_  (1.0f / ((1<<PA_DITHER_BITS_)-1))
static const float const_float_dither_scale_ = PA_FLOAT_DITHER_SCALE_;
float PaUtil_GenerateFloatTriangularDither( PaUtilTriangularDitherGenerator *state )
{
PaInt32 current, highPass;
state->randSeed1 = (state->randSeed1 * 196314165) + 907633515;
state->randSeed2 = (state->randSeed2 * 196314165) + 907633515;
current = (((PaInt32)state->randSeed1)>>DITHER_SHIFT_) +
(((PaInt32)state->randSeed2)>>DITHER_SHIFT_);
highPass = current - state->previous;
state->previous = current;
return ((float)highPass) * const_float_dither_scale_;
}
