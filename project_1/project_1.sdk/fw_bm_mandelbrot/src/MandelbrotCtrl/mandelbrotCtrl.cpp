/*
 * mandelbrotCtrl.cpp
 *
 *  Created on: 2018/01/27
 *      Author: tak
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
/* xilinx */
#include "xparameters.h"
#include "xil_cache.h"
#include "xmandelbrot.h"
/* my code */
#include "common.h"
#include "parameters.h"
#include "message.h"
#include "bufferMgr.h"
#include "mandelbrotCtrl.h"

MandelbrotCtrl::MandelbrotCtrl()
{
}

void MandelbrotCtrl::entryRun()
{
	XMandelbrot_Initialize(&m_mandelbrot, XPAR_XMANDELBROT_0_DEVICE_ID);
	drawSolid(0x000000FF);
	setDefaultParameters();

	uint32_t imageBuffer;

	while(1) {
		if ((imageBuffer = imageBuffer_getNew()) == 0) {
			LOG_E("cannot get new buffer\n");
			sleepMS(10);
			continue;
		}
//		uint32_t start = getMicroTime();

		/* draw buffer */
		XMandelbrot_Set_vram_V(&m_mandelbrot, imageBuffer);
		XMandelbrot_Set_centerX_V(&m_mandelbrot, m_centerX);
		XMandelbrot_Set_centerY_V(&m_mandelbrot, m_centerY);
		XMandelbrot_Set_zoomHeight_V(&m_mandelbrot, m_zoom);

		if(m_updateMap){
			XMandelbrot_Set_updateColorMap(&m_mandelbrot, 1);
			XMandelbrot_Write_colorMap_Bytes(&m_mandelbrot, 0, (char*)m_colorMap, MANDELBROT_LOOP_NUM * 3);
			m_updateMap = 0;
		} else {
			XMandelbrot_Set_updateColorMap(&m_mandelbrot, 0);
		}

		XMandelbrot_Start(&m_mandelbrot);
		while(!XMandelbrot_IsDone(&m_mandelbrot)) sleepMS(10);

//		printf("elapse time = %d\n", (int)(getMicroTime() - start));

		/* update display  */
		imageBuffer_incrementRefCnt(imageBuffer);	// for DisplayCtrl
		imageBuffer_incrementRefCnt(imageBuffer);	// for JpegCtrl
		MSG_TOP msg;
		msg.moduleFrom = MANDELBROT_CTRL;
		msg.moduleTo = VIDEO_CTRL;
		msg.command = CMD_VIDEO_CTRL::DISPLAY_IMAGE;
		msg.PRM.prmVideoCtrl.DISPLAY_IMAGE.address = imageBuffer;
		sendMsg(VIDEO_CTRL, &msg);

		msg.moduleTo = JPEG_CTRL;
		sendMsg(JPEG_CTRL, &msg);

		imageBuffer_decrementRefCnt(imageBuffer);	// I finished using this buffer
	}
}


void MandelbrotCtrl::entryApi()
{
	while(1) {
		MSG_TOP msg;
		recvMsg(MANDELBROT_CTRL, &msg, -1);
		LOG("recv: cmd = 0x%08X, from = %d\n", msg.command, msg.moduleFrom);
		MSG_PRM_MANDELBROT_CTRL *prm = &msg.PRM.prmMandelbrotCtrl;
		switch(msg.command) {
		case CMD_MANDELBROT_CTRL::SET_POSITION:
			setPosition(prm);
			break;
		case CMD_MANDELBROT_CTRL::MOVE_POSITION:
			movePosition(prm);
			break;
		case CMD_MANDELBROT_CTRL::SET_COLOR_MAP:
			setColorMap(prm);
			break;
		case CMD_MANDELBROT_CTRL::RANDOM_COLOR_MAP:
			setRandomColorMap();
			break;
		default:
			LOG_E("unsupported cmd = %d\n", msg.command);
			break;
		}
	}
}

