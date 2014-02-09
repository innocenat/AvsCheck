
#pragma once

#ifndef __DISPATCHER_H
#define __DISPATCHER_H

#define AVSC_DISPATCH_FUNC(name)                       ___##name##_dispatch__

#ifndef AVSC_DYNAMIC_DISPATCHING_MAIN
#   define AVSC_DISPATCH_REGISTER_FUNC(name)           extern decltype(&___##name##_dispatch__) name
#else
#   define AVSC_DISPATCH_REGISTER_FUNC(name)           decltype(&___##name##_dispatch__) name = ___##name##_dispatch__
#endif

#include "cpuid.h"
#define AVSC_DISPATCH(flag, name, name2)               if ((avsc_get_cpu_flag() & flag) == flag && (name = name2) != nullptr) return name2

#endif
