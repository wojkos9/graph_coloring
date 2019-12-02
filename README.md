# Tabu Search graph coloring

Usage:\
main [data source] [[size|filename]] [options]\
Data source:\
-i\t load from file {filename}\
-n {size} -s {saturation}\t generate graph with specified size and saturation\
-S {seed}\t specify seed for the generator\
Options:\
-T\t do Tabu Search\
-G\t do greedy coloring\
-g n\t generate graph dot file if solution better than n\
-v\t verbose 1 - print progress\
-vv\t verbose 2 - print everything (matrices etc.) \
-o file\t store the graph in file\
-p\t print graph\
-m\t set max iterations\
-a\t set alpha\
-f\t set f(g) - max random additional time