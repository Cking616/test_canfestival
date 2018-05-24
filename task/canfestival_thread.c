/*
 * canfestival_thread.c
 *
 *  Created on: 2018年5月21日
 *      Author: cking
 */
#include "canfestival.h"
#include "../bsp/can.h"

unsigned int TimeCNT=0;//时间计数
unsigned int NextTime=0;//下一次触发时间计数
unsigned int TIMER_MAX_COUNT=70000;//最大时间计数

void setTimer(TIMEVAL value)
{
    NextTime = (TimeCNT + value) % TIMER_MAX_COUNT;
}

TIMEVAL getElapsedTime(void)
{
    static TIMEVAL last_time_set = TIMEVAL_MAX;//上一次的时间计数
    int ret=0;
    ret = TimeCNT > last_time_set ? TimeCNT - last_time_set : TimeCNT + TIMER_MAX_COUNT - last_time_set;
    last_time_set = TimeCNT;
    return ret;
}

unsigned char canSend(CAN_PORT notused, Message *m)
{
    can_write(m->cob_id,  m->len, m->rtr, m->data);
    return 0x00;
}

void canfestival_period()
{
    TimeCNT++;
    if (TimeCNT>=TIMER_MAX_COUNT)
    {
        TimeCNT=0;
    }
    if (TimeCNT==NextTime)
    {
        TimeDispatch();
    }
}

