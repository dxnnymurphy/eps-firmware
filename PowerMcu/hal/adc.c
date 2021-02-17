/*
 * adc.c
 *
 *  Created on: 23 Oct 2015
 *      Author: Tom
 */

#include <msp430.h>
#include <stdint.h>

#include "adc.h"

static uint16_t lastResult;

void adc_init()
{
	ADC10AE0 |= BIT7;
	ADC10CTL0 &= ~ENC;      // Disable ADC.
	ADC10CTL0 = SREF_1      // VR+ = VREF+, VR- = VSS. Maybe should be SREF_0 as Vref+ used for Analog input.
			| ADC10SHT_3    // Sample-and-hold time 64x ADC10CLKs.
			| ADC10SR       // Decrease reference buffer capability to reduce current.
			| REFON         // Reference output on.
			| REFBURST      // Reference buffer on only during sample-and-conversion.
			| REF2_5V       // 2.5 V.
			| ADC10ON       // Enable ADC.
			| ADC10IE;      // Enable interrupt.
	lastResult = 0xFFFF;
}

uint16_t adc_get_last_result()
{
	return lastResult;
}

void adc_start_conversion(uint16_t ch)
{
	ADC10CTL1 = ADC10SSEL_0 // ADC10OSC source.
			  | ch;
	__delay_cycles (128);   // Allow VREF to settle.
	ADC10CTL0 |= ENC        // Enable ADC.
			  | ADC10SC;      // Start conversion.
}

uint16_t adc_convert(uint16_t ch)
{
	lastResult = 0xFFFF;
	adc_start_conversion(ch);
	while(lastResult == 0xFFFF);
	return lastResult;
}

#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR (void)
{
	lastResult = ADC10MEM;
	ADC10CTL0 &= ~ENC;      // Disable ADC.
}
