
#include "framelist.h"
#include "../logger.h"

Framelist::Framelist() : current_pos(0), current_frame(0) {}

Framelist::Framelist(std::string list) : current_pos(0), current_frame(0) {
    using namespace std;

    current_pos = 0;
    current_frame = 0;

    // Parse frame list and make vector
    {
        size_t curr = 0;
        size_t length = list.length();

        while (curr < length) {
            size_t begin = curr;
            size_t end = list.find(",", begin);

            if (end == string::npos)
                end = length;

            string item = list.substr(begin, end - begin);

            int start_fr, end_fr;

            size_t eq_pos = item.find("-");
            if (eq_pos != string::npos) {
                string st = item.substr(0, eq_pos);
                string ed = item.substr(eq_pos + 1);

                start_fr = stoi(st);
                end_fr = stoi(ed);
            } else {
                start_fr = end_fr = stoi(item);
            }

            frame_list.push_back(make_pair(start_fr, end_fr));

            curr = end + 1;
        }

        l_size = frame_list.size();

        if (l_size > 0) {
            current_frame = frame_list[0].first;
        } else {
            current_frame = -1;
        }
    }
}

int Framelist::Get(int num_frames) {
    if (current_pos >= l_size)
        return -1;

    int _ret = current_frame;

    std::pair<int, int> curr = frame_list[current_pos];

    if (current_frame >= curr.second) {
        if (current_pos == l_size - 1) {
            current_pos++;
            current_frame = -1;
        } else {
            curr = frame_list[++current_pos];
            current_frame = curr.first;
        }
    } else {
        current_frame++;
    }

    if (current_frame >= num_frames)
        return -1;

    return _ret;
}

void Framelist::Reset() {
    current_pos = 0;
    if (l_size > 0) {
        current_frame = frame_list[0].first;
    } else {
        current_frame = -1;
    }
}

AllFramelist::AllFramelist() : Framelist() {}
int AllFramelist::Get(int num_frames) { return current_frame >= num_frames ? -1 : current_frame++; }
void AllFramelist::Reset() { current_frame = 0; }
