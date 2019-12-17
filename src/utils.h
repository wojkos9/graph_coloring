#pragma once

#include <cstdio>
#include <random>
#include <ctime>
#include <functional>

#include <mutex>
using namespace std;

#define SWAP(a, b, t) ((t)=(a),(a)=(b),(b)=(t))

#define MAX(a,b) (((a) > (b)) ? (a) : (b))

static bool _assert_before, _assert_after;

void timerStart();
long long timerStop();

#ifdef __linux__
#include <pthread.h>
#include <unistd.h>
#define Sleep(t) usleep(t * 1000)

timespec _timer_start, _timer_end;

void timerStart() {
    clock_gettime(CLOCK_REALTIME, &_timer_start);
}
long long timerStop() {
    clock_gettime(CLOCK_REALTIME, &_timer_end);
    return (_timer_end.tv_sec - _timer_start.tv_sec) * 1000000 + (_timer_end.tv_nsec - _timer_start.tv_nsec) / 1000;
}

#else
#include <windows.h>
static LARGE_INTEGER _timer_start, _timer_end, _timer_frequency;

void timerStart() {
    QueryPerformanceFrequency(&_timer_frequency);
    QueryPerformanceCounter(&_timer_start);
}
long long timerStop() {
    QueryPerformanceCounter(&_timer_end);
    return (long long)((_timer_end.QuadPart - _timer_start.QuadPart) * 1000000) / _timer_frequency.QuadPart;
}
#endif

long long timeOp(std::function<void(void)> func, std::function<bool(void)> assert) {
    _assert_before = assert();
    timerStart();
    func();
    long long et = timerStop();
    _assert_after = assert();
    return et;
}

#define TIME_OP(func, assert) timeOp([&](){func;}, [&]()->bool{return assert;})

#define LAST_OP_SUCCESSFUL !_assert_before & _assert_after

#define NO_ASSERT false


mutex mu;
void thread_f(Graph *g,
    int *min_nc,
    vector<int> *min_colors,
    int *best_seed,
    int mx,
    int length,
    float alpha,
    int f,
    unsigned int seed,
    int id) {
        vector<int> colors;
        int nc = g->findColoring(mx, length, alpha, f, seed, colors, id);
        mu.lock();
        if (*min_nc == -1 || nc < *min_nc) {
            *min_nc = nc;
            *min_colors = colors;
            *best_seed = seed;
        }
        mu.unlock();

}

string strip_fname(string fname) {
    stringstream ss;
    int forw = fname.find_last_of('/');
    int back = fname.find_last_of('\\');
    int index = MAX(forw, back) +1;
    return fname.substr(index, fname.find_last_of('.')-index);
}