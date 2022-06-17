// Project identifier: 9504853406CBAC39EE89AA3AD238AA12CA198043
//  main.cpp
//  Project2A
//
//  Created by Barbara Ribeiro on 10/6/21.
//
#include <iostream>
#include <sstream>
#include <vector>
#include <deque>
#include <queue>
#include <string>
#include <utility>
#include <cmath>
#include <algorithm>
#include <stdint.h>
#include <getopt.h>
#include "xcode_redirect.hpp"
#include "P2random.h"
using namespace std;

class Zombie {
public:
    Zombie(string n, uint32_t d, uint32_t s, uint32_t h): name(n), distance(d), speed(s), health(h), active_rounds(1), isActive(true){}
    
    struct ZombieCompare{
        bool operator()(Zombie *a, Zombie *b){
            uint32_t a_ETA = (a->distance)/(a->speed);
            uint32_t b_ETA = (b->distance)/(b->speed);
            bool is_less = false;
            
            //ETA is first comparison. if a > b, return true
            if (a_ETA > b_ETA){
                is_less = true;
            }
            
            //if both ETAs are the same break ties
            else if (a_ETA == b_ETA){
                
                //health comparison. a > b, return true
                if (a->health > b->health){
                    is_less = true;
                }
                
                //if both health the same, lexicographical comparison. a > b, return true
                else if (a->health == b->health){
                    string nameA = a->name;
                    string nameB = b->name;
                    if (nameA > nameB){
                        is_less = true;
                    }
                }
            }
            return is_less;
        }
    };
    
    struct ZombieCompareMost{
        bool operator()(Zombie *a, Zombie *b){
            bool is_less = false;
            if (a->active_rounds < b->active_rounds){
                is_less = true;
            }
            
            else if (a->active_rounds == b->active_rounds){
                if (a->name > b->name){
                    is_less = true;
                }
            }
            return is_less;
        }
    };
    
    struct ZombieCompareLeast{
        bool operator()(Zombie *a, Zombie *b){
            bool is_less = false;
            if (a->active_rounds > b->active_rounds){
                is_less = true;
            }
            
            else if (a->active_rounds == b->active_rounds){
                if (a->name > b->name){
                    is_less = true;
                }
            }
            return is_less;
        }
    };
    
    uint32_t getDist(){ return distance; }
    uint32_t getSpeed(){ return speed; }
    uint32_t getHealth(){ return health; }
    uint32_t getRounds(){ return active_rounds; }
    string getName(){ return name; }
    bool isAct() { return isActive; }
    string printZombie(){
        string n = name;
        string d = to_string(distance);
        string s = to_string(speed);
        string h = to_string(health);
        string print_out = name + " (distance: " + d + ", speed: " + s + ", health: " + h + ")";
        return print_out;
    }
    void hurt(uint32_t arrows_hit){
        health -= arrows_hit;
    }
    void addRound(){
        ++active_rounds;
    }
    void changeDist(uint32_t newDist){
        distance = newDist;
    }
    void kill(){
        isActive = false;
    }
    

private:
    string name = "";
    uint32_t distance = 1;
    uint32_t speed = 1;
    uint32_t health = 1;
    uint32_t active_rounds = 0;
    bool isActive = true;

};

class Game {
public:
    Game(){};
    void mode(int argc, char * argv[]);
    void starter_info();
    void update_current_zombies();
    void create_new_rand_zombies();
    void create_new_custom_zombies();
    void shoot();
    void medianPartition(uint32_t lifetime);
    uint32_t medianCalc();
    void statsCalc();
    void round();

private:
    
    //Data structures
    priority_queue<Zombie*, vector<Zombie*>, Zombie::ZombieCompare> fightingZombies;
    
    priority_queue<Zombie*, vector<Zombie*>, Zombie::ZombieCompareMost> mostActive;
    priority_queue<Zombie*, vector<Zombie*>, Zombie::ZombieCompareLeast> leastActive;
    
