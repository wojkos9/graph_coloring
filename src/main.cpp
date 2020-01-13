#include <iostream>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <fstream>
#include <random>
#include <thread>
#include <mutex>
#include <chrono>


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
    unsigned long seed1 = get_now_ns();

    preset = parse_args(argc, argv);
    if (preset.seed1)
        seed1 = preset.seed1;

    if (preset.actions & FROM_FILE)
        graph.fromFile(preset.f_graph_in, !preset.double_entries);
    else if (preset.size) {
        if (preset.n_edges > 0)
            graph.randomize_n_m(preset.size, preset.n_edges, seed1);
        else if (0.0f <= preset.saturation && preset.saturation <= 1.0f)
            graph.randomize_n_sat(preset.size, preset.saturation, seed1);
        else {
            cerr << "Invalid parameters.\n";
            exit(-1);
        }
    } else {
        cerr << "Too few parameters.\n";
        exit(-1);
    }
    ccout(0) << "Loaded graph with " << graph.getNumVertices() << " vertices and " << graph.getNumEdges() << " edges\n";

    if (preset.actions & STORE_TXT && preset.f_graph_out) {
        if (graph.saveToTxt(preset.f_graph_out))
            ccout(0) << "Graph saved in " << preset.f_graph_out << endl;
    }

    int res_greedy;
    int res_tabu;

    if (preset.actions & DO_GREEDY) {
        auto exec_time = TIME_OP(res_greedy = graph.colorGreedily(), true);
        ccout(0) << "\nGreedy time:" << exec_time << "us\n";
        ccout(0) << "Colors: " << res_greedy << endl;
        if (preset.actions & GEN_GRAPH_FILE)
            graph.saveGraph("./out/greedy_col.gv");
        if (preset.actions & CHECK_COLORING) {
            int nc, ne;
            bool cc = graph.coloredCorrectly(nc, ne);
            if (!cc) {
                cerr << "WARNING: Graph incorrectly colored\n";
            }
            ccout(0) << "Greedy OK?: " << cc;
            ccout(0) << ", color check: " << nc+1 << " 2*edges: " << ne << endl;
        }
    }
        
    if (preset.actions & DO_TABU) {
        int max = preset.tabu_params.m;
        int length = preset.tabu_params.l;
        float alpha = preset.tabu_params.alpha;
        int g = preset.tabu_params.g;
        ccout(0) << "\nTabu Search (" << max << ", " << length << ", " << alpha << ", " << g << ")" << endl;
        ccout(0) << "Threads:" << preset.n_threads << endl;
        
        std::mt19937 gen;
        gen.seed(seed1);
        std::uniform_int_distribution<int> rng;

        int min_nc = -1;
        int best_seed = -1;
        vector<int> min_colors;
        
        timerStart();
        ccout(0) << BEGIN_DEBUG_STR;
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
        ccout(0) << END_DEBUG_STR;

        res_tabu = min_nc;

        ccout(0) << "Time: " << ((float)exec_t / 1000000.0f) << "s" << endl;
        ccout(0) << "Seed: " << best_seed << endl;
        ccout(0) << "Colors: " << min_nc << endl;

        graph.setColoring(min_colors);
            
        if (preset.actions & CHECK_COLORING) {
            int nc, ne;
            bool cc = graph.coloredCorrectly(nc, ne);
            if (!cc) {
                cerr << "WARNING: Graph incorrectly colored t\n";
            }
            ccout(0) << "Tabu OK?: " << cc;
            ccout(0) << ", color check: " << nc+1 << " 2*edges: " << ne << endl;
        }
        if (preset.actions & GEN_GRAPH_FILE && (preset.actions & FORCE_GEN || min_nc < preset.value_to_improve)) {
            string fname = strip_fname(preset.f_graph_in);
            stringstream ss;
            ss << "./out/" << fname << "_ts_" << min_nc << ".txt";
            ofstream file(ss.str());
            graph.saveColors(file);
            file.close();
            ccout(0) << "Stored solution in " << ss.str() << endl;
        } else {
            ofstream file(".last_ts");
            graph.saveColors(file);
            file.close();
        }

        if (preset.out_fmt) {
            printf(preset.out_fmt, res_tabu, res_greedy);
        }

    }
    return 0;
}