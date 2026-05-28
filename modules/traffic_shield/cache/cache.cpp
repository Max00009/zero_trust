#include "cache.h"
#include <iostream>
#include <string>
#include <unordered_map>    //for hashmap.
#include <ctime>    //to get time.
#include <mutex>
#include <cstdint>  //for int64_t
#include <vector>
struct CacheUrl{
    std::string url;
    int threat_score;
    int64_t last_time_scanned;
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
static int max_cache_size=0;
static int64_t time_to_live=0;

int64_t get_time(){
    return std::time(nullptr);
}
int cache_init(int max_size,int64_t ttl){   //initialize an empty cache in RAM.
    //store configuration values.
    max_cache_size=max_size;
    time_to_live=ttl;
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