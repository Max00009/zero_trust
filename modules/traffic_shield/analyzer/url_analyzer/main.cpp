#include "url_parser.h"
#include <string>
//Static Analysis
//Heuristic Analysis

int url_analizer(std::string url){ //we receive a std::string.
    //first we hwave to parse the url via the public parse() function.
    ParsedURL result=URLParser::parse(url);
}