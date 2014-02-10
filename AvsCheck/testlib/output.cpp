
#define AVSC_DYNAMIC_DISPATCHING_MAIN
#include "output.h"

bool AVSC_DISPATCH_FUNC(avsc_compare_output_planar)(const BYTE* p1, const BYTE* p2, int p1_pitch, int p2_pitch, int width, int height, double* avg_diff, int* max_diff) {
    AVSC_DISPATCH(AVSC_SSE2, avsc_compare_output_planar, avsc_compare_output_planar_sse2)(p1, p2, p1_pitch, p2_pitch, width, height, avg_diff, max_diff);
    return false;
}

bool AVSC_DISPATCH_FUNC(avsc_compare_output_yuy2)() {
    return false;
}

bool AVSC_DISPATCH_FUNC(avsc_compare_output_rgb24)() {
    return false;
}

bool AVSC_DISPATCH_FUNC(avsc_compare_output_rgb32)() {
    return false;
}


