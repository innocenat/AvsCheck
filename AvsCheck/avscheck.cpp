
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <windows.h>
#include <avisynth.h>

#include "avsenv.h"
#include "parser.h"
#include "logger.h"

#define AVSCHECK_VERSION "0.1.0"

using namespace std;


AVS_MAIN_FILE;

int main(int argc, char* argv[]) {

    printf("AvsCheck " AVSCHECK_VERSION " - a testing tool for AviSynth development\n"
           "-----------------------------------------------------------------------\n");

    if (argc < 2) {
        printf("\n"
               "    Usage: %s script.avs [-v] [-l file.log]\n"
               "\n"
               "Options:\n"
               "    -v          : Verbose\n"
               "    -l file.log : Also log output to file.\n"
               "\n", argv[1]);
        return 0;
    }

    const char* file = nullptr;
    const char* logfile = nullptr;

    { // Option parsing
        int curr = 1;

        file = argv[curr++];

        while (curr < argc) {
            if (strcmp(argv[curr], "-v") == 0) {
                logger_set_verbose(true);
                curr++;
            } else if (strcmp(argv[curr], "-l") == 0) {
                if (curr + 1 >= argc) {
                    printf("\n"
                           "Invalid option format: %s\n"
                           "\n", argv[curr]);
                    return -1;
                }
                curr++;
                logfile = argv[curr++];
            } else {
                printf("\n"
                       "Unknown option: %s\n"
                       "\n", argv[curr]);
                return -1;
            }
        }
    }

    Parser p(file);
    p.Parse();

    int passed = 0, partial = 0, error = 0;

    // Run each test case
    for (size_t i = 0; i < p.NumTestCase(); i++) {
        std::shared_ptr<Testcase> testcase = p.GetTestCase(i);
        std::shared_ptr<Tester> tester = testcase->GetTester();

        if (tester) {
            TestResult r = tester->Run(p.GetAvsManager(), file);

            avsc_log(NORMAL, "%79s\r", ""); // Clear any existing line
            switch (r.status) {
            case AVSC_FAIL:    avsc_log(NORMAL, "[FAIL] "); error++;  break;
            case AVSC_PARTIAL: avsc_log(NORMAL, "[PART] "); partial++; break;
            case AVSC_PASSED:  avsc_log(NORMAL, "[ OK ] "); passed++;   break;
            default:           avsc_log(NORMAL, "[WTF?] "); r.msg.assign("It looks like this tester did not do anything :-(");
            }

            avsc_log(NORMAL, "%s\n", testcase->GetName().c_str());
            avsc_log(NORMAL, "       %s\n", r.msg.c_str());
        } else {
            avsc_log(NORMAL, "[ ERR] Test case \"%s\" cannot be run.\n", testcase->GetName().c_str());
        }
    }

    avsc_log(NORMAL, "---------------------------------------------\n");
    int total = p.NumTestCase();
    avsc_log(NORMAL, "         Total: %d\n", total);
    avsc_log(NORMAL, "        Passed: %d (%d%%)\n", passed, 100 * passed / total);
    avsc_log(NORMAL, "Partial Passed: %d (%d%%)\n", partial, 100 * partial / total);
    avsc_log(NORMAL, "         Error: %d (%d%%)\n", error, 100 * error / total);

    return 0;

}

#if 0

int buffer_size(const VideoInfo& vi) {
    int base_size = vi.width*vi.height;

    if (vi.IsPlanar() && !vi.IsY8()) {
        const int height = vi.GetPlaneHeightSubsampling(PLANAR_U);
        const int width = vi.GetPlaneWidthSubsampling(PLANAR_U);

        int new_size = (vi.width >> width) * (vi.height >> height);
        base_size += new_size * 2;
    } else if (vi.IsYUY2()) {
        base_size *= 2;
    } else if (vi.IsRGB24()) {
        base_size *= 3;
    } else if (vi.IsRGB32()) {
        base_size *= 4;
    }

    return base_size;
}

void store_frame(BYTE* dst, PVideoFrame src, const VideoInfo& vi) {
    static int plane[] = { PLANAR_Y, PLANAR_U, PLANAR_V };
    int max_plane = vi.IsPlanar() ? 3 : 1;

    for (int i = 0; i < max_plane; i++) {
        const BYTE* srcp = src->GetReadPtr(plane[i]);
        for (int y = 0; y < src->GetHeight(plane[i]); y++) {
            for (int x = 0; x < src->GetRowSize(plane[i]); x++) {
                *dst = srcp[x];
                ++dst;
            }
            srcp += src->GetPitch(plane[i]);
        }
    }
}

double compare_frame(const BYTE* a, const BYTE* b, int size) {
    double diff = 0;

    for (int i = 0; i < size; i++) {
        diff += abs(*a - *b);
        a++;
        b++;
    }

    return diff / size;
}

int main0(int argc, char* argv[]) {

    if (argc < 4) {
        printf("Usage: avscheck avisynth1.dll avisynth2.dll script.avs interval\n");
        return 0;
    }

    const char* avs1 = argv[1];
    const char* avs2 = argv[2];
    const char* script_file = argv[3];
    int interval = atoi(argv[4]);

    printf("AvisynthCheck\n");
    printf("-------------------------------\n");

    int frame_size;

    shared_ptr<AvsEnv> avs_env1 = AvsEnv::Create(avs1);
    shared_ptr<AvsEnv> avs_env2 = AvsEnv::Create(avs2);

    if (!avs_env1) {
        printf("Error: cannot load %s\n", avs1);
        return -1;
    }

    if (!avs_env2) {
        printf("Error: cannot load %s\n", avs2);
        return -1;
    }

    AvsScriptEnvironment env1(avs_env1), env2(avs_env2);
    VideoInfo vi;
    AvsClip clip1, clip2;

    try {
        clip1 = env1->Invoke("Import", script_file).AsClip();
        clip2 = env2->Invoke("Import", script_file).AsClip();

        vi = clip1->GetVideoInfo();
    } catch (AvisynthError r) {
        printf("Avisynth error: %s\n", r.msg);
        return -1;
    }

    int len = vi.num_frames;
    frame_size = buffer_size(vi);

    printf("Frame count: %d\n", len);
    printf("Interval: %d\n", interval);
    printf("-------------------------------\n");

    BYTE* s1 = (BYTE*) malloc(sizeof(BYTE) * frame_size);
    BYTE* s2 = (BYTE*) malloc(sizeof(BYTE) * frame_size);

    for (int i = 0, j = 0; i < len; i += interval, j++) {
        printf("Fetching frames %d...\r", i);

        AvsVideoFrame src1 = clip1->GetFrame(i, env1);
        AvsVideoFrame src2 = clip2->GetFrame(i, env2);

        store_frame(s1, src1, vi);
        store_frame(s2, src2, vi);

        printf("Frame %4d: %lf\n", i, compare_frame(s1, s2, frame_size));
    }

    printf("Done \n");

    return 0;
}

#endif
