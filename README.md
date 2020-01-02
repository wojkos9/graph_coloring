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
-c check each generated coloring\
-b n generate graph dot file if solution better than n\
-v verbose 1 - print progress\
-vv (not implemented) verbose 2 - print everything (matrices etc.) \
-o file store the graph in file\
-m set max iterations\
-a set alpha\
-g set g - max random additional time\
-j specify the number of parallel solvers\
-p use a preset with the given index, defined in preset.h