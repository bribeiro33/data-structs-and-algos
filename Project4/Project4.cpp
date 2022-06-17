//  3E33912F8BAA7542FC4A1585D2DB6FE0312725B9
//  Project4.cpp
//  Project4
//
//  Created by Barbara Ribeiro on 12/4/21.
//
//template <typename T>
//void genPerms(vector<T> &path, uint32_t permLength) {
//  if (permLength == path.size()) {
//    // Do something with the path
//    return;
//  } // if
//  if (!promising(path, permLength))
//    return;
//  for (uint32_t i = permLength; i < path.size(); ++i) {
//    swap(path[permLength], path[i]);
//    genPerms(path, permLength + 1);
//    swap(path[permLength], path[i]);
//  }
//}
//
//MST
//Fast TSP
//Optimal TSP


#include <iostream>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <string.h>
#include <string>
#include <iostream>
#include <vector>
#include <cmath>
#include <getopt.h>
#include <limits>
#include <float.h>
#include "xcode_redirect.hpp"
using namespace std;

class Zookeeper {
    private:
        //coords for the cages aka cage locations
        class Coord {
            public:
                double x;
                double y;
            //I don't think I need these
                Coord() : x(0), y(0){}
                Coord(double xt, double yt) : x(xt), y(yt){}
        };
        
        //to simplify prt 3, keep all three vectors together
        class UncheckedCages {
            public:
                double unchecked_dist = DBL_MAX;
                uint32_t unchecked_index = 0;
                bool unchecked_status = false;
        };
        
        //All
        vector<Coord> cages;
        
        //A and C
        vector<uint32_t> checked;
    
        //A
        vector<uint32_t> parent;
        
        //B and C
        vector<uint32_t> path;
    
        //C
        vector<uint32_t> opt_path;
    
        //C
        double weight = 0;
        double temp_dist = 0;
        double final_dist = 0;
        
        uint32_t MODE = 0; //0: MST, 1: OPTTSP, 2: FASTTS
        uint32_t num = 0;
        
        //A count, error checking
        uint32_t wild_count = 0;
        uint32_t tame_count = 0;
        uint32_t wall_count = 0;
        
    
    
    
    public:

        //functions
        void mode(int argc, char * argv[]);
        void input();
        void findPath();
        void print();
        
        void help();
        void mst_create();
        void fastts_create();
        void opttsp_create();
        
        void mst_print();
        void fasttsp_print();
        void opttsp_print();
    
        //All
        double calcDist(Coord &cage1, Coord &cage2);
    
        //Prt A
        bool routePossible(Coord &cage1, Coord &cage2);
        void counts(Coord curr);
        
        //Prt C
        void fasttsp_c(vector<vector<double>> &temp_matrix);
        void genPerms(vector<vector<double>> &temp_matrix, uint32_t permLength);
        bool promising(vector<vector<double>> &temp_matrix, uint32_t permLength);
        
        //Prt C helpers
        uint32_t findCage();
        double mst_c(vector<vector<double>> &temp_matrix, uint32_t initial_index);
        
        uint32_t nearest(const vector<UncheckedCages> &uncheckedCages);

};
void Zookeeper::help(){
    cout << "Zookeeper care program.\n"
         << "Make paths to each animal cage to deliver food efficiently or construct canals to provide fresh water.\n"
         << "--mode or -m followed by MST, FASTTSP, or OPTTSP.\n";
}
void Zookeeper::mode(int argc, char * argv[]){
    opterr = false;
    int opt;
    int optionCount = 0;
    option long_options[] = {
        { "help", no_argument, nullptr, 'h'},
        { "mode", required_argument, nullptr, 'm'},
    };

    while ((opt = getopt_long(argc, argv, "hm:", long_options, &optionCount)) != -1) {
        switch (opt) {
            case 'h':
                help();
                break;

            case 'm':
                //optarg = str_mode
                if (strcmp(optarg, "MST") == 0) {
                    MODE = 0;
                    break;
                }
                else if(strcmp(optarg, "FASTTSP") == 0) {
                    MODE = 1;
                    break;
                }
                else if(strcmp(optarg, "OPTTSP") == 0) {
                    MODE = 2;
                    break;
                }
                
                else {
                    cerr << "Error: invalid mode" << endl;
                    exit(1);
                }

            default:
                cerr << "Error: invalid flag" << endl;
                exit(1);
        }
    }
}

double Zookeeper::calcDist(Coord &cage1, Coord &cage2){
    //Euclidean dist
    double dist1 = cage1.x - cage2.x;
    double dist2 = cage1.y - cage2.y;
    return sqrt((dist1 * dist1) + (dist2 * dist2));
}

