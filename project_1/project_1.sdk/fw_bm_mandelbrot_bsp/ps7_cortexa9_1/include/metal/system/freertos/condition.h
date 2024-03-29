/*
 * Copyright (c) 2016, Xilinx Inc. and Contributors. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Xilinx nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * @file	generic/condition.h
 * @brief	Generic condition variable primitives for libmetal.
 */

#ifndef __METAL_CONDITION__H__
#error "Include metal/condition.h instead of metal/freertos/condition.h"
#endif

#ifndef __METAL_FREERTOS_CONDITION__H__
#define __METAL_FREERTOS_CONDITION__H__

#include "metal/atomic.h"

#ifdef __cplusplus
extern "C" {
#endif

struct metal_condition {
	metal_mutex_t *m; /**< mutex.
	                       The condition variable is attached to
	                       this mutex when it is waiting.
	                       It is also used to check correctness
	                       in case there are multiple waiters. */

	atomic_int v; /**< condition variable value. */
};

/** Static metal condition variable initialization. */
#define METAL_CONDITION_INIT		{ NULL, ATOMIC_VAR_INIT(0) }

static inline void metal_condition_init(struct metal_condition *cv)
{
	/* TODO: Implement condition variable for FreeRTOS */
	(void)cv;
	return;
}

static inline int metal_condition_signal(struct metal_condition *cv)
{
	/* TODO: Implement condition variable for FreeRTOS */
	(void)cv;
	return 0;
}

static inline int metal_condition_broadcast(struct metal_condition *cv)
{
	/* TODO: Implement condition variable for FreeRTOS */
	(void)cv;
	return 0;
}


#ifdef __cplusplus
}
#endif

#endif /* __METAL_FREERTOS_CONDITION__H__ */
