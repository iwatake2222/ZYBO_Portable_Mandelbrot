/*
 * videoCtrl.cpp
 *
 *  Created on: 2018/01/27
 *      Author: tak
 */
#include "common.h"
#include "parameters.h"
#include "message.h"
#include "bufferMgr.h"
#include "hdmiOut.h"
#include "videoCtrl.h"

VideoCtrl::VideoCtrl()
{
	m_currentBuffer = 0;
}


void VideoCtrl::entry()
{
//	/* Initialize HDMI OUT */
	hdmiOut_init(XPAR_AXIVDMA_0_DEVICE_ID, XPAR_VTC_0_DEVICE_ID, IMAGE_WIDTH, IMAGE_HEIGHT, 3, IMAGE_STRIDE);
	hdmiOut_initIntr(XPAR_FABRIC_AXIVDMA_0_VEC_ID);
//	hdmiOut_setSrcAddress(IMAGE_BUFFER_A);
//	hdmiOut_start();
	hdmiOut_registerFrameCallback(VideoCtrl::callbackFrame, this);

	while(1) {
		loop();
	}
}

void VideoCtrl::callbackFrame(void *prm)
{
//	LOG("FR %d\n", getMicroTime());
	MSG_TOP msg;
	msg.moduleFrom = VIDEO_CTRL;
	msg.moduleTo = VIDEO_CTRL;
	msg.command = CMD_VIDEO_CTRL::INTERRUPT_FRAME;
	sendMsgFromISR(VIDEO_CTRL, &msg);
}

void VideoCtrl::loop()
{
	MSG_TOP msg;
	recvMsg(VIDEO_CTRL, &msg, -1);
//	LOG("recv: cmd = 0x%08X, from = %d\n", msg.command, msg.moduleFrom);
	MSG_PRM_VIDE_CTRL *prm = &msg.PRM.prmVideoCtrl;
	switch(msg.command) {
	case CMD_VIDEO_CTRL::DISPLAY_IMAGE:
		/* update buffer, which will be reflected in the next frame */
		hdmiOut_setSrcAddress(prm->DISPLAY_IMAGE.address);
//		LOG("[%d, %d] %p\n", getFrameCnt(), getMicroTime(), prm->DISPLAY_IMAGE.address);
		hdmiOut_start();
		if (m_nextBuffer != 0) {
			/* drop buffer if I receive this command in the one frame(16.6msec) */
			/* the last buffer will be used. the other frames are dropped */
//			LOG("Skip Frame.: %d\n", hdmiOut_getFrameCnt())
			imageBuffer_decrementRefCnt(m_nextBuffer);
		}
		m_nextBuffer = prm->DISPLAY_IMAGE.address;
		break;

	case CMD_VIDEO_CTRL::INTERRUPT_FRAME:
//		LOG("[%d, %d] RE %p %p\n", getFrameCnt(), getMicroTime(), m_currentBuffer, m_nextBuffer);
		if (m_nextBuffer == 0) {
			// do nothing. just keep using the current buffer
//			LOG("Drop Frame. Use the previous frame: %d\n", hdmiOut_getFrameCnt())
		} else {
			// release the previous buffer
			if (m_currentBuffer != 0) imageBuffer_decrementRefCnt(m_currentBuffer);
			m_currentBuffer = m_nextBuffer;
			m_nextBuffer = 0;
		}
		break;

	default:
		LOG_E("unsupported cmd = %d\n", msg.command);
		break;
	}
}

extern "C" void videoCtrl_entry(void *pvParameters);
void videoCtrl_entry(void *pvParameters)
{
	static VideoCtrl s_videoCtrl;
	s_videoCtrl.entry();
}