//PRT A only
bool Zookeeper::routePossible(Coord &cage1, Coord &cage2){
    //default type to 2 = safe
    uint32_t type = 2; //0 = wild, 1 = wall, 2 = safe
    
    //cage 2 is wild
    if (cage2.x < 0 && cage2.y < 0){
        type = 0;
    }
    
    //cage 2 is a wall
    else if ((cage2.x == 0 && cage2.y <= 0) || (cage2.x < 0 && cage2.y == 0)){
        type = 1;
    }
    
    // wall can connect to everything, border of 3rd quadrant
    if ((cage1.x == 0 && cage1.y <= 0) || (cage1.x < 0 && cage1.y == 0)){
        return true;
    }
    
    // wild can connect to wild0 and wall1, 3rd quad minus border
    else if (cage1.x < 0 && cage1.y < 0){
        return (type == 0 || type == 1);
    }
    
    // safe can connect to safe2 and wall1
    else {
        return (type == 2 || type == 1);
    }
}

uint32_t Zookeeper::findCage(){
    uint32_t cage_num = (uint32_t)checked.size();
    uint32_t new_cage = (uint32_t)((unsigned int) rand()) % cage_num;
    while (checked[new_cage] == 1){
        new_cage = (uint32_t)((unsigned int) rand()) % cage_num;
    }
    return new_cage;
}


void Zookeeper::input(){
    cin >> num;
    double tempX;
    double tempY;
    Coord new_coord;
    while (cin >> tempX >> tempY){
        new_coord.x = tempX;
        new_coord.y = tempY;
        cages.push_back(new_coord);
    }
}

void Zookeeper::counts(Coord curr){
    if (curr.x < 0 && curr.y < 0){
        ++wild_count;
    }
    else if (curr.x == 0 || curr.y == 0){
        ++wall_count;
    }
    else {
        ++tame_count;
    }
}

//modifies path, parent, checked, and type counts
//print uses parent
void Zookeeper::mst_create(){
    vector<double> dist;
    checked.assign(num, 0);
    dist.assign(num, DBL_MAX);
    parent.assign(num, 0);
    dist[0] = 0;
    checked[0] = 1;

    //first edge
    for (uint32_t i = 1; i < num; ++i){
        if (routePossible(cages[0], cages[i])){
            dist[i] = calcDist(cages[0], cages[i]);
        }
    }
    
    //double loop not the best
    for (uint32_t i = 1; i < num; ++i){
        uint32_t current_index = 0;
        double min_dist = DBL_MAX;
        
        //search for min_dist
        for (uint32_t j = 0; j < num; ++j){
            if (dist[j] < min_dist && checked[j] == 0){
                current_index = j;
                min_dist = dist[j];
            }
        }
        checked[current_index] = 1; //DO NOT MOVE, breaks
        
        for (uint32_t j = 0; j < num; ++j){
            // new dists
            if ((checked[j] == 0 && calcDist(cages[current_index], cages[j]) < dist[j]) && (routePossible(cages[current_index], cages[j]))){
                dist[j] = calcDist(cages[current_index], cages[j]);
                parent[j] = current_index;
            }
        }
        
    }
}


void Zookeeper::fastts_create(){
    //initialize path
    path.resize(num + 1);
    path[0] = 0;
    path[1] = 1;
    path[num] = 0;
    
    double minCost;// = figure out first value;
    //double weight_temp = 0;
    for(uint32_t i = 2; i < num; ++i){
        minCost = DBL_MAX;
        double least = 1;
        for(uint32_t j = 0; j < i; ++j){
            double temp = calcDist(cages[i], cages[path[j]]) + calcDist(cages[i], cages[path[j+1]]);
            double inbetween = calcDist(cages[path[j]], cages[path[j+1]]);
            if((temp - inbetween) < minCost) {
                minCost = temp - inbetween;
                least = j+1;
            }
        }
        //weight_temp += minCost;
        
        //path.push_back(path[least]);
        uint32_t count = i;
        for (; count > least; --count){
            path[count] = path[count - 1];
        }
        path[count] = i;
    }
    
}


