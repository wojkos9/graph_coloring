#include <iostream>
#include <cstdlib>
#include <sstream>
#include <getopt.h>

#include "graph.h"
#include "utils.h"
#include "constants.h"

static int VERBOSE = 0;
static bool GEN_GRAPH_FILE = false;
static bool STORE_TXT = false;

int main(int argc, char** argv) {

    int size = 0;
    float saturation = 0.6f;
    const char *resFname = NULL, *graphInFname = NULL, *graphOutFname = NULL;

    bool doGreedy = false, doTabu = false;
    int seed = -1;

    bool CHECK_COLORING = false;

    char opt;
    while ((opt = getopt(argc, argv, "hn:s:r:i:l:12go:GTS:vc")) != -1) {
        switch (opt) {
			case 'h':
				printf("%s", helpString);
				return 0;
            case 'n':
                size = atoi(optarg);
                break;
            case 's':
                saturation = atof(optarg);
                break;
            case 'r':
                resFname = optarg;
                break;
            case 'i':
                graphInFname = optarg;
                break;
            case 'v':
                VERBOSE = 1;
                break;
            case 'g':
                GEN_GRAPH_FILE = true;
                break;
            case 'o':
                STORE_TXT = true;
                graphOutFname = optarg;
                break;
            case 'G':
                doGreedy = true;
                break;
            case 'T':
                doTabu = true;
                break;
            case 'S':
                seed = atoi(optarg);
                break;
            case 'c':
                CHECK_COLORING = true;
                break;
        }
    }

    Graph g (VERBOSE);

    if (graphInFname)
        g.fromFile(graphInFname);
    else if (size) {
        if (seed == -1)
            seed = g.randomize(size, saturation);
        else
            g.randomize(size, saturation, seed);
    } else {
        cerr << "Too few parameters.\n";
        exit(-1);
    }
    if (STORE_TXT && graphOutFname) {
        g.saveToTxt(graphOutFname);
    }

    int res_greedy = -1, res_tabu = -1;
    if (doGreedy) {
        res_greedy = g.colorGreedily();
        if (GEN_GRAPH_FILE)
            g.saveGraph("./out/greedy_col.gv");
        if (CHECK_COLORING)
            cout << "Greedy OK?: " << g.coloredCorrectly() << endl;
    }
        
    if (doTabu) {
        res_tabu = g.sortTabu();
        if (GEN_GRAPH_FILE)
            g.saveGraph("./out/tabu_col.gv");
        if (CHECK_COLORING)
            cout << "Tabu OK?: " << g.coloredCorrectly() << endl;
    }
    if (seed != -1)
        cout << "Seed: " << seed;
    cout << endl << "Greedy:\t" << res_greedy << endl << "TS:\t" << res_tabu << endl;
    return 0;
}
