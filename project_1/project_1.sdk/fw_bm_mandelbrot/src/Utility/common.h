/*
 * common.h
 *
 *  Created on: 2018/01/25
 *      Author: tak
 */

#ifndef SRC_COMMON_H_
#define SRC_COMMON_H_

#include <stdint.h>
#include "xtime_l.h"
#include "xil_printf.h"
#include "parameters.h"

/* common return code */
#define RET_OK       0x00000000
#define RET_NO_DATA  0x00000001
#define RET_TIMEOUT  0x00000002
#define RET_ERR      0x80000001
#define RET_ERR_XDRV 0x80000002	// Error related to Xilinx Driver
#define RET_ERR_OF   0x80000003

/* LOG macros */
#define LOG(str, ...)   {xil_printf("\x1b[39m"); xil_printf("[%s:%d] " str, __FILE__, __LINE__, ##__VA_ARGS__);}
#define LOG_W(str, ...) {xil_printf("\x1b[33m"); xil_printf("[WARNING %s:%d] " str, __FILE__, __LINE__, ##__VA_ARGS__); xil_printf("\x1b[39m");}
#define LOG_E(str, ...) {xil_printf("\x1b[31m"); xil_printf("[ERROR %s:%d] " str, __FILE__, __LINE__, ##__VA_ARGS__); xil_printf("\x1b[39m");}
#define PRINT(str, ...) {xil_printf("\x1b[39m"); xil_printf(str, ##__VA_ARGS__);}


#ifdef __cplusplus
extern "C" {
#endif

	uint32_t getMicroTime();
	uint32_t getFrameCnt();
	void sleepMS(uint32_t timeMS);
	void delayMS(uint32_t timeMS);

#ifdef __cplusplus
}
#endif

#endif /* SRC_COMMON_H_ */