void Zookeeper::fasttsp_c(vector<vector<double>>& matrix_in){
    vector<double> dist;
    checked.assign(num, 0);
    dist.assign(num, DBL_MAX);
    dist[0] = 0;
    checked[0] = 1;
    
    // input distances from origin into dist
    for (uint32_t i = 1; i < num; ++i){
        dist[i] = matrix_in[0][i];
    }
    
    // find the next dist
    uint32_t curr_index = 0;
    double min_dist = DBL_MAX;
    for (uint32_t i = 0; i < num; ++i){
        if (checked[i] == 0 && dist[i] < min_dist){
            curr_index = i;
            min_dist = dist[i];
        }
    }
    checked[curr_index] = 1;
    path.push_back(0);
    path.push_back(curr_index);
    
    //rest of the cages
    uint32_t found_cage;
    for(uint32_t j = 2; j < num; ++j){
        found_cage = findCage();
        
        // insert at least cost spot after having found a new cage
        double min_dist = DBL_MAX;
        double curr_dist = 0;
        uint32_t index = 0;
        for (uint32_t i = 0; i < (uint32_t) j - 1; ++i){
            curr_dist = matrix_in[path[i]][found_cage] + matrix_in[found_cage][path[i + 1]] - matrix_in[path[i]][path[i + 1]];
            if (curr_dist < min_dist){
                min_dist = curr_dist;
                index = i + 1;
            }
        }
        
        checked[found_cage] = 1;
        path.insert(path.begin() + index, found_cage);
    }
}

uint32_t Zookeeper::nearest(const vector<UncheckedCages> &uncheckedCages) {
    bool found = false;
    double min_dist = -1;
    uint32_t min_index = 0;

    for (uint32_t i = 0; i < (uint32_t)uncheckedCages.size(); ++i) {
        if ((uncheckedCages[i].unchecked_status == false) && !found) {
            min_dist = uncheckedCages[i].unchecked_dist;
            min_index = i;
            found = true;
        }
        else if (((uncheckedCages[i].unchecked_status == false) && found) && (uncheckedCages[i].unchecked_dist < min_dist)) {
            min_dist = uncheckedCages[i].unchecked_dist;
            min_index = i;
        }

    }

    return min_index;

}

double Zookeeper::mst_c(vector<vector<double>> &matrix_in, uint32_t initial_index) {
    vector<UncheckedCages> uncheckedCages;
    uint32_t size = (uint32_t) path.size() - initial_index;
    uncheckedCages.resize(size);
    
    for (uint32_t i = 0; i < size; ++i){
        uncheckedCages[i].unchecked_index = (uint32_t)path[initial_index];
        ++initial_index;
    }

    double final_dist = 0;
    double dist_temp = 0;
    uint32_t current_num = 0;
    uint32_t vec_size = (uint32_t)uncheckedCages.size();
    uncheckedCages[0].unchecked_dist = 0;
    UncheckedCages checking_cage;
    uint32_t current_index = 0;
    //can't figure out what to put in place of auto
    auto it = uncheckedCages.begin();
    
    for (it = uncheckedCages.begin(); !uncheckedCages.empty(); ++it){
        
        //setup
        current_num = nearest(uncheckedCages);
        it = uncheckedCages.begin() + current_num;
        checking_cage = uncheckedCages[current_num];
        
        //goal
        current_index = uncheckedCages[current_num].unchecked_index;//uncheckedCages[current_num].unchecked_index; //path[id_count]
        final_dist += checking_cage.unchecked_dist;
        
        //cleanup
        uncheckedCages.erase(it);
        
        //make sure neighbors r correct given change
        for (uint32_t i = 0; i < vec_size - 1; ++i) {
            dist_temp = matrix_in[current_index][uncheckedCages[i].unchecked_index];//[path[temp_id]];//[path[id_count]];//[uncheckedCages[i].unchecked_index];
            if (uncheckedCages[i].unchecked_dist > dist_temp) {
                uncheckedCages[i].unchecked_dist = dist_temp;
            }
        }
        --vec_size;
    }

    return final_dist;
}


bool Zookeeper::promising(vector<vector<double>> &matrix_in, uint32_t initial_index) {
    if ((path.size() - initial_index) <= 3) {
        return true;
    }
 
    uint32_t initial_cage = (uint32_t) path[0];
    double first_dist = DBL_MAX;
    uint32_t length = (uint32_t)(path.size() - initial_index);
    uint32_t changing_index = initial_index;
    
    for (uint32_t i = 0; i < length; ++i){
        if ((matrix_in[initial_cage][path[changing_index]] < first_dist) && (initial_cage != path[changing_index])) {
            first_dist = matrix_in[initial_cage][path[changing_index]];
        }
        ++changing_index;
    }
    
    
    uint32_t last_cage = (uint32_t) path[initial_index - 1];
    double last_dist = DBL_MAX;
    changing_index = initial_index;
    
    for (uint32_t i = 0; i < length; ++i){
        if ((matrix_in[last_cage][path[changing_index]] < last_dist) && (last_cage != path[changing_index])) {
            last_dist = matrix_in[last_cage][path[changing_index]];
        }
        ++changing_index;
    }


    final_dist = last_dist + first_dist + temp_dist + mst_c(matrix_in, initial_index);
    
    if (final_dist >= weight) {
        return false;
    }
    return true;
}

