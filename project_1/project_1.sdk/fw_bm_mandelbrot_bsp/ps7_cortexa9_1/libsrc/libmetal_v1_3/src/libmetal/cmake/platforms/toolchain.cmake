set (CMAKE_SYSTEM_PROCESSOR "arm" CACHE STRING "")
set (MACHINE "zynq7")
set (CROSS_PREFIX "arm-none-eabi-" CACHE STRING "")
set (CMAKE_C_FLAGS "-O2 -c -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard -nostartfiles -Wall -Wextra -DUNDEFINE_FILE_OPS  -DUSE_AMP=1 -IC:/asd/dev/zynq/vivado/project_1/project_1.sdk/fw_bm_mandelbrot_bsp/ps7_cortexa9_1/include" CACHE STRING "")
set (CMAKE_SYSTEM_NAME "FreeRTOS" CACHE STRING "")
include (CMakeForceCompiler)
CMAKE_FORCE_C_COMPILER ("${CROSS_PREFIX}gcc" GNU)
CMAKE_FORCE_CXX_COMPILER ("${CROSS_PREFIX}g++" GNU)
set (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER CACHE STRING "")
set (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY NEVER CACHE STRING "")
set (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE NEVER CACHE STRING "")