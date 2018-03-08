/*
 * interrupt.c
 *
 *  Created on: 2018/01/26
 *      Author: tak
 */
/* xilinx */
#include "xparameters.h"
#include "xscugic.h"
/* my code */
#include "common.h"
#include "interrupt.h"

static XScuGic s_interruptController;

static int setUpInterruptSystem(XScuGic *XScuGicInstancePtr);

int interrupt_init(uint16_t deviceId)
{
	int status;

	XScuGic_Config *gicConfig;

	gicConfig = XScuGic_LookupConfig(deviceId);
	if (NULL == gicConfig) {
		LOG_E("XScuGic_LookupConfig: %d\n", 0);
		return RET_ERR_XDRV;
	}

	status = XScuGic_CfgInitialize(&s_interruptController, gicConfig, gicConfig->CpuBaseAddress);
	if (status != XST_SUCCESS) {
		LOG_E("XScuGic_CfgInitialize: %d\n", status);
		return RET_ERR_XDRV;
	}

	status = XScuGic_SelfTest(&s_interruptController);
	if (status != XST_SUCCESS) {
		LOG_E("XScuGic_SelfTest: %d\n", status);
		return RET_ERR_XDRV;
	}

	/* this is done by freertos */
//	status = interrupt_setUpInterruptSystem(&s_interruptController);
//	if (status != XST_SUCCESS) {
//		LOG_E("SetUpInterruptSystem: %d\n", status);
//		return RET_ERR_XDRV;
//	}

	return RET_OK;
}

int interrupt_register(uint32_t intId, Xil_InterruptHandler handler, void *callBackRef)
{
	int status;
	status = XScuGic_Connect(&s_interruptController, intId, handler, callBackRef);
	if (status != XST_SUCCESS) {
		LOG_E("XScuGic_Connect: %d\n", status);
		return RET_ERR_XDRV;
	}
	XScuGic_Enable(&s_interruptController, intId);
	return RET_OK;
}

void interrupt_unregister(uint32_t intId)
{
	XScuGic_Disable(&s_interruptController, intId);
	XScuGic_Disconnect(&s_interruptController, intId);
}


static int interrupt_setUpInterruptSystem(XScuGic *XScuGicInstancePtr)
{

	/*
	 * Connect the interrupt controller interrupt handler to the hardware
	 * interrupt handling logic in the ARM processor.
	 */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			(Xil_ExceptionHandler) XScuGic_InterruptHandler,
			XScuGicInstancePtr);

	/*
	 * Enable interrupts in the ARM
	 */
	Xil_ExceptionEnable();

	return XST_SUCCESS;
}