void MandelbrotCtrl::setPosition(MSG_PRM_MANDELBROT_CTRL *prm)
{
	/* check parameters */
	if (revertValue(&prm->SET_POSITION.centerX) > 1) {
		convertValue(1.0, &prm->SET_POSITION.centerX);
	} else if (revertValue(&prm->SET_POSITION.centerX) < -1) {
		convertValue(-1.0, &prm->SET_POSITION.centerX);
	}

	if (revertValue(&prm->SET_POSITION.centerY) > 1) {
		convertValue(1.0, &prm->SET_POSITION.centerY);
	} else if (revertValue(&prm->SET_POSITION.centerY) < -1) {
		convertValue(-1.0, &prm->SET_POSITION.centerY);
	}

	if (revertValue(&prm->SET_POSITION.zoom) > 1) {
		convertValue(1.0, &prm->SET_POSITION.zoom);
	}

	/* store parameters */
	m_centerX.word_0 = prm->SET_POSITION.centerX.word0;
	m_centerX.word_1 = prm->SET_POSITION.centerX.word1;
	m_centerX.word_2 = prm->SET_POSITION.centerX.word2;
	m_centerY.word_0 = prm->SET_POSITION.centerY.word0;
	m_centerY.word_1 = prm->SET_POSITION.centerY.word1;
	m_centerY.word_2 = prm->SET_POSITION.centerY.word2;
	m_zoom.word_0 = prm->SET_POSITION.zoom.word0;
	m_zoom.word_1 = prm->SET_POSITION.zoom.word1;
	m_zoom.word_2 = prm->SET_POSITION.zoom.word2;
}

void MandelbrotCtrl::movePosition(MSG_PRM_MANDELBROT_CTRL *prm)
{
	/* todo: should calculated in the form of MANDELBROT_VAL */
	double zoom = revertValue((MANDELBROT_VAL*)&m_zoom);
	if (prm->MOVE_POSITION.ratioZoom != 1) {
		zoom *= prm->MOVE_POSITION.ratioZoom;
		convertValue(zoom > 1 ? 1 : zoom, (MANDELBROT_VAL*)&m_zoom);
	}

	double height = zoom * 2;
	double width = height * (double)IMAGE_WIDTH / (double)IMAGE_HEIGHT;
	if (prm->MOVE_POSITION.deltaX != 1) {
		double centerX = revertValue((MANDELBROT_VAL*)&m_centerX);
		centerX += prm->MOVE_POSITION.deltaX * width;
		convertValue(centerX > 1 ? 1 : (centerX < -1 ? -1 : centerX), (MANDELBROT_VAL*)&m_centerX);
	}
	if (prm->MOVE_POSITION.deltaY != 1) {
		double centerY = revertValue((MANDELBROT_VAL*)&m_centerY);
		centerY += prm->MOVE_POSITION.deltaY * height;
		convertValue(centerY > 1 ? 1 : (centerY < -1 ? -1 : centerY), (MANDELBROT_VAL*)&m_centerY);
	}
}


void MandelbrotCtrl::setColorMap(MSG_PRM_MANDELBROT_CTRL *prm)
{
	memcpy(m_colorMap, prm->SET_COLOR_MAP.colorMap, MANDELBROT_LOOP_NUM * 3);
	m_updateMap = true;
	delete prm->SET_COLOR_MAP.colorMap;
}

void MandelbrotCtrl::setRandomColorMap()
{
	int cycleR = rand() % 8 + 1;
	int cycleG = rand() % 8 + 1;
	int cycleB = rand() % 8 + 1;
	for(int i = 0; i < 128; i++) {
		m_colorMap[(MANDELBROT_LOOP_NUM - 1 - i)*3 + 0] = i * cycleG;	//G
		m_colorMap[(MANDELBROT_LOOP_NUM - 1 - i)*3 + 1] = i * cycleB;	//B
		m_colorMap[(MANDELBROT_LOOP_NUM - 1 - i)*3 + 2] = i * cycleR;	//R
	}
	m_updateMap = true;
}

void MandelbrotCtrl::setDefaultParameters()
{
	for(int i = 0; i < 128; i++) {
		m_colorMap[(MANDELBROT_LOOP_NUM - 1 - i)*3 + 0] = i*3;	//G
		m_colorMap[(MANDELBROT_LOOP_NUM - 1 - i)*3 + 1] = i*4;	//B
		m_colorMap[(MANDELBROT_LOOP_NUM - 1 - i)*3 + 2] = i*7;	//R
	}
	m_updateMap = true;

	/* (0, 0), zoom = 1.0 */
	MANDELBROT_VAL val;
	convertValue(0.0, &val);
	revertValue(&val);
	m_centerX.word_0 = val.word0;
	m_centerX.word_1 = val.word1;
	m_centerX.word_2 = val.word2;

	convertValue(0.0, &val);
	m_centerY.word_0 = val.word0;
	m_centerY.word_1 = val.word1;
	m_centerY.word_2 = val.word2;
	convertValue(1.0, &val);
	m_zoom.word_0 = val.word0;
	m_zoom.word_1 = val.word1;
	m_zoom.word_2 =val.word2;

}

