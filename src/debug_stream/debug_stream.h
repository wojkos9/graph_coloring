#pragma once

#include <iostream>

#define endl "\n"

struct reql {int l; reql(int l): l(l) {}};

class DebugStream {
    public:
    int level, required_level;
    DebugStream() : level(0), required_level(1) {}
    template<class T>
    friend DebugStream& operator<<(DebugStream& str, T t)
    {
        if (str.level >= str.required_level) {
            std::cout << t;
        }
        return str;
    }
    friend DebugStream& operator<<(DebugStream& str, reql level)
    {
        str.required_level = level.l;
        return str;
    }
};