
#pragma once

#ifndef __CPUID_H
#define __CPUID_H

enum
{
    AVSC_SSE2       = 1 << 0,
    AVSC_SSE3       = 1 << 1,
    AVSC_SSSE3      = 1 << 2,
    AVSC_SSE4_1     = 1 << 3,
    AVSC_SSE4_2     = 1 << 4,
    AVSC_AVX        = 1 << 5,
    AVSC_AVX2       = 1 << 5,
    AVSC_FMA3       = 1 << 6,
    AVSC_F16C       = 1 << 7,
    AVSC_AVX512     = 1 << 8
};

int avsc_get_cpu_flag();

#endif
