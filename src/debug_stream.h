#pragma once

#include <iostream>

#define endl "\n"

class DebugStream {
    public:
    bool active;
    DebugStream(bool ac) : active(ac) {}
    template<class T>
    friend DebugStream& operator<<(DebugStream& str, T t) {
        if (str.active) {
            std::cout << t;
        }
        return str;
    }
};