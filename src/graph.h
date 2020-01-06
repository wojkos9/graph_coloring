#pragma once


#include <vector>
#include <list>
#include <algorithm>
#include <forward_list>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <random>
#include <fstream>

using namespace std;

#include "debug_stream/d_cout.h"
#include "utils.h"

class Graph {
    vector<list<int>> vertices;
    vector<int> colors;
    int num_edges;
    int n;

    int colorGreedily(vector<int> &colors) {
        int max_color = 0;
        int n = vertices.size();
        for (int i = 0; i < n; i++) {
            vector<bool> available(n, true);

            for (int w : vertices[i]) {
                int c = colors[w];
                if(c >= 0) {
                    available[c] = false;
                }
            }
            
            for (int color = 0; color < available.size(); color++) {
                if (available[color]) {
                    colors[i] = color;
                    if (color > max_color)
                        max_color = color;
                    break;
                }  
            }
            
        }
        return max_color+1;
    }

    

    void printVec(vector<int> &vec, int m) {
        for (int j=0; j < m; j++)
            d_cout << vec[j] << " ";
        d_cout << dendl;
    }
    
    void printMat(vector<vector<int>> &mat, int n, int m) {
        for (int i=0; i < n; i++) {
            d_cout << i << "| ";
            for (int j=0; j < m; j++)
                d_cout << mat[i][j] << " ";
            d_cout << dendl;
        }
        d_cout << dendl;
    }
    
public:
    Graph()  {
        num_edges = 0;
    }

    const vector<int>& getColors() {
        return colors;
    }

    void saveColors(ofstream& file) {
        for(int i = 0; i < vertices.size(); i++) {
            file << i+1 << ": " << colors[i] << endl;
        }
    }

    int colorGreedily() {
        return colorGreedily(colors);
    }

    void recolor(vector<int> &colors, int k) { // reduces the number of colors, creating conflicts
        for (int &col : colors) {
            if (col == k)
                col--;
        }
    }

    int costf(vector<int> &colors) { // computes the cost function
        int conflicts = 0;
        for (int v = 0; v < vertices.size(); v++) {
            int c = colors[v];
            for (int w : vertices[v]) {
                if (colors[w] == c)
                    conflicts++;
            }
        }
        return conflicts;
    }
    int costf_mat (vector<int> &colors, vector<vector<int>> &cost_mat) { // computes cost function from the cost matrix
        int conflicts = 0;
        int n = cost_mat.size();
        for(int i = 0; i < n; i++) {
            int cs = cost_mat[i][colors[i]];
            //d_cout << "conf " << i << " " << cs << dendl;
            conflicts += cs;
        }
        return conflicts;
    }

    void computeCM(vector<vector<int>> &cost_mat, vector<int> &colors, int k) { // fills cost matrix of k colors
        for (int v = 0; v < vertices.size(); v++) {
            for (int j=0; j < k; j++)
                cost_mat[v][j] = 0;
            for (int w : vertices[v]) {
                cost_mat[v][colors[w]]++;
            }
        }
    }

    bool coloredCorrectly(int &mc, int &ne) {
        mc = 0;
        ne = 0;
        for (int v=0; v < vertices.size(); v++) {
            if (colors[v] < 0)
                return false;
            if (colors[v] > mc)
                mc = colors[v];
            for(int w : vertices[v]) {
                ne++;
                if (colors[v] == colors[w])
                    return false;
            }
        }
        return true;
    }

    int getNumVertices() {
        return vertices.size();
    }
    int getNumEdges() {
        return num_edges;
    }

    bool setColoring(vector<int> &coloring) {
        if (coloring.size() != getNumVertices())
            return false;
        colors = coloring;
        return true;
    }

    int colorTabuSearch(int max, int len, float alpha, int f, unsigned int seed) {
        return findColoring({max, len, alpha, f}, seed, colors);
    }

