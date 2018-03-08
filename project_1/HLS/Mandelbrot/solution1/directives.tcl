############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2017 Xilinx, Inc. All Rights Reserved.
############################################################
set_directive_interface -mode m_axi -depth 2764800 -offset slave "mandelbrot" vram
set_directive_interface -mode s_axilite "mandelbrot" centerX
set_directive_interface -mode s_axilite "mandelbrot" centerY
set_directive_interface -mode s_axilite "mandelbrot" zoom
set_directive_interface -mode s_axilite "mandelbrot"
set_directive_loop_tripcount -max 128 "checkMandelbrot/checkMandelbrot_label3"
set_directive_loop_tripcount -max 320 "mandelbrot/mandelbrot_label2"
set_directive_occurrence -cycle 64 "checkMandelbrot4Pixel/setLoopNum1"
set_directive_occurrence -cycle 64 "checkMandelbrot4Pixel/setLoopNum3"
set_directive_occurrence -cycle 64 "checkMandelbrot4Pixel/setLoopNum2"
set_directive_occurrence -cycle 64 "checkMandelbrot4Pixel/setLoopNum0"
set_directive_inline "mandelbrotLine"
set_directive_interface -mode s_axilite "mandelbrot" colorMap
set_directive_interface -mode s_axilite "mandelbrot" updateColorMap
set_directive_interface -mode s_axilite "mandelbrot" zoomHeight
