//this program will break our url into different parts so we can detect any anomaly.
//we will follow generic URL syntax defined in RFC 3986.
#include "url_parser.h"
#include <regex>
#include <string>
#include <string_view>
#include <algorithm>
#include <vector>
#include <map>
#include <cctype> //for std::tolower
#include <unordered_set>

//public function parse()
//btw I am using std::string_view as argument instead of std::string cause all the actions we will perform are read-only(except the trim i guess which I am gonna explain below).
//in that case std::string_view makes our code faster cause it doesn't required copying.
ParsedURL URLParser::parse(std::string_view raw_url){
    ParsedURL result; //first create an instance of ParsedURL object.
    result.original_url=raw_url; //keep an original copy.
    //now we will trim the raw_url.
    trim_url(raw_url);

    //now extract scheme
    set_scheme(raw_url,result);
    //if no scheme found then set_scheme will set 'parse_successfull=false'
    //and at that point further parsing doesn't make any sense.so we will terminate.
    if (!result.parse_successfull){
        return result;
    }

    //now I will check if scheme is safe or not
    scheme_checker(result);


    //now check if host is present.
    detect_host(raw_url,result);
    // NOTE: IF HOST NOT PRESENT I HAVE TO HANDLE RELATIVE PATH CASES.

    //now extract credentials if present
    credential_extractor(raw_url,result);

    //now check if username contains any domain name looking content
    username_anomaly_checker(result);

    //NEXT TASK:extract host name

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

//define scheme_checker function
void URLParser::scheme_checker(ParsedURL& result){
    //first let's create a whitelist
    static const std::unordered_set<std::string_view> allowed_schemes={
        "http","https" //I will add other schemes later if needed
    };
    //should I also make a bad_schemes{} set?I don't know yet.
    //now check if the scheme is safe.access result.scheme
    if (allowed_schemes.count(result.scheme)){ //for unique keys count() will return 0 if not found.otherwise 1.
        result.is_scheme_safe=true;
    }
}

//define detect_host function
void URLParser::detect_host(std::string_view& raw_url,ParsedURL& result){
    //To check if our current state of raw_url starts with "//" I will use starts_with() member function.
    //C++20 is required.compile with -std=c++20 .
    if (raw_url.starts_with("//")){ //starts_with() automatically handles the case where the view is shorter than 2 characters.will return false in that case.
        //I have to handle an edge case where after scheme there is '///'.example: file:///home/user/document.pdf. 
        //in that case we will enter this if branch but then we have to check if next character is also '/' . if it is then host not present.
        //if not then there is only '//' after scheme and that means host present.
        if (raw_url.size()>2 && raw_url[2]=='/'){
            result.has_host=false;
        } //we don't need any else cause value of has_host is true by default.
        raw_url.remove_prefix(2); //advance after "//" .
    }else{
        result.has_host=false; //we enter this else branch when there is no '//'. so there is no question of host being present.
    };

}

//define credential_extractor function
void URLParser::credential_extractor(std::string_view& raw_url,ParsedURL& result){
    /*
    this is a basic structure of url with host: http://username:password@example.com/path
    to check if creds is present I will check if there is any '@' before next '/'.
    to do that first we have to locate next '/'.then look for '@' before it.
    but if we don't find any '/' then we will look for '@' in the whole remaining part anyway.e.g. "user:pass@mail.google.com". there is no '/'.
    here is a flowchart i am trying to build that shows possible outcomes:
                        
    look for next '/'---> if found then look for '@' before that '/'.if not found then look for '@' in the whole part.
    if we find '@'----> we extract everything before it as creds.if we don't find any '@' there ain't no creds and we do nothing.
    after extracting creds we look for ':'--->if found then we take the part before ':' as username and after ':' as password.if ':' not found then take everything as username.

    */

    //find position of next '/'
    size_t slash_pos=raw_url.find('/');
    if (slash_pos==0) return; //that means it's a '///' edge case.there will be no credentials.so no need to proceed further.
    
    //set the searching range for '@'
    size_t boundary_to_find_at_the=(slash_pos==std::string_view::npos)?raw_url.size():slash_pos;
    //search for '@'
    size_t at_the_pos=raw_url.substr(0,boundary_to_find_at_the).find('@');
    
    if (at_the_pos==std::string_view::npos){
        return; //cause no creds
    }else{
        result.has_at_in_host=true;
        if(at_the_pos!=0){
            result.has_credentials=true;
        }
        if (result.has_at_in_host && !result.has_credentials){
            result.blank_creds=true;
        }
        if (result.has_credentials){
            //the reason we are extracting username and password here instead of creating separate function is because after this function ends our raw_url will be pointed at next character after '@'.We will loose the creds.
            /*
            NOTE:while extracting credentials we have to look for any anomaly like domainname in place of credentials.
            like this classic attack: http://google.com@evil.com/login
            Here there are no real "credentials" — google.com is not a username. 
            But the browser sees @ and everything before it gets treated as credentials, so it actually navigates to evil.com. 
            The attacker is using @ to make you think you're going to google.com.
            */

            //first we will look for ':' inside credentials.
            size_t colon_pos=raw_url.substr(0,at_the_pos).find(':');
            //if no colon then we take everything as username and make 'blank_password=true'
            if (colon_pos==std::string_view::npos){
                result.username=raw_url.substr(0,at_the_pos);
                result.blank_password=true;
            }else{ //colon found
                if (colon_pos==0){ //if there is noting infront of colon
                    result.blank_username=true;
                    result.password=raw_url.substr(1,at_the_pos-1);
                }else{
                    result.username=raw_url.substr(0,colon_pos);
                    if ((at_the_pos-colon_pos)>1){ //cause if at_the_pos is just next of colon_pos that means there is no password
                        result.password=raw_url.substr(colon_pos+1,at_the_pos-colon_pos-1);
                    }else{
                        result.blank_password=true;
                    }
                }
            }
        }
        raw_url.remove_prefix(at_the_pos+1); // overtake the '@'
    }
}

//define username_anomaly_checker function
void URLParser::username_anomaly_checker(ParsedURL& result){
    //if we find '.' inside username then we flag it.
    if (result.username.find('.')!=std::string_view::npos){
        result.domain_as_username=true;
    }
}