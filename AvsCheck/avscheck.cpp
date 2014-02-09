
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <windows.h>
#include <avisynth.h>

#include "avsenv.h"
#include "parser.h"
#include "logger.h"

#define AVSCHECK_VERSION "0.1.0"

using namespace std;


AVS_MAIN_FILE;

int main(int argc, char* argv[]) {

    printf("AvsCheck " AVSCHECK_VERSION " - a testing tool for AviSynth development\n"
           "-----------------------------------------------------------------------\n");

    if (argc < 2) {
        printf("\n"
               "    Usage: %s script.avs [-v] [-l file.log]\n"
               "\n"
               "Options:\n"
               "    -v          : Verbose\n"
               "    -l file.log : Also log output to file.\n"
               "\n", argv[1]);
        return 0;
    }

    const char* file = nullptr;
    const char* logfile = nullptr;

    { // Option parsing
        int curr = 1;

        file = argv[curr++];

        while (curr < argc) {
            if (strcmp(argv[curr], "-v") == 0) {
                logger_set_verbose(true);
                curr++;
            } else if (strcmp(argv[curr], "-l") == 0) {
                if (curr + 1 >= argc) {
                    printf("\n"
                           "Invalid option format: %s\n"
                           "\n", argv[curr]);
                    return -1;
                }
                curr++;
                logfile = argv[curr++];
            } else {
                printf("\n"
                       "Unknown option: %s\n"
                       "\n", argv[curr]);
                return -1;
            }
        }
    }

    Parser p(file);
    p.Parse();

    int passed = 0, partial = 0, error = 0;

    shared_ptr<AvsManager> avs_manager = p.GetAvsManager();

        avsc_log(NORMAL, "      Main avisynth: %s\n", avs_manager->GetAvsEnv()->GetVersion());
    for (int i = 0; i < avs_manager->GetNumberOfDLL(); i++) {
        avsc_log(NORMAL, "Testing#%02d avisynth: %s\n", i+1, avs_manager->GetAvsEnv(i+1)->GetVersion());
    }
    avsc_log(NORMAL, "\n");

    // Run each test case
    for (size_t i = 0; i < p.NumTestCase(); i++) {
        std::shared_ptr<Testcase> testcase = p.GetTestCase(i);
        std::shared_ptr<Tester> tester = testcase->GetTester();

        if (tester) {
            avsc_log(NORMAL, "Initializing...\r");
            TestResult r = tester->Run(avs_manager, file);

            avsc_log(NORMAL, "%79s\r", ""); // Clear any existing line
            switch (r.status) {
            case AVSC_FAIL:    avsc_log(NORMAL, "[FAIL] "); error++;  break;
            case AVSC_PARTIAL: avsc_log(NORMAL, "[PART] "); partial++; break;
            case AVSC_PASSED:  avsc_log(NORMAL, "[ OK ] "); passed++;   break;
            default:           avsc_log(NORMAL, "[WTF?] "); r.msg.assign("It looks like this tester did not do anything :-(");
            }

            avsc_log(NORMAL, "%s\n", testcase->GetName().c_str());
            avsc_log(NORMAL, "       %s\n", r.msg.c_str());
        } else {
            avsc_log(NORMAL, "[ ERR] Test case \"%s\" cannot be run.\n", testcase->GetName().c_str());
        }
    }

    avsc_log(NORMAL, "-----------------------------------------------------------------------\n");
    int total = p.NumTestCase();
    avsc_log(NORMAL, "         Total: %d\n", total);
    avsc_log(NORMAL, "        Passed: %d (%d%%)\n", passed, 100 * passed / total);
    avsc_log(NORMAL, "Partial Passed: %d (%d%%)\n", partial, 100 * partial / total);
    avsc_log(NORMAL, "         Error: %d (%d%%)\n", error, 100 * error / total);
    avsc_log(NORMAL, "-----------------------------------------------------------------------\n");

    return 0;
}