    priority_queue<uint32_t, vector<uint32_t>, greater<>> rightPartition;
    priority_queue<uint32_t, vector<uint32_t>, less<>> leftPartition;
    
    //allZombies could potentially be a vector
    deque<Zombie> allZombies;
    vector<Zombie*> killedZombies;

    //Extra member variables
    string killer_name = "";
    string last_killed = "";
    bool alive = true;
    
    //Variables from input
    uint32_t quiver_capacity = 0;
    uint32_t random_seed = 1;
    uint32_t mr_distance = 1;
    uint32_t mr_speed = 1;
    uint32_t mr_health = 1;

    //Variables from command line
    bool isVerb = false;
    bool isStat = false;
    uint32_t stat_num = 0;
    bool isMed = false;

};

void help(char *argv[]) {
    cout << "Usage: " << argv[0] << "-v|-m|-s" << endl;
    cout << "Help: Zombie simulator game. Will the player survive the waves of zombies? \n";
    cout << "Flags: --verbose, -v: if set, will print when zombies are created, moved, and destroyed \n";
    cout << "--statistics <num>, -s <num>: num > 0, if set, will print out statistics  \n";
    cout << "--median, -m: if set, will print out messages of median time zombies have been activated before being killed";
}

void Game::mode(int argc, char * argv[]) {
    opterr = false;
    int opt;
    int optionCount = 0;
    option long_options[] = {
        { "verbose", no_argument, nullptr, 'v'},
        { "statistics", required_argument, nullptr, 's'},
        { "median", no_argument, nullptr, 'm'},
        { "help", no_argument, nullptr, 'h'},
        { nullptr, 0, nullptr, '\0'}
    };

    while ((opt = getopt_long(argc, argv, "vs:mh", long_options, &optionCount)) != -1) {
        switch (opt) {
            case 'h':
                help(argv);
                exit(0);
            
            case 's':
                stat_num = static_cast<uint32_t>(stoi(optarg));
                isStat = true;
                break;
                
            case 'v':
                isVerb = true;
                break;

            case 'm':
                isMed = true;
                break;

            default:
                cerr << "Error: invalid flag" << endl;
                exit(1);
        }
    }
}

//Read in input from beginning of file and initialize randomizer
void Game::starter_info(){
    string junk;
    getline(cin, junk);
    cin >> junk >> quiver_capacity;
    cin >> junk >> random_seed;
    cin >> junk >> mr_distance;
    cin >> junk >> mr_speed;
    cin >> junk >> mr_health;
    P2random::initialize(random_seed,mr_distance,mr_speed,mr_health);
}

//Update the location of the zombies and kill the player if a zombie reaches 0
void Game::update_current_zombies(){

    //Iterates through allZombie deque to get correct movement order
    for (uint32_t i = 0; i < allZombies.size(); ++i){
        Zombie* current_zombie = &allZombies[i];
        
        //if the current_zombie is alive/active, update it
        if (current_zombie->isAct()){
            uint32_t dist = current_zombie->getDist();
            uint32_t speed = current_zombie->getSpeed();
            uint32_t new_distance = dist;
            
            //check if dist > speed. If so, set zombie's distance to dist - speed
            if (dist > speed){
                new_distance = dist - speed;
                current_zombie->changeDist(new_distance);
            }
            //if speed >= dist, set zombie's distance to zero
            else {
                current_zombie->changeDist(0);
            }
            
            //verbose mode: print moved statement
            if (isVerb){
                cout << "Moved: " << current_zombie->Zombie::printZombie() << "\n";
            }
            
            //if current_zombie has reached the player (0 dist), kill the player
            //set alive to false and save the zombie's name as the killer.
            if (current_zombie->getDist() == 0){
                alive = false;
                if (killer_name == ""){
                    killer_name = current_zombie->getName();
                }
            }
            
            //increment current_zombie's active rounds by one
            current_zombie->addRound();
        }
    }
}

