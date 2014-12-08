#include "chip.h"

const uint32_t OscRateIn = 12000000;
const uint32_t ExtRateIn = 0;

/**
 * Set up and initialize hardware prior to call to main
 */
void SystemInit(void)
{
	/* Chip specific SystemInit */
	Chip_SystemInit();

    /* Clock updates */
    SystemCoreClockUpdate();
}
