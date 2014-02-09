
#include "cpuid.h"
#include <intrin.h>

#define check_bit(field, bit) ((field) & (1 << (bit)) ? true : false)

static int real_get_cpu_flag() {
    int flag = 0;
    int cpuid[4];
    bool avx = false;
    int max_val = 0;

    __cpuid(cpuid, 0);
    max_val = cpuid[0];

    if (max_val >= 1) {
        __cpuid(cpuid, 1);
        avx = check_bit(cpuid[2], 27) && (_xgetbv(_XCR_XFEATURE_ENABLED_MASK) & 0x6ull) == 0x6ull;

        flag |= check_bit(cpuid[3], 26) ? AVSC_SSE2 : 0;
        flag |= check_bit(cpuid[2],  0) ? AVSC_SSE3 : 0;
        flag |= check_bit(cpuid[2],  9) ? AVSC_SSSE3 : 0;
        flag |= check_bit(cpuid[2], 19) ? AVSC_SSE4_1 : 0;
        flag |= check_bit(cpuid[2], 20) ? AVSC_SSE4_2 : 0;
        flag |= check_bit(cpuid[2], 12) ? AVSC_FMA3 : 0;
        flag |= check_bit(cpuid[2], 29) ? AVSC_F16C : 0;
        flag |= avx && check_bit(cpuid[2], 28) ? AVSC_AVX : 0;
    }

    if (max_val >= 7) {
        __cpuid(cpuid, 7);
        flag |= avx && check_bit(cpuid[1], 5) ? AVSC_AVX2 : 0;
        flag |= avx && check_bit(cpuid[1], 16) ? AVSC_AVX512 : 0;
    }

    return flag;
}

int avsc_get_cpu_flag() {
    static int flag = real_get_cpu_flag();
    return flag;
}
