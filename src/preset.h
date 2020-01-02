#pragma once


#include "constants.h"
#include "utils.h"

typedef struct {
    unsigned actions;
    int n_threads;
    int size;
    float saturation;
    const char* f_graph_in;
    const char* f_graph_out;
    long seed;
    tabu_params_t tabu_params;
    int value_to_improve;
} preset_t;


preset_t pr_def = {.actions = DO_TABU | DO_GREEDY | CHECK_COLORING, .n_threads = 12, .tabu_params = {40000, 10, 1.0f, 50}, .value_to_improve = -1};
preset_t pr_light = {.actions = DO_TABU | DO_GREEDY, .n_threads = 1, .tabu_params = {10000, 10, 1.0f, 50}, .value_to_improve = -1};
preset_t pr_gen = {.actions = STORE_TXT, .size = 100, .saturation = 0.6, .f_graph_out = "instances/graph.col"};
preset_t presets[] = {pr_def, pr_light, pr_gen};