/*
 * comm.c
 *
 *  Created on: 9 Oct 2015
 *      Author: Tom
 *			Altered by: Tom Piga
 */

#include <hal/stopwatch.h>
#include <peripheral/errormon.h>
#include "hal/uart.h"
#include "hal/i2c.h"
#include <stdint.h>

#include "register.h"
#include "util/crc.h"

static void process_packet(uint8_t *message);
static void respond(uint8_t regId, uint8_t count);
static void clear_buffer();

static uint8_t responseBuffer[128];
static uint8_t msgBufferLength = 0;

void comm_init()
{
	uart_init();

	// Initialise I2C communication
	i2c_masterInit(I2C_CLOCKSOURCE_SMCLK, 80, I2C_TRANSMIT_MODE);
//      UCB0I2CIE = UCNACKIE;
	i2c_enableRXInterrupt();
	i2c_enableTXInterrupt();
}

void comm_process()
{
    static uint8_t msgBuffer[5];

	uint8_t len;
	while ((len = uart_rx(msgBuffer, msgBufferLength, 5 - msgBufferLength)) != 0)
	{
	    stopwatch_start(TICKS_PER_SECOND / 100, &clear_buffer);
		msgBufferLength += len;
		if (msgBufferLength == 5)
		{
			stopwatch_disable();
			process_packet(msgBuffer);
			msgBufferLength = 0;
		}
	}
}

static void clear_buffer()
{
	if (msgBufferLength != 0)
	{
		msgBufferLength = 0;
		errormon_increment(PacketTimeout);
	}
}

typedef struct
{
	uint8_t write;
	uint8_t regId;
	uint16_t value;
	uint16_t crc;
} PACKET;

static PACKET packet_parse(uint8_t *msg)
{
	return (PACKET){
		.write = msg[0] & 0x80,
		.regId = msg[0] & 0x7F,
		.value = ((uint16_t)msg[2] << 8) | msg[1],
		.crc = ((uint16_t)msg[4] << 8) | msg[3]
	};
}

static void process_packet(uint8_t *msg)
{
	const PACKET pkt = packet_parse(msg);
	uint16_t calculatedCrc = crc_generate(msg, 0, 3);

	if (pkt.crc != calculatedCrc)
	{
		errormon_increment(CrcInvalid);
		return;
	}

	uint8_t responseRegCount;
	if (pkt.write)
	{
		register_set(pkt.regId, pkt.value);
		responseRegCount = 1;
	}
	else
	{
		responseRegCount = pkt.value;
	}

	respond(pkt.regId, responseRegCount);
}

static void respond(uint8_t regId, uint8_t count)
{
	responseBuffer[0] = regId;
	responseBuffer[1] = count;

	uint8_t i;
	for (i = 0; i < count; i++)
	{
		uint16_t reg = register_get(regId + i);
		responseBuffer[2 + i * 2] = reg & 0xFF;
		responseBuffer[3 + i * 2] = (reg >> 8) & 0xFF;
	}

	uint16_t crcOut = crc_generate(responseBuffer, 0, 2 + count * 2);
	responseBuffer[2 + count * 2] = crcOut & 0xFF;
	responseBuffer[3 + count * 2] = (crcOut >> 8) & 0xFF;

	uart_tx(responseBuffer, 0, 4 + count * 2);
}

/* On a UART or an I2C transmission to us, flag the need for processing the
 * incoming packet */
#pragma vector = USCIAB0RX_VECTOR
__interrupt void USCIAB0RX_ISR(void)
{
    // If UART (Port A) interrupt is enabled, and there is a pending interrupt
    if ((IFG2 & UCA0RXIFG) && (IE2 & UCA0RXIE)) {
        uart_handle_rx_interrupt();

        // Only available in ISR
        core_check_wakeup(UART);
    }

    // If interrupt was triggered for something else instead, most likely for I2C
    else {
        i2c_handle_rx_interrupt();
    }
}
