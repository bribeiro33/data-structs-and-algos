//  950181F63D0A883F183EC0A5CC67B19928FE896A
//  project1.cpp
//  Project1
//
//  Created by Barbara Ribeiro on 9/13/21.
//

#include <iostream>
#include <string>
#include <sstream>
#include <deque>
#include <map>
#include <vector>
#include <stack>
#include <queue>
#include <utility>
#include <cmath>
#include <algorithm>
#include <stdint.h>
#include <getopt.h>
#include "xcode_redirect.hpp"

using namespace std;
class Station{
    public:
    
    struct Location{
        // coordinate floor
        uint32_t floor;
        // coordinate row
        uint32_t row;
        // coordinate col
        uint32_t col;
        
        //default constructor
        Location(): floor(0), row(0), col(0) {}
        //initialization constructor
        Location(uint32_t f, uint32_t r, uint32_t c) : floor(f), row(r), col(c){}
        Location(const Location &l) {
                    floor = l.floor;
                    row = l.row;
                    col = l.col;
        }
        
        //set Location
        Location operator=(const Location& a) {
            floor = a.floor;
            row = a.row;
            col = a.col;
            return *this;
        }
        
        //Location equals operator, true when floor, row, and col are the same
        bool operator==(const Location& a) {
            if ((this->floor == a.floor) && (this->row == a.row) && (this->col == a.col)){
                return true;
            }
            return false;
        }
        
        // Location not equal operator, true when floor, row, col aren't all the same
        bool operator!=(const Location& a) {
            if ((this->floor != a.floor) || (this->row != a.row) || (this->col != a.col)){
                return true;
            }
            return false;
        }
        
    };
                           
        

    struct Square{
        // Options: . # E S H
        char type;
        // Location approached from: n e s w floorlevel
        char loc;
        // whetehr or not tile discovered
        bool discovered;
        
        //default is a isn't discovered normal floor tile
        Square(): type('.'), loc('n'), discovered(false) {}
        Square(const Square &s) {
            type = s.type;
            loc = s.loc;
            discovered = s.discovered;
        }
        
        //set Square
        Square operator=(const Square& a) {
            type = a.type;
            loc = a.loc;
            discovered = a.discovered;
            return *this;
        }
    };
    
    //functions
    void mode(int argc, char * argv[]);
    void input();
    void path_search();
    void direction_check(char direction, Square* dSquare, Location* robot, bool* run);
    void backtrace();
    void backtrace_direction(char location, Location* robot);
    void output();
    void validPathOutput();
    void noPathOutput();
    //void game();
    
    private:
        //3D Vector of the whole map of the ship (squares)
        vector<vector<vector<Square>>> ship;
        //Search container to be used as a stack or a queue
        deque<Location> search;
        
        //Starting location
        Location startL;
        //Ending Location (Hangar)
        Location endL;
        
        
        //how to route container (queue or stack)
        char route_type;
        //input mode char
        char imode;
        //output mode
        char omode;
        
        uint32_t level_num;
        uint32_t size;
        
        //if there is a path or not, default to false;
        bool path = false;

};

// help func for player, not in Station class
void help(char *argv[]) {
    cout << "Usage: " << argv[0] << "-p|s --output(M|L)" << endl;
    cout << "Help: Back to the Ship Game. You start at Start and if a path to the hangar exists, the robot finds it and shows you how to get there \n";
    cout << "Flags: --stack, -s: if set, use stack-based routing scheme \n";
    cout << "--queue, -q: if set, use queue-based routing scheme \n";
    cout << "--output (M|L), -o(M|L): Output file format, M = map L = List, defaults to map";
}

