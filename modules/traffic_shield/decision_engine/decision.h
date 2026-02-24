//this is header file which will contain all function signature.that way my cpp code will be cleaner.
#ifndef DECISION_H
#define DECISION_H
#include <stddef.h>
#ifdef __cplusplus
extern "C"{
#endif

bool get_decision(size_t url_id); //we extract the decision from hashtable and erase that node from hashtable before sending to entry_point.py
void decision_making(size_t url_id,size_t threat_intelligence_score); //will take data from gateway.cpp and then decide result and call update_hashtable to add the key,pair value.
void update_hashtable(size_t url_id,bool decision);//take data from decision_making and add that to hashtable.
#ifdef __cplusplus
}
#endif
#endif