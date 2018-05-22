#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "bsp/driver.h"
#include "bsp/delay.h"
#include "utils/uartstdio.h"
#include "canfestival.h"

unsigned char nodeID=0x7F;
extern CO_Data TestSlave_Data;

int main()
{
    driver_init_hardware();

    driver_init_system();
    UARTprintf("\n\nInit System!\n");
    UARTprintf("Address:%d!\n", driver_get_address());
    setState(&TestSlave_Data, Initialisation);     // Init the state
    setNodeId(&TestSlave_Data, nodeID);
    driver_start_tick();
    setState(&TestSlave_Data, Operational);
    while(1)
    {
        masterSendNMTstateChange(&TestSlave_Data, 0x00, NMT_Start_Node);
        delay_ms(10);
        //sendSDO (CO_Data* d, UNS8 whoami, UNS8 CliServNbr, UNS8 *pData);
        //sendPDOevent (CO_Data* d);
    }
    return 1;
}
