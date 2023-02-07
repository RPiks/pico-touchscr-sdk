///////////////////////////////////////////////////////////////////////////////
//
//  Roman Piksaykin [piksaykin@gmail.com], R2BDY
//  https://www.qrz.com/db/r2bdy
//
///////////////////////////////////////////////////////////////////////////////
//
//
//  msp2807_calibration.h - Touch screen calibration
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
#ifndef _MSP2807_CALIBRATION_H
#define _MSP2807_CALIBRATION_H

#include <math.h>
#include <stdint.h>

/// @brief Coefficients of the calibration algorithm.
typedef struct
{
    float KX1, KX2, KX3;
    float KY1, KY2, KY3;

} calibration_mat_t;

void TouchTransformCoords(const calibration_mat_t *pcmat, int32_t *Px, int32_t *Py);

int CalculateCalibrationMat(const int16_t *pReferencePoint, 
                            const int16_t *pSamplePoint, int npoints, 
                            calibration_mat_t *pcmat);
#endif
