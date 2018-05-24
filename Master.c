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

//#define eprint(...) UARTprintf(...)
//extern s_BOARD MasterBoard;
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
	UNS32 PDO1_COBID = 0x0182;
	UNS32 PDO2_COBID = 0x0282;
	UNS32 size = sizeof(UNS32); 
	//UNS32 SINC_cicle=0;
	//UNS8 data_type = 0;
	
	UARTprintf("TestMaster_initialisation\n");

	/*****************************************
	 * Define RPDOs to match slave ID=2 TPDOs*
	 *****************************************/
#ifdef 0
	writeLocalDict( &TestMaster_Data, /*CO_Data* d*/
			0x1400, /*UNS16 index*/
			0x01, /*UNS8 subind*/ 
			&PDO1_COBID, /*void * pSourceData,*/ 
			&size, /* UNS8 * pExpectedSize*/
			RW);  /* UNS8 checkAccess */
			
	writeLocalDict( &TestMaster_Data, /*CO_Data* d*/
			0x1401, /*UNS16 index*/
			0x01, /*UNS8 subind*/ 
			&PDO2_COBID, /*void * pSourceData,*/ 
			&size, /* UNS8 * pExpectedSize*/
			RW);  /* UNS8 checkAccess */
#endif
}

// Step counts number of times ConfigureSlaveNode is called
static int init_step = 0;

/*Froward declaration*/
static void ConfigureSlaveNode(CO_Data* d, UNS8 nodeId);

/**/
static void CheckSDOAndContinue(CO_Data* d, UNS8 nodeId)
{
	UNS32 abortCode;	
	if(getWriteResultNetworkDict (d, nodeId, &abortCode) != SDO_FINISHED)
		UARTprintf("Master : Failed in initializing slave %X, step %d, AbortCode :%X \n", nodeId, init_step, abortCode);

	/* Finalise last SDO transfer with this node */
	closeSDOtransfer(&TestMaster_Data, nodeId, SDO_CLIENT);

	ConfigureSlaveNode(d, nodeId);
}

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
 
