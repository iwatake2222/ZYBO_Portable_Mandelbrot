/*
 * bufferMgr.cpp
 *
 *  Created on: 2018/01/27
 *      Author: tak
 */
#include "common.h"
#include "bufferMgr.h"

/* todo: make this class thread safe (mutex) */

BufferMgr::BufferMgr(uint32_t bufferNum, uint32_t bufferList[])
{
	m_bufferNum = bufferNum;
	for(uint32_t i = 0; i < bufferNum && i < MAX_BUFFER_NUM; i++) {
		m_bufferStatusList[i].address = bufferList[i];
		m_bufferStatusList[i].refCnt = 0;
	}
}


uint32_t BufferMgr::getNewBuffer()
{
	for (uint32_t i = 0; i < m_bufferNum; i++) {
		if (m_bufferStatusList[i].refCnt == 0) {
			m_bufferStatusList[i].refCnt++;		// increment cnt for the first user
			return m_bufferStatusList[i].address;
		}
	}
	LOG_E("Buffer over flow happened\n");
	return 0;
}

void BufferMgr::incrementRefCnt(uint32_t address)
{
	for (uint32_t i = 0; i < m_bufferNum; i++) {
		if (m_bufferStatusList[i].address == address) {
			m_bufferStatusList[i].refCnt++;
			return;
		}
	}
	LOG_E("invalid address %p\n", address);
}

void BufferMgr::decrementRefCnt(uint32_t address)
{
	for (uint32_t i = 0; i < m_bufferNum; i++) {
		if (m_bufferStatusList[i].address == address) {
			if (m_bufferStatusList[i].refCnt > 0) {
				m_bufferStatusList[i].refCnt--;
			} else {
				LOG_E("somethings wrong %p\n", address);
			}
			return;
		}
	}
	LOG_E("invalid address %p\n", address);
}


