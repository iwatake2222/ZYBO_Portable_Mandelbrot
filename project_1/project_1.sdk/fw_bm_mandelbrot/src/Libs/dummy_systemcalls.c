/*
 * dummy_systemcalls.c
 *
 *  Created on: 2018/01/30
 *      Author: tak
 */


#ifdef __cplusplus
extern "C" {
#endif


/* without these dummy system call functions, I get errors such as "" */
/* I should use "--specs=nosys.specs" option, but it seems this toolchain doesn't support it */
/* So, I implement these system calls instead */


int _unlink (const char *path)
{
	return -1;
}


#ifdef __cplusplus
}
#endif

