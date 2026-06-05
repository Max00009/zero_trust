//this program will break our url into different parts so we can detect any anomaly.
//we will follow generic URL syntax defined in RFC 3986.
#include "url_parser.h"
#include <regex>
#include <string>
#include <string_view>
#include <algorithm>
#include <vector>
#include <map>

//public function parse()
//btw I am using std::string_view as argument instead of std::string cause all the actions we will perform are read-only(except the trim i guess which I am gonna explain below).
//in that case std::string_view makes our code faster cause it doesn't required copying.
ParsedURL URLParser::parse(std::string_view raw_url){
    ParsedURL result;   //first create an instance of ParsedURL object.
    result.original_url=raw_url;
    result.parse_successfull=true; //always true unless found anomaly.
    //now we will trim the raw_url.
}

//define the trim function