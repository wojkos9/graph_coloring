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