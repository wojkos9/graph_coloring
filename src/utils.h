#pragma once

#include <cstdio>
#include <random>
#include <ctime>
#include <functional>

#ifdef __linux__
#include <unistd.h>
#define Sleep(t) usleep(t * 1000)
#else
#include <windows.h>
#endif

#define SWAP(a, b, t) ((t)=(a),(a)=(b),(b)=(t))

#define MAX(a,b) (((a) > (b)) ? (a) : (b))

static bool _assert_before, _assert_after;

void timerStart();
long long timerStop();

struct TimerData {
    int time;
    bool &flag;
};

bool TIMER_FINISHED = false;


int terminatorFunc(void* data) {
    TimerData tdata = *(TimerData*)data;
    Sleep(tdata.time);
    tdata.flag = !tdata.flag;
    TIMER_FINISHED = true;
    return 0;
}

#ifdef __linux__
#include <pthread.h>

timespec _timer_start, _timer_end;

void timerStart() {
    clock_gettime(CLOCK_REALTIME, &_timer_start);
}
long long timerStop() {
    clock_gettime(CLOCK_REALTIME, &_timer_end);
    return (_timer_end.tv_sec - _timer_start.tv_sec) * 1000000 + (_timer_end.tv_nsec - _timer_start.tv_nsec) / 1000;
}

void *terminatorThread(void *data) {
	terminatorFunc(data);
}

pthread_t _timer_thread;

#define CREATE_THREAD(func, args) \
(pthread_create(&_timer_thread, NULL, func, args), _timer_thread)

#define TERMINATE_THREAD(thread) pthread_cancel(thread)

#else
#include <windows.h>
static LARGE_INTEGER _timer_start, _timer_end, _timer_frequency;

DWORD WINAPI terminatorThread(void* data) {
    return terminatorFunc(data);
}
void timerStart() {
    QueryPerformanceFrequency(&_timer_frequency);
    QueryPerformanceCounter(&_timer_start);
}
long long timerStop() {
    QueryPerformanceCounter(&_timer_end);
    return (long long)((_timer_end.QuadPart - _timer_start.QuadPart) * 1000000) / _timer_frequency.QuadPart;
}

#define CREATE_THREAD(func, args) CreateThread(NULL, 0, func, args, 0, NULL)
#define TERMINATE_THREAD(thread) TerminateThread(thread, 0)

#endif

long long timeOp(std::function<void(void)> func, std::function<bool(void)> assert) {
    _assert_before = assert();
    timerStart();
    func();
    long long et = timerStop();
    _assert_after = assert();
    return et;
}

long long timeOpLimit(std::function<void(void)> func, std::function<bool(void)> assert, int time, bool &flag) {
    TIMER_FINISHED = false;
    TimerData tdata = {time, flag};
    auto hTimer = CREATE_THREAD(terminatorThread, &tdata);
    long long et = timeOp(func, assert);
    if (TIMER_FINISHED) {
        return time * 1000;
    } else {
        printf("terminated code ");
        printf("%d\n", TERMINATE_THREAD(hTimer));
        TIMER_FINISHED = true;
        return et;
    }
}

#define TIME_OP(func, assert) timeOp([&](){func;}, [&]()->bool{return assert;})
#define TIME_OP_LIMIT(func, assert, time, flag) timeOpLimit([&](){func;}, [&]()->bool{return assert;}, time, flag)

#define LAST_OP_SUCCESSFUL !_assert_before & _assert_after

#define NO_ASSERT false

struct mparams {
    Graph g;
    int *min_nc;
    vector<int> *min_colors;
    int *best_seed;
    int mx;
    int length;
    float alpha;
    int f;
    unsigned int seed;
    int id;
};

bool mut = 1;
DWORD WINAPI thread_func(void *params) {
    mparams *ps = (mparams*)params;
    vector<int> colors;
    int nc = ps->g.findColoring(ps->mx, ps->length, ps->alpha, ps->f, ps->seed, colors, ps->id);
    while (!mut);
    mut = 0;
    if (*ps->min_nc == -1 || nc < *ps->min_nc) {
        *ps->min_nc = nc;
        *ps->min_colors = colors;
        *ps->best_seed = ps->seed;
    }
    mut = 1;
}

#include <mutex>
using namespace std;
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

std::default_random_engine gen(time(NULL));

struct List {
    int v;
    List *next;
    void del() {
        if (next)
            next->del();
    }
};

template <class T>
void deleteList(T *list) {
    T* tmp;
    while (list) {
        tmp = list->next;
        delete list;
        list = tmp;
    }
}

void shuffle(int *keys, int n) {
    std::uniform_int_distribution<int> R(0, n-1);
    int ri, rj, t;
    for (int i=0; i < n; i++) {
        ri = R(gen);
        rj = R(gen);
        SWAP(keys[ri], keys[rj], t);
    }
}

int * randomPermutation(int n) {
    int *keys = new int[n];
    for (int i = 0; i < n; i++) {
        keys[i] = i;
    }
    shuffle(keys, n);
    return keys;
}