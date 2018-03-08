#ifndef PARAMETERS_H_
#define PARAMETERS_H_

#define IMAGE_WIDTH  640
#define IMAGE_HEIGHT 480
#define BYTE_PER_PIXEL 3

#define LOOP_NUM 128

#define BIT_NUM           96
#define DECIMAL_BIT_NUM   6

// need to consider over flow
typedef ap_fixed<BIT_NUM, DECIMAL_BIT_NUM, AP_TRN, AP_WRAP> TYPE_VAL;
//typedef uint32_t TYPE_VAL;

#endif /* PARAMETERS_H_ */
