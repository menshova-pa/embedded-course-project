#include "filter.h"

// =====================================================
// FILTER COEFFICIENTS
// =====================================================

// Numerator
static const float b0 =  0.005916267f;
static const float b1 =  0.0f;
static const float b2 = -0.011832535f;
static const float b3 =  0.0f;
static const float b4 =  0.005916267f;

// Denominator
static const float a1 = -3.7653801f;
static const float a2 =  5.3724618f;
static const float a3 = -3.4438744f;
static const float a4 =  0.8373099f;

// =====================================================
// FILTER HISTORY
// =====================================================

// Input history
static float x1 = 0.0f;
static float x2 = 0.0f;
static float x3 = 0.0f;
static float x4 = 0.0f;

// Output history
static float y_1 = 0.0f;
static float y_2 = 0.0f;
static float y_3 = 0.0f;
static float y_4 = 0.0f;

// =====================================================
// IIR FILTER
// =====================================================

float iir_filter(float x)
{
    float y =
          b0 * x
        + b1 * x1
        + b2 * x2
        + b3 * x3
        + b4 * x4

        - a1 * y_1
        - a2 * y_2
        - a3 * y_3
        - a4 * y_4;

    // Shift input history
    x4 = x3;
    x3 = x2;
    x2 = x1;
    x1 = x;

    // Shift output history
    y_4 = y_3;
    y_3 = y_2;
    y_2 = y_1;
    y_1 = y;

    return y;
}