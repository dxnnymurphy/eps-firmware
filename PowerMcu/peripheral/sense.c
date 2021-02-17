/*
 * sense.c
 *
 *  Created on: 23 Oct 2015
 *      Author: Tom
 */

#include <msp430.h>
#include <stdint.h>
#include "hal/adc.h"
#include "hal/mux.h"

uint16_t sense_get(uint8_t channel)
{
	mux_select(channel);
	return adc_convert(INCH_7);   /*what is INCH_7 should it not be channel?*/
}

