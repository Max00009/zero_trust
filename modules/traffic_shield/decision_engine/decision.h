//this is header file which will contain all function signature.that way my cpp code will be cleaner.
#ifndef DECISION_H
#define DECISION_H
#include <stddef.h>
#ifdef __cplusplus
extern "C"{
#endif

bool decision_making(size_t threat_intelligence_score); //will take data from gateway.cpp and then decide result and call update_hashtable to add the key,pair value.
#ifdef __cplusplus
}
#endif
#endif