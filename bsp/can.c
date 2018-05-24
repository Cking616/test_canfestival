/*
 * can.c
 *
 *  Created on: 2018Äê5ÔÂ2ÈÕ
 *      Author: Administrator
 */
#include <stdint.h>
#include <stdbool.h>
#include "can.h"
#include "inc/hw_memmap.h"
#include "inc/hw_can.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/rom.h"
#include "driverlib/can.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "canfestival.h"
#include "utils/uartstdio.h"

extern CO_Data TestMaster_Data;

// window ID:3  ID:0  ID:1  ID:2 door
// wheels: F:  0   2
//         R:  3   1
#define CAN_MAX_11BIT_MSG_ID    0x7ff

tCANMsgObject g_sCAN0RxMessage;
Message g_RxMessage;

tCANMsgObject g_sCAN0TxMessage;
#define RXOBJECT                6
#define TXOBJECT                2
volatile uint32_t g_ui32ErrFlag = 0;  // A global to keep track of the error flags that have been thrown so they may

volatile uint32_t g_ui32RXMsgCount = 0;   // A counter that keeps track of the number of times the TX & RX interrupt has occurred
volatile uint32_t g_ui32TXMsgCount = 0;

void can_init()
{
    // ************************************************************************************************************
    // Init CAN
    SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN0);     // The GPIO port and pins have been set up for CAN.

    ROM_GPIOPinTypeGPIOOutput( CANS_PORT, CANS_PIN );
    ROM_GPIOPinWrite(CANS_PORT, CANS_PIN, 0 );  // S = LOW

    ROM_IntMasterEnable();   // Enable processor interrupts.
    ROM_GPIOPinConfigure(GPIO_PB4_CAN0RX);      // Configure the GPIO pin muxing to select CAN0 functions for these pins.
    ROM_GPIOPinConfigure(GPIO_PB5_CAN0TX);
    ROM_GPIOPinTypeCAN(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5);  // Enable the alternate function on the GPIO pins.

    ROM_CANInit(CAN0_BASE);     // Initialize the CAN controller
    ROM_CANBitRateSet(CAN0_BASE, ROM_SysCtlClockGet(), 1000000);  // Set up the bit rate for the CAN bus.
    ROM_CANIntEnable(CAN0_BASE, CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS);  // Enable interrupts on the CAN peripheral. Name of the handler is in the vector table of startup code.
    ROM_IntEnable(INT_CAN0);   // Enable the CAN interrupt on the processor (NVIC).
    ROM_CANEnable(CAN0_BASE);   // Enable the CAN for operation.
}
//*****************************************************************************
//
// Can ERROR handling. When a message is received if there is an erro it is
// saved to g_ui32ErrFlag, the Error Flag Set. Below the flags are checked
// and cleared. It is left up to the user to add handling fuctionality if so
// desiered.
//
// For more information on the error flags please see the CAN section of the
// microcontroller datasheet.
//
// NOTE: you may experience errors during setup when only one board is powered
// on. This is caused by one board sending signals and there not being another
// board there to acknoledge it. Dont worry about these errors, they can be
// disregarded.
//
//*****************************************************************************
void
CANErrorHandler(void)
{
    //
    // CAN controller has entered a Bus Off state.
    //
    if(g_ui32ErrFlag & CAN_STATUS_BUS_OFF)
    {
        g_ui32ErrFlag &= ~(CAN_STATUS_BUS_OFF);
    }

    // CAN controller error level has reached warning level.
    if(g_ui32ErrFlag & CAN_STATUS_EWARN)
    {
        g_ui32ErrFlag &= ~(CAN_STATUS_EWARN);
    }

    // CAN controller error level has reached error passive level.
    if(g_ui32ErrFlag & CAN_STATUS_EPASS)
    {
        g_ui32ErrFlag &= ~(CAN_STATUS_EPASS);
    }

    // A message was received successfully since the last read of this status.
    if(g_ui32ErrFlag & CAN_STATUS_RXOK)
    {
        g_ui32ErrFlag &= ~(CAN_STATUS_RXOK);
    }

    // A message was transmitted successfully since the last read of this status.
    if(g_ui32ErrFlag & CAN_STATUS_TXOK)
    {
        g_ui32ErrFlag &= ~(CAN_STATUS_TXOK);
    }

    // This is the mask for the last error code field.
    if(g_ui32ErrFlag & CAN_STATUS_LEC_MSK)
    {
        // Clear CAN_STATUS_LEC_MSK Flag
        g_ui32ErrFlag &= ~(CAN_STATUS_LEC_MSK);
    }

    // A bit stuffing error has occurred.
    if(g_ui32ErrFlag & CAN_STATUS_LEC_STUFF)
    {
        g_ui32ErrFlag &= ~(CAN_STATUS_LEC_STUFF);
    }

    // A formatting error has occurred.
    if(g_ui32ErrFlag & CAN_STATUS_LEC_FORM)
    {
        g_ui32ErrFlag &= ~(CAN_STATUS_LEC_FORM);
    }

    // An acknowledge error has occurred.
    if(g_ui32ErrFlag & CAN_STATUS_LEC_ACK)
    {
        // Clear CAN_STATUS_LEC_ACK Flag
        g_ui32ErrFlag &= ~(CAN_STATUS_LEC_ACK);
    }

    // The bus remained a bit level of 1 for longer than is allowed.
    if(g_ui32ErrFlag & CAN_STATUS_LEC_BIT1)
    {
        g_ui32ErrFlag &= ~(CAN_STATUS_LEC_BIT1);
    }

    // The bus remained a bit level of 0 for longer than is allowed.
    if(g_ui32ErrFlag & CAN_STATUS_LEC_BIT0)
    {
        g_ui32ErrFlag &= ~(CAN_STATUS_LEC_BIT0);
    }

    // A CRC error has occurred.
    if(g_ui32ErrFlag & CAN_STATUS_LEC_CRC)
    {
        // Clear CAN_STATUS_LEC_CRC Flag
        g_ui32ErrFlag &= ~(CAN_STATUS_LEC_CRC);
    }

    // This is the mask for the CAN Last Error Code (LEC).
    if(g_ui32ErrFlag & CAN_STATUS_LEC_MASK)
    {
        g_ui32ErrFlag &= ~(CAN_STATUS_LEC_MASK);
    }

    // If there are any bits still set in g_ui32ErrFlag then something unhandled
    // has happened. Print the value of g_ui32ErrFlag.
    if(g_ui32ErrFlag !=0)
    {
    }
}



