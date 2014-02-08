
#pragma once

#ifndef __AVS_MANAGER_H
#define __AVS_MANAGER_H

#include <string>
#include <vector>
#include <memory>

#include "avsenv.h"

class AvsManager
{
    friend class Parser;
public:
    AvsManager() {}

    std::shared_ptr<AvsEnv> GetAvsEnv(int i = -1) {
        if (i >= GetNumberOfDLL()) {
            return std::shared_ptr<AvsEnv>();
        }

        if (i < 0) {
            return AvsEnv::Create(main_dll.c_str());
        }

        return AvsEnv::Create(testing_dll[i].c_str());
    }

    // TODO actually reuse this, not create new one every time.
    AvsScriptEnvironment GetScriptEnv() {
        return AvsScriptEnvironment(GetAvsEnv());
    }

    AvsScriptEnvironment GetScriptEnv(size_t i) {
        return AvsScriptEnvironment(GetAvsEnv(i));
    }

    int GetNumberOfDLL() {
        return testing_dll.size();
    }

private:
    std::string main_dll;
    std::vector<std::string> testing_dll;
};

#endif
