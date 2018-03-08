/*
 * interrupt.h
 *
 *  Created on: 2018/01/26
 *      Author: tak
 */

#ifndef SRC_INTERRUPT_H_
#define SRC_INTERRUPT_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*Xil_InterruptHandler)(void *data);

int interrupt_init(uint16_t deviceId);
int interrupt_register(uint32_t intId, Xil_InterruptHandler Handler, void *CallBackRef);
void interrupt_unregister(uint32_t intId);

#ifdef __cplusplus
}
#endif

#endif /* SRC_INTERRUPT_H_ */