//*****************************************************************************
//
// CAN 0 Interrupt Handler. It checks for the cause of the interrupt, and
// maintains a count of all messages that have been transmitted / received
//
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
void CAN0IntHandler(void)
{
    uint32_t ui32Status;
    // Read the CAN interrupt status to find the cause of the interrupt
    // CAN_INT_STS_CAUSE register values
    // 0x0000        = No Interrupt Pending
    // 0x0001-0x0020 = Number of message object that caused the interrupt
    // 0x8000        = Status interrupt
    // all other numbers are reserved and have no meaning in this system
    ui32Status = ROM_CANIntStatus(CAN0_BASE, CAN_INT_STS_CAUSE);
    // If this was a status interrupt acknowledge it by reading the CAN controller status register.
    if(ui32Status == CAN_INT_INTID_STATUS)
    {
        // Read the controller status.  This will return a field of status
        // error bits that can indicate various errors. Refer to the
        // API documentation for details about the error status bits.
        // The act of reading this status will clear the interrupt.
        ui32Status = ROM_CANStatusGet(CAN0_BASE, CAN_STS_CONTROL);

        // Add ERROR flags to list of current errors.
        g_ui32ErrFlag |= ui32Status;
    }

    // Check if the cause is message object RXOBJECT, which we are using
    // for receiving messages.
    else if(ui32Status == RXOBJECT)
    {
        // Reuse the same message object that was used earlier to configure
        // the CAN for receiving messages.  A buffer for storing the
        // received data must also be provided, so set the buffer pointer
        // within the message object.

        g_sCAN0RxMessage.pui8MsgData = (uint8_t *)  g_RxMessage.data;
        g_sCAN0RxMessage.ui32MsgLen = sizeof(g_RxMessage.data);

        // Read the message from the CAN.  Message object RXOBJECT is used
        // (which is not the same thing as CAN ID).  The interrupt clearing
        // flag is not set because this interrupt was already cleared in
        // the interrupt handler.
        //if ( g_bRXFlag == 0 ) {
        //int i = 0;
        ROM_CANMessageGet(CAN0_BASE, RXOBJECT, &g_sCAN0RxMessage, 0);
        //for(i = 0; i < g_sCAN0RxMessage.ui32MsgLen; i++)
        //    g_RxMessage.data[i] =  g_sCAN0RxMessage.pui8MsgData[i];
        // g_RxMessage.len = g_sCAN0RxMessage.ui32MsgLen;
        g_RxMessage.len = g_sCAN0RxMessage.ui32MsgLen;
        if(g_sCAN0RxMessage.ui32Flags & MSG_OBJ_REMOTE_FRAME)
        {
            g_RxMessage.rtr = 1;
        }
        else
        {
            g_RxMessage.rtr = 0;
        }

        g_RxMessage.cob_id = g_sCAN0RxMessage.ui32MsgID;
        //UARTprintf("Rec len:%d,Rec data[0]:0x%X\n",   g_RxMessage.len, g_RxMessage.data[0]);
        //}

        canDispatch(&TestMaster_Data, &g_RxMessage);
        // Clear the message object interrupt.
        while(HWREG(CAN0_BASE + CAN_O_IF2CRQ) & CAN_IF1CRQ_BUSY) { }
        // Only change the interrupt pending state by setting only the CAN_IF1CMSK_CLRINTPND bit.
        HWREG(CAN0_BASE + CAN_O_IF2CMSK) |= CAN_IF1CMSK_CLRINTPND;
        // Send the clear pending interrupt command to the CAN controller.
        HWREG(CAN0_BASE + CAN_O_IF2CRQ) = RXOBJECT;
        // Wait to be sure that this interface is not busy.
        while(HWREG(CAN0_BASE + CAN_O_IF2CRQ) & CAN_IF1CRQ_BUSY) { }

        // Increment a counter to keep track of how many messages have been
        // received.  In a real application this could be used to set flags to
        // indicate when a message is received.
        g_ui32RXMsgCount++;

        // Since a message was received, clear any error flags.
        // This is done because before the message is received it triggers
        // a Status Interrupt for RX complete. by clearing the flag here we
        // prevent unnecessary error handling from happeneing
        g_ui32ErrFlag = 0;
    }

    // Check if the cause is message object TXOBJECT, which we are using
    // for transmitting messages.
    else if(ui32Status == TXOBJECT)
    {
        // Getting to this point means that the TX interrupt occurred on
        // message object TXOBJECT, and the message reception is complete.
        // Clear the message object interrupt.
        HWREG(CAN0_BASE + CAN_O_IF1CMSK) = CAN_IF1CMSK_WRNRD | CAN_IF1CMSK_ARB | CAN_IF1CMSK_CONTROL;
        HWREG(CAN0_BASE + CAN_O_IF1ARB1) = 0;
        HWREG(CAN0_BASE + CAN_O_IF1ARB2) = 0;
        HWREG(CAN0_BASE + CAN_O_IF1MCTL) = 0;
        HWREG(CAN0_BASE + CAN_O_IF1CRQ) = TXOBJECT;  // Initiate programming the message object

        ROM_CANIntClear(CAN0_BASE, TXOBJECT);

        // Increment a counter to keep track of how many messages have been
        // transmitted. In a real application this could be used to set
        // flags to indicate when a message is transmitted.
        g_ui32TXMsgCount++;

        // Since a message was transmitted, clear any error flags.
        // This is done because before the message is transmitted it triggers
        // a Status Interrupt for TX complete. by clearing the flag here we
        // prevent unnecessary error handling from happeneing
        g_ui32ErrFlag = 0;
    }
    // Otherwise, something unexpected caused the interrupt.  This should never happen.
    else
    {
        // Spurious interrupt handling can go here.
    }
}



