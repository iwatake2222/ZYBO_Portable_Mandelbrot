/*
 * hdmiOut.c
 *
 *  Created on: 2018/01/25
 *      Author: tak
 */
/* xilinx */
#include "xparameters.h"
#include "xvtc.h"
#include "xaxivdma.h"
/* my code */
#include "common.h"
#include "interrupt.h"
#include "hdmiOut.h"


static XVtc	s_vtc;
static XAxiVdma s_axiVdma;
static uint32_t s_frameCnt = 0;

int hdmiOut_init(uint16_t vtcDeviceId, uint16_t vdmaDeviceId, int width, int height, int bytePerPixel, int stride)
{
	int status;
	XVtc_Config *configVtc;
	XAxiVdma_Config *configVdma;

	/*** Initialize VTC ***/
	configVtc = XVtc_LookupConfig(vtcDeviceId);
	if (!configVtc){
		LOG_E("XVtc_LookupConfig: %d\n", vtcDeviceId);
		return RET_ERR_XDRV;
	}
	status = XVtc_CfgInitialize(&s_vtc, configVtc, configVtc->BaseAddress);
	if (status != (XST_SUCCESS)) {
		LOG_E("XVtc_CfgInitialize: %d\n", status);
		return RET_ERR_XDRV;
	}

	/*** Initialize VDMA ***/
	configVdma = XAxiVdma_LookupConfig(vdmaDeviceId);
	if (!configVdma){
		LOG_E("XAxiVdma_LookupConfig: %d\n", vdmaDeviceId);
		return RET_ERR_XDRV;
	}

	status = XAxiVdma_CfgInitialize(&s_axiVdma, configVdma, configVdma->BaseAddress);
	if (status != XST_SUCCESS) {
		LOG_E("XAxiVdma_CfgInitialize: %d\n", status);
		return RET_ERR_XDRV;
	}

	XAxiVdma_DmaSetup ReadCfg;
	ReadCfg.VertSizeInput = height;
	ReadCfg.HoriSizeInput = width * bytePerPixel;
	ReadCfg.Stride = stride;
	ReadCfg.FrameDelay = 0;		/* 0 or 1 */
	ReadCfg.EnableCircularBuf = 1;
	ReadCfg.EnableSync = 1;  /* Gen-Lock */
	ReadCfg.PointNum = 0;
	ReadCfg.EnableFrameCounter = 0; /* Endless transfers */
	ReadCfg.FixedFrameStoreAddr = 0; /* We are not doing parking */
	status = XAxiVdma_DmaConfig(&s_axiVdma, XAXIVDMA_READ, &ReadCfg);
	if (status != XST_SUCCESS) {
		LOG_E("XAxiVdma_DmaConfig: %d\n", status);
		return RET_ERR_XDRV;
	}

	return RET_OK;
}

int hdmiOut_setSrcAddress(uint32_t address)
{
	UINTPTR FrameStoreStartAddr[1];
	FrameStoreStartAddr[0] = address;
	int status = XAxiVdma_DmaSetBufferAddr(&s_axiVdma, XAXIVDMA_READ, FrameStoreStartAddr);
	if (status != XST_SUCCESS) {
		LOG_E("XAxiVdma_DmaSetBufferAddr: %d\n", status);
		return RET_ERR_XDRV;
	}

	return RET_OK;
}

int hdmiOut_start()
{
	XVtc_Enable(&s_vtc);

	/* Start the Read channel of VDMA */
	int status = XAxiVdma_DmaStart(&s_axiVdma, XAXIVDMA_READ);
	if (status != XST_SUCCESS) {
		LOG_E("XAxiVdma_DmaStart: %d\n", status);
		return RET_ERR_XDRV;
	}

	return RET_OK;
}

void hdmiOut_stop()
{
	XAxiVdma_DmaStop(&s_axiVdma, XAXIVDMA_READ);
	XVtc_Disable(&s_vtc);
}


void hdmiOut_dumpStatus()
{
	XAxiVdma_DmaRegisterDump(&s_axiVdma, XAXIVDMA_READ);
}

uint32_t hdmiOut_getFrameCnt()
{
	return s_frameCnt;
}


static HDMI_OUT_CB s_cb;
static void* s_cbPrm;
void hdmiOut_registerFrameCallback(HDMI_OUT_CB cb, void* prm)
{
	s_cbPrm = prm;
	s_cb = cb;
}

static void hdmiOut_readCallBack(void *CallbackRef, u32 Mask)
{
	/* interrupt occurs every frame (16.66msec) because IRQFrameCount = 1 by default */
//	LOG("readCallBack %d\n", getMicroTime());
//	hdmiOut_dumpStatus();

	/* don't consider overflow because 32bit x 16.66msec is enough */
	s_frameCnt++;
	if(s_cb != NULL) s_cb(s_cbPrm);
}

static void hdmiOut_readErrorCallBack(void *CallbackRef, u32 Mask)
{
	LOG_E("readErrorCallBack\n");
}

int hdmiOut_initIntr(uint16_t readIntrId)
{
	XAxiVdma_IntrEnable(&s_axiVdma, XAXIVDMA_IXR_ERROR_MASK | XAXIVDMA_IXR_FRMCNT_MASK, XAXIVDMA_READ);
	interrupt_register(readIntrId, (XInterruptHandler)XAxiVdma_ReadIntrHandler, &s_axiVdma);
	XAxiVdma_SetCallBack(&s_axiVdma, XAXIVDMA_HANDLER_GENERAL, (void*)hdmiOut_readCallBack, (void*)&s_axiVdma, XAXIVDMA_READ);
	XAxiVdma_SetCallBack(&s_axiVdma, XAXIVDMA_HANDLER_ERROR, (void*)hdmiOut_readErrorCallBack, (void*)&s_axiVdma, XAXIVDMA_READ);
	return RET_OK;
}
