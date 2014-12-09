/*
 * @brief UART API in ROM (USART API ROM) interrupt example
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2012
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#include <string.h>

#include "chip.h"
#include "1-Wire.h"
#include "DS18x20.h"
#include "pid.h"

#include "config.h"

/* UART handle and memory for ROM API */
static UART_HANDLE_T *uartHandle;

/* Use a buffer size larger than the expected return value of
   uart_get_mem_size() for the static UART handle type */
static uint32_t uartHandleMEM[0x10];

/* Saved tick count used for a PWM cycle */
volatile uint32_t cycleTicks;

/* Milliseconds tick */
volatile uint32_t ticks = 0;

/* Number of Mhz for us delay calculation */
volatile uint32_t mhz = 0;

/* DS1820 temperature sensors */
uint8_t sensorCount = 0;
uint8_t sensorIDs[ONE_WIRE_DEVICE_NUMBER_MAX][DS18x20_SERIAL_NUM_SIZE];

extern void printf(char *format, ...);

/**
 * Handle interrupt from State Configurable Timer
 */
void SCT_IRQHandler(void)
{
    Chip_SCT_ClearEventFlag(LPC_SCT, SCT_EVT_0);
}

/**
 * Increment number of milliseconds
 */
void SysTick_Handler(void)
{
    ticks++;
}

/**
 * Delay a certain amount of milliseconds. In the mean time, wait for
 * interrupts.
 */
void delay_ms(uint32_t count)
{
    uint32_t end = ticks + count;

    while (ticks < end) {
        __WFI();
    }
}

/**
 * Delay a certian amount of microseconds. In the mean time, stall the bus
 * (including interrupts).
 */
void delay_us(uint32_t count)
{
    Chip_MRT_SetInterval(LPC_MRT_CH1, (count * mhz - 3));
}

/**
 * Initialize delay_ms and delay_us.
 */
static void init_delays()
{
    /* Systick at 1ms interval */
    SysTick_Config(Chip_Clock_GetSystemClockRate() / 1000);

    /* MRT for us delay */
    Chip_MRT_Init();
    Chip_MRT_SetMode(LPC_MRT_CH1, MRT_MODE_ONESHOT_BUS_STALL);
    mhz = Chip_Clock_GetSystemClockRate() / 1000000;
}

/**
 * UART Pin mux function - note that SystemInit() may already setup your pin
 * muxing at system startup
 */
static void init_pinmux(void)
{
    /* Enable the clock to the Switch Matrix */
    Chip_SWM_Init();

    Chip_Clock_SetUARTClockDiv(1);  /* divided by 1 */

    /* Setup UART output */
    Chip_SWM_MovablePinAssign(SWM_U0_TXD_O, 1); //PIO0_1

    /* Setup 1-Wire input */
    Chip_SWM_DisableFixedPin(SWM_FIXED_SWCLK); //PIO0_3

    /* Setup PWM */
    Chip_SWM_MovablePinAssign(SWM_CTOUT_0_O, 0);

    /* Setup tach */
    Chip_SWM_DisableFixedPin(SWM_FIXED_SWDIO);

    /* Disable the clock to the Switch Matrix to save power */
    Chip_SWM_Deinit();
}

/**
 * Setup UART handle and parameterss
 */
static void init_uart()
{
    uint32_t multiplier;

    /* Initialize UART */
    Chip_UART_Init(LPC_USART0);

    /* 115.2KBPS, 8N1, ASYNC mode, no errors, clock filled in later */
    UART_CONFIG_T cfg = {
        0,        /* U_PCLK frequency in Hz */
        115200,   /* Baud Rate in Hz */
        1,        /* 8N1 */
        0,        /* Asynchronous Mode */
        NO_ERR_EN /* Enable No Errors */
    };

    /* Perform a sanity check on the storage allocation */
    if (LPC_UART_API->uart_get_mem_size() > sizeof(uartHandleMEM)) {
        while(1) {};
    }

    /* Setup the UART handle */
    uartHandle = LPC_UART_API->uart_setup((uint32_t) LPC_USART0, (uint8_t *) &uartHandleMEM);

    if (uartHandle == NULL) {
        while(1) {};
    }

    /* Need to tell UART ROM API function the current UART peripheral clock speed */
    cfg.sys_clk_in_hz = Chip_Clock_GetSystemClockRate();

    /* Initialize the UART with the configuration parameters */
    multiplier = LPC_UART_API->uart_init(uartHandle, &cfg);

    if (multiplier) {
        Chip_SYSCTL_SetUSARTFRGDivider(0xFF); /* always 0xFF*/
        Chip_SYSCTL_SetUSARTFRGMultiplier(multiplier);
    }
}

/**
 * Initialize the PWM mode of the SCT.
 */
static void init_pwm()
{
    Chip_SCT_Init(LPC_SCT);

    /* Configure the SCT as a 32bit counter using the bus clock */
    LPC_SCT->CONFIG = SCT_CONFIG_32BIT_COUNTER | SCT_CONFIG_CLKMODE_BUSCLK;

    /* Initial CTOUT0 state is high */
    LPC_SCT->OUTPUT = (7 << 0);

    /* The PWM will use a cycle time of (PWMCYCLERATE)Hz based off the bus clock */
    cycleTicks = Chip_Clock_GetSystemClockRate() / PWM_FREQUENCY;

    /* Setup for match mode */
    LPC_SCT->REGMODE_L = 0;

    /* Setup match counter 0 for the number of ticks in a PWM sweep, event 0
        will be used with the match 0 count to reset the counter.  */
    LPC_SCT->MATCH[0].U = cycleTicks;
    LPC_SCT->MATCHREL[0].U = cycleTicks;
    LPC_SCT->EVENT[0].CTRL = 0x00001000;
    LPC_SCT->EVENT[0].STATE = 0xFFFFFFFF;
    LPC_SCT->LIMIT_L = (1 << 0);

    /* For CTOUT0 to CTOUT2, event 1 is used to clear the output */
    LPC_SCT->OUT[0].CLR = (1 << 0);
    LPC_SCT->OUT[1].CLR = (1 << 0);
    LPC_SCT->OUT[2].CLR = (1 << 0);

    /* Setup event 1 to trigger on match 1 and set CTOUT0 high */
    LPC_SCT->EVENT[1].CTRL = (1 << 0) | (1 << 12);
    LPC_SCT->EVENT[1].STATE = 1;
    LPC_SCT->OUT[0].SET = (1 << 1);

    /* Don't use states */
    LPC_SCT->STATE_L = 0;

    /* Unhalt the counter to start */
    LPC_SCT->CTRL_U &= ~(1 << 2);
}

