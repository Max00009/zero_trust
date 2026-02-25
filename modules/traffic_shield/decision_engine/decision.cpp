#include "decision.h"

//i could havve move this part into gateway.cpp
//but i wanted to keep the decision making logic in a separate file cause maybe later we can expand our logic into advance mode.
bool decision_making(size_t threat_intelligence_score){
    //logic for decision.
    bool decision=(threat_intelligence_score>(size_t)7)?true:false;
}