//Random zombie creator
//Takes in number of new random zombies this round and uses the randomized values to create them
void Game::create_new_rand_zombies(){
    string junk;
    string num;
    
    //  >> random-zombies: >> num
    cin >> junk >> num;
    
    uint32_t num_rand_zomb = static_cast<uint32_t>(stoi(num));
 
    //makes starting index for new zombies equal the current deque size
    uint32_t index = static_cast<uint32_t>(allZombies.size());
    
    for (uint32_t i = 0; i < num_rand_zomb; ++i){
        std::string name = P2random::getNextZombieName();
        uint32_t distance = P2random::getNextZombieDistance();
        uint32_t speed = P2random::getNextZombieSpeed();
        uint32_t health = P2random::getNextZombieHealth();
        
        //creates a random zombie using randomized values
        Zombie temp_zombie = Zombie(name, distance, speed, health);
        
        //adds new zombie to allZombies
        allZombies.push_back(temp_zombie);
        
        //create a ptr to new zombie's allZombies's location
        Zombie* temp_ptr = &allZombies[index];
        
        //pushes the new ptr to fightingZombies pq
        fightingZombies.push(temp_ptr);

        //verbose on prints created message
        if (isVerb){
            cout << "Created: " << temp_zombie.Zombie::printZombie() << "\n";
        }
        
        //increment allZombies index for new zombie location in next round
        ++index;
    }
}

//Custom zombie creation
//Takes in number of custom zombies to create this round and their names, distances, speeds, and health
void Game::create_new_custom_zombies(){
    string junk;
    string c_name;
    string c_dist;
    string c_speed;
    string c_health;
    string temp;
    
    //  >> custom-zombies >> num_of_custom_zomb
    cin >> junk >> temp;
    
    uint32_t customNum = static_cast<uint32_t>(stoi(temp));
    
    //makes starting index for new zombies equal the current deque size
    uint32_t custom_index = static_cast<uint32_t>(allZombies.size());
    
    for (uint32_t i = 0; i < customNum; ++i){
        
        //  >> name >> distance: >> dist >> speed: >> speed >> health: >> health
        cin >> c_name >> junk >> c_dist >> junk >> c_speed >> junk >> c_health;
        
        uint32_t distance = static_cast<uint32_t>(stoi(c_dist));
        uint32_t speed = static_cast<uint32_t>(stoi(c_speed));
        uint32_t health = static_cast<uint32_t>(stoi(c_health));
        
        //creates a custom zombie using input values
        Zombie temp_zombie = Zombie(c_name, distance, speed, health);
        
        //adds new zombie to allZombies
        allZombies.push_back(temp_zombie);
        
        //create a ptr to new zombie's allZombies's location
        Zombie* temp_ptr = &allZombies[custom_index];
        
        //pushes the new ptr to fightingZombies pq
        fightingZombies.push(temp_ptr);
        
        //verbose on prints created message
        if (isVerb){
            cout << "Created: " << temp_zombie.Zombie::printZombie() << "\n";
        }
        
        //increment allZombies index for new zombie location in next round
        ++custom_index;
    }
}

