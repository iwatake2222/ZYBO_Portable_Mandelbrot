/*
 * jpegCtrl.h
 *
 *  Created on: 2018/01/27
 *      Author: tak
 */

#ifndef SRC_JPEGCTRL_JPEGCTRL_H_
#define SRC_JPEGCTRL_JPEGCTRL_H_



class JpegCtrl
{
private:
	uint32_t m_currentBuffer;	// buffer address which is being encoding now
	uint32_t m_nextBuffer;		// buffer address which will be encoded in the next frame

public:
	JpegCtrl();
	void entryApi();
	void entryRun();

private:
	int encode(uint32_t jpegBuffer, uint32_t imageBuffer);
	void sendJpegBuffer(uint32_t jpegBuffer, int jpegSize);
};


#endif /* SRC_JPEGCTRL_JPEGCTRL_H_ */
