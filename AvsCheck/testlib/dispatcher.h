
#pragma once

#ifndef __DISPATCHER_H
#define __DISPATCHER_H

#define AVSC_DISPATCH_FUNC(name)                       ___##name##_dispatch__

#ifndef AVSC_DYNAMIC_DISPATCHING_MAIN
#   define AVSC_DISPATCH_REGISTER_FUNC(name)           extern decltype(&___##name##_dispatch__) name
#else
#   define AVSC_DISPATCH_REGISTER_FUNC(name)           decltype(&___##name##_dispatch__) name = ___##name##_dispatch__
#endif

#define AVSC_SET_FUNC(name, name2)                     name = name2

#endif
