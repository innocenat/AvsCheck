
#pragma once

#ifndef __AVS_FRAMELIST_H
#define __AVS_FRAMELIST_H

#include <string>
#include <vector>

class Framelist
{
public:
    Framelist();
    Framelist(std::string list);
    virtual int Get(int);
    virtual void Reset();

protected:
    int current_frame;

private:
    size_t current_pos;
    size_t l_size = 0;
    std::vector<std::pair<int, int>> frame_list;
};

class AllFramelist : public Framelist
{
public:
    AllFramelist();
    int Get(int);
    void Reset();
};

#endif