//Get the input, output, and search modes from command line
void Station::mode(int argc, char * argv[]) {
    //if stay false then need to go to error
    bool haveOutputType = false;
    bool haveRouteType = false;
    string mode;
    
    //Check lab for getopt help
    opterr = false;
    //option but long_op
    int opt;
    int optionCount = 0;
    option long_options[] = {
        { "stack", no_argument, nullptr, 's'},
        { "queue", no_argument, nullptr, 'q'},
        { "output", required_argument, nullptr, 'o'},
        { "help", no_argument, nullptr, 'h'},
        { nullptr, 0, nullptr, '\0'}
    };

    // help, output, stack, and queue flags
    while ((opt = getopt_long(argc, argv, "sqo:h", long_options, &optionCount)) != -1) {
        switch (opt) {
            case 'h':
                help(argv);
                exit(0);

            case 'o':
                mode = optarg;
                haveOutputType = true;
                omode = static_cast<char>(mode[0]);
                break;
                    
            case 's':
                if (haveRouteType) {
                    cerr << "Error: Multiple routing modes specified" << endl;
                    exit(1);
                }
                haveRouteType = true;
                route_type = 's';
                break;
                    
            case 'q':
                if (haveRouteType) {
                    cerr << "Error: Multiple routing modes specified" << endl;
                    exit(1);
                }
                haveRouteType = true;
                route_type = 'q';
                break;

            default:
                cerr << "Error: invalid flag" << endl;
                exit(1);
        }
    }
    
    // Error if there's no route type identified
    if (!haveRouteType) {
        cerr << "Error: No routing mode specified" << endl;
        exit(1);
    }
    
    // don't have to specify omode, default to map
    if (!haveOutputType) {
        omode = 'M';
        haveOutputType = true;
    }
}

void Station::input(){
    //Initialize a deafult Square
    Square sq;
    
    //vars for both map and list
    string junk;

//    //read in imode
//    string inputMode;
//    getline(cin, inputMode);
//    imode = inputMode[0];
//
//    //read in number of levels and convert to uint
//    string levels;
//    getline(cin, levels);
//    level_num = static_cast<uint32_t>(levels[0] - 0);
//
//    //read in station size and convert to uint
//    string n;
//    getline(cin, n);
//    size = static_cast<uint32_t>(n[0] - 0);
    
    //read in imode, level_num, and map size
    cin >> imode >> level_num >> size;

    ship.resize(level_num, vector<vector<Square>>(size, vector<Square>(size, sq)));
    string input;
    // if input mode == map
    if (imode == 'M') {
        uint32_t currR = 0;
        uint32_t currF = 0;
        //getline(cin, junk);
        while (getline(cin, input)) {
            //checks for comment and blank line
            if (input[0] != '/' && input != "") {
                for (uint32_t currC = 0; currC < size; ++currC) {
                    //current char = string indexed to correct char by col (array)
                    char currChar = input[currC];
                    
                    // error checking, is the char a real char?
                    if (!(currChar == 'S' || currChar == 'H' || currChar == 'E' || currChar == '#' || currChar == '.')) {
                        cerr << "Error: Invalid map character" << endl;
                        exit(1);
                    }
                    
                    // check to see if char is S and set startL to all the same vals
                    if (currChar == 'S') {
                        startL.floor = currF;
                        startL.row = currR;
                        startL.col = currC;
                    }
            
                    // store the square type
                    ship[currF][currR][currC].type = currChar;
                    
                }
                //increase row by 1
                ++currR;
                //if at the end of the row, reset row and increase floor by 1, avoids three nested for loops
                if (currR == size) {
                    ++currF;
                    currR = 0;
                }
            }
        }
    }
        
    // if input mode == list
    if (imode == 'L') {
        uint32_t cfloor;
        uint32_t crow;
        uint32_t ccol;
        char input;
        while (cin >> input) {
            if (input != '/' && input != ' ') {
                // read in each line: floor space row space col space type, input = junk
                cin >> cfloor >> input >> crow >> input >> ccol >> input >> sq.type >> input;
                
                // check for any errors like out of bounds size or incorrect char
                if (cfloor >= level_num) {
                    cerr << "Error: Invalid map level" << endl;
                    exit(1);
                }
                if (crow >= size) {
                    cerr << "Error: Invalid map row" << endl;
                    exit(1);
                }
                if (ccol >= size) {
                    cerr << "Error: Invalid map column" << endl;
                    exit(1);
                }
                if (!(sq.type == 'S' || sq.type == 'H' || sq.type == 'E' || sq.type == '#' || sq.type == '.')) {
                    cerr << "Error: Invalid map character" << endl;
                    exit(1);
                }
                
                // if square is the start, make the floor, row, and col of that location startL
                if (sq.type == 'S') {
                    startL = {cfloor, crow, ccol};
                }
                
                //make the current square a part of ship
                ship[cfloor][crow][ccol] = sq;
            }
            
            else{
                getline(cin, junk);
            }
        }
    }
}