//*****************************************************************************
//*****************************************************************************
void can_start_listening()
{
    // Initialize a message object to be used for receiving CAN messages with
    // any CAN ID.  In order to receive any CAN ID, the ID and mask must both
    // be set to 0, and the ID filter enabled.
    //
    g_sCAN0RxMessage.ui32MsgID = 0;
    g_sCAN0RxMessage.ui32MsgIDMask = 0; // accept ALL
    g_sCAN0RxMessage.ui32Flags = MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER;
    g_sCAN0RxMessage.ui32MsgLen = 8;

    //
    // Now load the message object into the CAN peripheral.  Once loaded the
    // CAN will receive any message on the bus, and an interrupt will occur.
    // Use message object RXOBJECT for receiving messages (this is not the
    //same as the CAN ID which can be any value in this example).
    //
    ROM_CANMessageSet(CAN0_BASE, RXOBJECT, &g_sCAN0RxMessage, MSG_OBJ_TYPE_RX);
}

//*****************************************************************************
//*****************************************************************************
void can_write(uint32_t id, uint32_t len,  uint8_t mode, uint8_t* data )
{
    g_sCAN0TxMessage.ui32MsgID = id;
    g_sCAN0TxMessage.ui32MsgIDMask = 0;
    if(mode == 1)
    {
        g_sCAN0TxMessage.ui32Flags = MSG_OBJ_TX_INT_ENABLE | MSG_OBJ_REMOTE_FRAME;
    }
    else
    {
        g_sCAN0TxMessage.ui32Flags = MSG_OBJ_TX_INT_ENABLE;
    }
    g_sCAN0TxMessage.ui32MsgLen = len;
    g_sCAN0TxMessage.pui8MsgData = data;
    ROM_CANRetrySet( CAN0_BASE, 0 );
    ROM_CANMessageSet(CAN0_BASE, TXOBJECT, &g_sCAN0TxMessage, MSG_OBJ_TYPE_TX);
}
