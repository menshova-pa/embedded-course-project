#include "filter.h"

#define N 4

static float b[N+1] = {
    0.005916267385, 0.0, -0.011832534770, 0.0, 0.005916267385
};

static float a[N+1] = {
    1.0, -3.765380122377, 5.37246188595, -3.443874362769, 0.837309865775
};

static float w[N+1] = {0};

float iir_filter(float x)
{
    float y;

    w[0] = x 
         - a[1]*w[1]
         - a[2]*w[2]
         - a[3]*w[3]
         - a[4]*w[4];

    y = b[0]*w[0]
      + b[1]*w[1]
      + b[2]*w[2]
      + b[3]*w[3]
      + b[4]*w[4];

    for (int i = N; i > 0; i--) {
        w[i] = w[i-1];
    }

    return y;
}
