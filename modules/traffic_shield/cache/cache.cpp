#include "cache.h"
#include <iostream>
#include <string>
#include <unordered_map>    //for hashmap.
#include <ctime>    //to get time.
#include <mutex>
#include <cstdint>  //for int64_t
#include <vector>
#include "../traffic_shield_config.h"

//load the environments variabels via our lambda function
static const bool config_loaded = [](){
    load_config();
    return true;
}();
struct CacheUrl{
    std::string url;
    int threat_score;
    int64_t last_time_scanned; //the reason I am using int64_t(long long) for all time related parameter is because get_time will return long long and we have to do arithmatic between these.
    //these two are pointers for double-linked-list.these are required for LRU(least recently used).
    CacheUrl *perv;
    CacheUrl *next;
    std::vector<std::string> api_list;
};

//here i declare global variables that i need.
static std::mutex cache_mutex;
static std::unordered_map<std::string,CacheUrl*> hashtable; //<URL,pointer_to_cache_of_that_url>
static CacheUrl *prev=nullptr;
static CacheUrl *next=nullptr;
//initialize our config values
static const size_t MAX_CACHE_SIZE=get_config_size("MAX_CACHE_SIZE");
static const int64_t CACHE_TIME_TO_LIVE=get_config_int64_t("CACHE_TIME_TO_LIVE");

int64_t get_time(){
    return std::time(nullptr); //this returns time_t which on modern 64-bit systems is long long.
}

//LATER I HAVE TO WORK ON THIS cache_init() FUNCTION
int cache_init(){   //initialize an empty cache in RAM.
    //discard negative config value
    if (CACHE_TIME_TO_LIVE<=0){
    std::cerr<< "[config] ERROR: TIME_TO_LIVE must be positive."<<std::endl;
    std::exit(1);
    }
    //for safety we need to reset data structures.

    hashtable.clear();
    prev=nullptr;
    next=nullptr;

    return 0;
}
int cache_fetch(const std::string& url){ //as we are not going to modify or move ownership of the url,we are passing by reference instead of passing by value.No copy made.No memory allocation.
    std::lock_guard<std::mutex> lock(cache_mutex);  //lock the hashtable
    auto it=hashtable.find(url);    //hashtable.find() returns an iterator.
    if (it==hashtable.end()){
        return 0;
    }else{
        return it->second->threat_score;    //we extract the value from iterator.and then extract threat_score from the value.
    }
}
//int cache_insert(){}