//Zombie offense: use arrows to decrease the health of the zombie at the top of the pq
void Game::shoot(){
    
    //resets number of arrows to quiver_capacity for the new round
    uint32_t arrows = quiver_capacity;
    
    //while the quiver doesn't run out and there are still living zombies:
    while (arrows > 0 && fightingZombies.size() > 0){
        Zombie* current_zombie = fightingZombies.top();
        
        //timesShot = which one runs out first, arrows or the current zombie's health
        uint32_t timesShot = min(arrows, current_zombie->getHealth());
        
        //decrease arrows completely if arrows are min, or deplete them if zombie's health is lower
        arrows -= timesShot;
        
        //decrease the current zombie's health by the number of times its been shot
        current_zombie->hurt(timesShot);
        
        //if the zombie's health is 0, it's dead
        if (current_zombie->getHealth() == 0){
            
            //stat on adds zombie to back of killedZombies vector
            if (isStat){
                killedZombies.push_back(current_zombie);
            }
            
            else {
                last_killed = current_zombie->getName();
            }
            
            //median on adds the lifetime to one of the two median pqs depending on its value
            if (isMed){
                uint32_t lifetime = current_zombie->getRounds();
                medianPartition(lifetime);
            }
            
            //verbose on prints destroyed message
            if (isVerb){
                cout << "Destroyed: " << current_zombie->Zombie::printZombie() << "\n";
            }
            
            //set zombie to not active as it's dead
            current_zombie->kill();
            
            //pop the ptr pointing to current zombie from the fighting pq
            fightingZombies.pop();
        }
    }
}
    
//pushes the new lifetime into the appropriate pq depending on its value
void Game::medianPartition(uint32_t lifetime){
    int rightSize = static_cast<int>(rightPartition.size());
    int leftSize = static_cast<int>(leftPartition.size());
    
    //if left partition is empty, push to the right because that's the side that values get compared to
    if (leftSize == 0){
        rightPartition.push(lifetime);
    }
    
    //if new lifetime > than the top of the right partition (lowest num on the right), push to the right
    else if (lifetime > rightPartition.top()){
        rightPartition.push(lifetime);
    }
    
    //if not, push to the left
    else {
        leftPartition.push(lifetime);
    }

    //if the difference in size between the two partitions is greater than 1, balance it
    int larger = max(leftSize, rightSize);
    int smaller = min(leftSize, rightSize);
    if (larger - smaller > 1){
        //if the left is smaller, push the right's top value to left and pop it out of the right
        if (leftSize < rightSize){
            leftPartition.push(rightPartition.top());
            rightPartition.pop();
        }
        //if the right is smaller, push the left's top value to right and pop it out of the left
        else {
            rightPartition.push(leftPartition.top());
            leftPartition.pop();
        }
    }
}

//finds the median using the two partition pqs
uint32_t Game::medianCalc(){
    
    //if the two sides' sizes are equal, add the top values of the right and the left (the two middle values)
    //and divide by two
    if (rightPartition.size() == leftPartition.size()){
        return ((rightPartition.top() + leftPartition.top()) / 2);
    }
    
    //if the right is bigger than the left, print the top of the right (the middle value)
    else if (rightPartition.size() > leftPartition.size()){
        return rightPartition.top();
    }
    
    //if the left is bigger than the right, print the top of the left (the middle value)
    else{
        return leftPartition.top();
    }
}


//finds the statistic calulations: zombies still active, first and last killed, most and least active
void Game::statsCalc(){
    //killed_size is now the casted killed vec size
    uint32_t killed_size = static_cast<uint32_t> (killedZombies.size());
    uint32_t num = stat_num;
    
    cout << "Zombies still active: " << fightingZombies.size() << "\n";
    
    cout << "First zombies killed:\n";
    
    //if the stat N is larger than the size of the killed vector, num = the size of the vector
    if (num > killedZombies.size()){
        num = killed_size;
    }
    
    //from 0 to num increasing, prints the name and order of killed zombies
    for (uint32_t i = 0; i < num; ++i){
        cout << killedZombies[i]->getName() << " " << (i + 1) << "\n";
    }
    
    cout << "Last zombies killed:\n";
    
    //from num to 0 decrementing, prints the name and order of killed zombies starting from the end of the vector
    for (uint32_t i = num; i > 0; --i){
        cout << killedZombies[killed_size - 1]->getName() << " " << (i) << "\n";
        --killed_size;
    }
    
    //from i to size of allZombies deque, push ptrs of all the zombies into the mostActive and leastActive pqs
    for (uint32_t i = 0; i < allZombies.size(); ++i){
        mostActive.push(&allZombies[i]);
        leastActive.push(&allZombies[i]);
    }
    
    //if stat_num is larger than the activity pqs, set num to the size of mostActive
    num = stat_num;
    if (num > mostActive.size()){
        num = static_cast<uint32_t> (mostActive.size());
    }
    
    //from 0 to num incrementing, print the top of mostActive's name and active rounds and then pop it out
    cout << "Most active zombies:\n";
    for (uint32_t i = 0; i < num; ++i){
        cout << mostActive.top()->getName() << " " << mostActive.top()->getRounds() << "\n";
        mostActive.pop();
    }
    
    //from 0 to num incrementing, print the top of leastActive's name and active rounds and then pop it out
    cout << "Least active zombies:\n";
    for (uint32_t i = 0; i < num; ++i){
        cout << leastActive.top()->getName() << " " << leastActive.top()->getRounds() << "\n";
        leastActive.pop();
    }
}

