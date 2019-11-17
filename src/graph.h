#pragma once

#include <vector>
#include <list>
#include <algorithm>
#include <forward_list>
#include <cstdio>
#include <iostream>
#include <ctime>
#include <random>

using namespace std;

#include "debug_stream.h"

class Graph {
    vector<list<int>> vertices;
    vector<int> colors;
    int num_edges;
    int n;
    DebugStream d_cout;
    
    public:
        Graph(bool debug) : d_cout(debug) {
            num_edges = 0;
        }
        void saveToTxt(const char* fname) {
            FILE *file = fopen(fname, "w");
            int n = vertices.size();
            fprintf(file, "%d\n", n);
            for (int i=0; i < n; i++) {
                for (int j=i+1; j < n; j++) {
                    if (isEdge(i, j))
                        fprintf(file, "%d %d\n", i+1, j+1);
                }
            }
            fclose(file);
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
            d_cout << "Loaded\n";
        }
        int colorGreedily() {
            return colorGreedily(colors);
        }
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

        void recolor(vector<int> &colors, int k) {
            for (int &col : colors) {
                if (col == k)
                    col--;
            }
        }

        int costf(vector<int> &colors) {
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
        int costf_mat (vector<int> &colors, vector<vector<int>> &cost_mat) {
            int conflicts = 0;
            int n = cost_mat.size();
            for(int i = 0; i < n; i++) {
                int cs = cost_mat[i][colors[i]];
                //d_cout << "conf " << i << " " << cs << endl;
                conflicts += cs;
            }
            return conflicts;
        }

        void computeCM(vector<vector<int>> &cost_mat, vector<int> &colors, int k) {
            for (int v = 0; v < vertices.size(); v++) {
                for (int j=0; j < k; j++)
                    cost_mat[v][j] = 0;
                for (int w : vertices[v]) {
                    cost_mat[v][colors[w]]++;
                }
            }
        }
        typedef pair<int, int> move_t;

        void printVec(vector<int> &vec, int m) {
            for (int j=0; j < m; j++)
                d_cout << vec[j] << " ";
            d_cout << endl;
        }
        void printMat(vector<vector<int>> &mat, int n, int m) {
            for (int i=0; i < n; i++) {
                d_cout << i << "| ";
                for (int j=0; j < m; j++)
                    d_cout << mat[i][j] << " ";
                d_cout << endl;
            }
            d_cout << endl;
        }

        bool coloredCorrectly() {
            for (int v=0; v < vertices.size(); v++) {
                for(int w : vertices[v]) {
                    if (colors[v] == colors[w])
                        return false;
                }
            }
            return true;
        }

        int sortTabu() {

            list<move_t> tabu_list;
            int n = vertices.size();
            int nb = 0;
            vector<int> s(n, -1);
            int nc = colorGreedily(s);
            vector<int> valid_coloring(s);
            printVec(s, n);
            recolor(s, nc-1);
            printVec(s, n);
            nc--;
            int max = 1000;
            int l = 10;
            vector<vector<int>> cost_mat(n, vector<int>(nc));
            
            while (nb < max) {

                computeCM(cost_mat, s, nc); // compute cost matrix
                printMat(cost_mat, n, nc);

                int f1 = costf_mat(s, cost_mat); // f1 = number of conflicts
                d_cout << "f1: " << f1 << endl;

                while (nb < max && f1) {
                    int best_cr = 0; // best cost reduction
                    bool has_candidate = false;
                    move_t best_move;
                    for (int i = 0; i < n; i++) {
                        int ci = s[i];
                        int cost1 = cost_mat[i][ci];
                        if (cost1) { // if vertex has conflicts
                            for (int j = 0; j < nc; j++) { // find best move from i
                                if (j != ci) {
                                    int cr = cost1 - cost_mat[i][j];
                                    move_t move(i, j);
                                    if (cr > best_cr || !has_candidate) {
                                        d_cout << "p " << i << " " << ci << "->" << j << " " << cr << endl;
                                        auto it = find(tabu_list.begin(), tabu_list.end(), move);
                                        if (it == tabu_list.end()) { // if move is not on TS
                                            best_move = move;
                                            best_cr = cr;
                                            has_candidate = true;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    if (has_candidate) {
                        int i = best_move.first;
                        int old_c = s[i];
                        int new_c = best_move.second;

                        tabu_list.push_front(move_t(i, old_c));
                        
                        
                        for(int w : vertices[i]) { // update cost matrix
                            cost_mat[w][old_c]--;
                            cost_mat[w][new_c]++;
                        }
                        d_cout << best_move.first << " " << old_c << "->" << new_c << endl;
                        printMat(cost_mat, n, nc);
                        s[i] = new_c; // move
                        d_cout << "AFTER: ";
                        printVec(s, n);
                        f1 -= 2*best_cr;
                        d_cout << "f1: " << f1 << endl;
                    } else {
                        tabu_list.pop_back();
                    }
                    if (tabu_list.size() > l) {
                        tabu_list.pop_back();
                    }
                    nb++;
                }
                if (f1 <= 0) {
                    valid_coloring = vector<int>(s);
                    recolor(s, nc-1);
                    nc--;
                }
                
            }
            colors = vector<int>(valid_coloring);
            return nc+1;
        }
/*


*/

        inline bool isEdge(int v, int w) {
            for(int w2 : vertices[v]) {
                if (w2 == w)
                    return true;
            }
            return false;
        }
        bool saveGraph() {
            char fname2[64] = {0};
            sprintf(fname2, "out\\graph_%d_%d.gv", vertices.size(), num_edges);
            return saveGraph(fname2);
        }
        bool saveGraph(const char* fname) // stores graph in DOT format in "out/graph_{N}.gv"
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

        
};