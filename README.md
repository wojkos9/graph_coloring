# Tabu Search graph coloring

Usage:\
main [data source] [[size|filename]] [options]\
Data source:\
-i load from file {filename}\
-n {size} -s {saturation} generate graph with specified size and saturation\
-S {seed} specify seed for the generator\
Options:\
-T do Tabu Search\
-G do greedy coloring\
-g n generate graph dot file if solution better than n\
-v verbose 1 - print progress\
-vv verbose 2 - print everything (matrices etc.) \
-o file store the graph in file\
-p print graph\
-m set max iterations\
-a set alpha\
-f set f(g) - max random additional time