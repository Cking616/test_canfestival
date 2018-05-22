/*
 * can.h
 *
 *  Created on: 2018Äê5ÔÂ2ÈÕ
 *      Author: Administrator
 */

#ifndef CAN_H_
#define CAN_H_

#include <stdbool.h>
#include <stdint.h>

void can_init();

void can_start_listening();

void can_write(uint32_t id, uint32_t len, uint8_t mode, uint8_t* data );

//extern volatile uint32_t CanMsgIn[2];
//extern volatile bool g_bRXFlag;
//extern volatile uint16_t CanRXAd;

#define CANS_PORT GPIO_PORTD_BASE
#define CANS_PIN GPIO_PIN_6

#endif /* CAN_H_ */
