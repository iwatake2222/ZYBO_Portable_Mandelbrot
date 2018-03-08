/*
 * parameters.h
 *
 *  Created on: 2018/01/27
 *      Author: tak
 */

#ifndef SRC_UTILITY_PARAMETERS_H_
#define SRC_UTILITY_PARAMETERS_H_


/* platform config */
#define WITH_OPEN_AMPLIB	// need this if I add openamp lib to BSP no matter whether I use it or not

//#define STANDALONE
// if not STANDALONE, don't forget to set  -DUSE_AMP=1 to BSP settings

#define IMAGE_WIDTH  640
#define IMAGE_HEIGHT 480
#define BYTE_PER_PIXEL 3
#define IMAGE_STRIDE (IMAGE_WIDTH * BYTE_PER_PIXEL)
//#define IMAGE_STRIDE 0x1000

#define MANDELBROT_LOOP_NUM 128

/* word2 = MSB, word 0 = LSB */
typedef struct {
	uint32_t word0;
	uint32_t word1;
	uint32_t word2;
} MANDELBROT_VAL;


#define IMAGE_BUFFER_A  0x3F000000
#define IMAGE_BUFFER_B  0x3F100000
#define IMAGE_BUFFER_C  0x3F200000
#define IMAGE_BUFFER_D  0x3F300000
#define IMAGE_BUFFER_E  0x3F400000
#define IMAGE_BUFFER_F  0x3F500000
#define IMAGE_BUFFER_G  0x3F600000
#define IMAGE_BUFFER_H  0x3F700000

#define JPEG_BUFFER_A  0x3F800000
#define JPEG_BUFFER_B  0x3F900000
#define JPEG_BUFFER_C  0x3FA00000
#define JPEG_BUFFER_D  0x3FB00000
#define JPEG_BUFFER_E  0x3FC00000
#define JPEG_BUFFER_F  0x3FD00000
#define JPEG_BUFFER_G  0x3FE00000
#define JPEG_BUFFER_H  0x3FF00000

#endif /* SRC_UTILITY_PARAMETERS_H_ */
