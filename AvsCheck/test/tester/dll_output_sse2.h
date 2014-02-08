
#pragma once

#ifndef __TEST_OUTPUT_H
#define __TEST_OUTPUT_H

#include <string>
#include <memory>

#include "../tester.h"
#include "../framelist.h"

AVSC_TESTER(DllOutputTesterSSE2)
{
public:
    DllOutputTesterSSE2(std::string cmd, std::shared_ptr<Framelist> frame_list) : cmd(cmd), frame_list(frame_list) {}
    ~DllOutputTesterSSE2() {}

    AVSC_TESTER_MAIN_DEF;

private:
    std::string cmd;
    std::shared_ptr<Framelist> frame_list;
};

#endif
