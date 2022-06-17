//  Project Identifier: 01BD41C3BF016AD7E8B6F837DF18926EC3E83350
//  Project3.cpp
//  Project3
//
//  Created by Barbara Ribeiro on 11/4/21.
//

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <iterator>
#include <deque>
#include <queue>
#include <string>
#include <utility>
#include <cmath>
#include <algorithm>
#include <stdint.h>
#include <getopt.h>
#include <cctype>

#include <unordered_map>
#include <unordered_set>
#include <set>
#include <stdio.h>
#include <time.h>
#include "xcode_redirect.hpp"

using namespace std;
struct LogEntry {
    
    //default constructor, sets entry id to 0
    LogEntry() {
        entryID = 0;
    }
    
    //custom constructor
    LogEntry(uint32_t id, string sTime, string ogC, string m):
        entryID(id), strTimestamp(sTime), ogCategory(ogC), message(m)  {}
    
    uint32_t entryID;

    int64_t longTimestamp;
    string strTimestamp;
    string ogCategory;
    //string allLowerCategory;
    string message;
    
    // less than operator, returns true if left is less than right
    bool operator< (LogEntry& right) {
        return longTimestamp < right.longTimestamp;
    }
    
    //greater than operator, returns true if right is less than left
    bool operator> (LogEntry& right) {
        return longTimestamp >= right.longTimestamp;
    }
    
    //conversion operator, returns true if 1 < 2
    bool operator() (const LogEntry& log1, const LogEntry& log2) {
        return log1.longTimestamp < log2.longTimestamp;
    }
    
};

bool logCompare(LogEntry& log1, LogEntry& log2) {
    //default is false
    bool result = false;
    
    //if log 1's timestamp < log's, return true
    if (log1.longTimestamp < log2.longTimestamp){
        result = true;
    }
    
    //if the timestamps are the same
    else if (log1.longTimestamp == log2.longTimestamp) {
        
        //don't know why this has to be before cate<cate, but it breaks when I switch it so don't touch
        //if the categories are the same, break ties with entryID
        string lower_category1 = log1.ogCategory;
        string lower_category2 = log2.ogCategory;
        transform(lower_category1.begin(), lower_category1.end(), lower_category1.begin(), ::tolower);
        transform(lower_category2.begin(), lower_category2.end(), lower_category2.begin(), ::tolower);
        if (lower_category1 == lower_category2 && log1.entryID < log2.entryID) {
            result = true;
        }
        
        //if timestamps are the same, break ties with categories
        else if (lower_category1 < lower_category2){
            result = true;
        }
    }
    
    return result;
}

class LogMan {
public:
    LogMan(){};
    //functions
    bool input_file(string& filename);
    void commands();
    int64_t timeStrToInt(string& str_time);
    //string timeIntToStr(int64_t& int_time);
    void toLower(string& str);
    void initializer();
    void logPrint(uint32_t i);
    
    void timestampHelper(LogEntry &log1, LogEntry &log2, string print);
    
    void timestampSearch();
    void matchingSearch();
    void categorySearch();
    void keywordSearch();
    
    void appendEntry();
    void appendSearchResults();
    void deleteLogEntry();
    
    void moveToBegin();
    void moveToEnd();
    
    void sortExcerpts();
    void clearExcerpts();
    
    void printRecentSearches();
    void printExcerpt();
private:
    unordered_map<string, vector<uint32_t>> keywordMap;
    unordered_map<string, vector<uint32_t>> categoryMap;
    //unordered_map<uint32_t, uint32_t> indexID;
    vector<LogEntry> logVec;
    vector<uint32_t> searchedLogs;
    deque<uint32_t> excerpts;
    bool hasSearched = false;
};

// time converter - string to num
int64_t LogMan::timeStrToInt(string& str_time) {
    int64_t num_time = 0;
    string str_time_temp = str_time.substr(0, 2) + str_time.substr(3, 2) + str_time.substr(6, 2) + str_time.substr(9, 2) + str_time.substr(12, 2);
    num_time = stoll(str_time_temp);
    return num_time;
}

//string LogMan::timeIntToStr(int64_t& int_time){
//    string str_time;
//    str_time =
//}

