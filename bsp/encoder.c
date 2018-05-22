/*
 * encoder.c
 *
 *  Created on: 2018Äê4ÔÂ25ÈÕ
 *      Author: Administrator
 */
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_qei.h"
#include "inc/hw_types.h"
#include "driverlib/qei.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "inc/hw_gpio.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "encoder.h"
#include "utils/uartstdio.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"

volatile int _encoder_dir = 1;


void encoder_init()
{
    //
    // Enable the QEI0 peripheral
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_QEI0);
    //
    // Wait for the QEI0 module to be ready.
    //
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_QEI0))
    {
    }

    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;   // unlock port F
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) = 0x0F;
    GPIOPinConfigure(GPIO_PF0_PHA0);
    GPIOPinConfigure(GPIO_PF1_PHB0);
    GPIOPinTypeQEI(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    //
    // Configure the quadrature encoder to capture edges on both signals and
    // maintain an absolute position by resetting on index pulses. Using a
    // 1000 line encoder at four edges per line, there are 4000 pulses per
    // revolution; therefore set the maximum position to 3999 as the count
    // is zero based.
    //
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD);
    QEIConfigure(QEI0_BASE, (QEI_CONFIG_CAPTURE_A_B | QEI_CONFIG_NO_RESET), 0xffffffff);
    QEIVelocityConfigure(QEI0_BASE, QEI_VELDIV_1, SysCtlClockGet() / 10);

    QEIVelocityEnable(QEI0_BASE);
    QEIEnable(QEI0_BASE);
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;  // relock port F
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) = 0x00;
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
}

inline int encoder_get_speed()
{
    int dir = QEIDirectionGet(QEI0_BASE);
    return dir * ROM_QEIVelocityGet(QEI0_BASE);
}

//*****************************************************************************
inline int encoder_get_value()
{
    int reslv =  ROM_QEIPositionGet( QEI0_BASE );
    return _encoder_dir * reslv;
}

inline void encoder_reset()
{
    ROM_QEIPositionSet( QEI0_BASE, 0 );
}

