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
-r file\t file to store the results in\n\
-g\t generate graph dot file\n\
-a\t number of measurements to take the average from\n\
-v\t verbose\n\
-o file\t store the graph in file\n\
-p\t print graph\n"; 