
#define AVSC_DYNAMIC_DISPATCHING_MAIN
#include "output.h"

#include <tmmintrin.h>
#include <cstdint>

void frame_diff_planar(const BYTE* p1, const BYTE* p2, int p1_pitch, int p2_pitch, int width, int height, double* avg_diff, int* max_diff) {
    auto avg = _mm_setzero_si128();
    auto max = avg;

    int64_t avg_l = 0;
    int max_l = 0;

    int wMod16 = (width / 16) * 16;

    for (auto y = 0; y < height; y++) {
        for (auto x = 0; x < wMod16; x += 16) {
            auto data1 = _mm_loadu_si128((const __m128i*) (p1 + x));
            auto data2 = _mm_loadu_si128((const __m128i*) (p2 + x));

            auto sad = _mm_sad_epu8(data1, data2);

            auto diff1 = _mm_subs_epu8(data1, data2);
            auto diff2 = _mm_subs_epu8(data2, data1);
            auto diff = _mm_or_si128(diff1, diff2);

            max = _mm_max_epu8(max, diff);
            avg = _mm_add_epi64(avg, sad);
        }

        for (int x = wMod16; x < width; x++) {
            int diff = p1[x] - p2[x];
            if (diff < 0) diff = -diff;
            if (diff > max_l) max_l = diff;
            avg_l += diff;
        }

        p1 += p1_pitch;
        p2 += p2_pitch;
    }

    uint8_t max_result[16];
    int64_t avg_result[2];

    _mm_storeu_si128((__m128i*) max_result, max);
    _mm_storeu_si128((__m128i*) avg_result, avg);

    avg_l += avg_result[0] + avg_result[1];

    for (int i = 0; i < 16; i++) {
        if (max_result[i] > max_l)
            max_l = max_result[i];
    }

    *max_diff = max_l;
    *avg_diff = double(avg_l) / (width*height);
}

void AVSC_DISPATCH_FUNC(avsc_compare_output_planar)(const BYTE* p1, const BYTE* p2, int p1_pitch, int p2_pitch, int width, int height, double* avg_diff, int* max_diff) {
    AVSC_SET_FUNC(avsc_compare_output_planar, frame_diff_planar);
    return frame_diff_planar(p1, p2, p1_pitch, p2_pitch, width, height, avg_diff, max_diff);
}

void AVSC_DISPATCH_FUNC(avsc_compare_output_yuy2)() {

}

void AVSC_DISPATCH_FUNC(avsc_compare_output_rgb24)() {

}

void AVSC_DISPATCH_FUNC(avsc_compare_output_rgb32)() {

}


