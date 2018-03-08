/*
 * mandelbrotCtrl.h
 *
 *  Created on: 2018/01/27
 *      Author: tak
 */

#ifndef SRC_MANDELBROTCTRL_VIDEOCTRL_H_
#define SRC_MANDELBROTCTRL_VIDEOCTRL_H_

#include "common.h"
#include "xmandelbrot.h"

class MandelbrotCtrl
{
private:
	uint8_t m_colorMap[MANDELBROT_LOOP_NUM*3];
	XMandelbrot_Centerx_v m_centerX;
	XMandelbrot_Centery_v m_centerY;
	XMandelbrot_Zoomheight_v m_zoom;
	XMandelbrot m_mandelbrot;
	bool    m_updateMap;

private:
	void setDefaultParameters();
	void drawSolid(uint32_t color);
	void setPosition(MSG_PRM_MANDELBROT_CTRL *prm);
	void movePosition(MSG_PRM_MANDELBROT_CTRL *prm);
	void setColorMap(MSG_PRM_MANDELBROT_CTRL *prm);
	void setRandomColorMap();

public:
	MandelbrotCtrl();
	void entryApi();
	void entryRun();
	static void convertValue(double srcVal, MANDELBROT_VAL* dstVal);
	static double revertValue(MANDELBROT_VAL* srcVal);
	void getCurrentPositionStr(char *strPosition);
};


#endif /* SRC_MANDELBROTCTRL_VIDEOCTRL_H_ */
