/*
 * hdmiOut.h
 *
 *  Created on: 2018/01/25
 *      Author: tak
 */

#ifndef SRC_HDMIOOUT_H_
#define SRC_HDMIOOUT_H_

#ifdef __cplusplus
extern "C" {
#endif

int hdmiOut_init(uint16_t vtcDeviceId, uint16_t vdmaDeviceId, int width, int height, int bytePerPixel, int stride);
int hdmiOut_setSrcAddress(uint32_t address);
int hdmiOut_start();
void hdmiOut_stop();
void hdmiOut_dumpStatus();
int hdmiOut_initIntr(uint16_t readIntrId);
uint32_t hdmiOut_getFrameCnt();

typedef void (*HDMI_OUT_CB)(void* prm);
void hdmiOut_registerFrameCallback(HDMI_OUT_CB cb, void* prm);

#ifdef __cplusplus
}
#endif

#endif /* SRC_HDMIOOUT_H_ */
