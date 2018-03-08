/*
 * videoCtrl.h
 *
 *  Created on: 2018/01/27
 *      Author: tak
 */

#ifndef SRC_VIDEOCTRL_VIDEOCTRL_H_
#define SRC_VIDEOCTRL_VIDEOCTRL_H_



class VideoCtrl
{
private:
	uint32_t m_currentBuffer;	// buffer address which is being displayed now
	uint32_t m_nextBuffer;		// buffer address which will be displayed in the next frame

public:
	VideoCtrl();
	void entry();

private:
	void loop();
	static void callbackFrame(void *prm);
};


#endif /* SRC_VIDEOCTRL_VIDEOCTRL_H_ */
