#include "decision.h"
#include <iostream>
#include <unordered_map>
#include <string>
#include <mutex>

static std::unordered_map<size_t,bool> result_hashtable;
static std::mutex result_hashtable_mutex;
/*
we will create an unordered map
gateway.cpp.update_hashtable() will directly insert result for alread cached url.
gateway.cpp.decision_making() will insert result via score comparing logic.
entry_point.py.get_decision() will earse that node from hashtable.
*/
bool get_decision(size_t url_id){
    //we will extract the whole node from result_hashtable.so that after we fetch the decision that key,pair value is gone.
    //we need -c=17++ for this extract.
    {   
        std::unique_lock<std::mutex> lock(result_hashtable_mutex);
        auto node=result_hashtable.extract(url_id);
        if (!node.empty()){
            return node.mapped();
        }
    }
}
void decision_making(size_t url_id,size_t threat_intelligence_score){
    //logic for decision.
    bool decision=(threat_intelligence_score>(size_t)7)?true:false;
    update_hashtable(url_id,decision);
}
void update_hashtable(size_t url_id,bool decision){
    //we have to use try_emplace() to insert key value pair.
    //cause it avoids copying so it's efficient.
    //and also if the insertion fails(for example if the key already exists) it doesn't move the arguments.so it's also safe.
    {
        std::unique_lock<std::mutex> lock(result_hashtable_mutex);  
        result_hashtable.try_emplace(url_id,decision); //insert the id and result.
    }
}
