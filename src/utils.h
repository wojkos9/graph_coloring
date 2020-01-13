#pragma once

#include <cstdio>
#include <random>
#include <ctime>
#include <functional>


using namespace std;

#define SWAP(a, b, t) ((t)=(a),(a)=(b),(b)=(t))
#define MAX(a,b) (((a) > (b)) ? (a) : (b))


typedef struct {
int m;
int l;
float alpha;
int g;
} tabu_params_t;


string strip_fname(string fname) {
    stringstream ss;
    int forw = fname.find_last_of('/');
    int back = fname.find_last_of('\\');
    int index = MAX(forw, back) +1;
    return fname.substr(index, fname.find_last_of('.')-index);
}

unsigned long get_now_ns() {
    auto now = std::chrono::system_clock::now();
    auto now_ns = std::chrono::time_point_cast<std::chrono::nanoseconds>(now);
    auto value = now_ns.time_since_epoch();
    return value.count();
}