#pragma once

const char * helpString = "Usage: \n \
main [data source] [[size|filename]] [options]\n\
Data source: \n \
-i\t load from file {filename} \n \
-n {size} -s {saturation}\t generate graph with specified size and saturation\n\
-S {seed}\t specify seed for the generator\n\
Options:\n\
-T\t do Tabu Search\n\
-G\t do greedy coloring\n\
-g n\t generate graph dot file if solution better than n\n\
-v\t verbose 1 - print progress\n\
-vv\t verbose 2 - print everything (matrices etc.) \n\
-o file\t store the graph in file\n\
-p\t print graph\n\
-m\t set max iterations\n\
-a\t set alpha\n\
-f\t set f(g) - max random additional time\n"; 