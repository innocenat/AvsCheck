
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>

#include "parser.h"
#include "logger.h"

#define out(x) (cout << x);

Parser::Parser(const char* fname) : file_name(fname), is_parsed(false), is_valid(true) {
    manager = std::make_shared<AvsManager>();
}

bool Parser::Parse() {
    using namespace std;

    if (is_parsed)
        return true;

    ifstream file(file_name);

    if (!file.good()) {
        avsc_fatal("Parser: cannot open %s\n", file_name);
        is_valid = false;
        return false;
    }

    // File path
    string fname(file_name);
    size_t last_slash = fname.find_last_of("/\\");
    string fpath = fname.substr(0, last_slash + 1);

    avsc_log(VERBOSE, "Parser: File base directory: %s\n", fpath.c_str());

    while (file.good()) {
        string line;
        getline(file, line);

        if (line.length() < 3) {
            continue;
        }

        // Check if it's config line
        if (line.length() < 3 || line[0] != '#' || line[1] != '~' || line[2] != '!') {
            continue;
        }

        // Tokenize line
        size_t cur_str_pos = 3;
        size_t line_length = line.length();
        vector<string> tokens;
        while (cur_str_pos < line_length) {
            // Scroll past all whitespace
            while (isspace(line[cur_str_pos])) {
                cur_str_pos++;
            }

            size_t token_begin = cur_str_pos;
            size_t token_end = token_begin;
            
            size_t quote_count = 0;
            
            while (token_end < line_length) {
                if (line[token_end] == '\"' && line[token_end - 1] != '\\') {
                    quote_count++;
                }

                if (quote_count % 2 == 0 && isspace(line[token_end])) {
                    break;
                }

                token_end++;
            }
            
            tokens.push_back(line.substr(token_begin, token_end - token_begin));
            cur_str_pos = token_end;
        }

        size_t num_token = tokens.size();
        if (num_token < 2)
            continue;

        if (tokens[0] == "using:") {
            manager->SetMainDll(fpath + tokens[1]);
            avsc_log(VERBOSE, "Parser: main AviSynth to load : %s\n", (fpath + tokens[1]).c_str());
        } else if (tokens[0] == "test:") {
            manager->AddTestDll(fpath + tokens[1]);
            avsc_log(VERBOSE, "Parser: additional AviSynth : %s\n", (fpath + tokens[1]).c_str());
        } else if (tokens[0] == "case") {
            shared_ptr<Testcase> new_testcase = make_shared<Testcase>(tokens);
            testcases.push_back(new_testcase);
        }
    }

    return true;

}

 
