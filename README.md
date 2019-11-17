# graph_coloring
Usage:  
main [data source] [[size|filename]] [options]
Data source:  
-i load from file {filename}  
-n {size} -s {saturation} generate graph with specified size and saturation
-S {seed} specify seed for the generator
Options:
-T do Tabu Search
-G do greedy coloring
-r file file to store the results in
-g generate graph dot file
-a number of measurements to take the average from
-v verbose
-o file store the graph in file
-p print graph
