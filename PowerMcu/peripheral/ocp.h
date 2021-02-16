/*
 * ocp.h
 *
 *  Created on: 6 Nov 2015
 *      Author: Tom
 */

#ifndef PERIPHERAL_OCP_H_
#define PERIPHERAL_OCP_H_

#include <stdint.h>


/* OCP1 Backup Radio        5v, formerly 3v3
 * OCP2 Radio               5v, formerly 3v3          rails have since been changed need to change this 
 * OCP3 Camera OCP          5v
 * OCP4 FLight MCU          3v3
 * OCP5 GPS                 3v3
 * OCP6 LNA                 ~8v
 */



void ocp_process();

void ocp_toggle_rail(uint8_t value);
void ocp_deactivate_all_rails();

// Set the active state of each rail.
// value: bitfield with the state of each rail
//     0 - Off
//     1 - On (except in overcurrent state)
//     rails & 0x1 is for OCP0
//     rails & 0x2 is for OCP1 etc
//void ocp_activate_rail(uint8_t value);                          //Appears value indicates both the new state of the rail and which to change but a deactivate function also exists
//void ocp_deactivate_rail(uint8_t value);
// Gets the active state of each rail.
// Return value: bitfield with the state of each rail
//     0 - Off
//     1 - On (except in overcurrent state)
//     rails & 0x1 is for OCP0
//     rails & 0x2 is for OCP1 etc
uint8_t ocp_get_rails_activated();

uint16_t ocp_get_boots(uint8_t railIndex);
uint16_t ocp_get_fails(uint8_t railIndex);

#endif /* PERIPHERAL_OCP_H_ */