    int findColoring(tabu_params_t params, unsigned int seed, vector<int> &colors, int id=0) {
        typedef pair<int, int> move_t; // custom type for storing moves in the tabu list

        int max = params.m;
        int length = params.l;
        float alpha = params.alpha;
        int g = params.g;

        std::mt19937 gen;
        gen.seed(seed); // 
        std::uniform_int_distribution<int> rng;

        list<pair<move_t, int>> tabu_list;
        int n = vertices.size();
        vector<int> s(n, -1); // CURRENT COLORING
        int nc = 0; // current number of colors used

        // start from greedy coloring
        nc  = colorGreedily(s); 
        vector<int> valid_coloring(s);
        //printVec(s, n);

        // replace the highest color k with k-1
        recolor(s, nc-1); 
        nc--;
        
        /* COST MATRIX:
        matrix of size N x NC storing info about 
        each of N vertices conflicts (neighbours of the same color)
        that would result from changing its color to one of NC colors
        */
        vector<vector<int>> cost_mat(n, vector<int>(nc));

        int nb = 0; // iteration counter
        int ti = 0; // total iterations
        int start = 0;
        while (nb < max) {

            computeCM(cost_mat, s, nc); // compute cost matrix

            int f1 = costf_mat(s, cost_mat); // f1 = number of conflicts

            start = rng(gen) % n;//rand()%n;
            while (nb < max && f1) {
                int best_cr = -1; // best cost reduction
                bool has_candidate = false;
                move_t best_move;
                for (int k = 0; k < n; k++) {
                    int i = (k+start) % n;
                    int ci = s[i]; // color of vertex i
                    int cost1 = cost_mat[i][ci]; // current number of conflicts for i-th vertex
                    if (cost1) { // if vertex has conflicts
                        for (int j = 0; j < nc; j++) { // find best move from i
                            if (j != ci) { //
                                int cr = cost1 - cost_mat[i][j]; // cost reduction - potential decrease in the number of conflicts
                                move_t move(i, j);
                                // find maximal cost reduction for a move not in the tabu list
                                if (cr > best_cr || !has_candidate) {
                                    bool found = false;
                                    for(auto &it : tabu_list) {
                                        if (it.first == move) {
                                            found = true;
                                            break;
                                        }
                                    }
                                    if (!found) { // if move is not on TL
                                        best_move = move;
                                        best_cr = cr;
                                        has_candidate = true;
                                    }
                                }
                            }
                        }
                    }
                }
                auto it = tabu_list.begin();
                while (it != tabu_list.end()) {
                    if (--(it->second) <= 0) { // if time left on tabu list reached 0
                        it = tabu_list.erase(it);
                    } else {
                        it++;
                    }
                }
                if (has_candidate) {
                    int i = best_move.first;
                    int old_c = s[i];
                    int new_c = best_move.second;
                    
                    int l = f1 * alpha + rng(gen) % g; // time to stay on tabu list (number of iterations)

                    tabu_list.push_front({move_t(i, old_c), l}); // prevent from moving back to current state
                    
                    
                    for(int w : vertices[i]) { // update cost matrix
                        cost_mat[w][old_c]--;
                        cost_mat[w][new_c]++;
                    }
                    s[i] = new_c; // make the move (change color of i)

                    // cost function is reduced by twice the cost for 1 vertex, 
                    // because each conflict involves 2 vertices
                    f1 -= 2*best_cr;
                } else if (tabu_list.empty() != true) {
                    tabu_list.pop_back(); // if tabu list is blocking all the moves 
                }
                nb++;
                ti++;
            }
            if (f1 <= 0) { // if there are no conflicts left
                d_cout.lock();
                d_cout << id << " " << ti << " " << nc << dendl;
                d_cout.unlock();
                valid_coloring = vector<int>(s); // store last valid coloring
                recolor(s, nc-1); // try to reduce the number of colors
                nc--;
                nb = 0;
            }
            
        }
        colors = vector<int>(valid_coloring);
        return nc+1;
    }

    inline bool isEdge(int v, int w) {
        for(int w2 : vertices[v]) {
            if (w2 == w)
                return true;
        }
        return false;
    }
    

    time_t randomize(int n, float sat) {
        return randomize(n, sat, time(NULL));
    }

    time_t randomize(int n, float saturation, time_t seed) // creates a random graph (undirected)
    {
        vertices = vector<list<int>>(n);
        colors = vector<int>(n, -1);
        int M = saturation * n * (n-1);
        uniform_int_distribution<int> R(0, n-1);
        default_random_engine gen(seed);

        

        int v, w;
        num_edges = 0;
        for (int i=0; i < M / 2; i++) {
            do {
                v = R(gen);
                do w = R(gen) % n; while (w == v);
            } while (isEdge(v, w));
            vertices[v].push_front(w);
            vertices[w].push_front(v);
            num_edges++;
        }
        return seed;
    }

    bool saveGraph() {
        char fname2[64] = {0};
        sprintf(fname2, "out\\graph_%d_%d.gv", vertices.size(), num_edges);
        return saveGraph(fname2);
    }

    bool saveToTxt(const char* fname) {
        FILE *file = fopen(fname, "w");
        if (!file)
            return false;
        int n = vertices.size();
        fprintf(file, "%d\n", n);
        for (int i=0; i < n; i++) {
            for (int j=i+1; j < n; j++) {
                if (isEdge(i, j))
                    fprintf(file, "%d %d\n", i+1, j+1);
            }
        }
        fclose(file);
        return true;
    }
    void fromFile(const char* fname) {
        FILE *file = fopen(fname, "r");
        if (!file)
            cerr << "Error file\n";
        int n;
        num_edges = 0;
        fscanf(file, "%d", &n);
        d_cout << n;
        vertices = vector<list<int>>(n);
        colors = vector<int>(n, -1);
        int v, w;
        while (fscanf(file, "%d%d", &v, &w) != -1) {
            vertices[v-1].push_front(w-1);
            vertices[w-1].push_front(v-1);
            num_edges++;
        }
        fclose(file);
        d_cout << "Loaded" << dendl;
    }

    bool saveGraph(const char* fname) // stores colored graph in DOT format
    {   
        FILE *graphOut;
        graphOut = fopen(fname, "w");
        
        if (!graphOut) {
            return false;
        }
        fprintf(graphOut, "graph Graph%d {\n", vertices.size());

        for (int v=0; v < vertices.size(); v++) {
            for (int w : vertices[v]) {
                if (v < w)
                    fprintf(graphOut, "\"%d(%d)\" -- \"%d(%d)\";\n", v+1, colors[v], w+1, colors[w]);
            }
        }

        fprintf(graphOut, "\n}");
        fclose(graphOut);
        return true;
    }
    
};