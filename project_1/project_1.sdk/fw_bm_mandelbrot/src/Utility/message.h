/*
 * message.h
 *
 *  Created on: 2018/01/27
 *      Author: tak
 */

#ifndef SRC_UTILITY_MESSAGE_H_
#define SRC_UTILITY_MESSAGE_H_


typedef enum {
	VIDEO_CTRL,
	JPEG_CTRL,
	MANDELBROT_CTRL,
	DEBUG_MONITOR,
	SERVER_IF,
	MODULE_ID_NUM,
	DUMMY,
} MODULE_ID;

/* Command to VIDEO_CTRL */
typedef enum {
	DISPLAY_IMAGE,
	INTERRUPT_FRAME,
} CMD_VIDEO_CTRL;

typedef union {
	struct {
		uint32_t address;
	} DISPLAY_IMAGE;
} MSG_PRM_VIDE_CTRL;

/* Command to JPEG_CTRL */
typedef enum {
	ENCODE_IMAGE,
} CMD_JPEG_CTRL;

typedef union {
	struct {
		uint32_t address;
	} ENCODE_IMAGE;
} MSG_PRM_JPEG_CTRL;

/* Command to MANDELBROT_CTRL */
typedef enum {
	SET_POSITION,
	MOVE_POSITION,
	SET_COLOR_MAP,
	RANDOM_COLOR_MAP,
} CMD_MANDELBROT_CTRL;

typedef union {
	struct {
		MANDELBROT_VAL centerX;
		MANDELBROT_VAL centerY;
		MANDELBROT_VAL zoom;
	} SET_POSITION;
	struct {
		double deltaX;
		double deltaY;
		double ratioZoom;
	} MOVE_POSITION;
	struct {
//		uint8_t colorMap[MANDELBROT_LOOP_NUM * 3];
		uint8_t *colorMap;
	} SET_COLOR_MAP;
} MSG_PRM_MANDELBROT_CTRL;

/* Command to SERVER_IF */
typedef enum {
	UPDATE_JPEG,
} CMD_SERVER_IF;

typedef union {
	struct {
		uint32_t address;
		uint32_t size;
	} UPDATE_JPEG;
} MSG_PRM__SERVER_IF;

typedef struct {
	uint32_t command;
	uint32_t moduleFrom;
	uint32_t moduleTo;
	union {
		uint32_t  val[4];
		MSG_PRM_VIDE_CTRL prmVideoCtrl;
		MSG_PRM_JPEG_CTRL prmJpegCtrl;
		MSG_PRM_MANDELBROT_CTRL prmMandelbrotCtrl;
		MSG_PRM__SERVER_IF prmServerIf;
	} PRM;
} MSG_TOP;


#ifdef __cplusplus
extern "C" {
#endif

void initQueue();
int sendMsg(MODULE_ID moduleId, MSG_TOP *msg);
void sendMsgFromISR(MODULE_ID moduleId, MSG_TOP *msg);
int recvMsg(MODULE_ID moduleId, MSG_TOP *msg, int timeoutMS);

#ifdef __cplusplus
}
#endif

#endif /* SRC_UTILITY_MESSAGE_H_ */
