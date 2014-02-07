
#include <cstdio>
#include <cstdlib>
#include <windows.h>
#include <avisynth.h>
#include "avsenv.h"

using namespace std;

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

AVS_MAIN_FILE;

int main(int argc, char* argv[]) {

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
