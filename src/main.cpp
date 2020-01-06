#include <iostream>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <fstream>
#include <random>
#include <thread>
#include <mutex>


#include "graph.h"
#include "utils.h"
#include "timing.h"
#include "constants.h"
#include "debug_stream/d_cout.h"

#include "parser.h"

using namespace std;

mutex mu;
void thread_f(Graph *g, int *min_nc, vector<int> *min_colors, int *best_seed, tabu_params_t params, unsigned int seed, int id) {
        vector<int> colors;
        int nc = g->findColoring(params, seed, colors, id);
        mu.lock();
        if (*min_nc == -1 || nc < *min_nc) {
            *min_nc = nc;
            *min_colors = colors;
            *best_seed = seed;
        }
        mu.unlock();
}


Graph graph;
preset_t preset;

int main(int argc, char** argv) {
    unsigned int seed1 = time(NULL);

    preset = parse_args(argc, argv);
    

    if (preset.actions & FROM_FILE)
        graph.fromFile(preset.f_graph_in);
    else if (preset.size) {
        graph.randomize(preset.size, preset.saturation);
    } else {
        cerr << "Too few parameters.\n";
        exit(-1);
    }
    cout << "Loaded graph with " << graph.getNumVertices() << " vertices and " << graph.getNumEdges() << " edges\n";

    if (preset.actions & STORE_TXT && preset.f_graph_out) {
        if (graph.saveToTxt(preset.f_graph_out))
            cout << "Graph saved in " << preset.f_graph_out << endl;
    }

    if (preset.actions & DO_GREEDY) {
        int res_greedy;
        cout << "\nGreedy time:" << TIME_OP(res_greedy = graph.colorGreedily(), true) << "us\n";
        cout << "Colors: " << res_greedy << endl;
        if (preset.actions & GEN_GRAPH_FILE)
            graph.saveGraph("./out/greedy_col.gv");
        if (preset.actions & CHECK_COLORING) {
            int nc, ne;
            cout << "Greedy OK?: " << graph.coloredCorrectly(nc, ne);
            cout << ", color check: " << nc+1 << " 2*edges: " << ne << endl;
        }
    }
        
    if (preset.actions & DO_TABU) {
        int max = preset.tabu_params.m;
        int length = preset.tabu_params.l;
        float alpha = preset.tabu_params.alpha;
        int g = preset.tabu_params.g;
        cout << "\nTabu Search (" << max << ", " << length << ", " << alpha << ", " << g << ")" << endl;
        cout << "Threads:" << preset.n_threads << endl;
        
        std::mt19937 gen;
        gen.seed(seed1);
        std::uniform_int_distribution<int> rng;

        int min_nc = -1;
        int best_seed = -1;
        vector<int> min_colors;
        
        timerStart();
        cout << BEGIN_DEBUG_STR;
        vector<thread> threads;

        for(int i = 0; i < preset.n_threads; i++) {
            unsigned int seed_tmp = (preset.actions & USE_CUSTOM_SEED) ? preset.seed : rng(gen);
            threads.push_back(thread(thread_f, &graph, &min_nc, &min_colors, &best_seed, 
                                     preset.tabu_params, seed_tmp, i));
        }
        for (thread &t : threads) {
            t.join();
        }
        long long exec_t = timerStop();
        cout << END_DEBUG_STR;

        cout << "Time: " << ((float)exec_t / 1000000.0f) << "s" << endl;
        cout << "Seed: " << best_seed << endl;
        cout << "Colors: " << min_nc << endl;

        graph.setColoring(min_colors);
            
        if (preset.actions & CHECK_COLORING) {
            int nc, ne;
            cout << "Tabu OK?: " << graph.coloredCorrectly(nc, ne);
            cout << ", color check: " << nc+1 << " 2*edges: " << ne << endl;
        }
        if (preset.actions & GEN_GRAPH_FILE && (preset.actions & FORCE_GEN || min_nc < preset.value_to_improve)) {
            string fname = strip_fname(preset.f_graph_in);
            stringstream ss;
            ss << "./out/" << fname << "_ts_" << min_nc << ".txt";
            ofstream file(ss.str());
            graph.saveColors(file);
            file.close();
            cout << "Stored solution in " << ss.str() << endl;
        } else {
            ofstream file(".last_ts");
            graph.saveColors(file);
            file.close();
        }
    }
    return 0;
}