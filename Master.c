/*
This file is part of CanFestival, a library implementing CanOpen Stack. 

Copyright (C): Edouard TISSERANT and Francis DUPIN

See COPYING file for copyrights details.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "Master.h"
#include "canfestival.h"
#include "utils/uartstdio.h"
#include "bsp/delay.h"


UNS8 __is_sending = 0;

static void CheckSDOAndContinue(CO_Data* d, UNS8 nodeId)
{
	UNS32 abortCode;
	if (getWriteResultNetworkDict(d, nodeId, &abortCode) != SDO_FINISHED)
		UARTprintf("Master : Failed in initializing slave %X, AbortCode :%X \n", nodeId, abortCode);

	/* Finalise last SDO transfer with this node */
	closeSDOtransfer(&TestMaster_Data, nodeId, SDO_CLIENT);

	__is_sending = 0;
}

void CanOpen_Reset_TPDO(CO_Data* d, UNS8 nodeId)
{
	UNS8 data8 = 0x01;
	int32_t data = 0;

	data8 = 0x0;
	writeNetworkDictCallBack(d, /*CO_Data* d*/
		nodeId, /*UNS8 nodeId*/
		0x1a00, /*UNS16 index*/
		0x00, /*UNS16 index*/
		1, /*UNS8 count*/
		0, /*UNS8 dataType*/
		&data8,/*void *data*/
		CheckSDOAndContinue, /*SDOCallback_t Callback*/
		0); /* use block mode */
	__is_sending = 1;
	while (__is_sending) { delay_us(50); }


	data = (0x180 + 0x2) + 0x80000000;
	writeNetworkDictCallBack(d, /*CO_Data* d*/
		nodeId, /*UNS8 nodeId*/
		0x1800, /*UNS16 index*/
		0x01, /*UNS16 index*/
		4, /*UNS8 count*/
		uint32, /*UNS8 dataType*/
		&data,/*void *data*/
		CheckSDOAndContinue, /*SDOCallback_t Callback*/
		0); /* use block mode */
	__is_sending = 1;
	while (__is_sending) { delay_us(50); }


	delay_ms(5);
	data = 0x60410010;
	writeNetworkDictCallBack(d, /*CO_Data* d*/
		nodeId, /*UNS8 nodeId*/
		0x1a00, /*UNS16 index*/
		0x01, /*UNS16 index*/
		4, /*UNS8 count*/
		uint32, /*UNS8 dataType*/
		&data,/*void *data*/
		CheckSDOAndContinue, /*SDOCallback_t Callback*/
		0); /* use block mode */
	__is_sending = 1;
	while (__is_sending) { delay_us(50); }


	data = 0x60640020;
	writeNetworkDictCallBack(d, /*CO_Data* d*/
		nodeId, /*UNS8 nodeId*/
		0x1a00, /*UNS16 index*/
		0x02, /*UNS16 index*/
		4, /*UNS8 count*/
		uint32, /*UNS8 dataType*/
		&data,/*void *data*/
		CheckSDOAndContinue, /*SDOCallback_t Callback*/
		0); /* use block mode */
	__is_sending = 1;
	while (__is_sending) { delay_us(50); }

	delay_ms(5);
	data8 = 0x2;
	writeNetworkDictCallBack(d, /*CO_Data* d*/
		nodeId, /*UNS8 nodeId*/
		0x1a00, /*UNS16 index*/
		0x00, /*UNS16 index*/
		1, /*UNS8 count*/
		0, /*UNS8 dataType*/
		&data8,/*void *data*/
		CheckSDOAndContinue, /*SDOCallback_t Callback*/
		0); /* use block mode */
	__is_sending = 1;
	while (__is_sending) { delay_us(50); }


	delay_ms(10);
	data8 = 0x1;
	writeNetworkDictCallBack(d, /*CO_Data* d*/
		nodeId, /*UNS8 nodeId*/
		0x1800, /*UNS16 index*/
		0x02, /*UNS8 subindex*/
		1, /*UNS8 count*/
		0, /*UNS8 dataType*/
		&data8,/*void *data*/
		CheckSDOAndContinue, /*SDOCallback_t Callback*/
		0); /* use block mode */
	__is_sending = 1;
	while (__is_sending) { delay_us(50); }

	data = 0x182;
	writeNetworkDictCallBack(d, /*CO_Data* d*/
		nodeId, /*UNS8 nodeId*/
		0x1800, /*UNS16 index*/
		0x01, /*UNS16 index*/
		4, /*UNS8 count*/
		uint32, /*UNS8 dataType*/
		&data,/*void *data*/
		CheckSDOAndContinue, /*SDOCallback_t Callback*/
		0); /* use block mode */
	__is_sending = 1;
	while (__is_sending) { delay_us(50); }
}