static void ConfigureSlaveNode(CO_Data* d, UNS8 nodeId)
{
	/* Master configure heartbeat producer time at 1000 ms 
	 * for slave node-id 0x02 by DCF concise */
	 
	//UNS8 Transmission_Type = 0x01;
	UNS8 Transmission_Type = 0xFF;
	int32_t data = 0;
	UNS8 res;
	UARTprintf("Master : ConfigureSlaveNode %X\n", nodeId);

	switch(++init_step){
		case 1: /*First step : setup Slave's TPDO 1 to be transmitted on SYNC*/
//#ifdef 0
			UARTprintf("Master : set slave %X TPDO 1 transmit type\n", nodeId);
			res = writeNetworkDictCallBack (d, /*CO_Data* d*/
					nodeId, /*UNS8 nodeId*/
					0x1800, /*UNS16 index*/
					0x02, /*UNS8 subindex*/
					1, /*UNS8 count*/
					0, /*UNS8 dataType*/
					&Transmission_Type,/*void *data*/
					CheckSDOAndContinue, /*SDOCallback_t Callback*/
                    0); /* use block mode */
					break;
//#endif
		case 2:	/*Second step*/
//#ifdef 0
			UARTprintf("Master : set slave %X TPDO 2 transmit type\n", nodeId);
			writeNetworkDictCallBack (d, /*CO_Data* d*/
					nodeId, /*UNS8 nodeId*/
					0x1801, /*UNS16 index*/
					0x02, /*UNS16 index*/
					1, /*UNS8 count*/
					0, /*UNS8 dataType*/
					&Transmission_Type,/*void *data*/
					CheckSDOAndContinue, /*SDOCallback_t Callback*/
                    0); /* use block mode */
					break;
//#endif
        case 3:
            data = 0x8;
            writeNetworkDictCallBack (d, /*CO_Data* d*/
                     nodeId, /*UNS8 nodeId*/
                     0x3202, /*UNS16 index*/
                     0x00, /*UNS16 index*/
                     4, /*UNS8 count*/
                     uint32, /*UNS8 dataType*/
                     &data,/*void *data*/
                     CheckSDOAndContinue, /*SDOCallback_t Callback*/
                     0); /* use block mode */
                     break;
        case 4:
            data = 600;
            writeNetworkDictCallBack (d, /*CO_Data* d*/
                     nodeId, /*UNS8 nodeId*/
                     0x2031, /*UNS16 index*/
                     0x00, /*UNS16 index*/
                     4, /*UNS8 count*/
                     uint32, /*UNS8 dataType*/
                     &data,/*void *data*/
                     CheckSDOAndContinue, /*SDOCallback_t Callback*/
                     0); /* use block mode */
                     break;
        case 5:
            data = -50;
            writeNetworkDictCallBack (d, /*CO_Data* d*/
                     nodeId, /*UNS8 nodeId*/
                     0x2037, /*UNS16 index*/
                     0x00, /*UNS16 index*/
                     4, /*UNS8 count*/
                     int32, /*UNS8 dataType*/
                     &data,/*void *data*/
                     CheckSDOAndContinue, /*SDOCallback_t Callback*/
                     0); /* use block mode */
                     break;
        case 6:
            data = 512;
            writeNetworkDictCallBack (d, /*CO_Data* d*/
                     nodeId, /*UNS8 nodeId*/
                     0x2057, /*UNS16 index*/
                     0x00, /*UNS16 index*/
                     4, /*UNS8 count*/
                     int32, /*UNS8 dataType*/
                     &data,/*void *data*/
                     CheckSDOAndContinue, /*SDOCallback_t Callback*/
                     0); /* use block mode */
                     break;
        case 7:
            data = 4;
            writeNetworkDictCallBack (d, /*CO_Data* d*/
                     nodeId, /*UNS8 nodeId*/
                     0x2058, /*UNS16 index*/
                     0x00, /*UNS16 index*/
                     4, /*UNS8 count*/
                     int32, /*UNS8 dataType*/
                     &data,/*void *data*/
                     CheckSDOAndContinue, /*SDOCallback_t Callback*/
                     0); /* use block mode */
                     break;
        case 8:
            data = 500;
            writeNetworkDictCallBack (d, /*CO_Data* d*/
                     nodeId, /*UNS8 nodeId*/
                     0x6048, /*UNS16 index*/
                     0x01, /*UNS16 index*/
                     4, /*UNS8 count*/
                     uint32, /*UNS8 dataType*/
                     &data,/*void *data*/
                     CheckSDOAndContinue, /*SDOCallback_t Callback*/
                     0); /* use block mode */
                     break;
        case 9:
            data = 1;
            writeNetworkDictCallBack (d, /*CO_Data* d*/
                     nodeId, /*UNS8 nodeId*/
                     0x6048, /*UNS16 index*/
                     0x02, /*UNS16 index*/
                     2, /*UNS8 count*/
                     uint16, /*UNS8 dataType*/
                     &data,/*void *data*/
                     CheckSDOAndContinue, /*SDOCallback_t Callback*/
                     0); /* use block mode */
                     break;
        case 10:
            data = 500;
            writeNetworkDictCallBack (d, /*CO_Data* d*/
                     nodeId, /*UNS8 nodeId*/
                     0x6049, /*UNS16 index*/
                     0x01, /*UNS16 index*/
                     4, /*UNS8 count*/
                     uint32, /*UNS8 dataType*/
                     &data,/*void *data*/
                     CheckSDOAndContinue, /*SDOCallback_t Callback*/
                     0); /* use block mode */
                     break;
        case 11:
            data = 1;
            writeNetworkDictCallBack (d, /*CO_Data* d*/
                     nodeId, /*UNS8 nodeId*/
                     0x6049, /*UNS16 index*/
                     0x02, /*UNS16 index*/
                     2, /*UNS8 count*/
                     uint16, /*UNS8 dataType*/
                     &data,/*void *data*/
                     CheckSDOAndContinue, /*SDOCallback_t Callback*/
                     0); /* use block mode */
                     break;
        case 12:
            data = 150;
            writeNetworkDictCallBack (d, /*CO_Data* d*/
                     nodeId, /*UNS8 nodeId*/
                     0x604C, /*UNS16 index*/
                     0x01, /*UNS16 index*/
                     4, /*UNS8 count*/
                     int32, /*UNS8 dataType*/
                     &data,/*void *data*/
                     CheckSDOAndContinue, /*SDOCallback_t Callback*/
                     0); /* use block mode */
                     break;
        case 13:
            data = 60000;
            writeNetworkDictCallBack (d, /*CO_Data* d*/
                     nodeId, /*UNS8 nodeId*/
                     0x604C, /*UNS16 index*/
                     0x02, /*UNS16 index*/
                     4, /*UNS8 count*/
                     int32, /*UNS8 dataType*/
                     &data,/*void *data*/
                     CheckSDOAndContinue, /*SDOCallback_t Callback*/
                     0); /* use block mode */
                     break;
        case 14:
            data = 1000;
            writeNetworkDictCallBack (d, /*CO_Data* d*/
                     nodeId, /*UNS8 nodeId*/
                     0x6042, /*UNS16 index*/
                     0x00, /*UNS16 index*/
                     2, /*UNS8 count*/
                     int16, /*UNS8 dataType*/
                     &data,/*void *data*/
                     CheckSDOAndContinue, /*SDOCallback_t Callback*/
                     0); /* use block mode */
                     break;
        case 15:
            data = 1;
            writeNetworkDictCallBack (d, /*CO_Data* d*/
                     nodeId, /*UNS8 nodeId*/
                     0x6060, /*UNS16 index*/
                     0x00, /*UNS16 index*/
                     1, /*UNS8 count*/
                     int8, /*UNS8 dataType*/
                     &data,/*void *data*/
                     CheckSDOAndContinue, /*SDOCallback_t Callback*/
                     0); /* use block mode */
                     break;
		default :
		
		/****************************** START *******************************/
		
			/* Put the master in operational mode */
			setState(d, Operational);
		 
			/* Ask slave node to go in operational mode */
			masterSendNMTstateChange (d, nodeId, NMT_Start_Node);
			
	}
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
    /*
	UARTprintf("TestMaster_post_sync\n");
	UARTprintf("Master: %d %d %d %d %d %d %d %d %d %x %x %d %d\n",
		MasterMap1,
		MasterMap2,
		MasterMap3, 
		MasterMap4,
		MasterMap5,
		MasterMap6,
		MasterMap7,
		MasterMap8,
		MasterMap9,
		MasterMap10,
		MasterMap11,
		MasterMap12,
		MasterMap13);*/
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

	/* Finalise last SDO transfer with this node */
	closeSDOtransfer(&TestMaster_Data, nodeId, SDO_CLIENT);
}