void MandelbrotCtrl::drawSolid(uint32_t color)
{
	uint32_t imageBuffer;

	if ((imageBuffer = imageBuffer_getNew()) == 0) {
		LOG_E("cannot get new buffer\n");
		return;
	}
	volatile uint8_t* buf = (uint8_t*)imageBuffer;
	for (int y = 0; y < IMAGE_HEIGHT; y++) {
		for (int x = 0; x < IMAGE_WIDTH; x++) {
			*buf++ = (color >>  0) & 0xFF;
			*buf++ = (color >>  8) & 0xFF;
			*buf++ = (color >> 16) & 0xFF;
		}
	}
	Xil_DCacheFlush();	// so that the reader can read from DDR

	imageBuffer_incrementRefCnt(imageBuffer);	// for DisplayCtrl
	imageBuffer_incrementRefCnt(imageBuffer);	// for JpegCtrl
	MSG_TOP msg;
	msg.moduleFrom = MANDELBROT_CTRL;
	msg.moduleTo = VIDEO_CTRL;
	msg.command = CMD_VIDEO_CTRL::DISPLAY_IMAGE;
	msg.PRM.prmVideoCtrl.DISPLAY_IMAGE.address = imageBuffer;
	sendMsg(VIDEO_CTRL, &msg);

	msg.moduleTo = JPEG_CTRL;
	sendMsg(JPEG_CTRL, &msg);

	imageBuffer_decrementRefCnt(imageBuffer);	// I finished using this buffer

}

void MandelbrotCtrl::convertValue(double srcVal, MANDELBROT_VAL* dstVal)
{
	int32_t temp;
	temp = srcVal * pow(2, 32 - 6);
	dstVal->word2 = temp;
	srcVal -= temp * pow(2, -(32 - 6));
	temp = srcVal * pow(2, 32 - 6 + 32);
	dstVal->word1 = temp;
	srcVal -= temp * pow(2, -(32 - 6 + 32));
	temp = srcVal * pow(2, 32 - 6 + 32 + 32);
	dstVal->word0 = temp;

//	PRINT("convert value: %08X %08X %08X\n", dstVal->word2, dstVal->word1, dstVal->word0);
}

double MandelbrotCtrl::revertValue(MANDELBROT_VAL* srcVal)
{
	double ret;

	if(srcVal->word2 >= ((uint32_t)1 << 31)) {
		/* minus value */
		uint32_t temp = srcVal->word2;
		temp &= ~((uint32_t)1 << 31);
		temp = ((uint32_t)1<<31) - temp;
		ret = (double)temp / pow(2, 32 - 6);
		ret *= -1;
	} else {
		ret = srcVal->word2 / pow(2, 32 - 6);
	}
//	PRINT("reverted value: %d / 1000\n", (int)(ret * 1000));
	return ret;
}

void MandelbrotCtrl::getCurrentPositionStr(char *strPosition)
{
	sprintf(strPosition, "X = %lf, Y = %lf, Zoom = %lf",
			revertValue((MANDELBROT_VAL*)&m_centerX),
			revertValue((MANDELBROT_VAL*)&m_centerY),
			revertValue((MANDELBROT_VAL*)&m_zoom));
}


static MandelbrotCtrl s_mandelbrotCtrl;
extern "C" void mandelbrotCtrlApi_entry(void *pvParameters);
void mandelbrotCtrlApi_entry(void *pvParameters)
{
	s_mandelbrotCtrl.entryApi();
}

extern "C" void mandelbrotCtrlRun_entry(void *pvParameters);
void mandelbrotCtrlRun_entry(void *pvParameters)
{
	s_mandelbrotCtrl.entryRun();
}


// todo: too lazy
void getCurrentPositionStrWrapper(char *strPosition)
{
	s_mandelbrotCtrl.getCurrentPositionStr(strPosition);
}
