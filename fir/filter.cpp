//  Created by William A. Mckee circa 2006.
//  Copyright © 2006-2021 William McKee. All rights reserved.

#include <iostream>
using namespace std;

#include "filter.h"
#include "cheby.h"

//#define highpass
#define cheby

/* DSP functions
 */

    #include <cmath>

    #define pi 3.1415926535897932

    // fc is f-cutoff / f-sample, x goes -M/2 to M/2 (not time shifted)
    // (choose M+1 points around the center of symmetry)
    // 0.0 < fc < 0.5
    // where the transition bandwidth is ~4/M * f-sample
    // M = 100 is a good value
    double sinc (double x, double fc)
    {
        if (x == 0.0) // center
            return 2.0 * fc;
        else
            return sin (2.0 * pi * fc * x) / (pi * x);
    }

    double factoral (int n)
    {
        double prod = 1.0;
        for (int i = 2; i <= n; i++)
            prod *= i;
        return prod;
    }

    // zero order Modified Bessel Function of the first kind
    double I0 (double z, double epsilon)
    {
        double sum = 1.0;
        double last = 1e100;
        z *= 0.5;
        for (int k = 1; k < 100; k++)
        {
            double ip = pow (z, k) / factoral (k);
            sum += ip * ip;
            if (fabs (sum - last) < epsilon) break;
            last = sum;
        }
        return sum;
    }

    // x = n / N, where N is the window size and n is the position in the window
    // n typically goes 0 to N-1
    // a is the shape of the window, 1 < a < 12 ... a = 8.7 is optimal
    double Kaiser (double x, double a)
    {
        if (-1.0 <= x && x <= 1.0)
        {
            return I0 (a * sqrt (1.0 - x * x), 1e-12) / I0 (a, 1e-12);
        }
        else
        {
            return 0.0;
        }
    }

    double Hamming (double x)
    {
        if (-1.0 <= x && x <= 1.0)
        {
            return 0.54 - 0.46 * cos (2.0 * pi * x);
        }
        else
        {
            return 0.0;
        }
    }

    double Hanning (double x)
    {
        if (-1.0 <= x && x <= 1.0)
        {
            return 0.5 * (1.0 - cos (2.0 * pi * x));
        }
        else
        {
            return 0.0;
        }
    }

    double Blackman (double x)
    {
        if (-1.0 <= x && x <= 1.0)
        {
            return 0.42 - 0.5 * cos (2.0 * pi * x) + 0.08 * cos (4.0 * pi * x);
        }
        else
        {
            return 0.0;
        }
    }

    double BlackmanExtact (double x)
    {
        if (-1.0 <= x && x <= 1.0)
        {
            return 0.42659071 - 0.49656062 * cos (2.0 * pi * x)
                              + 0.07684867 * cos (4.0 * pi * x);
        }
        else
        {
            return 0.0;
        }
    }

    double BlackmanHarris (double x)
    {
        if (-1.0 <= x && x <= 1.0)
        {
            return 0.42323 - 0.49755 * cos (2.0 * pi * x)
                           + 0.07922 * cos (4.0 * pi * x);
        }
        else
        {
            return 0.0;
        }
    }

    double Bartlett (double x) // triangle window
    {
        if (0.0 <= x && x <= 1.0)
            return 1.0 - x;
        else
        if (-1.0 <= x && x <= 0.0)
            return 1.0 + x;
        else
            return 0;
    }

/*
 */

inline int rnd (double d)
{
    return (int) (d < 0.0 ? d - 0.5 : d + 0.5);
}

extern "C" void computeLowpassFilter
(
    int M, double fc,
    double * w // must be M+1 values in array
)
{
#ifdef highpass
fc = 0.5 - fc;
#endif

    if (M%2 == 0) // even M, center at M/2
    {
        int center = M/2;
        int N = M/2+1;
        w [center] = sinc (0, fc);
        for (int x = 1; x < N; x++)
            w [center-x] = w[center+x] = sinc (x, fc);
    }
    else // odd M, center between M/2 and M/2+1
    {
        int left_of_center = M/2;
        int right_of_center = M/2+1;
        int N = M/2+1;
        for (int x = 0; x < N; x++)
            w[left_of_center-x] = w[right_of_center+x] = sinc (x + 0.5, fc);
    }

#ifdef highpass
    for (int u = 0; u < M+1; u+=2)
        w[u] *= -1.0;
#endif

#ifdef cheby
    double * W = NULL;
    {
    int _NF = M+1;
    double _DP = 0.005;
    double _DF = 0.0;
    int _N = 0;
    double _X0 = 0;
    int _XN = 0;
    CHEBC (_NF, _DP, _DF, _N, _X0, _XN);
    W = new double [_N];
    CHEBY (_NF, W, _DP, _DF);
    }
#endif

    for (int q = 0; q < M+1; q++)
    {
        double center = M / 2.0;
#ifdef cheby
        w [q] *= W [(int) abs (rnd (center - q))];
#else
        w [q] *= Kaiser ((center - q) / (center + 0.5), 8.7);
#endif
        // w [q] *= Bartlett ((center - q) / (center + 0.5));
    }

#ifdef cheby
    delete [] W;
#endif
}

int lowpass
(
    int numberOfSamples, const float * dataIn,
    float * & dataOut,
    int M, double fc
)
{
    dataOut = NULL;

    double * w = new double [M+1];
    double * buffer = new double [M+1];

    if (w == NULL || buffer == NULL)
    {
        delete [] w;
        delete [] buffer;

        cerr << "not enough memory (filter.cpp)." << endl;
        return -1;
    }

    computeLowpassFilter(M, fc, w);

    int p = 0;

    for (int j = 0; j <= M; j++) buffer [j] = 0.0;

    dataOut = new float [numberOfSamples + (M+1)];

    if (dataOut == NULL)
    {
        delete [] w;
        delete [] buffer;

        cerr << "not enough memory (filter.cpp)." << endl;
        return -1;
    }

    int t;
    for (t = 0; t < numberOfSamples; t++)
    {
        buffer [p++] = dataIn [t];
        if (p >= (M+1)) p = 0;

        double v = 0.0;
        for (int k = 0; k <= M; k++)
            v += w [k] * buffer [(k + p) % (M+1)];
        dataOut [t] = v; // output time delayed by M/2
    }

// filter run out
    for (; t < numberOfSamples + (M+1); t++)
    {
        buffer [p++] = 0.0;
        if (p >= (M+1)) p = 0;

        double v = 0.0;
        for (int k = 0; k <= M; k++)
            v += w [k] * buffer [(k + p) % (M+1)];
        dataOut [t] = v; // output time delayed by M/2
    }

    delete [] w;
    delete [] buffer;

    return numberOfSamples + (M+1);
}