void TestMaster_post_TPDO(CO_Data* d)
{
    static int MasterSyncCount = 0;
    if(MasterSyncCount % 50 == 0){
            int data = 5000;
            //UNS8 transmitiontype;
            //UARTprintf("Master : Change slave's transmit type to 0xFF\n");
            //transmitiontype = 0xFF;
            writeNetworkDictCallBack (&TestMaster_Data, /*CO_Data* d*/
                        2, /*UNS8 nodeId*/
                        0x607A, /*UNS16 index*/
                        0x00, /*UNS16 index*/
                        4, /*UNS8 count*/
                        int32, /*UNS8 dataType*/
                        &data,/*void *data*/
                        CheckSDO, /*SDOCallback_t Callback*/
                        0); /* use block mode */
    }
	//UARTprintf("TestMaster_post_TPDO MasterSyncCount = %d \n", MasterSyncCount);
#ifdef 0
	if(MasterSyncCount % 17 == 0){
		UARTprintf("Master : Ask RTR PDO (0x1402)\n");
		sendPDOrequest(&TestMaster_Data, 0x1402 );
		sendPDOrequest(&TestMaster_Data, 0x1403 );
	}
	if(MasterSyncCount % 50 == 0){
		UNS8 transmitiontype;
		UARTprintf("Master : Change slave's transmit type to 0xFF\n");
		transmitiontype = 0xFF;
		writeNetworkDictCallBack (&TestMaster_Data, /*CO_Data* d*/
					2, /*UNS8 nodeId*/
					0x1802, /*UNS16 index*/
					0x02, /*UNS16 index*/
					1, /*UNS8 count*/
					0, /*UNS8 dataType*/
					&transmitiontype,/*void *data*/
					CheckSDO, /*SDOCallback_t Callback*/
                    0); /* use block mode */
	}   
	if(MasterSyncCount % 50 == 25){
		UNS8 transmitiontype = 0x00;
		UARTprintf("Master : Change slave's transmit type to 0x00\n");
		writeNetworkDictCallBack (&TestMaster_Data, /*CO_Data* d*/
					2, /*UNS8 nodeId*/
					0x1802, /*UNS16 index*/
					0x02, /*UNS16 index*/
					1, /*UNS8 count*/
					0, /*UNS8 dataType*/
					&transmitiontype,/*void *data*/
					CheckSDO, /*SDOCallback_t Callback*/
                    0); /* use block mode */
	}
#endif
	MasterSyncCount++;
}

void TestMaster_post_SlaveBootup(CO_Data* d, UNS8 nodeid)
{
	UARTprintf("TestMaster_post_SlaveBootup %x\n", nodeid);
	
	ConfigureSlaveNode(d, nodeid);
}

void TestMaster_post_SlaveStateChange(CO_Data* d, UNS8 nodeId, e_nodeState newNodeState)
{
    UARTprintf("TestMaster_post_SlaveStateChange %x\n", nodeId);

    switch(newNodeState)
    {
        case Pre_operational:
        {
            ConfigureSlaveNode(d, nodeId);
            break;
        }
        default:
        {
            break;
        }
    }
}
