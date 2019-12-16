#pragma once

#include <iostream>
#include <mutex>


struct reql {int l; reql(int l): l(l) {}};
static struct unlock_t {} unlock;
static struct dendl_t {} dendl;



class DebugStream {
    std::mutex mut;
    int owner;
    public:
    int level, required_level;
    DebugStream() : level(0), owner(0), required_level(1) {}

    void lock() {
        mut.lock();
    }
    void unlock() {
        mut.unlock();
    }

    bool canPrint() {
        return level >= required_level;
    }
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
    friend DebugStream& operator<<(DebugStream& str, unlock_t)
    {
        str.unlock();
        return str;
    }
    friend DebugStream& operator<<(DebugStream& str, dendl_t)
    {
        if (str.level >= str.required_level) {
            std::cout << std::endl;
        }
        return str;
    }
};