void Station::direction_check(char direction, Square* dsquare, Location* robot, bool* run){
    //defaults to north
    int r = 0;
    int c = 0;
    
    if (direction == 'n'){
        r = -1;
        c = 0;
    }
    
    if (direction == 'e'){
        r = 0;
        c = 1;
    }
    else if (direction == 's'){
        r = 1;
        c = 0;
    }
    else if (direction == 'w'){
        r = 0;
        c = -1;
    }
    
    
    if  (dsquare->type != '#' && !dsquare->discovered) {
        //go into if tile is the hanger and then break out of loop
        if (dsquare->type == 'H') {
            path = true;
            endL.floor = robot->floor;
            
            int temp_row = static_cast<int>(robot->row);
            int temp_col = static_cast<int>(robot->col);
            temp_row = temp_row + r;
            temp_col = temp_col + c;
            
            endL.row = static_cast<uint32_t>(temp_row);
            endL.col = static_cast<uint32_t>(temp_col);
            
            if (direction == 'n'){ ship[endL.floor][endL.row][endL.col].loc = 's'; }
            else if (direction == 'e'){ ship[endL.floor][endL.row][endL.col].loc = 'w'; }
            else if (direction == 's'){ ship[endL.floor][endL.row][endL.col].loc = 'n'; }
            else if (direction == 'w'){ ship[endL.floor][endL.row][endL.col].loc = 'e'; }
            
            *run = false;
        }
        else {
            // is discovered
            dsquare->discovered = true;
            
            // direction came from
            if (direction == 'n'){ dsquare->loc = 's'; }
            else if (direction == 'e'){ dsquare->loc = 'w'; }
            else if (direction == 's'){ dsquare->loc = 'n'; }
            else if (direction == 'w'){ dsquare->loc = 'e'; }
            
            //push to search
            int temp_row = static_cast<int>(robot->row);
            int temp_col = static_cast<int>(robot->col);
            temp_row = temp_row + r;
            temp_col = temp_col + c;
            uint32_t new_row = static_cast<uint32_t>(temp_row);
            uint32_t new_col = static_cast<uint32_t>(temp_col);
            
            Location newL(robot->floor, new_row, new_col);
            search.push_back(newL);
        }
    }
}


void Station::path_search() {
    //push start location into search container
    search.push_back(startL);
    //change start tile to discovered
    ship[startL.floor][startL.row][startL.col].discovered = true;
    Location robotL;
    
    //keep running search till hit hangar and run == false;
    bool run = true;
    //while the search container still has tiles to search off of
    while (!search.empty() && run){
        // stack LIFO
        if (route_type == 's') {
            robotL = search.back();
            search.pop_back();
        }
        // queue FIFO
        else {
            robotL = search.front();
            search.pop_front();
        }
        
        
        //current = csquare
        Square* current = &ship[robotL.floor][robotL.row][robotL.col];
        
//        if (ship[robotL.floor][robotL.row][robotL.col].type == 'E') {
//            // check floors 0 to level_num for elevators in same coord
//            for (uint32_t current_level = 0; current_level < level_num; ++current_level) {
//                //Square* current_elevator = &ship[current_level][robotL.row][robotL.col];
//                if (ship[current_level][robotL.row][robotL.col].type == 'E' && !ship[current_level][robotL.row][robotL.col].discovered) {
//                    // approached from current floor
//                    ship[current_level][robotL.row][robotL.col].loc = static_cast<char>('0' + robotL.floor);
//                    ship[current_level][robotL.row][robotL.col].discovered = true;
//                    Location newL(current_level, robotL.row, robotL.col);
//                    search.push_back(newL);
//                }
//            }
//        }
        
        // NORTH exists, !wall, !discovered
        if  (robotL.row != 0) {
            //northern = dsquare
            Square* northern = &ship[robotL.floor][(robotL.row) - 1][robotL.col];
            direction_check('n', northern, &robotL, &run);
        }

        // EAST exists, !wall, !discovered
        if  (robotL.col < (size - 1)) {
            Square* eastern = &ship[robotL.floor][robotL.row][(robotL.col) + 1];
            direction_check('e', eastern, &robotL, &run);
        }

        // SOUTH exists, !wall, !discovered
        if  (robotL.row < (size - 1)){
            Square* southern = &ship[robotL.floor][(robotL.row) + 1][robotL.col];
            direction_check('s', southern, &robotL, &run);
        }

        // WEST exists, !wall, !discovered
        if  (robotL.col != 0){
            Square* western = &ship[robotL.floor][robotL.row][(robotL.col) - 1];
            direction_check('w', western, &robotL, &run);
        }
        
        // ELEVATOR
        if (current->type == 'E') {
            // check floors 0 to level_num for elevators in same coord
            for (uint32_t current_level = 0; current_level < level_num; ++current_level) {
                Square* current_elevator = &ship[current_level][robotL.row][robotL.col];
                if (current_elevator->type == 'E' && !current_elevator->discovered) {
                    // approached from current floor
                    current_elevator->loc = static_cast<char>('0' + robotL.floor);
                    current_elevator->discovered = true;
                    Location newL(current_level, robotL.row, robotL.col);
                    search.push_back(newL);
                }
            }
        }
    }
}