// converts string to all lowercase
void LogMan::toLower(string& str) {
    //::tolower syntax from geeks
    transform(str.begin(), str.end(), str.begin(), ::tolower);
}

// initializes categoryMap and keywordMap
void LogMan::initializer() {
    string info;
    string word = "";
    int vecID = 0;
    
    //make range based for loop?? TODO
    for(vector<LogEntry>::iterator it = logVec.begin(); it != logVec.end(); ++it) {
            //initialize and sort the master data by category
            info = (*it).ogCategory;
            toLower(info);
            categoryMap[info].push_back((unsigned int)vecID);

            //initialize the master data with keywords contained
            info = (*it).ogCategory;
            info.push_back(' ');
            info.append((*it).message);
            toLower(info);

            for(string::iterator it = info.begin(); it != info.end(); ++it) {

                while(isalnum(*it) != 0 && it != info.end()) {
                    word.push_back(*it);
                    ++it;
                }

                if(!word.empty()) {
                    keywordMap[word].push_back((unsigned int)vecID);
                    word.clear();
                }

                if(it == info.end()) {
                    break;
                }
            }

            vecID++;
    }
}

// prints log in correct format: id|timestamp|category|message
void LogMan::logPrint(uint32_t i) {
    cout << logVec[i].entryID << "|" << logVec[i].strTimestamp << "|" << logVec[i].ogCategory << "|" << logVec[i].message << "\n";
}


// reads in info from file
bool LogMan::input_file(string& filename) {
    string word;
    ifstream input_file(filename);
    getline(input_file, word);
    if (!input_file.is_open()) {
        return false;
    }
    uint32_t i = 0;
    while(!word.empty()) {
        long end = (long)word.find_last_of("|");
        LogEntry temp_log(i++, word.substr(0, 14), word.substr(15, (unsigned long)(end - 15)),  word.substr((unsigned long)end + 1));
        temp_log.longTimestamp = timeStrToInt(temp_log.strTimestamp);
        logVec.push_back(temp_log);
        getline(input_file, word);
    }
    
    input_file.close();
    cout << i << " entries read\n";

    sort(logVec.begin(), logVec.end(), logCompare);

    initializer();

    return true;
}

//creates start and end iters and pushes_back the entries inbetween the two
//prints out number of logs found
void LogMan::timestampHelper(LogEntry &log1, LogEntry &log2, string print){
    vector<LogEntry>::iterator first = lower_bound(logVec.begin(), logVec.end(), log1, LogEntry());
    if (first == logVec.end()) {
        cout << print << " search: 0 entries found\n";
        return;
    }
    vector<LogEntry>::iterator last = upper_bound(logVec.begin(), logVec.end(), log2, LogEntry());
    
    while (first != last) {
        searchedLogs.push_back(uint32_t(first - logVec.begin()));
        ++first;
    }
    cout << print << " search: " << searchedLogs.size() << " entries found\n";
}

void LogMan::timestampSearch(){
    hasSearched = true;
    searchedLogs.clear();
    //read in timestamps - no space, so separate start and end later
    string unsplitTimestamps;
    cin >> unsplitTimestamps;
    
    //if timestamps don't add up to 29 (still has colons so 14 * 2 + 1 for the |)
    if (unsplitTimestamps.length() != 29) {
        cerr << "Error: incorrect timestamp formatting\n";
        return;
    }
    
    LogEntry timestamp1;
    LogEntry timestamp2;
    
    //split timestamp, convert from str to long and change the log entries timestamp to temp_time
    string temp_time = unsplitTimestamps.substr(0, 14);
    timestamp1.longTimestamp = timeStrToInt(temp_time);
    temp_time = unsplitTimestamps.substr(15, 14);
    timestamp2.longTimestamp = timeStrToInt(temp_time);
    
    timestampHelper(timestamp1, timestamp2, "Timestamps");
}

void LogMan::matchingSearch(){
    hasSearched = true;
    searchedLogs.clear();
    //same logic for almost everything in t search, need iter in case multiple w/ same timestamp
    string time;
    cin >> time;
    
    //if timestamps don't add up to 14 (still has colons)
    if (time.length() != 14) {
        cerr << "Error: incorrect timestamp formatting\n";
        return;
    }
    
    LogEntry time_log;
    time_log.longTimestamp = timeStrToInt(time);
    timestampHelper(time_log, time_log, "Timestamp");
}

