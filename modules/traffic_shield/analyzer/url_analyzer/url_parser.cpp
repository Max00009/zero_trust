//this program will break our url into different parts so we can detect any anomaly.
//we will follow generic URL syntax defined in RFC 3986.
#include "url_parser.h"
#include <regex>
#include <string>
#include <string_view>
#include <algorithm> //for std::transform
#include <vector>
#include <map>
#include <cctype> //for std::tolower

//public function parse()
//btw I am using std::string_view as argument instead of std::string cause all the actions we will perform are read-only(except the trim i guess which I am gonna explain below).
//in that case std::string_view makes our code faster cause it doesn't required copying.
ParsedURL URLParser::parse(std::string_view raw_url){
    ParsedURL result;   //first create an instance of ParsedURL object.
    result.original_url=raw_url;
    result.parse_successfull=true; //always true unless found anomaly.
    //now we will trim the raw_url.
    trim_url(raw_url);

    //now extract scheme
    set_scheme(raw_url,result);
    //now check if the scheme is safe.access result.scheme
    //create a whitelist.and compare.
    //handle '://' and ':' case accordingly.

    return result;
}

//define the trim function
void URLParser::trim_url(std::string_view& url){
    while (!url.empty() && std::isspace(url.front())){ //just a double check on empty string.
        url.remove_prefix(1);
    }
    while (!url.empty() && std::isspace(url.back())){ //just a double check on empty string.
        url.remove_suffix(1);
    }
}

//NOT RELATED TO THIS CODE.IGNORE
//I just learnt that 'static' holds different purpose based on how is it used.
//if static is in front of a variable:"create this variable once and keep it alive for the entire program. Don't recreate it every time the function is called"
//if static is in front of a function inside a class:"This function does not depend on any object state. Give it an input, it gives you an output.We don't need to create an object to use this function"
//if static is in front of a free function:"this function only exists inside this file. No other file can see or call it."
//just put it here cause I don't know where to write this down.

//define get_scheme function
void URLParser::set_scheme(std::string_view& raw_url,ParsedURL& result){
    size_t pos=raw_url.find(':');
    if (pos!=std::string_view::npos && pos!=0){
        result.scheme=raw_url.substr(0,pos);
        //we could have convert our scheme to lowercase but that would require us to declare scheme as std::string cause we are modifying it.
        //However that will cause heap allocation that I don't want.
        //so we will keep our scheme as it is.and later during comparision will convert each character to lower on the fly.
        //remember converting each character on fly is possible with std::string_view cause that way we are not storing the modified version.just picking a char and lower casing to compare and delete.
        
        raw_url.remove_prefix(pos+1); //to advance the raw url after extracting.
    
    }else{
        result.parse_successfull=false;
    }
    
}