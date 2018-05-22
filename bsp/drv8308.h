/*
 * drv8308.h
 *
 *  Created on: 2018Äê5ÔÂ17ÈÕ
 *      Author: cking
 */

#ifndef DVR8308_H_
#define DVR8308_H_


void drv8308_init(void);
void drv8308_set_pwm(int speed);
void drv8308_enable(int on);

#define MOTOR_DIR_CW                                   0
#define MOTOR_DIR_CCW                                  1
void drv8308_set_dir(int dir);

void drv8308_set_brake(int on);
#endif /* DVR8308_H_ */
