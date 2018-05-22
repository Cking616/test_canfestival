/*
 * encoder.h
 *
 *  Created on: 2018Äê4ÔÂ25ÈÕ
 *      Author: Administrator
 */

#ifndef ENCODER_H_
#define ENCODER_H_


void encoder_init();

int encoder_get_value();

void encoder_reset();

#define ENCODER_DIR_CW                                   1
#define ENCODER_DIR_CCW                                  -1
extern volatile int _encoder_dir;
#define encoder_set_dir(dir)    _encoder_dir = dir

int encoder_get_speed();
#endif /* ENCODER_H_ */
