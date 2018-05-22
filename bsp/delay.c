/*
 * delay.c
 *
 *  Created on: 2018Äê5ÔÂ15ÈÕ
 *      Author: cking
 */
#include <stdint.h>
#include <stdbool.h>
#include "delay.h"
#include "driverlib/rom.h"

inline void delay_ms(int ms)
{
        volatile uint32_t K;
        for ( K = 0 ; K < ms ; K++ ) {
            ROM_SysCtlDelay( 26600 ); // assume 80MHz clock
    //        SysCtlDelay( 3990 ); // assume 16MHz clock
        }
}

inline void delay_s(int s)
{
    int i = 0;
    for(i = 0; i < s; i++)
    {
        delay_ms(1000);
    }
}
