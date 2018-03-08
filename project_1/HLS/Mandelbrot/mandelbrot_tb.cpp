#include <stdint.h>
#include <ap_int.h>
#include <stdio.h>
#include "parameters.h"

static ap_uint<32> s_vram[IMAGE_WIDTH * IMAGE_HEIGHT * BYTE_PER_PIXEL / sizeof(ap_uint<32>)];

void fileout(const char* filename, uint8_t *vram, uint32_t size)
{
	FILE *fd = fopen(filename, "wb");
	fwrite(vram, 1, size, fd);
}

extern void mandelbrot(
		ap_uint<32> *vram,
		TYPE_VAL centerX,
		TYPE_VAL centerY,
		TYPE_VAL zoom,
		uint8_t  updateColorMap,
		uint8_t  colorMap[LOOP_NUM*3]
);


int main()
{
	printf("Hello \n");

	uint8_t colorMap[LOOP_NUM*3];
	for(int i = 0; i < LOOP_NUM; i++) {
		colorMap[(LOOP_NUM - 1 - i)*3 + 0] = i*4;
		colorMap[(LOOP_NUM - 1 - i)*3 + 1] = i*3;
		colorMap[(LOOP_NUM - 1 - i)*3 + 2] = i*2;
	}

	// mandelbrot(s_vram, 0.0 * SFPN_SHIFT_VAL, 0.0 * SFPN_SHIFT_VAL, 1 * SFPN_SHIFT_VAL, 0);
	mandelbrot(s_vram, 0, 0, 1, 1, colorMap);
	fileout("test1.raw", (uint8_t *)s_vram, IMAGE_WIDTH * IMAGE_HEIGHT * BYTE_PER_PIXEL);

	mandelbrot(s_vram, 0.2, 0, 1, 0, colorMap);
	fileout("test2.raw", (uint8_t *)s_vram, IMAGE_WIDTH * IMAGE_HEIGHT * BYTE_PER_PIXEL);
//	mandelbrot(s_vram, 0.2999999999999998, 0.48749999999999993, 0.1);
	// mandelbrot(s_vram, 0.33843749999999984 * SFPN_SHIFT_VAL, 0.5578125 * SFPN_SHIFT_VAL, 0.1 * SFPN_SHIFT_VAL, 0);



	return 0;
}
