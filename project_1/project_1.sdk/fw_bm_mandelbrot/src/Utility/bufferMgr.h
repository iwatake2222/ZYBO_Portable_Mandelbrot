/*
 * bufferMgr.h
 *
 *  Created on: 2018/01/27
 *      Author: tak
 */

#ifndef SRC_UTILITY_BUFFERMGR_H_
#define SRC_UTILITY_BUFFERMGR_H_


class BufferMgr
{
private:
	static const int MAX_BUFFER_NUM = 8;
	typedef struct {
		uint32_t address;
		uint32_t refCnt;
	} BUFFER_STATUS;

private:
	BUFFER_STATUS m_bufferStatusList[MAX_BUFFER_NUM];
	uint32_t m_bufferNum;

public:
	BufferMgr(uint32_t bufferNum, uint32_t bufferList[]);
	uint32_t getNewBuffer();
	void incrementRefCnt(uint32_t address);
	void decrementRefCnt(uint32_t address);
};

extern "C" {
	void bufferMgr_init();
	uint32_t imageBuffer_getNew();
	void imageBuffer_incrementRefCnt(uint32_t address);
	void imageBuffer_decrementRefCnt(uint32_t address);
	uint32_t jpegBuffer_getNew();
	void jpegBuffer_incrementRefCnt(uint32_t address);
	void jpegBuffer_decrementRefCnt(uint32_t address);
}

#endif /* SRC_UTILITY_BUFFERMGR_H_ */
