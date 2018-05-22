/*
 * sonar.c
 *
 *  Created on: 2018Äê5ÔÂ15ÈÕ
 *      Author: cking
 */
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_timer.h"
#include "inc/hw_gpio.h"
#include "inc/hw_pwm.h"
#include "inc/hw_types.h"
#include "driverlib/pin_map.h"
#include "driverlib/timer.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/pwm.h"
#include "driverlib/systick.h"
#include "driverlib/rom.h"

#include "sonar.h"

void inputInt();
void Captureinit();

//This is to avoid doing the math everytime you do a reading
const double temp = 1.0/80.0;

//Stores the pulse length
volatile uint32_t pulse=0;

//Tells the main code if the a pulse is being read at the moment
volatile uint8_t echowait=0;

void sonar_init()
{
    //Configures the timer
    Captureinit();
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
    ROM_SysCtlDelay(3);
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTG_BASE, GPIO_PIN_1);
    //Configure Echo pin
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
    ROM_SysCtlDelay(3);
    ROM_GPIOPinTypeGPIOInput(GPIO_PORTG_BASE, GPIO_PIN_0);
    GPIOIntEnable(GPIO_PORTG_BASE, GPIO_PIN_0);
    ROM_GPIOIntTypeSet(GPIO_PORTG_BASE, GPIO_PIN_0,GPIO_BOTH_EDGES);
    GPIOIntRegister(GPIO_PORTG_BASE,inputInt);
}

void inputInt(){
  //Clear interrupt flag. Since we only enabled on this is enough
    GPIOIntClear(GPIO_PORTG_BASE, GPIO_PIN_0);

  /*
    If it's a rising edge then set he timer to 0
    It's in periodic mode so it was in some random value
  */
  if ( ROM_GPIOPinRead(GPIO_PORTG_BASE, GPIO_PIN_0) == GPIO_PIN_0){
    HWREG(TIMER2_BASE + TIMER_O_TAV) = 0; //Loads value 0 into the timer.
    ROM_TimerEnable(TIMER2_BASE,TIMER_A);
    echowait=1;
  }
  /*
    If it's a falling edge that was detected, then get the value of the counter
  */
  else{
    pulse = ROM_TimerValueGet(TIMER2_BASE,TIMER_A); //record value
    ROM_TimerDisable(TIMER2_BASE,TIMER_A);
    echowait=0;
  }


}

void Captureinit(){
  /*
    Set the timer to be periodic.
  */
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);
    ROM_SysCtlDelay(3);
    ROM_TimerConfigure(TIMER2_BASE, TIMER_CFG_PERIODIC_UP);
    ROM_TimerEnable(TIMER2_BASE,TIMER_A);
}

void sonar_send_pulse(void)
{
    //Does the required pulse of 10uS
    ROM_GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_1, GPIO_PIN_1);
    ROM_SysCtlDelay(266);
    ROM_GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_1, ~GPIO_PIN_1);
}

int sonar_get_echo_wait(void)
{
    return echowait;
}

int sonar_get_dis(void)
{
    //Converts the counter value to cm.
    pulse =(uint32_t)(temp * pulse);
    pulse = pulse / 58;
    return pulse;
}
