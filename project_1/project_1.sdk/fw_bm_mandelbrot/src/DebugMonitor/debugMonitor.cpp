/*
 * debugMonitor.cpp
 *
 *  Created on: 2018/01/27
 *      Author: tak
 */
#include <stdio.h>
#include <stdlib.h>
/* xilinx */
#include "xuartps_hw.h"
/* my code */
#include "common.h"
#include "message.h"
#include "MandelbrotCtrl/mandelbrotCtrl.h"

#define DEBUG_MONITOR_BUFFER_SIZE 64
#define DEBUG_MONITOR_ARGC_SIZE 4

typedef struct {
	char* cmd;
	int (*func)(char *argv[], uint32_t argc);
} DEBUG_MON_COMMAND;

static int test1(char *argv[], uint32_t argc)
{
	PRINT("test1\n");
	PRINT("argc = %d\n", argc);
	for (uint32_t i = 0; i < argc; i++) {
		PRINT("argv[%d] = %s\n", i, argv[i]);
	}
//	PRINT("a %d\n", atoi(argv[0]));
	return RET_OK;
}

static int setPosition(char *argv[], uint32_t argc)
{
	if(argc < 3) {
		PRINT("Invalid parameter format. Usage:\n")
		PRINT("setpos centerX centerY zoom\n");
		return RET_ERR;
	}
	double x, y, zoom;
	x = atof(argv[0]);
	y = atof(argv[1]);
	zoom = atof(argv[2]);
	PRINT("Set Position\n");
	//printf doesn't support float
	PRINT("x = %d, y = %d, zoom = %d  (/1000)\n", (int)(x*1000), (int)(y*1000), (int)(zoom*1000));

	/* create parameters */
	MSG_TOP msg;
	MANDELBROT_VAL val;
	MandelbrotCtrl::convertValue(x, &msg.PRM.prmMandelbrotCtrl.SET_POSITION.centerX);
	MandelbrotCtrl::convertValue(y, &msg.PRM.prmMandelbrotCtrl.SET_POSITION.centerY);
	MandelbrotCtrl::convertValue(zoom, &msg.PRM.prmMandelbrotCtrl.SET_POSITION.zoom);

	/* send parameters  */
	msg.moduleFrom = DEBUG_MONITOR;
	msg.moduleTo = MANDELBROT_CTRL;
	msg.command = CMD_MANDELBROT_CTRL::SET_POSITION;
	sendMsg(MANDELBROT_CTRL, &msg);

	return RET_OK;
}

DEBUG_MON_COMMAND s_debugCommands[] = {
	{"test1", test1},
	{"setpos", setPosition},
	{NULL, NULL},
};

static void debugMonitor_show()
{
	PRINT("\nCommand List:\n");
	for (uint32_t i = 0; s_debugCommands[i].cmd != NULL; i++) {
		PRINT("%s\n", s_debugCommands[i].cmd);
	}
	PRINT("\n\n>");
}

static void debugMonitor_move(int deltaX, int deltaY, double deltaZoom)
{
	const static double STEP = 10;
	static double x = 0;
	static double y = 0;
	static double zoom = 1.0;

	x += deltaX * zoom / STEP;
	y += deltaY * zoom / STEP;
	zoom *= deltaZoom;

	/* create parameters */
	MSG_TOP msg;
	MANDELBROT_VAL val;
	MandelbrotCtrl::convertValue(x, &val);
	memcpy(&msg.PRM.prmMandelbrotCtrl.SET_POSITION.centerX, &val, sizeof(MANDELBROT_VAL));
	MandelbrotCtrl::convertValue(y, &val);
	memcpy(&msg.PRM.prmMandelbrotCtrl.SET_POSITION.centerY, &val, sizeof(MANDELBROT_VAL));
	MandelbrotCtrl::convertValue(zoom, &val);
	memcpy(&msg.PRM.prmMandelbrotCtrl.SET_POSITION.zoom, &val, sizeof(MANDELBROT_VAL));

	/* send parameters  */
	msg.moduleFrom = DEBUG_MONITOR;
	msg.moduleTo = MANDELBROT_CTRL;
	msg.command = CMD_MANDELBROT_CTRL::SET_POSITION;
	sendMsg(MANDELBROT_CTRL, &msg);
}

