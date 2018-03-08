/*
 * bufferMgrInstance.cpp
 *
 *  Created on: 2018/01/27
 *      Author: tak
 */
#include "common.h"
#include "parameters.h"
#include "bufferMgr.h"

static BufferMgr *s_pBufferMgrImage;
static BufferMgr *s_pBufferMgrJpeg;

void bufferMgr_init()
{
	uint32_t imageBufferList[] = {
		IMAGE_BUFFER_A,
		IMAGE_BUFFER_B,
		IMAGE_BUFFER_C,
		IMAGE_BUFFER_D,
		IMAGE_BUFFER_E,
		IMAGE_BUFFER_F,
		IMAGE_BUFFER_G,
		IMAGE_BUFFER_H,
	};
	s_pBufferMgrImage = new BufferMgr(sizeof(imageBufferList) / sizeof(uint32_t), imageBufferList);

	uint32_t jpegBufferList[] = {
		JPEG_BUFFER_A,
		JPEG_BUFFER_B,
		JPEG_BUFFER_C,
		JPEG_BUFFER_D,
		JPEG_BUFFER_E,
		JPEG_BUFFER_F,
		JPEG_BUFFER_G,
		JPEG_BUFFER_H,
	};
	s_pBufferMgrJpeg = new BufferMgr(sizeof(jpegBufferList) / sizeof(uint32_t), jpegBufferList);
}

uint32_t imageBuffer_getNew()
{
	return s_pBufferMgrImage->getNewBuffer();
}

void imageBuffer_incrementRefCnt(uint32_t address)
{
	s_pBufferMgrImage->incrementRefCnt(address);
}

void imageBuffer_decrementRefCnt(uint32_t address)
{
	s_pBufferMgrImage->decrementRefCnt(address);
}

uint32_t jpegBuffer_getNew()
{
	return s_pBufferMgrJpeg->getNewBuffer();
}

void jpegBuffer_incrementRefCnt(uint32_t address)
{
	s_pBufferMgrJpeg->incrementRefCnt(address);
}

void jpegBuffer_decrementRefCnt(uint32_t address)
{
	s_pBufferMgrJpeg->decrementRefCnt(address);
}
