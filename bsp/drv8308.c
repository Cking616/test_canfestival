/*
 * drv8308.c
 *
 *  Created on: 2018Äê5ÔÂ17ÈÕ
 *      Author: cking
 */
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_gpio.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_i2c.h"
#include "inc/hw_ssi.h"
#include "inc/hw_timer.h"
#include "driverlib/pwm.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/i2c.h"
#include "driverlib/ssi.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "driverlib/systick.h"
#include "driverlib/rom.h"
#include "drv8308.h"

// for 3Phase ONLY
#define LED_GREEN_A GPIO_PORTG_BASE
#define LED_GREEN_B GPIO_PIN_3
#define RESET_A GPIO_PORTG_BASE
#define RESET_B GPIO_PIN_4
#define ENABLE_A GPIO_PORTG_BASE
#define ENABLE_B GPIO_PIN_5
#define DIR_A GPIO_PORTF_BASE
#define DIR_B GPIO_PIN_3
#define BRAKE_A GPIO_PORTF_BASE
#define BRAKE_B GPIO_PIN_2
#define SSI_CS_A GPIO_PORTA_BASE
#define SSI_CS_B GPIO_PIN_3

void PWM1IntHandler(void)
{

}


uint16_t ReadDrv( int Ad )
{
    uint32_t D[3];
    ROM_GPIOPinWrite( SSI_CS_A, SSI_CS_B, SSI_CS_B );
    ROM_SysCtlDelay( 26 ); //1uSec
    ROM_SSIDataPut(SSI0_BASE, 0x80 + Ad  );  // read address
    ROM_SSIDataPut(SSI0_BASE, 0  );
    SSIAdvDataPutFrameEnd( SSI0_BASE,  0 );
    while(ROM_SSIBusy(SSI0_BASE)) { }
    ROM_SysCtlDelay( 26 ); //1uSec
    ROM_GPIOPinWrite( SSI_CS_A, SSI_CS_B, 0 );
    ROM_SSIDataGet(SSI0_BASE, &D[0]);
    ROM_SSIDataGet(SSI0_BASE, &D[1]);
    ROM_SSIDataGet(SSI0_BASE, &D[2]);
    ROM_SysCtlDelay( 26 ); //1uSec
    return ((D[1] & 0xFF) << 8) | (D[2] & 0xFF);
}

void WriteDrv( int Ad, uint16_t Data )
{
    //uint32_t D[3];
    ROM_GPIOPinWrite( SSI_CS_A, SSI_CS_B, SSI_CS_B );
    ROM_SysCtlDelay( 26 ); //1uSec
    ROM_SSIDataPut(SSI0_BASE, Ad  );  // write address
    ROM_SSIDataPut(SSI0_BASE, (Data >> 8) & 0xFF );
    SSIAdvDataPutFrameEnd( SSI0_BASE,  Data & 0xFF );
    while(ROM_SSIBusy(SSI0_BASE)) { }
    ROM_SysCtlDelay( 26 ); //1uSec
    ROM_GPIOPinWrite( SSI_CS_A, SSI_CS_B, 0 );
    //SSIDataGet(SSI0_BASE, &D[0]);
    //SSIDataGet(SSI0_BASE, &D[1]);
    //SSIDataGet(SSI0_BASE, &D[2]);
    //SysCtlDelay( 26 ); //1uSec
}