static void debugMonitor_color()
{
	/* create parameters */
	uint8_t *colorMap = new uint8_t[MANDELBROT_LOOP_NUM * 3];	// released by receiver

	int cycleR = rand()%32;
	int cycleG = rand()%32;
	int cycleB = rand()%32;
	for(int i = 0; i < 128; i++) {
		colorMap[(MANDELBROT_LOOP_NUM - 1 - i)*3 + 0] = i * cycleG;	//G
		colorMap[(MANDELBROT_LOOP_NUM - 1 - i)*3 + 1] = i * cycleB;	//B
		colorMap[(MANDELBROT_LOOP_NUM - 1 - i)*3 + 2] = i * cycleR;	//R
	}

	/* send parameters  */
	MSG_TOP msg;
	msg.moduleFrom = DEBUG_MONITOR;
	msg.moduleTo = MANDELBROT_CTRL;
	msg.command = CMD_MANDELBROT_CTRL::SET_COLOR_MAP;
	msg.PRM.prmMandelbrotCtrl.SET_COLOR_MAP.colorMap = colorMap;
	sendMsg(MANDELBROT_CTRL, &msg);
}

static int debugMonitor_immediateCommand(char c)
{
	/* A, S, D, W keys to move */
	/* Z, X keys to zoom in/out */
	/* C key to change color */
	switch (c){
	case 'A':
		debugMonitor_move(-1, 0, 1);
		break;
	case 'D':
		debugMonitor_move(1, 0, 1);
		break;
	case 'W':
		debugMonitor_move(0, 1, 1);
		break;
	case 'S':
		debugMonitor_move(0, -1, 1);
		break;
	case 'Z':
		debugMonitor_move(0, 0, 0.9);
		break;
	case 'X':
		debugMonitor_move(0, 0, 1.1);
		break;
	case 'C':
		debugMonitor_color();
		break;
	default:
		return 0;	// command was treated;
	}
	return 1;	// command was treated
}

static void debugMonitor_run(char c)
{
	static char s_storedCommand[DEBUG_MONITOR_BUFFER_SIZE];
	static uint32_t s_storedCommandIndex = 0;

	if(debugMonitor_immediateCommand(c) != 0) return;
//	putchar(c);	/* echo back anyway */
//	XUartPs_SendByte(XPS_UART1_BASEADDR, c);
	PRINT("%c", c);
	s_storedCommand[s_storedCommandIndex++] = c;

	/* check if one line is done */
	if(c == '\r' || c == '\n' || s_storedCommandIndex == DEBUG_MONITOR_BUFFER_SIZE) {
		s_storedCommand[s_storedCommandIndex-1] = '\0';

		/* split input command (convert ' ' to '\0') */
		char *argv[DEBUG_MONITOR_ARGC_SIZE] = {0};
		uint32_t argc = 0;	/* argc < DEBUG_MONITOR_ARGC_SIZE */
		argv[argc++] = &s_storedCommand[0];
		for (uint32_t i = 2; i < s_storedCommandIndex; i++) {
			if (s_storedCommand[i] == ' ') {
				s_storedCommand[i] = '\0';
				argv[argc++] = &s_storedCommand[i+1];
				if (argc == DEBUG_MONITOR_ARGC_SIZE) break;
			}
		}

		/* call corresponding debug command */
		int ret = RET_ERR;
		for (uint32_t i = 0; s_debugCommands[i].cmd != NULL; i++) {
			if (strcmp(s_debugCommands[i].cmd, argv[0]) == 0) {
				ret = s_debugCommands[i].func(&argv[1], argc-1);
				printf(">");
			}
		}

		if (ret != RET_OK) debugMonitor_show();
		s_storedCommandIndex = 0;
	}

}

extern "C" void debugMonitor_entry(void *pvParameters);
void debugMonitor_entry(void *pvParameters)
{
#ifndef WITH_OPEN_AMPLIB
	/* disable buffer to get the input key immediately */
	setvbuf(stdin, NULL, _IONBF, 0);

	while(1){
		int isReceived = XUartPs_IsReceiveData(XPS_UART1_BASEADDR);
		if(isReceived == 0){
			sleepMS(10);
		} else {
			char c = getchar();
			debugMonitor_run(c);
		}
	}
#else
	while(1){
		int isReceived = XUartPs_IsReceiveData(XPS_UART1_BASEADDR);
		if(isReceived == 0){
			sleepMS(10);
		} else {
			char c = XUartPs_RecvByte(XPS_UART1_BASEADDR);
			debugMonitor_run(c);
		}
	}
#endif
}
