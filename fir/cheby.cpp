/*
MIT License
Copyright (c) 2023 wamckee
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

//
// ORIGIAL FORTRAN CITATION:
//-----------------------------------------------------------------------
// MAIN PROGRAM: WINDOW DESIGN OF LINEAR PHASE, LOWPASS, HIGHPASS
//               BANDPASS, AND BANDSTOP FIR DIGITAL FILTERS
// AUTHOR:       LAWRENCE R. RABINER AND CAROL A. MCGONEGAL
//               BELL LABORATORIES, MURRAY HILL, NEW JERSEY, 07974
// MODIFIED JAN. 1978 BY DOUG PAUL, MIT LINCOLN LABORATORIES
//-----------------------------------------------------------------------
//
// Extracted and converted to C++ by William A. McKee on Feb. 23, 2006
//

#include "cheby.h"

#include <cmath>

//
//-----------------------------------------------------------------------
// FUNCTION:  COSHIN
// FUNCTION FOR HYPERBOLIC INVERSE COSINE OF X
//-----------------------------------------------------------------------
//

static double COSHIN (double X)
{
      return log (X + sqrt (X * X - 1.0));
}

//
//-----------------------------------------------------------------------
// SUBROUTINE:  CHEBC
// SUBROUTINE TO GENERATE CHEBYSHEV WINDOW PARAMETERS WHEN
// ONE OF THE THREE PARAMETERS NF,DP AND DF IS UNSPECIFIED
//-----------------------------------------------------------------------
//

void CHEBC (int & NF, double & DP, double & DF, int & N, double & X0, int & XN)
{
//
// NF = FILTER LENGTH (IN SAMPLES)
// DP = FILTER RIPPLE (ABSOLUTE SCALE)
// DF = NORMALIZED TRANSITION WIDTH OF FILTER
//  N = (NF+1)/2 = FILTER HALF LENGTH
// X0 = (3-C0)/(1+C0) WITH C0=COS(2*PI*DF) = CHEBYSHEV WINDOW CONSTANT
// XN = NF-1
//
      double PI = 4.0 * atan (1.0);
      if (NF == 0)
      {
//
// DP,DF SPECIFIED, DETERMINE NF
//
          double C1 = COSHIN ((1.0 + DP) / DP);
          double C0 = cos (PI * DF);
          double X = 1.0 + C1 / COSHIN (1.0 / C0);
//
// INCREMENT BY 1 TO GIVE NF WHICH MEETS OR EXCEEDS SPECS ON DP AND DF
//
          NF = int (X + 1.0);
          XN = NF - 1;
      }
      else if (DF == 0.0)
      {
//
// NF,DP SPECIFIED, DETERMINE DF
//
          XN = NF - 1;
          double C1 = COSHIN ((1.0 + DP) / DP);
          double C2 = cosh (C1 / XN);
          DF = acos (1.0 / C2) / PI;
      }
      else
      {
//
// NF,DF SPECIFIED, DETERMINE DP
//
          XN = NF - 1;
          double C0 = cos (PI * DF);
          double C1 = XN * COSHIN (1.0 / C0);
          DP = 1.0 / (cosh (C1) - 1.0);
      }
      N = (NF + 1) / 2;
      X0 = (3.0 - cos (2.0 * PI * DF)) / (1.0 + cos (2.0 * PI * DF));
}

//
// CHEBYSHEV POLYNOMIALS
//
static double T (double N, double X)
{
    if (fabs (X) <= 1.0)
        return cos (N * acos (X));
    else
        return cosh (N * COSHIN (X));
}

//
//-----------------------------------------------------------------------
// SUBROUTINE:  CHEBY
// DOLPH CHEBYSHEV WINDOW DESIGN
//-----------------------------------------------------------------------
//
void CHEBY (int NF, double * W, double DP, double DF)
{
//
//  NF = FILTER LENGTH IN SAMPLES
//  W = WINDOW ARRAY OF SIZE N
//  N = HALF LENGTH OF FILTER = (NF+1)/2
//  IEO = EVEN-ODD INDICATOR--IEO=0 FOR NF EVEN
//  DP = WINDOW RIPPLE ON AN ABSOLUTE SCALE
//  DF = NORMALIZED TRANSITION WIDTH OF WINDOW
//  X0 = WINDOW PARAMETER RELATED TO TRANSITION WIDTH
//  XN = NF-1
//
      int N = (NF + 1) / 2;

      bool IEO = (NF % 2 == 1); // (true for NF odd, false for NF even)
      int XN = (NF - 1);

      double PIE = 4.0 * atan (1.0);
      double TWOPI = 2.0 * PIE;

      double X0 = (3.0 - cos (2.0 * PIE * DF)) / (1.0 + cos (2.0 * PIE * DF));

      double ALPHA = (X0 + 1.0) / 2.0;
      double BETA = (X0 - 1.0) / 2.0;

      double * PR, * PI;

      PR = new double [NF];
      PI = new double [NF];

      int I;

      for (I = 0; I < NF; I++)
      {
        double F = double (I) / NF;
        double X = ALPHA * cos (TWOPI * F) + BETA;
        double P = DP * T (XN / 2.0, X);
        if (IEO)
        {
          PI [I] = 0.0;
          PR [I] = P;
        }
        else
//
// FOR EVEN LENGTH FILTERS USE A ONE-HALF SAMPLE DELAY
// ALSO THE FREQUENCY RESPONSE IS ANTISYMMETRIC IN FREQUENCY
//
        {
          PR [I] = P * cos (PIE * F);
          PI [I] = -P * sin (PIE * F);
          if (I > NF / 2) { PR [I] = -PR [I]; PI [I] = -PI [I]; }
        }
      }
//
// USE DFT TO GIVE WINDOW
//
      double TWN = TWOPI / NF;
      for (I = 0; I < N; I++)
      {
        W [I] = 0.0;
        for (int J = 0; J < NF; J++)
        {
          W [I] += PR [J] * cos (TWN * J * I) + PI [J] * sin (TWN * J * I);
        }
      }
//
// NORMALIZE THE WINDOW
//
      for (I = 1; I < N; I++)
      {
        W [I] /= W [0];
      }
      W [0] = 1.0;

      delete [] PR;
      delete [] PI;
}

