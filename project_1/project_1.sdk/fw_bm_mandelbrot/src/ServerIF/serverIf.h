/*
 * serverIf.h
 *
 *  Created on: 2018/01/27
 *      Author: tak
 */

#ifndef SRC_SERVERIF_SERVERIF_H_
#define SRC_SERVERIF_SERVERIF_H_



class ServefIf
{
private:
	uint32_t m_jpegAddress;	// jpeg address received from JpegCtrl
	uint32_t m_jpegSize;
	uint32_t m_notifiedJpegAddress;	// jpeg address notified to server

public:
	ServefIf();
	void entry();

private:
	void loop();
	void treatMsgFromServer(char* msg, int len);
};


#endif /* SRC_SERVERIF_SERVERIF_H_ */
