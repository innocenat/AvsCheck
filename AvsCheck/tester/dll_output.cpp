
#include <memory>
#include <cstdint>
#include <avisynth.h>
#include <xmmintrin.h>

#include "dll_output.h"
#include "../logger.h"
#include "../testlib/output.h"

AVSC_TESTER_MAIN(DllOutputTester) {
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
    if (!val1.IsClip()) {
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

        AvsVideoFrame frame1 = clip1->GetFrame(frame, env1);
        AvsVideoFrame frame2 = clip2->GetFrame(frame, env2);

        for (int i = 0; i < 3; i++) {
            const int plane = planes[i];
            double avg;
            int max;

            bool r = avsc_compare_output_planar(frame1->GetReadPtr(plane), frame2->GetReadPtr(plane),
                                                frame1->GetPitch(plane), frame2->GetPitch(plane),
                                                frame1->GetRowSize(plane), frame1->GetHeight(plane),
                                                &avg, &max);

            if (!r) {
                AVSC_TEST_ERROR("Tester doesn't support current architecture.");
            }

            avsc_log(VERBOSE, "Frame %d: plane=%d avg=%lf, max=%d\n", frame, plane, avg, max);

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