/**
 * Set the PWM value. Value is a percentage between 0-100.
 */
void set_pwm(uint8_t percentage)
{
    uint32_t value, newTicks;;

    if (percentage >= 100) {
        value = 100;
    }
    else if (percentage == 0) {
        value = cycleTicks + 1;
    }
    else {
        newTicks = (cycleTicks * percentage) / 100;

        /* Approximate duty cycle rate */
        value = cycleTicks - newTicks;
    }

    LPC_SCT->MATCHREL[1].U = value;
}

/**
 * Send a single character on the UART using polling mode.
 */
void put_char(char c)
{
    LPC_UART_API->uart_put_char(uartHandle, c);
}

/**
 * Send a string on the UART terminated by a NULL character using polling mode.
 */
void put_string(char* s)
{
    UART_PARAM_T param;

    param.buffer = (uint8_t *) s;
    param.size = strlen(s);

    /* Polling mode, do not append CR/LF to sent data */
    param.transfer_mode = TX_MODE_SZERO;
    param.driver_mode = DRIVER_MODE_POLLING;

    /* Transmit the data */
    if (LPC_UART_API->uart_put_line(uartHandle, &param)) {
        while(1) {};
    }
}

/**
 * Main Function
 */
int main(void)
{
    uint8_t i;
    uint8_t state = 0;

    int32_t temperature[NUMBER_OF_SENSORS];
    pid_data_t pid[NUMBER_OF_SENSORS];

    /* Initialize delays */
    init_delays();

    /* Initialize pins */
    init_pinmux();

    /* Initialize peripherals */
    init_uart();
    init_pwm();

    /* Main loop for application. */
    while (1) {
        if (state == 0) {
            /* Reset 1-Wire bus */
            uint8_t attempts = 10;

            while (attempts-- && One_Wire_Reset()) {
                delay_ms(100);
            }

            /* Find connected sensors */
            DS18x20_Search_Rom(&sensorCount, &sensorIDs);

            /* Check for temperature sensors, otherwise go into error mode */
            printf("Found %i/%i sensors\r\n", (int16_t) sensorCount, NUMBER_OF_SENSORS);

            if (sensorCount != NUMBER_OF_SENSORS) {
                state = 128;
            } else {
                state = 1;
            }
        } else if (state == 1) {
            /* Setup PID controllers */
            for (i = 0; i < NUMBER_OF_SENSORS; i++) {
                PID_Controller_Init(&pid[i], PID_SETTINGS[i][0], PID_SETTINGS[i][1], PID_SETTINGS[i][2]);
            }

            state = 2;
        } else if (state == 2) {
            /* Start reading temperature by all sensors */
            DS18x20_Start_Conversion_Skip_Rom();

            /* Wait for conversion to complete. */
            delay_ms(1000);

            /* Fetch results individually */
            uint8_t success = 1;

            for (i = 0; i < NUMBER_OF_SENSORS; i++) {
                uint8_t result = DS18x20_Get_Conversion_Result_by_ROM_CRC(&sensorIDs[i], &temperature[i]);

                /* Advance to output state */
                if (result == ONE_WIRE_SUCCESS) {
                    if ((temperature[i] / 100) > 1000) {
                        printf("Temperature of sensor %i invalid: %n\r\n", (int16_t) i, temperature);
                        success = 0;
                    }
                } else {
                    printf("Reading of sensor %i failed: %i\r\n", (int16_t) i, (int16_t) result);
                    success = 0;
                }
            }

            /* Advance only on success */
            if (success) {
                state = 3;
            }
        } else if (state == 3) {
            /* Update PID controllers */
            uint8_t speed[NUMBER_OF_SENSORS], maxSpeed;

            for (i = 0; i < NUMBER_OF_SENSORS; i++) {
                int16_t out = -1 * PID_Controller_Update(&pid[i], temperature[i] / 100, TEMPERATURE_SETTINGS[i]);

                /* Set new speed. Fan won't run below 10%. */
                if (out >= MINIMAL_FAN_SPEED) {
                    speed[i] = (out * 100) / 128;
                } else {
                    speed[i] = 0;
                }

                /* Take max speed */
                if (speed[i] > maxSpeed) {
                    maxSpeed = speed[i];
                }

                /* Debugging */
                printf("%i,%n,%n,%u,%i,", i, temperature[i] / 100, TEMPERATURE_SETTINGS[i], (uint16_t) speed[i], out);
            }

            /* Debugging */
            printf("%u\r\n", (uint16_t) maxSpeed);

            /* Set new speed */
            set_pwm(maxSpeed);

            /* Advance to measurement state */
            state = 2;
        } else if (state == 128) {
            /* Infinitely spin fan up and down to tell something is wrong. */
            set_pwm(100);
            delay_ms(5000);
            set_pwm(0);
            delay_ms(5000);
        }
    }

    /* Should not end here */
    return 0;
}
