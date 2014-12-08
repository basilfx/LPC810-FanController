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
    pid->sumError = 0;
    pid->lastMeasurement = 0;

    // Tuning constants for PID loop
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;

    // Limits to avoid overflow
    pid->maxError = MAX_INT / (pid->Kp + 1);
    pid->maxSumError = MAX_I_TERM / (pid->Ki + 1);
}


/*! \brief PID control algorithm.
 *
 *  Calculates output from setpoint, process value and PID status.
 *
 *  \param setPoint  Desired value.
 *  \param processValue  Measured value.
 *  \param pid_st  PID status struct.
 */
int16_t PID_Controller_Update(pid_data_t *pid, int16_t measurement, int16_t target)
{
    int16_t error, p_term, d_term;
    int32_t i_term, ret, temp;

    error = target - measurement;

    // Calculate Pterm and limit error overflow
    if (error > pid->maxError) {
        p_term = MAX_INT;
    } else if (error < -pid->maxError) {
        p_term = -MAX_INT;
    } else {
        p_term = pid->Kp * error;
    }

    // Calculate Iterm and limit integral runaway
    temp = pid->sumError + error;

    if (temp > pid->maxSumError) {
        i_term = MAX_I_TERM;
        pid->sumError = pid->maxSumError;
    } else if (temp < -pid->maxSumError) {
        i_term = -MAX_I_TERM;
        pid->sumError = -pid->maxSumError;
    } else {
        pid->sumError = temp;
        i_term = pid->Ki * pid->sumError;
    }

    // Calculate Dterm
    d_term = pid->Kd * (measurement - pid->lastMeasurement);
    pid->lastMeasurement = measurement;
    ret = (p_term + i_term - d_term) / SCALING_FACTOR;

    if (ret > MAX_INT) {
        ret = MAX_INT;
    } else if(ret < -MAX_INT){
        ret = -MAX_INT;
    }

    return (int16_t) ret;
}

/*! \brief Resets the integrator.
 *
 *  Calling this function will reset the integrator in the PID regulator.
 */
void PID_Controller_Reset(pid_data_t* pid)
{
    pid->sumError = 0;
}
