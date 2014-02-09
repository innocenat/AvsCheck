
#pragma once

#ifndef __TESTLIB_OUTPUT_H
#define __TESTLIB_OUTPUT_H

#include "dispatcher.h"
#include <avisynth.h>

bool AVSC_DISPATCH_FUNC(avsc_compare_output_planar)(const BYTE* p1, const BYTE* p2, int p1_pitch, int p2_pitch, int width, int height, double* avg_diff, int* max_diff);
bool AVSC_DISPATCH_FUNC(avsc_compare_output_yuy2)();
bool AVSC_DISPATCH_FUNC(avsc_compare_output_rgb24)();
bool AVSC_DISPATCH_FUNC(avsc_compare_output_rgb32)();

AVSC_DISPATCH_REGISTER_FUNC(avsc_compare_output_planar);
AVSC_DISPATCH_REGISTER_FUNC(avsc_compare_output_yuy2);
AVSC_DISPATCH_REGISTER_FUNC(avsc_compare_output_rgb24);
AVSC_DISPATCH_REGISTER_FUNC(avsc_compare_output_rgb32);

bool avsc_compare_output_planar_sse2(const BYTE* p1, const BYTE* p2, int p1_pitch, int p2_pitch, int width, int height, double* avg_diff, int* max_diff);

#endif
