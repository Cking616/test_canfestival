/*
 * driver.c
 *
 *  Created on: 2018Äê5ÔÂ15ÈÕ
 *      Author: cking
 */

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "can.h"
#include "drv8308.h"
#include "encoder.h"
#include "lights.h"
#include "sonar.h"

volatile int _address = 0;

//*****************************************************************************
//
// Configure the UART and its pins.  This must be called before UARTprintf().
//
//*****************************************************************************
void
ConfigureUART(void)
{
    //
    // Enable the GPIO Peripheral used by the UART.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Enable UART0
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Configure GPIO Pins for UART mode.
    //
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Use the internal 16MHz oscillator as the UART clock source.
    //
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_SYSTEM);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, SysCtlClockGet());
}

void driver_init_hardware(void)
{
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_RCC2_USERCC2 | SYSCTL_RCC2_DIV400); // 80 Mhz

    // Enable the GPIO port that is used for the on-board LED.
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
    //ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER1);

    // Check if the peripheral access is enabled.
    while(!ROM_SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA)) { }

    // SW1..4 inputs
    ROM_GPIOPinTypeGPIOInput( GPIO_PORTE_BASE, GPIO_PIN_0 );  // SW1
    ROM_GPIOPinTypeGPIOInput( GPIO_PORTD_BASE, GPIO_PIN_7 );  // SW2
    ROM_GPIOPinTypeGPIOInput( GPIO_PORTC_BASE, GPIO_PIN_7 );  // SW3
    ROM_GPIOPinTypeGPIOInput( GPIO_PORTC_BASE, GPIO_PIN_6 );  // SW4

    ROM_GPIODirModeSet(GPIO_PORTE_BASE, GPIO_PIN_0, GPIO_DIR_MODE_IN );
    ROM_GPIODirModeSet(GPIO_PORTD_BASE, GPIO_PIN_7, GPIO_DIR_MODE_IN );
    ROM_GPIODirModeSet(GPIO_PORTC_BASE, GPIO_PIN_7 | GPIO_PIN_6, GPIO_DIR_MODE_IN );
    ROM_GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    ROM_GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_7, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    ROM_GPIOPadConfigSet(GPIO_PORTC_BASE, GPIO_PIN_7 | GPIO_PIN_6, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    _address =   (ROM_GPIOPinRead( GPIO_PORTE_BASE, GPIO_PIN_0)==0? 1 : 0 ) |
                 (ROM_GPIOPinRead( GPIO_PORTD_BASE, GPIO_PIN_7)==0? 2 : 0 ) |
                 (ROM_GPIOPinRead( GPIO_PORTC_BASE, GPIO_PIN_7)==0? 4 : 0 ) |
                 (ROM_GPIOPinRead( GPIO_PORTC_BASE, GPIO_PIN_6)==0? 8 : 0 );


    ConfigureUART();

    can_init();

    encoder_init();

    sonar_init();

    lights_init();

    //step_motor_init();

    drv8308_init();
    // ************************************************************************************************************
    ROM_SysTickPeriodSet(80000);  // every 1mSec, @ 80Mhz
}

inline int driver_get_address(void)
{
    return _address;
}

void driver_init_system(void)
{
    can_start_listening();

    encoder_reset();

    //speed_controller_init();
    //SysTickIntRegister(SysTickIntHandler);

    lights_turn_led_red(1);
    lights_turn_rgb(LIGHTS_RGB_RED);

    drv8308_enable(1);
    //can_write(_address, 0x12345699, 0 ); // magic number means reset just happened
}

inline void driver_start_tick()
{
    ROM_SysTickEnable();
    ROM_SysTickIntEnable();
}
