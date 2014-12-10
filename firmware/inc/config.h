#ifndef __CONIFG_H__
#define __CONFIG_H__

/* PWM frequency (Hz). Higher values produce less PWM noise. */
#define PWM_FREQUENCY (50000)

/* Minimal fan speed (%) */
#define MINIMAL_FAN_SPEED (10)

/* Number of temperature sensors */
#define NUMBER_OF_SENSORS (2)

/* Target temperature per sensor (C * 10) */
const int32_t TEMPERATURE_SETTINGS[NUMBER_OF_SENSORS] = { 350, 450 };

/* PID settings per sensor (Kp, Ki, Kd) */
const int16_t PID_SETTINGS[NUMBER_OF_SENSORS][3] = { PID_VALUES(2, 0.5, 1.0), PID_VALUES(0.1, 0.1, 0.1) };

#endif