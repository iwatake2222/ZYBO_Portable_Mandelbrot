/*
 * message.cpp
 *
 *  Created on: 2018/01/27
 *      Author: tak
 */
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
/* my code */
#include "common.h"
#include "message.h"


static QueueHandle_t s_queue[MODULE_ID_NUM];

void initQueue()
{
	for (int moduleId = 0; moduleId < MODULE_ID_NUM; moduleId++) {
		s_queue[moduleId] = xQueueCreate(8, sizeof(MSG_TOP));
	}
}

int sendMsg(MODULE_ID moduleId, MSG_TOP *msg)
{
	BaseType_t ret;
	/* non blocking */
	ret = xQueueSend(s_queue[moduleId], msg, 0UL);
	if (ret != pdTRUE) {
		LOG_E("xQueueSend: ret = %d, moduleId = %d, command = 0x%08X\n", ret, moduleId, msg->command);
		return RET_ERR_OF;
	}

	/* invoke context switch  !important! */
	vTaskDelay(1);

	return RET_OK;
}

void sendMsgFromISR(MODULE_ID moduleId, MSG_TOP *msg)
{
	BaseType_t xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = pdFALSE;
	do {
		xQueueSendFromISR(s_queue[moduleId], msg, &xHigherPriorityTaskWoken);
	} while(0);
//	if( xHigherPriorityTaskWoken ) taskYIELD_FROM_ISR ();
}


int recvMsg(MODULE_ID moduleId, MSG_TOP *msg, int timeoutMS)
{
	TickType_t timeout = (timeoutMS == -1) ? portMAX_DELAY : pdMS_TO_TICKS(timeoutMS);
	return (xQueueReceive(s_queue[moduleId], msg, timeout) == pdTRUE) ? RET_OK : RET_ERR;
}
