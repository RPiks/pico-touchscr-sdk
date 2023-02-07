///////////////////////////////////////////////////////////////////////////////
//
//  Roman Piksaykin [piksaykin@gmail.com], R2BDY
//  https://www.qrz.com/db/r2bdy
//
///////////////////////////////////////////////////////////////////////////////
//
//
//  msp2807_calibration.c - Touch screen calibration
// 
//
//  DESCRIPTION
//
//      Functions to estimate transform coefficients of the touch screen to
//  actual screen's coordinate system.
//
//  PLATFORM
//      The code is platform-independent.
//
//  REVISION HISTORY
// 
//      Rev 0.5   05 Jan 2023
//  Initial release has been refactored from AN-1021 of Analog Devices:
//  - global variables are removed;
//  + double to float substitution.
//  + readability.
//  + divizion by zero checks.
//  + sub-pixel coordinate transformation.
//
////////////////////////////////////////////////////////////////////////////////
#include "msp2807_calibration.h"

/// @brief Transforms coordinates [Px, Py] obtained from touch screen to display
/// @param pcmat The matrix of coeffs calculated by CalculateCalibrationMat.
/// @param Px X coordinate from touch device.
/// @param Py Y coordinate from touch device.
void TouchTransformCoords(const calibration_mat_t *pcmat, 
                            int32_t *Px, int32_t *Py)
{
    static const float d1024 = 1.f / 1024.f;
    *Px = (int16_t)(d1024 * pcmat->KX1 * (float)(*Px) + d1024 * pcmat->KX2 
        * (float)(*Py) + pcmat->KX3 + .5f);

    *Py = (int16_t)(d1024 * pcmat->KY1 * (float)(*Px) + d1024 * pcmat->KY2 
        * (float)(*Py) + pcmat->KY3 + .5f);
}

/// @brief Calculates calibration coefficients.
/// @param pReferencePoint Ptr to reference point's array x0, y0, x1, y1, ...
/// @param pSamplePoint Ptr to array of values sampled from device xs0, xs1,..
/// @param npoints A number of input points.
/// @param pcmat Output coefficients calculated.
/// @return Error code (shouldn't be below zero if normal operation.)
int CalculateCalibrationMat(const int16_t *pReferencePoint,
                            const int16_t *pSamplePoint, int npoints, 
                            calibration_mat_t *pcmat)
{
    int i;
    float a[3], b[3], c[3], d[3], k;

    if(npoints < 3)
    {
        return -1;  // The number of points must be at least 3.
    }
    else
    {
        const float fnpointsM1 = 1.f / (float)npoints;

        if(3 == npoints)
        {
            for(i = 0; i < npoints; ++i)
            {
                const int ixl = i << 1;

                a[i] = (float)(pSamplePoint[ixl + 0]);
                b[i] = (float)(pSamplePoint[ixl + 1]);
                c[i] = (float)(pReferencePoint[ixl + 0]);
                d[i] = (float)(pReferencePoint[ixl + 1]);
            }
        }
        else if(npoints > 3)
        {
            for(i = 0; i < 3; ++i)
            {
                a[i] = b[i] = c[i] = d[i] = .0f;
            }
            
            for(i = 0; i < npoints; ++i)
            {
                const int ixl = i << 1;

                a[2] += (float)(pSamplePoint[ixl + 0]);
                b[2] += (float)(pSamplePoint[ixl + 1]);

                c[2] += (float)(pReferencePoint[ixl + 0]);
                d[2] += (float)(pReferencePoint[ixl + 1]);

                a[0] += (float)(pSamplePoint[ixl + 0]) 
                      * (float)(pSamplePoint[ixl + 0]);

                a[1] += (float)(pSamplePoint[ixl + 0]) 
                      * (float)(pSamplePoint[ixl + 1]);
                
                b[0] = a[1];
                
                b[1] += (float)(pSamplePoint[ixl + 1])
                      * (float)(pSamplePoint[ixl + 1]);

                c[0] += (float)(pSamplePoint[ixl + 0])
                      * (float)(pReferencePoint[ixl + 0]);

                c[1] += (float)(pSamplePoint[ixl + 1])
                      * (float)(pReferencePoint[ixl + 0]);

                d[0] += (float)(pSamplePoint[ixl + 0])
                      * (float)(pReferencePoint[ixl + 1]);

                d[1] += (float)(pSamplePoint[ixl + 1])
                      * (float)(pReferencePoint[ixl + 1]);
            }

            if(fabs(a[2]) < 1e-9 || fabs(b[2]) < 1e-9)
            {
                return -2;
            }
            a[0] = a[0] / a[2];
            a[1] = a[1] / b[2];
            b[0] = b[0] / a[2];
            b[1] = b[1] / b[2];
            c[0] = c[0] / a[2];
            c[1] = c[1] / b[2];
            d[0] = d[0] / a[2];
            d[1] = d[1] / b[2];

            a[2] = a[2] * fnpointsM1;
            b[2] = b[2] * fnpointsM1;
            c[2] = c[2] * fnpointsM1;
            d[2] = d[2] * fnpointsM1;
        }

        k = (a[0]-a[2])*(b[1]-b[2])-(a[1]-a[2])*(b[0]-b[2]);
        if(fabs(k) < 1e-9)
        {
            return -3;
        }

        const float kM1 = 1.f / k;

        pcmat->KX1 = ((c[0]-c[2])*(b[1]-b[2])-(c[1]-c[2])*(b[0]-b[2])) * kM1;
        pcmat->KX2 = ((c[1]-c[2])*(a[0]-a[2])-(c[0]-c[2])*(a[1]-a[2])) * kM1;

        pcmat->KX3 = (b[0]*(a[2]*c[1]-a[1]*c[2])+b[1]*(a[0]*c[2]-a[2]*c[0])
                    + b[2]*(a[1]*c[0]-a[0]*c[1])) * kM1;

        pcmat->KY1 = ((d[0]-d[2])*(b[1]-b[2])-(d[1]-d[2])*(b[0]-b[2])) * kM1;
        pcmat->KY2 = ((d[1]-d[2])*(a[0]-a[2])-(d[0]-d[2])*(a[1]-a[2])) * kM1;

        pcmat->KY3 = (b[0]*(a[2]*d[1]-a[1]*d[2])+b[1]*(a[0]*d[2]-a[2]*d[0])
                    + b[2]*(a[1]*d[0]-a[0]*d[1])) * kM1;

        return npoints;
    }
}
