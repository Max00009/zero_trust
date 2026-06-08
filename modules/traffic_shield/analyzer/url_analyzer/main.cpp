#include "url_parser.h"
#include <string>
//Static Analysis
//Heuristic Analysis

bool url_analyzer(std::string url){ //we receive a std::string.
    //first let's do a safety check to see if the string is empty.
    if (url.empty()){
        return false;
    }
    //first we hwave to parse the url via the public parse() function.
    ParsedURL result=URLParser::parse(url);
}