void LogMan::categorySearch(){
    hasSearched = true;
    searchedLogs.clear();
    
    string cSearch;
    
    //takes in everything in, not like keyword, check spec if breaks again
    getline(cin, cSearch);
    //lowercase needs to be done before delete, do not change
    toLower(cSearch);
    cSearch.erase(0,1);
    unordered_map<string, vector<uint32_t>>::iterator it = categoryMap.find(cSearch);

    if (it != categoryMap.end()) {
        vector<uint32_t> &temp_vec = it->second;
        for (uint32_t i : temp_vec) {
            searchedLogs.push_back(i);
        }
    }
    cout << "Category search: " << searchedLogs.size() << " entries found\n";
}

void LogMan::keywordSearch(){
    searchedLogs.clear();
    hasSearched = true;
    
    vector<uint32_t> final_vec;
    string log_message;
    string word;
    getline(cin, log_message);
    transform(log_message.begin(), log_message.end(), log_message.begin(), ::tolower);

    bool first = true;

    //make range based for loop?? TODO
    for(string::iterator it = log_message.begin(); it != log_message.end(); ++it) {
        //separate keywords
        while(isalnum(*it) != 0 && it != log_message.end()) {
            word.push_back(*it);
            ++it;
        }
        
        
        if(!word.empty()) {
            if(first) {
                searchedLogs = keywordMap[word];
                //erase duplicates
                vector<uint32_t>::iterator last = unique(searchedLogs.begin(), searchedLogs.end());
                searchedLogs.erase(last, searchedLogs.end());
                first = false;
            }

            else {
                set_intersection(searchedLogs.begin(), searchedLogs.end(), keywordMap[word].begin(), keywordMap[word].end(), back_inserter(final_vec));
                searchedLogs = final_vec;
                final_vec.clear();
            }
            //new keyword next round
            word.clear();
        }

        if(it == log_message.end() || (!first && searchedLogs.empty())) {
            break;
        }
    }
    cout << "Keyword search: " << searchedLogs.size() << " entries found" << '\n';
}

void LogMan::appendEntry(){
    int id;
    cin >> id;
    
    if (((unsigned long)id) >= logVec.size() || id < 0) {
        cerr << "Error: <integer> isn't a valid position1./n";
        return;
    }
    
    //to get rid of IndexID
    for(int i = 0; i < int (logVec.size()); ++i) {
        unsigned long temp_i = (unsigned long) i;
        if(logVec[temp_i].entryID == (uint32_t)id) {
            excerpts.push_back((unsigned int)i);
            break;
        }
    }

    cout << "log entry " << id << " appended\n";
}

void LogMan::appendSearchResults(){
    if (!hasSearched) {
        cerr << "Error: no search has occured.\n";
        return;
    }
    
    //all logs found by most recent search - searchedLogs - gets appended to excerpts (sorted order)
    for (uint32_t i : searchedLogs) {
        excerpts.push_back((unsigned int)i);
    }
    
    cout << searchedLogs.size() << " log entries appended\n";
}

void LogMan::deleteLogEntry(){
    int i = 0;
    cin >> i;
    
    if (((unsigned long)i) >= (excerpts.size()) || i < 0) {
        cerr << "Error: <integer> isn't a valid position2./n";
        return;
    }
    deque<uint32_t>::iterator it = excerpts.begin();
    excerpts.erase(it + i);
    cout << "Deleted excerpt list entry " << i << "\n";
}

