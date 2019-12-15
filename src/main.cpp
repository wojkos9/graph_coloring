#include <iostream>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <fstream>
#include <random>
#include <algorithm> // for max/2
#include "klib/ketopt.h"

#include "graph.h"
#include "utils.h"
#include "constants.h"
#include "debug_stream/d_cout.h"
#include "globals.h"

using namespace std;

int VERBOSE_LEVEL = 0;
static bool GEN_GRAPH_FILE = false;
static bool STORE_TXT = false;

int main(int argc, char** argv) {
    unsigned int seed1 = time(NULL);
    
    int size = 0;
    float saturation = 0.6f;
    const char *resFname = NULL, *graphInFname = NULL, *graphOutFname = NULL;

    bool doGreedy = false, doTabu = false;
    int seed = -1;
    bool use_seed = false;

    bool CHECK_COLORING = false;
    int max=1000, length=10, f=50;
    float alpha = 1.0f;
    int if_better = -1;

    int n_threads = 1;

    ketopt_t opt = KETOPT_INIT;
    char c;
    while ((c = ketopt(&opt, argc, argv, 1, "hn:s:r:i:l:12g:o:GTS:vcm:l:a:f:x:z:j:", 0)) != -1) {
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
                if_better = atoi(opt.arg);
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
                use_seed = true;
                break;
            case 'z':
                seed1 = atoi(opt.arg);
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
            case 'a':
                alpha = atof(opt.arg);
                break;
            case 'f':
                f = atoi(opt.arg);
                break;
            case 'j':
                n_threads = atoi(opt.arg);
                break;
        }
    }

    srand(seed1);

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

    int res_greedy = -1;
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
    int t_wait = 180'000;
        
    if (doTabu) {
        cout << "\nTabu Search (" << max << ", " << length << ", " << alpha << ", " << f << ")" << endl;
        
        std::mt19937 gen;
        gen.seed(seed1);
        std::uniform_int_distribution<int> rng;

        HANDLE *threads = new HANDLE[n_threads];

        int min_nc = -1;
        vector<int> min_colors;
        int best_seed = -1;
        mparams* targsv = new mparams[n_threads];
        //{g, min_nc, min_colors, max, length, alpha, f}
        timerStart();
        cout << BEGIN_DEBUG_STR;
        for(int i = 0; i < n_threads; i++) {
            mparams &ta = targsv[i];
            ta.g = g; ta.min_nc = &min_nc; ta.min_colors = &min_colors; ta.best_seed = &best_seed;
            ta.mx = max; ta.length = length; ta.alpha = alpha; ta.f = f;
            ta.seed = use_seed ? seed : rng(gen);
            ta.id = i;
            threads[i] = CREATE_THREAD(thread_func, &targsv[i]);
        }

        WaitForMultipleObjects(n_threads, threads, true, t_wait);
        long long exec_t = timerStop();
        cout << END_DEBUG_STR;

        cout << "Time: " << ((float)exec_t / 1000000.0f) << "s" << endl;
        cout << "Seed: " << best_seed << endl;
        cout << "Colors: " << min_nc << endl;

        g.setColoring(min_colors);

        if (GEN_GRAPH_FILE && (if_better == -1 || min_nc < if_better)) {
            stringstream ss;
            string fname(graphInFname);
            int forw = fname.find_last_of('/');
            int back = fname.find_last_of('\\');
            int index = MAX(forw, back) +1;
            fname = fname.substr(index, fname.find_last_of('.')-index);

            ss = stringstream();
            ss << "./out/" << fname << "_ts_" << min_nc << ".txt";
            ofstream file(ss.str());
            g.saveColors(file);
            file.close();
            cout << "Stored solution in " << ss.str() << endl;
        }
            
        if (CHECK_COLORING) {
            cout << "Tabu OK?: " << g.coloredCorrectly(mcts);
            cout << ", color check: " << mcts+1 << endl;
        }
    }
    return 0;
}