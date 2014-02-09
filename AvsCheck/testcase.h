
#pragma once

#ifndef __AVS_TESTCASE_H
#define __AVS_TESTCASE_H

#include <string>
#include <vector>
#include <memory>

#include "tester.h"

class Testcase
{
public:
    Testcase(const std::vector<std::string>&);
    std::shared_ptr<Tester> GetTester();

    std::string GetName() { return name; };

private:
    std::vector<std::pair<std::string, std::string>> options;
    std::string name;

    std::string FindKey(const std::string&, const std::string&);
};

#endif
