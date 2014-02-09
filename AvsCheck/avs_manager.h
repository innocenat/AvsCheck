
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

    std::shared_ptr<AvsEnv> GetAvsEnv(int i = 0) {
        if (i > GetNumberOfDLL()) {
            return std::shared_ptr<AvsEnv>();
        }

        if (i == 0) {
            return main_env;
        }
        return testing_env[i - 1];
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

    std::shared_ptr<AvsEnv> main_env;
    std::vector<std::shared_ptr<AvsEnv>> testing_env;

    void SetMainDll(std::string dll) {
        main_dll = dll;
        main_env = AvsEnv::Create(dll.c_str());
    }

    void AddTestDll(std::string dll) {
        testing_dll.push_back(dll);
        testing_env.push_back(AvsEnv::Create(dll.c_str()));
    }
};

#endif
