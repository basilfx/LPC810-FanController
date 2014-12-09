/*This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief Header file for pid.c.
 *
 * - File:               pid.h
 * - Compiler:           IAR EWAAVR 4.11A
 * - Supported devices:  All AVR devices can be used.
 * - AppNote:            AVR221 - Discrete PID controller
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support email: avr@atmel.com
 *
 * $Name$
 * $Revision: 456 $
 * $RCSfile$
 * $Date: 2006-02-16 12:46:13 +0100 (to, 16 feb 2006) $
 *****************************************************************************/

#ifndef PID_H
#define PID_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

#define SCALING_FACTOR  128

/*! \brief PID Status
 *
 * Setpoints and data used by the PID control algorithm
 */
typedef struct {
    //! Last process value, used to find derivative of process value.
    int16_t lastMeasurement;
    //! Summation of errors, used for integrate calculations
    int32_t sumError;

    //! The Proportional tuning constant, multiplied with SCALING_FACTOR
    int16_t Kp;
    //! The Integral tuning constant, multiplied with SCALING_FACTOR
    int16_t Ki;
    //! The Derivative tuning constant, multiplied with SCALING_FACTOR
    int16_t Kd;

    //! Maximum allowed error, avoid overflow
    int16_t maxError;
    /* Maximum allowed sumerror, avoid overflow */
    int32_t maxSumError;
} pid_data_t;

/*! \brief Maximum values
 *
 * Needed to avoid sign/overflow problems
 */
// Maximum value of variables
#define MAX_I_TERM (100 * SCALING_FACTOR)
#define MAX_INT    (100 * SCALING_FACTOR)
#define MAX_LONG   INT32_MAX

// Helper for defining PID values without the scaling factor
#define PID_VALUES(x,y,z) { x * SCALING_FACTOR, y * SCALING_FACTOR, z * SCALING_FACTOR }

void PID_Controller_Init(pid_data_t *pid, int16_t Kp, int16_t Ki, int16_t Kd);
int16_t PID_Controller_Update(pid_data_t *pid, int16_t measurement, int16_t target);
void PID_Controller_Reset(pid_data_t *pid);

#ifdef __cplusplus
}
#endif

#endif