void drv8308_init(void)
{
    uint32_t I;
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
    //ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
    ROM_SysCtlPWMClockSet( SYSCTL_PWMDIV_8 );  // 10Mhz --> 50Khz division is 25

    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);

    ROM_GPIOPinTypeGPIOOutput( RESET_A,  RESET_B );
    ROM_GPIOPinTypeGPIOOutput( ENABLE_A, ENABLE_B );
    ROM_GPIOPinTypeGPIOOutput( DIR_A,    DIR_B );
    ROM_GPIOPinTypeGPIOOutput( BRAKE_A,  BRAKE_B );
    ROM_GPIOPinWrite( RESET_A,  RESET_B,  0 );
    ROM_GPIOPinWrite( ENABLE_A, ENABLE_B, 0 );
    ROM_GPIOPinWrite( DIR_A,    DIR_B,    0 );
    ROM_GPIOPinWrite( BRAKE_A,  BRAKE_B,  0 );

    ROM_GPIOPinWrite( RESET_A,  RESET_B,  RESET_B );
    ROM_SysCtlDelay(80000);
    //Delay_ms(10);
    ROM_GPIOPinWrite( RESET_A,  RESET_B,  0 );


    // PWM yuli
    ROM_GPIOPinConfigure(GPIO_PA6_M1PWM2); // clock to Drv
    ROM_GPIOPinTypePWM(GPIO_PORTA_BASE, GPIO_PIN_6 );
    ROM_PWMGenConfigure(PWM1_BASE, PWM_GEN_1, PWM_GEN_MODE_NO_SYNC);  // Configure the PWM0 to count up/down without synchronization.
    ROM_PWMGenPeriodSet(PWM1_BASE, PWM_GEN_1, 200 );    // 50000 = 25Hz, 0xffff = 19.1hz
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_2, 10);
    ROM_PWMOutputState(PWM1_BASE, PWM_OUT_2_BIT, true);  // Enable the PWM Bits output signals.
    ROM_PWMGenEnable(PWM1_BASE, PWM_GEN_1);     // Enables the counter for a PWM generator block.

    ROM_GPIOPinConfigure(GPIO_PA2_SSI0CLK);
    ROM_GPIOPinConfigure(GPIO_PA4_SSI0RX);
    ROM_GPIOPinConfigure(GPIO_PA5_SSI0TX);
    ROM_GPIOPinTypeGPIOOutput( SSI_CS_A, SSI_CS_B );
    ROM_GPIOPinWrite( SSI_CS_A, SSI_CS_B, 0 );
    //      PA5 - SSI0Tx
    //      PA4 - SSI0Rx
    //      PA3 - SSI0Fss
    //      PA2 - SSI0CLK
    ROM_GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_2);
    ROM_SSIConfigSetExpClk(SSI0_BASE, ROM_SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,SSI_MODE_MASTER, 1000000, 8);
    ROM_SSIEnable(SSI0_BASE);
    // Read any residual data from the SSI port.
    while(ROM_SSIDataGetNonBlocking( SSI0_BASE, &I)) ;

     WriteDrv(0x2A, 0);  // clear errors
     WriteDrv(0x00,0x0011); // AG_SETPT[15:12]=191hz, SYNRECT[8]=1 PWMF[7:6]=50khz SPDMODE[5:4]=Clock Freq RETRY[0]=1
     WriteDrv(0x01,0);
     WriteDrv(0x02,0xff00);  // SPDREVS[15:8]=4 period before lock, MINSPD[7:0] min period x 2,56msec=450mSec, 25.6mSec = 39hz
     WriteDrv(0x03,0xe800);  // SPEEDTH[14:12]=1.56% MOD120=3970 scale  input dutt cycle
     WriteDrv(0x04,0x49d2);  // ESINE[8] TDRIVE[7:6]=5uS DTIME[5:3]=240nS IDRIVE[2:0]=10mA
//        WriteDrv(0x04,0x40d2);  // ESINE[8] TDRIVE[7:6]=5uS DTIME[5:3]=240nS IDRIVE[2:0]=10mA
     WriteDrv(0x04,0x40fa);  // ESINE[8] TDRIVE[7:6]=5uS DTIME[5:3]=240nS IDRIVE[2:0]=10mA
     WriteDrv(0x05,0x0000);  // INTCLK[14:12]=6.3Mhz SPDGAI?N [11:0] speed compensator gain
     WriteDrv(0x06,0x0000);  // HALLPOL[15] FILK1[11:0]
     WriteDrv(0x07,0x0000);  // FILK2[11:0]
     WriteDrv(0x08,0x0000);  // COMPK1[11:0]
     WriteDrv(0x09,0x0000);  // COMPK2[11:0]
     WriteDrv(0x0A,0x3000);  // OCPDEG[15:14]=5uS,6uS OCPTH[13:12]=1V LOOPGAIN[9:0]

    /*
     WriteDrv(0x2A, 0);  // clear errors
     WriteDrv(0x00,0x4141); // AG_SETPT[15:12]=48hz, SYNRECT[8]=1 PWMF[7:6]=50khz SPDMODE[5:4]=Clock Freq RETRY[0]=1
     WriteDrv(0x01,0);
     WriteDrv(0x02,0xff00);  // SPDREVS[15:8]=4 period before lock, MINSPD[7:0] min period x 2,56msec=450mSec, 25.6mSec = 39hz
     WriteDrv(0x03,0xe800);  // SPEEDTH[14:12]=1.56% MOD120=3970 scale  input dutt cycle
     WriteDrv(0x04,0x40fa);  // ESINE[8] TDRIVE[7:6]=5uS DTIME[5:3]=240nS IDRIVE[2:0]=10mA
     WriteDrv(0x05,0x0020);  // INTCLK[14:12]=6.3Mhz SPDGAI?N [11:0] speed compensator gain
     WriteDrv(0x06,0x0000);  // HALLPOL[15] FILK1[11:0]
     WriteDrv(0x07,0x0000);  // FILK2[11:0]
     WriteDrv(0x08,0x0000);  // COMPK1[11:0]
     WriteDrv(0x09,0x0000);  // COMPK2[11:0]
     WriteDrv(0x0A,0x3020);  // OCPDEG[15:14]=5uS,6uS OCPTH[13:12]=1V LOOPGAIN[9:0]
     WriteDrv(0x0B, 4000);
     */
}

void drv8308_set_pwm(int speed)
{
    // at least 3
    if(speed < 3)
        speed = 3;
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_2, speed);
}

void drv8308_enable(int on)
{
    ROM_GPIOPinWrite( ENABLE_A, ENABLE_B, on? ENABLE_B : 0 );
}

void drv8308_set_dir(int dir)
{
    ROM_GPIOPinWrite( DIR_A,    DIR_B,    dir? DIR_B: 0);
}

void drv8308_set_brake(int on)
{
    ROM_GPIOPinWrite( BRAKE_A,  BRAKE_B,  on? BRAKE_B: 0);
}