void LogMan::moveToBegin(){
    int i = 0;
    cin >> i;

    if (((unsigned long)i) >= (excerpts.size()) || i < 0) {
        cerr << "Error: <integer> isn't a valid position3./n";
        return;
    }
    
    //if not already at beginning
    if (!(i == 0)) {
        excerpts.push_front(excerpts[(unsigned long)i]);
        excerpts.erase(excerpts.begin() + i + 1);
    }
    
    cout << "Moved excerpt list entry " << i << "\n";
    
}
void LogMan::moveToEnd(){
    int i = 0;
    cin >> i;
    if (((unsigned long)i) >= excerpts.size() || i < 0) {
        cerr << "Error: <integer> isn't a valid position4./n";
        return;
    }
    
    //if not already at end
    if (!(((unsigned int) i) == (excerpts.size() - 1))) {
        uint32_t temp = excerpts[(unsigned long)i];
        //can't figure out type
        auto it = excerpts.begin();
        excerpts.erase(it + i);
        excerpts.push_back(temp);
    }
    cout << "Moved excerpt list entry " << i << "\n";
}

void LogMan::sortExcerpts(){
    cout << "excerpt list sorted\n";
    if (excerpts.empty()) {
        cout << "(previously empty)\n";
        return;
    }
    
    //pre-sort
    cout << "previous ordering:\n";
    //first entry
    cout << "0|";
    logPrint(excerpts[0]);
    cout << "...\n";
    //last entry
    cout << excerpts.size() - 1 << "|";
    logPrint(excerpts[excerpts.size() - 1]);
    
    //actual sort
    sort(excerpts.begin(), excerpts.end());
    
    cout << "new ordering:\n";
    //first entry
    cout << "0|";
    logPrint(excerpts[0]);
    cout << "...\n";
    //last entry
    cout << excerpts.size() - 1 << "|";
    logPrint(excerpts[excerpts.size() - 1]);
}

void LogMan::clearExcerpts(){
    cout << "excerpt list cleared\n";
    if (excerpts.empty()) {
        cout << "(previously empty)\n";
        return;
    }
    
    cout << "previous contents:\n0|";
    logPrint(excerpts[0]);
    cout << "...\n";
    cout << excerpts.size() - 1 << "|";
    logPrint(excerpts[excerpts.size() - 1]);
    excerpts.clear();
}

void LogMan::printRecentSearches(){
    if (!hasSearched) {
        cerr << "Error: no search has occured.\n";
        return;
    }
    //uses goes through all the logs in searchedLogs and prints them using ids
    for (uint32_t entryID : searchedLogs) {
        logPrint(entryID);
    }
}
void LogMan::printExcerpt(){
    uint32_t count = 0;
    //similar to recent searches but prints with excerpt index so needs a counter
    for (uint32_t entryID : excerpts) {
        cout << count << "|";
        logPrint(entryID);
        ++count;
    }
}


void LogMan::commands(){
    char command;
    bool quit = false;
    while (cin >> command && !quit) {
        cout << "% ";
        switch (command) {
            case 't':
            {
                timestampSearch();
                break;
            }
            case 'm':
            {
                matchingSearch();
                break;
            }
            case 'c':
            {
                categorySearch();
                break;
            }
            case 'k':
            {
                keywordSearch();
                break;
            }
            case 'a':
            {
                appendEntry();
                break;
            }
            case 'r':
            {
                appendSearchResults();
                break;
            }
            case 'd':
            {
                deleteLogEntry();
                break;
            }
            case 'b':
            {
                moveToBegin();
                break;
            }
            case 'e':
            {
                moveToEnd();
                break;
            }
            case 's':
            {
                sortExcerpts();
                break;
            }
            case 'l':
            {
                clearExcerpts();
                break;
            }
            case 'g':
            {
                printRecentSearches();
                break;
            }
            case 'p':
            {
                printExcerpt();
                break;
            }
            case '#':
            {
                string temp;
                getline(cin, temp);
                break;
            }
            case 'q':
            {
                quit = true;
                break;
            }
            default:
            {
                cerr << "Error: invalid command\n";
                break;
            }
        }
    }
}


int main(int argc, char* argv[]) {
    std::ios_base::sync_with_stdio(false);
    xcode_redirect(argc, argv);
    
    if (argc > 2){
        cerr << "Wrong number of arguments" << endl;
        exit(0);
    }
    
    string filename = argv[1];
    if (filename == "-h" || filename == "--help") {
        cout << "Takes in a txt log file\n";
        exit(1);
    }
    
    LogMan log;
    if (!log.input_file(filename)){
        cerr << "Error: Unable to read file" << endl;
        exit(0);
    }
    log.commands();
    return 0;
}
