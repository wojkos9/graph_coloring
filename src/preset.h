#pragma once


#include "constants.h"
#include "utils.h"

typedef struct {
    unsigned actions;
    bool double_entries;
    int n_threads;
    int size;
    float saturation;
    int n_edges;
    const char* f_graph_in;
    const char* f_graph_out;
    long seed;
    tabu_params_t tabu_params;
    int value_to_improve;
    const char* out_fmt;
    long seed1;
} preset_t;


preset_t pr_def = {DO_TABU | DO_GREEDY | CHECK_COLORING, false, 12, 0, 0.0f, 0, 0, 0, 0, {40000, 10, 1.0f, 50}, -1, 0, 0};
preset_t pr_light = {DO_TABU | DO_GREEDY | CHECK_COLORING, false, 1, 0, 0.0f, 0, 0, 0, 0, {10000, 10, 1.0f, 50}, -1, 0, 0};
preset_t pr_gen = {STORE_TXT, false, 0, 100, 0.5f, 0, 0, "instances/graph.col", 0, {}, -1, 0, 0};
preset_t presets[] = {{}, pr_def, pr_light, pr_gen};