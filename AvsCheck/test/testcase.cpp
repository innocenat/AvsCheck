
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

#include "testcase.h"
#include "framelist.h"
#include "../logger.h"

#define VAL_ERROR "[error]"

// Tester file
#include "tester/dll_output_sse2.h"

// TODO make more tester;
std::shared_ptr<Tester> Testcase::GetTester() {
    using namespace std;

    Tester* t;

    // Frame list
    string frame = FindKey("frame", "all");
    Framelist* _frame_list;
    if (frame == "all") {
        _frame_list = new AllFramelist();
    } else {
        _frame_list = new Framelist(frame);
    }
    auto frame_list = shared_ptr<Framelist>(_frame_list);

    // Command
    string cmd = FindKey("cmd", VAL_ERROR);
    if (cmd == VAL_ERROR) {
        avsc_log(ERR, "\nTestcase: cmd option required in test case \"%s\"\n", name.c_str());
        return std::shared_ptr<Tester>();
    }

    string dll = FindKey("dll", "1");

    avsc_log(VERBOSE, "\nTestcase: Using DLLOutputTester for test case \"%s\"\n", name.c_str());
    t = new DllOutputTesterSSE2(cmd, frame_list, stoi(dll));

    return std::shared_ptr<Tester>(t);
}


Testcase::Testcase(const std::vector<std::string>& tokens) {
    bool named = false;
    for (size_t i = 0; i < tokens.size(); i++) {
        std::string obj = tokens[i];

        if (obj == "case" || obj == ":") {
            continue;
        } else if (!named) {
            named = true;

            size_t len = obj.length();
            if (obj[len - 1] == ':') {
                name = obj.substr(1, len - 3);
            } else {
                name = obj.substr(1, len - 2);
            }

            avsc_log(VERBOSE, "\nTestcase: %s\n", name.c_str());

            continue;
        }

        size_t eq_pos = obj.find("=");
        std::string key = obj.substr(0, eq_pos);
        std::string val = obj.substr(eq_pos + 1);

        options.push_back(make_pair(key, val));
        avsc_log(VERBOSE, "        : key=%s : value=%s\n", key.c_str(), val.c_str());
    }
}

std::string Testcase::FindKey(const std::string& k, const std::string& d) {
    for (size_t i = 0; i < options.size(); i++) {
        if (k == options[i].first)
            return options[i].second;
    }

    return d;
}
