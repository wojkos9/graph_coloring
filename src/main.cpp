#include <iostream>
#include <cstdlib>
#include <sstream>
#include "klib/ketopt.h"

#include "graph.h"
#include "utils.h"
#include "constants.h"
#include "debug_stream/d_cout.h"
#include "globals.h"

int VERBOSE_LEVEL = 0;
static bool GEN_GRAPH_FILE = false;
static bool STORE_TXT = false;

int main(int argc, char** argv) {
    int size = 0;
    float saturation = 0.6f;
    const char *resFname = NULL, *graphInFname = NULL, *graphOutFname = NULL;

    bool doGreedy = false, doTabu = false;
    int seed = -1;

    bool CHECK_COLORING = false;
    int max=0, length=0;

    ketopt_t opt = KETOPT_INIT;
    char c;
    while ((c = ketopt(&opt, argc, argv, 1, "hn:s:r:i:l:12go:GTS:vcm:l:", 0)) != -1) {
        switch (c) {
			case 'h':
				printf("%s", helpString);
				return 0;
            case 'n':
                size = atoi(opt.arg);
                break;
            case 's':
                saturation = atof(opt.arg);
                break;
            case 'r':
                resFname = opt.arg;
                break;
            case 'i':
                graphInFname = opt.arg;
                break;
            case 'v':
                d_cout.level++;
                break;
            case 'g':
                GEN_GRAPH_FILE = true;
                break;
            case 'o':
                STORE_TXT = true;
                graphOutFname = opt.arg;
                break;
            case 'G':
                doGreedy = true;
                break;
            case 'T':
                doTabu = true;
                break;
            case 'S':
                seed = atoi(opt.arg);
                break;
            case 'c':
                CHECK_COLORING = true;
                break;
            case 'm':
                max = atoi(opt.arg);
                break;
            case 'l':
                length = atoi(opt.arg);
                break;
        }
    }

    Graph g;

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
    cout << "Loaded graph with " << g.getNumVertices() << " vertices and " << g.getNumEdges() << " edges\n";

    if (STORE_TXT && graphOutFname) {
        if (g.saveToTxt(graphOutFname))
            cout << "Graph saved in " << graphOutFname << endl;
    }

    int res_greedy = -1, res_tabu = -1;
    int mcg = -1, mcts = -1;
    if (doGreedy) {
        cout << "\nGreedy time:" << TIME_OP(res_greedy = g.colorGreedily(), true) << "us\n";
        cout << "Colors: " << res_greedy << endl;
        if (GEN_GRAPH_FILE)
            g.saveGraph("./out/greedy_col.gv");
        if (CHECK_COLORING) {
            cout << "Greedy OK?: " << g.coloredCorrectly(mcg);
            cout << ", color check: " << mcg+1 << endl;
        }
    }
        
    if (doTabu) {
        int m = max ? max : 1000;
        int l = length ? length : 10;
        cout << "\nTabu Search (" << m << ", " << l << ") time: " << TIME_OP(res_tabu = g.colorTabuSearch(m, l), true) << "us\n";
        cout << "Colors: " << res_tabu << endl;
        if (GEN_GRAPH_FILE)
            g.saveGraph("./out/tabu_col.gv");
        if (CHECK_COLORING) {
            cout << "Tabu OK?: " << g.coloredCorrectly(mcts);
            cout << ", color check: " << mcts+1 << endl;
        }
    }
    if (seed != -1)
        cout << "Seed was: " << seed;
    //cout << endl << "Greedy:\t" << res_greedy << endl << "TS:\t" << res_tabu << endl;
    return 0;
}