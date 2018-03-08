/*
 * Empty C++ Application
 */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
/* Xilinx includes. */
#include "xil_printf.h"
#include "xil_exception.h"
#include "xparameters.h"
#include "sleep.h"
/* my code */
#include "common.h"
#include "message.h"
#include "interrupt.h"

static void initTask();

#include <stdio.h>
#include "xuartps_hw.h"
int main( void )
{
	Xil_ExceptionDisable();

	LOG("Hello World\n" );

	/* initialize freertos */
	initTask();
	initQueue();

	interrupt_init(XPAR_SCUGIC_0_DEVICE_ID);

	extern void bufferMgr_init();
	bufferMgr_init();

	vTaskStartScheduler();

	while(1);
}


static void initTask()
{
	#define CRE_TASK(TASK, STACK_SIZE, PRIORITY) {\
		static TaskHandle_t x ## TASK;\
		extern void TASK (void *pvParameters);\
		xTaskCreate(TASK,\
		( const char * ) "" #TASK "",\
		STACK_SIZE,\
		NULL,\
		PRIORITY,\
		&(x ## TASK));\
	}

	CRE_TASK(serverIf_entry, 1000, tskIDLE_PRIORITY + 12);
	CRE_TASK(videoCtrl_entry, 1000, tskIDLE_PRIORITY + 10);
	CRE_TASK(jpegCtrlApi_entry, 1000, tskIDLE_PRIORITY + 8);
	CRE_TASK(mandelbrotCtrlApi_entry, 1000, tskIDLE_PRIORITY + 8);
	CRE_TASK(mandelbrotCtrlRun_entry, 1000, tskIDLE_PRIORITY + 6);
#ifdef STANDALONE
	CRE_TASK(debugMonitor_entry, 1000, tskIDLE_PRIORITY + 4);
#endif
	CRE_TASK(jpegCtrlRun_entry, 1000, tskIDLE_PRIORITY + 2);	// low priority
}

