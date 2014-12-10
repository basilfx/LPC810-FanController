/*This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief General PID implementation for AVR.
 *
 * Discrete PID controller implementation. Set up by giving P/I/D terms
 * to Init_PID(), and uses a struct PID_DATA to store internal values.
 *
 * - File:               pid.c
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

#include "pid.h"

/*! \brief Initialisation of PID controller parameters.
 *
 *  Initialise the variables used by the PID algorithm.
 *
 *  \param p_factor  Proportional term.
 *  \param i_factor  Integral term.
 *  \param d_factor  Derivate term.
 *  \param pid  Struct with PID status.
 */
void PID_Controller_Init(pid_data_t *pid, int16_t Kp, int16_t Ki, int16_t Kd)
{
    // Start values for PID controller
    pid->lastProcessValue = 0;
    pid->lastIntegral = 0;

    // Tuning constants for PID loop
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;

    // Output limits
    pid->maxOutput = 255 * SCALING_FACTOR;
    pid->minOutput = -255 * SCALING_FACTOR;
}


/*! \brief PID control algorithm.
 *
 *  Calculates output from setpoint, process value and PID status.
 *
 *  \param setPoint  Desired value.
 *  \param processValue  Measured value.
 *  \param pid_st  PID status struct.
 */
int16_t PID_Controller_Update(pid_data_t *pid, int16_t processValue, int16_t setPoint)
{
    int32_t error, result, temp;

    /* Compute the error */
    error = setPoint - processValue;

    /* Compute integrator term. When limits are hit, stop summing. */
    temp = pid->lastIntegral + (pid->Ki * error);

    if (temp > pid->maxOutput) {
        temp = pid->maxOutput;
    } else if (temp < -pid->maxOutput) {
        temp = -pid->maxOutput;
    }

    /* Compute the output */
    result = (pid->Kp * error) + temp - pid->Kd * (processValue - pid->lastProcessValue);

    if (result > pid->maxOutput) {
        result = pid->maxOutput;
    } else if (result < -pid->maxOutput) {
        result = -pid->maxOutput;
    }

    /* Store last values */
    pid->lastProcessValue = processValue;
    pid->lastIntegral = temp;

    /* Return output */
    return (int16_t) (result / SCALING_FACTOR);
}

/*! \brief Resets the integrator.
 *
 *  Calling this function will reset the integrator in the PID regulator.
 */
void PID_Controller_Reset(pid_data_t* pid)
{
    pid->lastIntegral = 0;
}