void CanOpen_Change_Param(CO_Data* d, UNS8 nodeId)
{
	UNS8 data8 = 0x01;
	int32_t data = 0;

	data = 3600;
	writeNetworkDictCallBack(&TestMaster_Data, /*CO_Data* d*/
		2, /*UNS8 nodeId*/
		0x6081, /*UNS16 index*/
		0x00, /*UNS16 index*/
		4, /*UNS8 count*/
		uint32, /*UNS8 dataType*/
		&data,/*void *data*/
		CheckSDOAndContinue, /*SDOCallback_t Callback*/
		0); /* use block mode */
	__is_sending = 1;
	while (__is_sending) { delay_us(50); }

    data = 800000;
    writeNetworkDictCallBack(&TestMaster_Data, /*CO_Data* d*/
        2, /*UNS8 nodeId*/
        0x607a, /*UNS16 index*/
        0x00, /*UNS16 index*/
        4, /*UNS8 count*/
        int32, /*UNS8 dataType*/
        &data,/*void *data*/
        CheckSDOAndContinue, /*SDOCallback_t Callback*/
        0); /* use block mode */
    __is_sending = 1;
    while (__is_sending) { delay_us(50); }

	data = 1;
	writeNetworkDictCallBack(&TestMaster_Data, /*CO_Data* d*/
		2, /*UNS8 nodeId*/
		0x3202, /*UNS16 index*/
		0x00, /*UNS16 index*/
		4, /*UNS8 count*/
		uint32, /*UNS8 dataType*/
		&data,/*void *data*/
		CheckSDOAndContinue, /*SDOCallback_t Callback*/
		0); /* use block mode */
	__is_sending = 1;
	while (__is_sending) { delay_us(50); }

	data8 = 1;
	writeNetworkDictCallBack(&TestMaster_Data, /*CO_Data* d*/
		2, /*UNS8 nodeId*/
		0x6060, /*UNS16 index*/
		0x00, /*UNS16 index*/
		1, /*UNS8 count*/
		0, /*UNS8 dataType*/
		&data,/*void *data*/
		CheckSDOAndContinue, /*SDOCallback_t Callback*/
		0); /* use block mode */
	__is_sending = 1;
	while (__is_sending) { delay_us(50); }
}

void CanOpen_Reset_RPDO(CO_Data* d, UNS8 nodeId)
{
	int32_t data;
	UNS8 data8;

	data = 0x0202;
	writeNetworkDictCallBack(d, /*CO_Data* d*/
		nodeId, /*UNS8 nodeId*/
		0x1400, /*UNS16 index*/
		0x01, /*UNS16 index*/
		4, /*UNS8 count*/
		uint32, /*UNS8 dataType*/
		&data,/*void *data*/
		CheckSDOAndContinue, /*SDOCallback_t Callback*/
		0); /* use block mode */
	__is_sending = 1;
	while (__is_sending) { delay_us(50); }

    data8 = 0xFF;
    writeNetworkDictCallBack(d, /*CO_Data* d*/
        nodeId, /*UNS8 nodeId*/
        0x1400, /*UNS16 index*/
        0x02, /*UNS8 subindex*/
        1, /*UNS8 count*/
        0, /*UNS8 dataType*/
        &data8,/*void *data*/
        CheckSDOAndContinue, /*SDOCallback_t Callback*/
        0); /* use block mode */
    __is_sending = 1;
    while (__is_sending) { delay_us(50); }

    data = 0x0302;
    writeNetworkDictCallBack(d, /*CO_Data* d*/
        nodeId, /*UNS8 nodeId*/
        0x1401, /*UNS16 index*/
        0x01, /*UNS16 index*/
        4, /*UNS8 count*/
        uint32, /*UNS8 dataType*/
        &data,/*void *data*/
        CheckSDOAndContinue, /*SDOCallback_t Callback*/
        0); /* use block mode */
    __is_sending = 1;
    while (__is_sending) { delay_us(50); }

    data8 = 0xFF;
    writeNetworkDictCallBack(d, /*CO_Data* d*/
        nodeId, /*UNS8 nodeId*/
        0x1401, /*UNS16 index*/
        0x02, /*UNS8 subindex*/
        1, /*UNS8 count*/
        0, /*UNS8 dataType*/
        &data8,/*void *data*/
        CheckSDOAndContinue, /*SDOCallback_t Callback*/
        0); /* use block mode */
    __is_sending = 1;
    while (__is_sending) { delay_us(50); }
}

