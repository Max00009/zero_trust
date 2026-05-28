//this is header file which will contain all function signature.that way my cpp code will be cleaner.
//we don't need any extern C here cause this function is not being called from any pyhton code.
#ifndef DECISION_H
#define DECISION_H
#include <cstddef>


bool decision_making(size_t threat_intelligence_score); //will take data from gateway.cpp and then decide result and call update_hashtable to add the key,pair value.

#endif