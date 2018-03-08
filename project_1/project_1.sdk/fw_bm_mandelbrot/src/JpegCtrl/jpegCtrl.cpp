/*
 * jpegCtrl.cpp
 *
 *  Created on: 2018/01/27
 *      Author: tak
 */
/* xilinx */
#include "xil_cache.h"
/* mycode */
#include "common.h"
#include "parameters.h"
#include "message.h"
#include "bufferMgr.h"
#include "jpegCtrl.h"

JpegCtrl::JpegCtrl()
{
	m_currentBuffer = 0;
	m_nextBuffer = 0;
}


void JpegCtrl::entryApi()
{
	while(1) {
		MSG_TOP msg;
		recvMsg(JPEG_CTRL, &msg, -1);
//		LOG("recv: cmd = 0x%08X, from = %d\n", msg.command, msg.moduleFrom);
		MSG_PRM_JPEG_CTRL *prm = &msg.PRM.prmJpegCtrl;
		switch(msg.command) {
		case CMD_JPEG_CTRL::ENCODE_IMAGE:
			if (m_currentBuffer == 0) {
				m_currentBuffer = prm->ENCODE_IMAGE.address;
			} else {
				/* update buffer, which will be reflected in the next encode */
				if (m_nextBuffer != 0) {
					/* the last buffer will be used. the other frames are dropped */
					imageBuffer_decrementRefCnt(m_nextBuffer);
				}
				m_nextBuffer = prm->ENCODE_IMAGE.address;
			}
			break;

		default:
			LOG_E("unsupported cmd = %d\n", msg.command);
			break;
		}
	}
}

void JpegCtrl::entryRun()
{
	while(1) {
		/* wait until new image is ready */
		if(m_currentBuffer == 0) {
			sleepMS(10);
			continue;
		}

		/* get jpeg buffer */
		uint32_t jpegBuffer;
		if ((jpegBuffer = jpegBuffer_getNew()) == 0) {
			sleepMS(10);
			continue;
		}

		/* encode one frame */
//		LOG("Encode %08X to %08X\n", m_currentBuffer, jpegBuffer);
		int jpegSize;
		Xil_DCacheInvalidateRange(m_currentBuffer, IMAGE_WIDTH * IMAGE_HEIGHT * BYTE_PER_PIXEL);
		Xil_DCacheInvalidateRange(jpegBuffer, jpegSize);	// so that the reader can read from DDR
		jpegSize = encode(jpegBuffer, m_currentBuffer);
		Xil_DCacheFlushRange(jpegBuffer, jpegSize);	// so that the reader can read from DDR
#if 1
		/* todo: for some reasons, I need these redundant operations. Otherwise, created jpeg image destroyed */
		/* there might be problem related to cache coherrency */
		jpegSize = encode(jpegBuffer, m_currentBuffer);
		Xil_DCacheFlushRange(jpegBuffer, jpegSize);	// so that the reader can read from DDR
		jpegSize = encode(jpegBuffer, m_currentBuffer);
		Xil_DCacheFlushRange(jpegBuffer, jpegSize);	// so that the reader can read from DDR
#endif

		/*** encode done ***/
		sendJpegBuffer(jpegBuffer, jpegSize);

		/* release frame and prepare next frame */
		imageBuffer_decrementRefCnt(m_currentBuffer);
		m_currentBuffer = m_nextBuffer;	/* todo: mutex b/w entryApi */
		m_nextBuffer = 0;				/* todo: mutex b/w entryApi */
	}

}

extern "C" {
	int encodeJpegMem(uint8_t *imgJpeg, uint8_t *imgRGB, uint32_t width, uint32_t height, uint32_t imgJpegSize);
}

int JpegCtrl::encode(uint32_t jpegBuffer, uint32_t imageBuffer)
{
	return encodeJpegMem((uint8_t*)jpegBuffer, (uint8_t*)imageBuffer, IMAGE_WIDTH, IMAGE_HEIGHT, 0x30000);
}

void JpegCtrl::sendJpegBuffer(uint32_t jpegBuffer, int jpegSize)
{
	/* update jpeg buffer  */
	jpegBuffer_incrementRefCnt(jpegBuffer);	// for ServerIf
	MSG_TOP msg;
	msg.moduleFrom = JPEG_CTRL;
	msg.moduleTo = SERVER_IF;
	msg.command = CMD_SERVER_IF::UPDATE_JPEG;
	msg.PRM.prmServerIf.UPDATE_JPEG.address = jpegBuffer;
	msg.PRM.prmServerIf.UPDATE_JPEG.size = jpegSize;
	sendMsg(SERVER_IF, &msg);
	jpegBuffer_decrementRefCnt(jpegBuffer);	// I finished using this buffer
}

static JpegCtrl s_jpegCtrl;
extern "C" void jpegCtrlApi_entry(void *pvParameters);
void jpegCtrlApi_entry(void *pvParameters)
{

	s_jpegCtrl.entryApi();
}

extern "C" void jpegCtrlRun_entry(void *pvParameters);
void jpegCtrlRun_entry(void *pvParameters)
{
	s_jpegCtrl.entryRun();
}
