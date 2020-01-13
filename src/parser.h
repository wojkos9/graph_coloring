#pragma once

#include <cstdio>
#include <cstdlib>
#include "klib/ketopt.h"
#include "constants.h"

#include "preset.h"
#include "debug_stream/d_cout.h"


preset_t parse_args(int argc, char** argv) {
    ketopt_t opt;
    char c;
    preset_t preset = presets[1];

    const char *ostr = "hp:n:M:s:i:I:l:b:o:GTS:vcm:l:a:g:j:f:z:";

    opt = KETOPT_INIT;
    while ((c = ketopt(&opt, argc, argv, 0, ostr, NULL)) != -1) {// check if should build on top of a preset
        if (c == 'p') {
            int pn = atoi(opt.arg);
            if (pn < 0 || pn >= sizeof(presets)/sizeof(preset_t))
                printf("Invalid preset index %d\n", pn);
            else {
                preset = presets[pn];
            }
            break;
        }
    }


    opt = KETOPT_INIT;
    while ((c = ketopt(&opt, argc, argv, 0, ostr, NULL)) != -1) {
        switch(c) {    
            case 'h':
                printf("%s", helpString);
                exit(0);
            case 'n':
                preset.size = atoi(opt.arg);
                break;
            case 'M':
                preset.n_edges = atoi(opt.arg);
                break;
            case 's':
                preset.saturation = atof(opt.arg);
                break;
            case 'I':
                preset.double_entries = true;
            case 'i':
                preset.f_graph_in = opt.arg;
                preset.actions |= FROM_FILE;
                break;
            case 'v':
                d_cout.level++;
                break;
            case 'b':
                preset.actions |= GEN_GRAPH_FILE;
                if (opt.arg[0] == '-')
                    preset.actions |= FORCE_GEN;
                else
                    preset.value_to_improve = atoi(opt.arg);
                break;
            case 'o':
                preset.actions |= STORE_TXT;
                preset.actions &= ~(DO_TABU | DO_GREEDY);
                preset.f_graph_out = opt.arg;
                break;
            case 'G':
                preset.actions |= DO_GREEDY;
                break;
            case 'T':
                preset.actions |= DO_TABU;
                break;
            case 'S':
                preset.seed = atoi(opt.arg);
                preset.actions |= USE_CUSTOM_SEED;
                break;
            case 'c':
                preset.actions |= CHECK_COLORING;
                break;
            case 'm':
                preset.tabu_params.m = atoi(opt.arg);
                break;
            case 'l':
                preset.tabu_params.l = atoi(opt.arg);
                break;
            case 'a':
                preset.tabu_params.alpha = atof(opt.arg);
                break;
            case 'g':
                preset.tabu_params.g = atoi(opt.arg);
                break;
            case 'j':
                preset.n_threads = atoi(opt.arg);
                break;
            case 'f':
                preset.out_fmt = opt.arg;
                d_cout.level = -1;
                break;
            case 'z':
                preset.seed1 = atoi(optarg);
                break;
        }
    }
    return preset;
}
