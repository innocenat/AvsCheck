
#pragma once

#ifndef __AVS_TESTER_H
#define __AVS_TESTER_H

#include <string>
#include <memory>

#include "avsenv.h"
#include "avs_manager.h"

enum TestStatus
{
    AVSC_PASSED, AVSC_PARTIAL, AVSC_FAIL, AVSC_WTF, AVSC_ERROR
};

// -----------------------------------------------
// Macro for writing test framework

#define AVSC_TESTER(x)              class x : public Tester
#define AVSC_TESTER_MAIN_DEF        TestResult Run(std::shared_ptr<AvsManager>, const char*)
#define AVSC_TESTER_MAIN(x)         TestResult x::Run(std::shared_ptr<AvsManager> _____manager, const char* _____name)

#define AVSC_AVS(x)                 _____manager->GetScriptEnv(x);
#define AVSC_NUM_AVS                (1+_____manager->GetNumberOfDLL())
#define AVSC_AVS_INIT(x)            x->Invoke("Import", _____name);

#define AVSC_TESTER_ENTER           TestResult _____result; _____result.status = AVSC_FAIL;

#define AVSC_TEST_FAIL(msg_)           \
    _____result.status = AVSC_FAIL;    \
    _____result.msg = msg_;            \
    return _____result;

#define AVSC_TEST_SUCCESS(msg_)        \
    _____result.status = AVSC_PASSED;  \
    _____result.msg = msg_;            \
    return _____result;

#define AVSC_TEST_PARTIAL(msg_)        \
    _____result.status = AVSC_PARTIAL; \
    _____result.msg = msg_;            \
    return _____result;

#define AVSC_TEST_ERROR(msg_)          \
    _____result.status = AVSC_ERROR;   \
    _____result.msg = msg_;            \
    return _____result;

#define AVSC_TEST_WTF()            \
    _____result.status = AVSC_WTF; \
    return _____result;

// -----------------------------------------------


struct TestResult
{
    TestStatus status;
    std::string msg;
};

class Tester
{
public:
    virtual ~Tester() = 0;
    virtual AVSC_TESTER_MAIN_DEF = 0;
};

inline Tester::~Tester() {}

#endif
