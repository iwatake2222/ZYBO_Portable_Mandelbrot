#include <stdint.h>
#include <string.h>
#include <ap_int.h>
#include "parameters.h"


uint8_t checkMandelbrot(const TYPE_VAL x, const TYPE_VAL y)
{
#pragma HLS allocation instances=mul limit=1 operation
	TYPE_VAL zx = 0;
	TYPE_VAL zy = 0;
	uint8_t loopNum;
	for (loopNum = 0; loopNum < LOOP_NUM - 1; loopNum++) {
		/* f(z) = z^2 + C = (zx + zyi)^2 + C = (zx^2) + (2 * zx * zy) - (zy^2) + C = (zx^2) + (2 * zx * zyi) - (zy^2) + (x + yi) */
		/* f(z)_x = (zx^2) - (zy^2) + x */
		/* f(z)_y = (2 * zx * zy) + y */
		const TYPE_VAL orgZx = zx;
		const TYPE_VAL orgZy = zy;
		zx = (orgZx * orgZx) - (orgZy * orgZy) + x;
		zy = (orgZx * orgZy) * 2 + y;
#if 0
		if (zx * zx + zy * zy > 4) break;
#else
		TYPE_VAL absValX, absValY;
		if (zx > 0) {absValX = zx;} else {absValX = -zx;}
		if (zy > 0) {absValY = zy;} else {absValY = -zy;}
		if (absValX + absValY > 3) break;	// when the ABS(Z) >2, the maximum of (ABS(Zx) + ABS(Zy)) is 2 * 2/sqrt(2)
#endif
	}
	return loopNum;
}

static uint8_t s_colorMap[LOOP_NUM * BYTE_PER_PIXEL];

static void convertToColor(uint8_t loopNum, uint8_t *r, uint8_t *g, uint8_t *b) {
	*r = s_colorMap[loopNum * BYTE_PER_PIXEL + 0];
	*g = s_colorMap[loopNum * BYTE_PER_PIXEL + 1];
	*b = s_colorMap[loopNum * BYTE_PER_PIXEL + 2];
}

static void mandelbrotLine(ap_uint<32> *vram, TYPE_VAL x, const TYPE_VAL y, const TYPE_VAL stepX)
{
	ap_uint<32> linebuf[IMAGE_WIDTH * BYTE_PER_PIXEL / sizeof(ap_uint<32>)];
	for (uint16_t cntX = 0; cntX < IMAGE_WIDTH / 4; cntX += 1) {
		uint8_t r0, g0, b0;
		uint8_t r1, g1, b1;
		uint8_t r2, g2, b2;
		uint8_t r3, g3, b3;

		uint8_t loopNum;
		loopNum = checkMandelbrot(x + (cntX*4 + 0)*stepX, y);
		convertToColor(loopNum, &r0, &g0, &b0);
		loopNum = checkMandelbrot(x + (cntX*4 + 1)*stepX, y);
		convertToColor(loopNum, &r1, &g1, &b1);
		loopNum = checkMandelbrot(x + (cntX*4 + 2)*stepX, y);
		convertToColor(loopNum, &r2, &g2, &b2);
		loopNum = checkMandelbrot(x + (cntX*4 + 3)*stepX, y);
		convertToColor(loopNum, &r3, &g3, &b3);

		/* pack 4-pixel data to accommodate with uint32_t */
		linebuf[cntX * 3 + 0] = r1 << 24 | b0 << 16 | g0 << 8 | r0;
		linebuf[cntX * 3 + 1] = g2 << 24 | r2 << 16 | b1 << 8 | g1;
		linebuf[cntX * 3 + 2] = b3 << 24 | g3 << 16 | r3 << 8 | b2;
	}

	/* copy one line */
	memcpy(vram, linebuf, IMAGE_WIDTH * BYTE_PER_PIXEL);
}

void mandelbrot(
		ap_uint<32> *vram,
		TYPE_VAL centerX,
		TYPE_VAL centerY,
		TYPE_VAL zoomHeight,	// e.g. set 0.5 for x2
		uint8_t  updateColorMap,
		uint8_t  colorMap[LOOP_NUM*3]
)
{
	/* prepare parameters for calculation */
	const TYPE_VAL height = 2 * zoomHeight;
	const TYPE_VAL width = height * (TYPE_VAL)((float)IMAGE_WIDTH / IMAGE_HEIGHT);
	const TYPE_VAL x0 = centerX - (width / 2);
	TYPE_VAL y = centerY + (height / 2);
	const TYPE_VAL stepX = width / IMAGE_WIDTH;
	const TYPE_VAL stepY = height / IMAGE_HEIGHT;

	if (updateColorMap != 0) {
		for(int i = 0; i < LOOP_NUM; i++) {
			s_colorMap[i * BYTE_PER_PIXEL + 0] = colorMap[i * BYTE_PER_PIXEL + 0];
			s_colorMap[i * BYTE_PER_PIXEL + 1] = colorMap[i * BYTE_PER_PIXEL + 1];
			s_colorMap[i * BYTE_PER_PIXEL + 2] = colorMap[i * BYTE_PER_PIXEL + 2];
		}
	}


//	double val;
//	val = zoom;	printf("zoom = %lf\n", val);
//	val = width;	printf("width = %lf\n", val);
//	val = height;	printf("height = %lf\n", val);
//	val = x0;	printf("x0 = %lf\n", val);
//	val = y0;	printf("y0 = %lf\n", val);
//	val = stepX;	printf("stepX = %lf\n", val);
//	val = stepY;	printf("stepY = %lf\n", val);

//	uint8_t loopNum;
//	checkMandelbrot(0.0, 0.0, &loopNum);
//	printf("%d\n", loopNum);
//	checkMandelbrot(0.3, 0.0, &loopNum);
//	printf("%d\n", loopNum);

	/* calculate Mandelbrot set for each pixel */
	for (uint16_t cntY = 0; cntY < IMAGE_HEIGHT; cntY++) {
//		printf("cntY = %d / %d\n", cntY, IMAGE_HEIGHT);
		mandelbrotLine(vram, x0, y, stepX);
		vram = vram + BYTE_PER_PIXEL * IMAGE_WIDTH / sizeof(ap_uint<32>);
		y = y - stepY;
	}

}