void Station::backtrace_direction(char direction, Location* robot){
   //defaults to north
    int r = 0;
    int c = 0;
    if (direction == 'n'){
        r = -1;
    }
    if (direction == 'e'){
        c = 1;
    }
    else if (direction == 's'){
        r = 1;
    }
    else if (direction == 'w'){
        c = -1;
    }
    
    int temp_row = static_cast<int>(robot->row);
    int temp_col = static_cast<int>(robot->col);
    temp_row = temp_row + r;
    temp_col = temp_col + c;
    
    uint32_t new_row = static_cast<uint32_t>(temp_row);
    uint32_t new_col = static_cast<uint32_t>(temp_col);
    
    Location newL(robot->floor, new_row, new_col);
    search.push_back(newL);
    
    //change type to opposite of loc to then be able to print out
    if (direction == 'n'){
        ship[newL.floor][newL.row][newL.col].type = 's';
    }

    else if (direction == 'e'){
        ship[newL.floor][newL.row][newL.col].type = 'w';
    }

    else if (direction == 's'){
        ship[newL.floor][newL.row][newL.col].type = 'n';
    }

    else if (direction == 'w'){
        ship[newL.floor][newL.row][newL.col].type = 'e';
    }

    *robot = newL;
    
}

void Station::backtrace() {
    if (!path){
        return;
    }
    // clear the search container
    search.clear();
    
    Location robotL = endL;
    while (robotL != startL){
        
        //char ctype = current.type;
        char cloc = ship[robotL.floor][robotL.row][robotL.col].loc;

        // came from a direction
        if (cloc == 'n' || cloc == 'e' || cloc == 's' || cloc == 'w') {
            backtrace_direction(cloc, &robotL);
        }
        
        // came from an elevator
        else {
            uint32_t next_floor = static_cast<uint32_t>(ship[robotL.floor][robotL.row][robotL.col].loc - '0');
            Location newL(next_floor, robotL.row, robotL.col);
            search.push_back(newL);
            ship[next_floor][newL.row][newL.col].type = static_cast<char>('0' + robotL.floor);
            robotL = newL;
            continue;
        }
    }
}

void Station::validPathOutput(){
    
    //if omode is list:
    if (omode == 'L') {
        // print each square
        cout << "//path taken\n";
        while (!search.empty()) {
            Location currentL = search.back();
            Square currentS = ship[currentL.floor][currentL.row][currentL.col];
            cout << '(' << currentL.floor << ',' << currentL.row << ',' << currentL.col << ',' << currentS.type << ')' << "\n";
            
            // remove tile from search
            search.pop_back();
        }
    }
    
    //defaults to map omode
    else {
        // print each square
        cout << "Start in level " << startL.floor << ", row " << startL.row << ", column " << startL.col << "\n";
        for (uint32_t level = 0; level < level_num; ++level) {
            // print level_num numbers
            cout << "//level " << level << "\n";
            for (uint32_t row = 0; row < size; ++row) {
                for (uint32_t col = 0; col < size; ++col) {
                    cout << ship[level][row][col].type;
                }
                cout << "\n";
            }
        }
    }
}

void Station::noPathOutput(){
    
    //if omode is list:
    if (omode == 'L') {
            cout << "//path taken";
    }
    
    //if omdode isn't list, defaults to map
    else {
        // print all tiles
        
        for (uint32_t level = 0; level < level_num; ++level) {
            // print level numbers
            cout << "//level " << level << endl;
            for (uint32_t row = 0; row < size; ++row) {
                for (uint32_t col = 0; col < size; ++col) {
                    cout << ship[level][row][col].type;
                }
                cout << "\n";
            }
        }
    }
}

void Station::output(){
    if (path){
        validPathOutput();
    }
    else{
        noPathOutput();
    }
}

int main(int argc,char * argv[]) {
    ios_base::sync_with_stdio(false);
    xcode_redirect(argc, argv);
    Station station_game;
    station_game.mode(argc, argv);
    station_game.input();
    station_game.path_search();
    station_game.backtrace();
    station_game.output();
    return 0;
}
