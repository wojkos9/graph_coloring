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