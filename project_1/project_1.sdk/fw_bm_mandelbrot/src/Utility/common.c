/*
 * common.c
 *
 *  Created on: 2018/01/27
 *      Author: tak
 */
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
/* Xilinx includes. */
#include "xil_printf.h"
#include "xparameters.h"
#include "sleep.h"
/* my code */
#include "common.h"
#include "message.h"


uint32_t getMicroTime()
{
	XTime t;
	XTime_GetTime(&t);
	return (t / (double)COUNTS_PER_SECOND) * 1000000;
}

extern uint32_t hdmiOut_getFrameCnt();
uint32_t getFrameCnt()
{
	return hdmiOut_getFrameCnt();
}

void sleepMS(uint32_t timeMS)
{
	vTaskDelay(pdMS_TO_TICKS(timeMS));
}

void delayMS(uint32_t timeMS)
{
	usleep(timeMS * 1000);
}