/*****************************************************************************/
void TestMaster_heartbeatError(CO_Data* d, UNS8 heartbeatID)
{
	UARTprintf("TestMaster_heartbeatError %d\n", heartbeatID);
}

/********************************************************
 * ConfigureSlaveNode is responsible to
 *  - setup master RPDO 1 to receive TPDO 1 from id 2
 *  - setup master RPDO 2 to receive TPDO 2 from id 2
 ********************************************************/
void TestMaster_initialisation(CO_Data* d)
{
	UARTprintf("TestMaster_initialisation\n");

	/*****************************************
	 * Define RPDOs to match slave ID=2 TPDOs*
	 *****************************************/
}

// Step counts number of times ConfigureSlaveNode is called
static int init_step = 0;

/********************************************************
 * ConfigureSlaveNode is responsible to
 *  - setup slave TPDO 1 transmit type
 *  - setup slave TPDO 2 transmit type
 *  - switch to operational mode
 *  - send NMT to slave
 ********************************************************
 * This an example of :
 * Network Dictionary Access (SDO) with Callback 
 * Slave node state change request (NMT) 
 ********************************************************
 * This is called first by TestMaster_post_SlaveBootup
 * then it called again each time a SDO exchange is
 * finished.
 ********************************************************/
 
void ConfigureSlaveNode(CO_Data* d, UNS8 nodeId)
{
	/* Master configure heartbeat producer time at 1000 ms 
	 * for slave node-id 0x02 by DCF concise */
	 
	UARTprintf("Master : ConfigureSlaveNode %X\n", nodeId);
	CanOpen_Reset_RPDO(d, nodeId);
	CanOpen_Reset_TPDO(d, nodeId);
	CanOpen_Change_Param(d, nodeId);
	/* Put the master in operational mode */
	setState(d, Operational);
	Contolword2 = 0x86;
	/* Ask slave node to go in operational mode */
	masterSendNMTstateChange (d, nodeId, NMT_Start_Node);
}


void TestMaster_preOperational(CO_Data* d)
{
	UARTprintf("TestMaster_preOperational\n");
}

void TestMaster_operational(CO_Data* d)
{
	UARTprintf("TestMaster_operational\n");
}

void TestMaster_stopped(CO_Data* d)
{
	UARTprintf("TestMaster_stopped\n");
}

void TestMaster_post_sync(CO_Data* d)
{
}

void TestMaster_post_emcy(CO_Data* d, UNS8 nodeID, UNS16 errCode, UNS8 errReg)
{
	UARTprintf("Master received EMCY message. Node: %X  ErrorCode: %X  ErrorRegister: %X\n", nodeID, errCode, errReg);
}

char query_result = 0;
char waiting_answer = 0;

static void CheckSDO(CO_Data* d, UNS8 nodeId)
{
	UNS32 abortCode;	
	if(getWriteResultNetworkDict (d, nodeId, &abortCode) != SDO_FINISHED)
		UARTprintf("Master : Failed in changing Slave's transmit type AbortCode :%X \n", abortCode);
	UARTprintf("Master :Sended\n");
	/* Finalise last SDO transfer with this node */
	closeSDOtransfer(&TestMaster_Data, nodeId, SDO_CLIENT);
}



void TestMaster_post_TPDO(CO_Data* d)
{
    static int MasterSyncCount = 0;

	if(MasterSyncCount % 2 == 0){
	    //sendOnePDOevent(&TestMaster_Data, 0x0);

	    sendPDOevent(&TestMaster_Data);
        sendPDOrequest(&TestMaster_Data, 0x1400 );
	}
	MasterSyncCount++;
}

void TestMaster_post_SlaveBootup(CO_Data* d, UNS8 nodeid)
{
	UARTprintf("TestMaster_post_SlaveBootup %x\n", nodeid);
}

void TestMaster_post_SlaveStateChange(CO_Data* d, UNS8 nodeId, e_nodeState newNodeState)
{
    UARTprintf("TestMaster_post_SlaveStateChange %x\n", nodeId);
}
