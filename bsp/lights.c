/*
 * lights.c
 *
 *  Created on: 2018Äê5ÔÂ14ÈÕ
 *      Author: cking
 */
#include <stdint.h>
#include <stdbool.h>
#include "lights.h"
#include "driverlib/rom.h"
#include "inc/hw_sysctl.h"
#include "driverlib/sysctl.h"
#include "inc/hw_gpio.h"
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"

void lights_init(void)
{
    //ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    // Enable the GPIO pin for the LED
    ROM_GPIOPinTypeGPIOOutput( LED_RED_PORT,   LED_RED_PIN );
    ROM_GPIOPinTypeGPIOOutput( RGB_RED_PORT,   RGB_RED_PIN );
    ROM_GPIOPinTypeGPIOOutput( RGB_GREEN_PORT, RGB_GREEN_PIN );
    ROM_GPIOPinTypeGPIOOutput( RGB_BLUE_PORT,  RGB_BLUE_PIN );
}

void lights_turn_led_red(int on)
{
    ROM_GPIOPinWrite(LED_RED_PORT, LED_RED_PIN, on ? LED_RED_PIN : 0);
}

void lights_turn_rgb(unsigned int flag)
{
    if(flag & 1)
    {
        ROM_GPIOPinWrite(RGB_BLUE_PORT, RGB_BLUE_PIN, RGB_BLUE_PIN);
    }
    else
    {
        ROM_GPIOPinWrite(RGB_BLUE_PORT, RGB_BLUE_PIN, 0);
    }

    if((flag >> 1) & 1)
    {
        ROM_GPIOPinWrite(RGB_GREEN_PORT, RGB_GREEN_PIN, RGB_GREEN_PIN);
    }
    else
    {
        ROM_GPIOPinWrite(RGB_GREEN_PORT, RGB_GREEN_PIN, 0);
    }

    if((flag >> 2) & 1)
    {
        ROM_GPIOPinWrite(RGB_RED_PORT, RGB_RED_PIN, RGB_RED_PIN);
    }
    else
    {
        ROM_GPIOPinWrite(RGB_RED_PORT, RGB_RED_PIN, 0);
    }
}
