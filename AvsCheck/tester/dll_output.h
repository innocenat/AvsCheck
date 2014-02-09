
#pragma once

#ifndef __TEST_OUTPUT_H
#define __TEST_OUTPUT_H

#include <string>
#include <memory>

#include "../tester.h"
#include "../framelist.h"

AVSC_TESTER(DllOutputTester)
{
public:
    DllOutputTester(std::string cmd, std::shared_ptr<Framelist> frame_list, int dll) : cmd(cmd), frame_list(frame_list), dll(dll) {}
    ~DllOutputTester() {}

    AVSC_TESTER_MAIN_DEF;

private:
    std::string cmd;
    std::shared_ptr<Framelist> frame_list;
    int dll;
};

#endif
