#include <math.h>
#include <string.h>
#include "biquad_filter.h"
#define FILTER_PI  (3.141592653589793238462643)
static void BiquadFilter_CalculateCommon( BiquadFilter *filter, double ratio, double Q )
{
double omega;
memset( filter, 0, sizeof(BiquadFilter) );
if( ratio >= 0.499 ) ratio = 0.499;
omega = 2.0 * (double)FILTER_PI * ratio;
filter->cos_omega = (double) cos( omega );
filter->sin_omega = (double) sin( omega );
filter->alpha = filter->sin_omega / (2.0 * Q);
}
void BiquadFilter_SetupHighPass( BiquadFilter *filter, double ratio, double Q )
{
double    scalar, opc;
if( ratio  < BIQUAD_MIN_RATIO )  ratio  = BIQUAD_MIN_RATIO;
if( Q < BIQUAD_MIN_Q ) Q = BIQUAD_MIN_Q;
BiquadFilter_CalculateCommon( filter, ratio, Q );
scalar = 1.0 / (1.0 + filter->alpha);
opc = (1.0 + filter->cos_omega);
filter->a0 = opc * 0.5 * scalar;
filter->a1 =  - opc * scalar;
filter->a2 = filter->a0;
filter->b1 = -2.0 * filter->cos_omega * scalar;
filter->b2 = (1.0 - filter->alpha) * scalar;
}
void BiquadFilter_SetupNotch( BiquadFilter *filter, double ratio, double Q )
{
double    scalar, opc;
if( ratio  < BIQUAD_MIN_RATIO )  ratio  = BIQUAD_MIN_RATIO;
if( Q < BIQUAD_MIN_Q ) Q = BIQUAD_MIN_Q;
BiquadFilter_CalculateCommon( filter, ratio, Q );
scalar = 1.0 / (1.0 + filter->alpha);
opc = (1.0 + filter->cos_omega);
filter->a0 = scalar;
filter->a1 =  -2.0 * filter->cos_omega * scalar;
filter->a2 = filter->a0;
filter->b1 = filter->a1;
filter->b2 = (1.0 - filter->alpha) * scalar;
}
void BiquadFilter_Filter( BiquadFilter *filter, float *inputs, float *outputs, int numSamples )
{
int i;
double xn, yn;
double a0 = filter->a0;
double a1 = filter->a1;
double a2 = filter->a2;
double b1 = filter->b1;
double b2 = filter->b2;
double xn1 = filter->xn1;
double xn2 = filter->xn2;
double yn1 = filter->yn1;
double yn2 = filter->yn2;
for( i=0; i<numSamples; i++)
{
xn = inputs[i];
yn = (a0 * xn) + (a1 * xn1) + (a2 * xn2) - (b1 * yn1) - (b2 * yn2);
outputs[i] = yn;
xn2 = xn1;
xn1 = xn;
yn2 = yn1;
yn1 = yn;
if( (i & 7) == 0 )
{
yn1 += (double) 1.0E-26;
yn2 -= (double) 1.0E-26;
}
}
filter->xn1 = xn1;
filter->xn2 = xn2;
filter->yn1 = yn1;
filter->yn2 = yn2;
}
