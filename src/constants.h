#pragma once


#define DO_TABU 1
#define DO_GREEDY 2
#define CHECK_COLORING 4
#define GEN_GRAPH_FILE 8
#define FORCE_GEN 16
#define STORE_TXT 32
#define USE_CUSTOM_SEED 64
#define FROM_FILE 128

const char * helpString = "Usage: \n \
main [data source] [[size|filename]] [options]\n\
Data source: \n \
-i\t load from file {filename} \n \
-n {size} -s {saturation}\t generate graph with specified size and saturation\n\
-S {seed}\t specify seed for the generator\n\
Options:\n\
-T\t do Tabu Search\n\
-G\t do greedy coloring\n\
-c\t check each generated coloring\n\
-b n\t generate graph dot file if solution better than n\n\
-v\t verbose 1 - print progress\n\
-vv\t (not implemented) verbose 2 - print everything (matrices etc.) \n\
-o file\t store the graph in file\n\
-m\t set max iterations\n\
-a\t set alpha\n\
-g\t set g - max random additional time\n\
-j\t specify the number of parallel solvers\n\
-p\t use a preset with the given index, defined in preset.h\n"; 
