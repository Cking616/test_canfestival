#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "bsp/driver.h"
#include "bsp/delay.h"
#include "utils/uartstdio.h"
#include "canfestival.h"
#include "Master.h"

unsigned char nodeID=0x7F;
extern CO_Data TestMaster_Data;

int main()
{
    driver_init_hardware();

    driver_init_system();
    UARTprintf("\n\nInit System!\n");
    UARTprintf("Address:%d!\n", driver_get_address());

    TestMaster_Data.heartbeatError = TestMaster_heartbeatError;
    TestMaster_Data.initialisation = TestMaster_initialisation;
    TestMaster_Data.preOperational = TestMaster_preOperational;
    TestMaster_Data.operational = TestMaster_operational;
    TestMaster_Data.stopped = TestMaster_stopped;
    TestMaster_Data.post_sync = TestMaster_post_sync;
    TestMaster_Data.post_TPDO = TestMaster_post_TPDO;
    TestMaster_Data.post_emcy = TestMaster_post_emcy;
    TestMaster_Data.post_SlaveBootup=TestMaster_post_SlaveBootup;
    TestMaster_Data.post_SlaveStateChange = TestMaster_post_SlaveStateChange;

    setNodeId(&TestMaster_Data, nodeID);
    driver_start_tick();
    setState(&TestMaster_Data, Initialisation);     // Init the state
    delay_ms(500);
    setState(&TestMaster_Data, Pre_operational);
    //masterRequestNodeState(&TestMaster_Data, 0x02);
    //delay_ms(500);
    //masterSendNMTstateChange(&TestMaster_Data, 0x02, NMT_Start_Node);
    //delay_ms(500);
    masterSendNMTstateChange(&TestMaster_Data, 0x02, NMT_Enter_PreOperational);
    while(1)
    {
        //masterRequestNodeState(&TestMaster_Data, 0x02);
        delay_ms(300);
        //sendSDO (CO_Data* d, UNS8 whoami, UNS8 CliServNbr, UNS8 *pData);
        //sendPDOevent (CO_Data* d);
    }
    return 1;
}
