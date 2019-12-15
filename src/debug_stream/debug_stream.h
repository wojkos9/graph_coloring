#pragma once

#include <iostream>
#include <windows.h>


struct reql {int l; reql(int l): l(l) {}};
static struct unlock_t {} unlock;
static struct dendl_t {} dendl;

#define dendlu dendl << unlock


class DebugStream {
    bool mut;
    int owner;
    public:
    int level, required_level;
    DebugStream() : level(0), mut(1), owner(0), required_level(1) {}

    void lock() {
        int pid = GetCurrentThreadId();
        while (!mut && owner != 0 && pid != owner);
        owner = pid;
        mut = 0;
    }

    bool canPrint() {
        return level >= required_level;
    }
    template<class T>
    friend DebugStream& operator<<(DebugStream& str, T t)
    {
        str.lock();
        if (str.level >= str.required_level) {
            std::cout << t;
        }
        return str;
    }
    friend DebugStream& operator<<(DebugStream& str, reql level)
    {
        str.lock();
        str.required_level = level.l;
        return str;
    }
    friend DebugStream& operator<<(DebugStream& str, unlock_t)
    {
        str.lock();
        str.mut = 1;
        return str;
    }
    friend DebugStream& operator<<(DebugStream& str, dendl_t)
    {
        str.lock();
        if (str.level >= str.required_level) {
            std::cout << std::endl;
        }
        return str;
    }
};