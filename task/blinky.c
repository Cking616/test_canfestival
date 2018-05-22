/*
 * blinky.c
 *
 *  Created on: 2018Äê5ÔÂ15ÈÕ
 *      Author: cking
 */

#include "blinky.h"
#include "..\bsp\lights.h"

volatile int _led_red = 0;
volatile int _rgb = LIGHTS_RGB_GREEN;
volatile int _is_read_dis = 0;

void blinky_thread(void)
{
    lights_turn_led_red(_led_red);
    _led_red = !_led_red;
    lights_turn_rgb(_rgb);
    if(_rgb == 0)
    {
        _rgb = LIGHTS_RGB_RED;
    }
    else
    {
        _rgb =  _rgb >> 1;
    }
}
