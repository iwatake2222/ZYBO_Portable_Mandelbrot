############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2017 Xilinx, Inc. All Rights Reserved.
############################################################
open_project Mandelbrot
set_top mandelbrot
add_files Mandelbrot/parameters.h
add_files Mandelbrot/mandelbrot.cpp
add_files -tb Mandelbrot/mandelbrot_tb.cpp
open_solution "solution1"
set_part {xc7z020clg400-1} -tool vivado
create_clock -period 10 -name default
source "./Mandelbrot/solution1/directives.tcl"
csim_design
csynth_design
cosim_design
export_design -rtl verilog -format ip_catalog
