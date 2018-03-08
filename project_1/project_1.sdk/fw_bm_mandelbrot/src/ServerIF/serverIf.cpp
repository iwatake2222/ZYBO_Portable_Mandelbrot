/*
 * serverIf.cpp
 *
 *  Created on: 2018/01/27
 *      Author: tak
 */
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "common.h"
#include "message.h"
#include "bufferMgr.h"
#include "serverIf.h"
#include "MandelbrotCtrl/MandelbrotCtrl.h"

extern "C" {
	void openAmpWrapper_stop();
	void openAmpWrapper_start();
	void openAmpWrapper_recv(void *data, int *len);
	void openAmpWrapper_send(const void *data, int len);
}

ServefIf::ServefIf()
{
	m_jpegAddress = 0;
}


void ServefIf::entry()
{
#ifndef STANDALONE
	openAmpWrapper_start();
#endif
	while(1) {
		loop();
	}
	// never reach
#ifndef STANDALONE
	openAmpWrapper_stop();
#endif
}

void ServefIf::loop()
{
	MSG_TOP msg;	// msg from another module
	int ret;
	ret = recvMsg(SERVER_IF, &msg, 10);
	if(ret != RET_OK) {
		/* check message from server(Linux) every 10 msec */
#ifndef STANDALONE
		char serverMsg[256];	// msg from server
		int len;
		openAmpWrapper_recv(serverMsg, &len);
		if(len > 0) treatMsgFromServer(serverMsg, len);
#endif
	} else {
		/* when new message received */
	//	LOG("recv: cmd = 0x%08X, from = %d\n", msg.command, msg.moduleFrom);
		MSG_PRM__SERVER_IF *prm = &msg.PRM.prmServerIf;
		switch(msg.command) {
		case CMD_SERVER_IF::UPDATE_JPEG:
//			PRINT("update jpeg %08X\n", prm->UPDATE_JPEG.address);
			if(m_jpegAddress) jpegBuffer_decrementRefCnt(m_jpegAddress);
			m_jpegAddress = prm->UPDATE_JPEG.address;
			m_jpegSize = prm->UPDATE_JPEG.size;
			break;

		default:
			LOG_E("unsupported cmd = %d\n", msg.command);
			break;
		}
	}
}

void ServefIf::treatMsgFromServer(char* msg, int len)
{
	char separators[] = ",";
	char *prm;
	char retStr[256];
	if ((prm = strtok(msg, separators)) == NULL) {LOG_E("err: %s\n", msg); return;}
	if (strncmp(prm, "pos", 3) == 0) {
		double x, y, zoom;
		if ((prm = strtok(NULL, separators)) == NULL) {LOG_E("err: %s\n", msg); return;}
		x = atof(prm);
		if ((prm = strtok(NULL, separators)) == NULL) {LOG_E("err: %s\n", msg); return;}
		y = atof(prm);
		if ((prm = strtok(NULL, separators)) == NULL) {LOG_E("err: %s\n", msg); return;}
		zoom = atof(prm);
		MSG_TOP msg;
		MandelbrotCtrl::convertValue(x, &msg.PRM.prmMandelbrotCtrl.SET_POSITION.centerX);
		MandelbrotCtrl::convertValue(y, &msg.PRM.prmMandelbrotCtrl.SET_POSITION.centerY);
		MandelbrotCtrl::convertValue(zoom, &msg.PRM.prmMandelbrotCtrl.SET_POSITION.zoom);
		msg.moduleFrom = SERVER_IF;
		msg.moduleTo = MANDELBROT_CTRL;
		msg.command = CMD_MANDELBROT_CTRL::SET_POSITION;
		sendMsg(MANDELBROT_CTRL, &msg);
		sprintf(retStr, "{\"ret\":\"ok\"}");

	} else if (strncmp(prm, "move", 4) == 0) {
		double deltaX, deltaY, ratioZoom;
		if ((prm = strtok(NULL, separators)) == NULL) {LOG_E("err: %s\n", msg); return;}
		deltaX = atof(prm);
		if ((prm = strtok(NULL, separators)) == NULL) {LOG_E("err: %s\n", msg); return;}
		deltaY = atof(prm);
		if ((prm = strtok(NULL, separators)) == NULL) {LOG_E("err: %s\n", msg); return;}
		ratioZoom = atof(prm);
		MSG_TOP msg;
		msg.PRM.prmMandelbrotCtrl.MOVE_POSITION.deltaX = deltaX;
		msg.PRM.prmMandelbrotCtrl.MOVE_POSITION.deltaY = deltaY;
		msg.PRM.prmMandelbrotCtrl.MOVE_POSITION.ratioZoom = ratioZoom;
		msg.moduleFrom = SERVER_IF;
		msg.moduleTo = MANDELBROT_CTRL;
		msg.command = CMD_MANDELBROT_CTRL::MOVE_POSITION;
		sendMsg(MANDELBROT_CTRL, &msg);
		sprintf(retStr, "{\"ret\":\"ok\"}");

	} else if (strncmp(prm, "color", 5) == 0) {
		MSG_TOP msg;
		msg.moduleFrom = SERVER_IF;
		msg.moduleTo = MANDELBROT_CTRL;
		msg.command = CMD_MANDELBROT_CTRL::RANDOM_COLOR_MAP;
		sendMsg(MANDELBROT_CTRL, &msg);
		sprintf(retStr, "{\"ret\":\"ok\"}");

	} else if (strncmp(prm, "get", 3) == 0) {
		char strCurrentPosition[256];
		extern void getCurrentPositionStrWrapper(char *strPosition);
		getCurrentPositionStrWrapper(strCurrentPosition);

		if(m_notifiedJpegAddress) {
			/* release jpeg buffer notified to the server at the last time */
			jpegBuffer_decrementRefCnt(m_notifiedJpegAddress);
			m_notifiedJpegAddress = 0;
		}
		if(m_jpegAddress) {
			/* notify the latest jpeg buffer to the server, and keep the jpeg buffer using */
			m_notifiedJpegAddress = m_jpegAddress;
			jpegBuffer_incrementRefCnt(m_notifiedJpegAddress);
			sprintf(retStr, "{\"ret\":\"ok\", \"jpegAddress\":\"0x%08X\", \"jpegSize\":\"%d\", \"position\":\"%s\"}",
					m_notifiedJpegAddress, m_jpegSize, strCurrentPosition);
		} else {
			sprintf(retStr, "{\"ret\":\"ok\", \"jpegAddress\":\"0x%08X\", \"jpegSize\":\"%d\", \"position\":\"%s\"}",
					0, 0, strCurrentPosition);
		}

	} else {
		LOG_E("unsupported command: %s\n", msg);
		/* do nothing because the server cannot treat error, and this error message destroy communication protocol */
//		sprintf(retStr, "{\"ret\":\"error\"}");
		return;
	}

	openAmpWrapper_send(retStr, strnlen(retStr, 256));
}

extern "C" void serverIf_entry(void *pvParameters);
void serverIf_entry(void *pvParameters)
{
	static ServefIf s_serverIf;
	s_serverIf.entry();
}