void Zookeeper::genPerms(vector<vector<double>> &matrix_in, uint32_t permLength){
    if (path.size() == permLength) {
        temp_dist += matrix_in[path[path.size() - 1]][path[0]];
        if (temp_dist < weight) {
            //updating final
            weight = temp_dist;
            opt_path = path;
        }
        temp_dist -= matrix_in[path[path.size() - 1]][path[0]];
        return;
    }
    if (!promising(matrix_in, permLength)){
        return;
    }
    for (uint32_t i = permLength; i < path.size(); ++i) {
        swap(path[permLength], path[i]);
        temp_dist += matrix_in[path[permLength - 1]][path[permLength]];
        genPerms(matrix_in, permLength + 1);
        temp_dist -= matrix_in[path[permLength - 1]][path[permLength]];
        swap(path[permLength], path[i]);
    }
}

void Zookeeper::opttsp_create(){
    //Set up matrix, if need to edit reserve look up 2D reserve bc nitpicky
    //temp_matrix is actually final_matrix but too late to change :/
    vector<vector<double>> temp_matrix(num, vector<double>(num, -1));
    double distance = 0;
    for (uint32_t i = 0; i < num; ++i) {
        for (uint32_t j = 0; j < num; ++j) {
            //don't want to go to calcDist if i == j
            if (i != j) {
                distance = calcDist(cages[(unsigned long)i], cages[(unsigned long)j]);
                temp_matrix[(unsigned long)i][(unsigned long)j] = distance;
            }
            else if (i == j) {
                temp_matrix[(unsigned long)i][(unsigned long)j] = 0;
            }
        }
    }
    
    fasttsp_c(temp_matrix);
    
    //find upper weight
    for (uint32_t i = 0; i < (uint32_t) path.size() - 1; ++i){
        weight += temp_matrix[path[i]][path[i + 1]];
    }
    weight += temp_matrix[path[0]][path[path.size() - 1]];
    
    genPerms(temp_matrix, 1);
    
}

void Zookeeper::mst_print(){
    double final_distance = 0;
    for (uint32_t i = 1; i < num; ++i){
        final_distance += calcDist(cages[(unsigned int)i], cages[parent[(unsigned int)i]]);
    }
    
    cout << final_distance << "\n";
    for (uint32_t i = 1; i < num; ++i){
        if (parent[(unsigned int)i] < i){
            cout << parent[(unsigned int)i] << " " << i << "\n";
        }
        else {
            cout << i << " " << parent[(unsigned int)i] << "\n";
        }
    }
}

void Zookeeper::fasttsp_print(){
    double final_distance = 0;
    uint32_t cage1 = 0;
    uint32_t cage2 = 0;
    for (uint32_t i = 0; i < num - 1; ++i){
        cage1 = path[(unsigned int)i];
        cage2 = path[(unsigned int)i + 1];
        final_distance += calcDist(cages[cage1], cages[cage2]);
    }
    final_distance += calcDist(cages[0], cages[path[num - 1]]);
    
    cout << final_distance << "\n";
    cout << path[0];
    for (uint32_t i = 1; i < num; ++i){
        cout << " " << path[(unsigned int)i];
    }
}

void Zookeeper::opttsp_print(){
    cout << weight << "\n";
    for (int i = 0; i < (int)opt_path.size(); ++i) {
        cout << opt_path[(unsigned int)i] << " ";
    }
    cout << "\n";
}

void Zookeeper::findPath(){
    //MST
    if (MODE == 0){
        mst_create();
    }
    //FASTTSP
    else if (MODE == 1){
        fastts_create();
    }
    //OPTTSP
    else if (MODE == 2){
        opttsp_create();
    }
    
    else {
        cerr << "Error: Mode input not working." << endl;
    }
}

void Zookeeper::print(){
    if (MODE == 0){
        mst_print();
    }
    else if (MODE == 1){
        fasttsp_print();
    }
    else if (MODE == 2){
        opttsp_print();
    }
    else {
        cerr << "Error: Mode input not working." << endl;
    }
}

int main(int argc,  char *argv[]) {
    ios_base::sync_with_stdio(false);
    xcode_redirect(argc, argv);
    
    cout << setprecision(2);
    cout << std::fixed;
    
    Zookeeper irwin;
    irwin.mode(argc, argv);
    irwin.input();
    
    irwin.findPath();
    irwin.print();
    return 0;
}



