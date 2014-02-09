
#include <memory>
#include <cstdint>
#include <avisynth.h>
#include <tmmintrin.h>

#include "dll_output_sse2.h"
#include "../../logger.h"

static inline void frame_diff_planar(const BYTE* p1, const BYTE* p2, int p1_pitch, int p2_pitch, int width, int height, double* avg_diff, int* max_diff) {
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

AVSC_TESTER_MAIN(DllOutputTesterSSE2) {
    AVSC_TESTER_ENTER;

    if (AVSC_NUM_AVS < dll) {
        AVSC_TEST_FAIL("Only single DLL loaded, cannot check between two DLLs.")
    }

    AvsScriptEnvironment env1 = AVSC_AVS(0);
    AvsScriptEnvironment env2 = AVSC_AVS(dll);
    
    AVSC_AVS_INIT(env1);
    AVSC_AVS_INIT(env2);

    avsc_log(VERBOSE, "Reference Avisynth: %s\n", env1.GetVersion());
    avsc_log(VERBOSE, "  Testing Avisynth: %s\n", env2.GetVersion());

    AVSValue val1, val2;
    AvsClip clip1, clip2;
    try {
        val1 = env1->Invoke("Eval", cmd.c_str());
    } catch (AvisynthError err) {
        avsc_log(ERR, "AvisynthError: %s\n", err.msg);
        AVSC_TEST_FAIL("AvisynthError occur.");
    }
    if (!val1.IsClip()) {\
        AVSC_TEST_FAIL("Reference DLL does not return clip on command.");
    }
    clip1 = val1.AsClip();

    try {
        val2 = env2->Invoke("Eval", cmd.c_str());
    } catch (AvisynthError err) {
        avsc_log(ERR, "AvisynthError: %s\n", err.msg);
        AVSC_TEST_FAIL("AvisynthError occur.");
    }
    if (!val2.IsClip()) {
        AVSC_TEST_FAIL("Reference DLL does not return clip on command.");
    }
    clip2 = val2.AsClip();

    VideoInfo vi = clip1->GetVideoInfo();
    int num_frames = vi.num_frames;

    if (!vi.IsPlanar()) {
        AVSC_TEST_FAIL("Only planar color spaces supported.");
    }

    const static int planes[] = { PLANAR_Y, PLANAR_U, PLANAR_V };

    int real_max = 0;
    double avg_max = 0;

    int frame;
    while ((frame = frame_list->Get(num_frames)) >= 0) {
        avsc_log(NORMAL, "Processing frame %d...\r", frame);
        avsc_log(VERBOSE, "Processing frame %d...\n", frame);

        AvsVideoFrame frame1 = clip1->GetFrame(frame, env1);
        AvsVideoFrame frame2 = clip2->GetFrame(frame, env2);

        for (int i = 0; i < 3; i++) {
            const int plane = planes[i];
            double avg;
            int max;

            frame_diff_planar(frame1->GetReadPtr(plane), frame2->GetReadPtr(plane),
                              frame1->GetPitch(plane), frame2->GetPitch(plane),
                              frame1->GetRowSize(plane), frame1->GetHeight(plane),
                              &avg, &max);

            avsc_log(VERBOSE, "Plane %d: avg=%lf, max=%d\n", plane, avg, max);

            if (real_max < max) real_max = max;
            if (avg_max < avg) avg_max = avg;
        }
    }

    avsc_log(VERBOSE, "Max diff: %d; max average diff: %lf\n", real_max, avg_max);

    if (real_max == 0) {
        AVSC_TEST_SUCCESS("All right!");
    } else if (real_max <= 2 && avg_max <= 0.02) {
        AVSC_TEST_PARTIAL("Rounding difference");
    } else {
        AVSC_TEST_FAIL("Too large margin of error.");
    }
}
