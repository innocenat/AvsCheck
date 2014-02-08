
#pragma once

#ifndef __AVS_PARSER_H
#define __AVS_PARSER_H

#include <string>
#include <vector>
#include <memory>

#include "avsenv.h"
#include "avs_manager.h"
#include "test/testcase.h"

class Parser
{
public:
    explicit Parser(const char* ptr);
    bool Parse();

    std::shared_ptr<Testcase> GetTestCase(int i) {
        return testcases[i];
    }

    size_t NumTestCase() {
        return testcases.size();
    }

    std::shared_ptr<AvsManager> GetAvsManager() {
        return manager;
    }

private:
    const char* file_name;
    bool is_parsed, is_valid;

    std::vector<std::shared_ptr<Testcase>> testcases;
    std::shared_ptr<AvsManager> manager;
};

#endif