//main Game function, calls the other ones to make the full game run
void Game::round(){
    string junk;
    string c_round;
    
    //the current round the game is on
    int round_num = 0;
    //the next round given in input
    int input_round = 0;
    
    bool remainingFile = true;
    bool isInputEqualCurr = true;
    
    //  >> --- >> round: >> inputted_round
    cin >> junk >> junk >> c_round;
    input_round = static_cast<uint32_t>(stoi(c_round));
    
    while ((fightingZombies.size() > 0) || remainingFile){
        ++round_num;
        int input_num = static_cast<int>(input_round);
        
        //1: if input round and current round aren't the same, don't read in input in this iteration
        if (input_num != round_num){
            isInputEqualCurr = false;
        }
        
        //2: verbose on then print round number
        if (isVerb){
            cout << "Round: " << round_num << "\n";
        }
        
        //3: update/move current zombies & kill player if necessary
        update_current_zombies();
        
        //4: if player is dead, print the defeat line, stats if it's on, and break the loop
        if (!alive){
            cout << "DEFEAT IN ROUND " << round_num << "! " << killer_name << " ate your brains!\n";
            if (isStat){
                statsCalc();
            }
            break;
        }
        
        //5: create new zombies if can read in input in the iteration (if input_round == curr_round)
        if (isInputEqualCurr){
            create_new_rand_zombies();
            create_new_custom_zombies();
        }
        
        //6: player offense, shoot and kill zombies in pq order
        shoot();
        
        //7: if --m and a zombie has been killed, message about median time
        if (isMed && !rightPartition.empty()){
            uint32_t med = medianCalc();
            cout << "At the end of round " << round_num << ", the median zombie lifetime is " << med << "\n";
        }
        
        //8: make sure there's still a file. If there isn't, set bool to fail the next loop.
        //If there is, read in next round from input file
        if (isInputEqualCurr){
            string test;
            //  >> ---
            cin >> test;
            if (cin.fail()){
                remainingFile = false;
            }
            else {
                string c_round;
                //  >> round: >> num
                cin >> junk >> c_round;
                input_round = static_cast<uint32_t>(stoi(c_round));
            }
        }
        
        //default isInputEqualCurr to true (first conditional in iteration can break it if necessary)
        isInputEqualCurr = true;
        
    }
    
    //Broken out of loop, so if alive, player won the game
    if (alive){
        
        //TODO: Something here is causing a memory error: Use of uninitialised value of size 8
        
        if (isStat){
            last_killed = killedZombies.back()->getName();
        }

        int test_num = round_num;
        cout << "VICTORY IN ROUND " << test_num << "! " << last_killed << " was the last zombie.\n";
        
        if (isStat){
            statsCalc();
        }
    }
}


int main(int argc, char * argv[]) {
    ios_base::sync_with_stdio(false);
    xcode_redirect(argc, argv);
    
    Game zombie_game;
    zombie_game.mode(argc, argv);
    zombie_game.starter_info();
    zombie_game.round();
    
    return 0;
}
