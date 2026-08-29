//this program will break our url into different parts so we can detect any anomaly.
//we will follow generic URL syntax defined in RFC 3986.
#include <regex>
#include <string>
#include <string_view>
#include <algorithm>
#include <vector>
#include <map>
#include <fstream>
#include <cctype> //for std::isspace
#include <unordered_set>
#include "../../traffic_shield_config.h"
#include "url_parser.h"
#include "../utils/utils.h" //for is_same() function

//let's load our config values
static const bool config_loaded=[](){
    load_config(); //we can call our function here.
    return true;
}(); //last () is to call our lambda function immediately

//get our config values
static const size_t MAX_URL_LENGTH=get_config_size("MAX_URL_LENGTH");
static const size_t MAX_HOSTNAME_LENGTH=get_config_size("MAX_HOSTNAME_LENGTH");
static const size_t MAX_LABEL_LENGTH=get_config_size("MAX_LABEL_LENGTH"); //will use it in domain_breakdown() function
static const size_t SUBDOMAIN_DEPTH=get_config_size("SUBDOMAIN_DEPTH");

//public function parse()
//btw I am using std::string_view as argument instead of std::string cause all the actions we will perform are read-only(except the trim i guess which I am gonna explain below).
//in that case std::string_view makes our code faster cause it doesn't required copying.
ParsedURL URLParser::parse(std::string_view raw_url){
    ParsedURL result; //first create an instance of ParsedURL object.
    result.original_url=raw_url; //keep an original copy.

    //at the very first we check if the url is suspiciously long
    if (raw_url.size()>MAX_URL_LENGTH){
        result.very_long_url=true;
        return result; //we don't want to waste work on this url
    }

    //now we will trim the raw_url.
    trim_url(raw_url);

    //now extract scheme
    set_scheme(raw_url,result);
    //if no scheme found then set_scheme will set 'parse_successfull=false'
    //and at that point further parsing doesn't make any sense.so we will terminate.
    if (!result.parse_successfull) return result;

    //now I will check if scheme is safe or not
    scheme_checker(result);

    //now check if host is present.
    detect_host(raw_url,result);
    // NOTE: IF HOST NOT PRESENT I HAVE TO HANDLE RELATIVE PATH CASES.

    //now extract credentials if present
    credential_extractor(raw_url,result);

    //now check if username contains any domain name looking content
    username_anomaly_checker(result);

    //now extract the host name and port
    host_extractor(raw_url,result);

    //check if hostname too long.if it is then we don't want to waste time
    if (result.very_long_hostname || result.malformed_host) return result;

    //domain brekdown if needed
    if (!result.full_host_without_port.empty() && result.is_domain_name) domain_breakdown(result);
    
    //check if domain name is malformed.if it is we don't want to waste time
    if (result.malformed_domain_name) return result;


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

    //first we will do a size check.if the size of scheme is much larger or smaller than our whitelist schemes range then we return early.
    //instead of hardcoding the size value I will iterate through the set and find the smallest size and biggest size.
    static const size_t min_len=std::min_element( //std::min_element will iterate through every element in our set and use our comparator function
        allowed_schemes.begin(), //start of set 
        allowed_schemes.end(), //end of set
        [](std::string_view a,std::string_view b){return a.size() < b.size() ; } //comparator lambda function.this function takes two std::string_view arguments,compare their size and return an iterator to the shorter one.
    )->size(); //this extracts the size from the iterator

    static const size_t max_len=std::max_element( //std::max_element will iterate through every element in our set and use our comparator function
        allowed_schemes.begin(), //start of set
        allowed_schemes.end(), //end of set 
        [](std::string_view a,std::string_view b){return a.size() < b.size() ;} //comparator lambda function.this function takes two std::string_view arguments,compare their size and return an iterator to the bigger one.
    )->size(); //this extracts the size from the iterator

    //now that we have our size range let's do size check.
    if (result.scheme.size()<min_len || result.scheme.size()>max_len) return; //return early.there is no point of comparing.

    //now we will do a case insensitive comparison so that 'http' and 'Http' both is passed as safe scheme.
    for (const auto& scheme:allowed_schemes){
        if (is_same(result.scheme,scheme)){ //we will use is_same function
            result.is_scheme_safe=true;
            break;
        }
    }
    //should I also make a bad_schemes{} set?I don't know yet.
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

void URLParser::host_extractor(std::string_view& raw_url,ParsedURL& result){
    //if there is no host then we return early
    if (!result.has_host) return;
    //now we have to set the host end boundary.hostname will end with any of these [/,?,#]
    //example:
    //https://example.com/path
    //https://example.com?query=1
    //https://google.com#section
    //so to set boundary we have to find first occuerence of any of these [/,?,#].if none found then we take the whole remaining part to search for.
    size_t pos=raw_url.find_first_of("/?#");
    size_t boundary_of_hostname=(pos==std::string_view::npos)?raw_url.size():pos;

    //first let's extract the full host with port(if present)
    result.full_host=raw_url.substr(0,boundary_of_hostname);

    //we will find first and last occuerence of ':'.if both returns same index that means we have only one ':' in our full host name.that means it can't be ipv6.
    //in that case we will treat everything after that as port.
    //if no ':' found then it can't be ipv6 and also no port specified.
    size_t first_colon=result.full_host.find(":");
    size_t last_colon=result.full_host.rfind(":");

    if (first_colon!=std::string_view::npos){ //that means colon present
        //now that we know colon is present let's see if it's multiple
        if (first_colon!=last_colon){ //that means multiple
            //now let's confirm if it's ipv6 by checking '[' and ']'
            if ((result.full_host.starts_with('[')) && result.full_host.rfind(']')!=std::string_view::npos){ //confirmed ipv6
                result.is_ip_address=true;
                result.is_ipv6=true;
                size_t pos_of_close_square_brac=result.full_host.rfind(']');
                result.full_host_without_port=result.full_host.substr(1,pos_of_close_square_brac-1); //leaving []
                host_length_checker(result);
                //this branch doesn't call detect_address_type() so we need to do a manual check if very_long_hostname so we can skip next if statement.
                if (result.very_long_hostname){
                    raw_url.remove_prefix(boundary_of_hostname);
                    return; //return early
                }
                //now let's see if port is present.first we will check if there is any remaining part after [] by comparing size
                if ((result.full_host.size()>(pos_of_close_square_brac+1))){
                    if ((pos_of_close_square_brac+1)==last_colon){ //we will check if the last colon is present after ']'.that way we can handle situation like [ipv6]:fake:port
                        std::string_view remaining_part=result.full_host.substr(last_colon+1);
                        port_extractor(remaining_part,result); //send the remaining part to port_extractor function
                    }else{
                        //that means there are more than one ':' left after ']' or there is no ':' after ']'. what should I do idk yet.maybe just mark it as malformed.
                        result.malformed_host=true;
                    }
                }
            }else{ //multiple colons but not ipv6->has to be malformed
                result.malformed_host=true;
            }
        }else{ //single colon. ipv4 with port or domain name with port is possible
            //take everything after ':' as port
            result.full_host_without_port=result.full_host.substr(0,first_colon);
            host_length_checker(result);
            std::string_view remaining_part=result.full_host.substr(first_colon+1);
            port_extractor(remaining_part,result);
            detect_address_type(result.full_host_without_port,result);
        }
    }else{ //no colon present.portless ipv4 or domain name is possible
        result.full_host_without_port=result.full_host;
        host_length_checker(result);
        detect_address_type(result.full_host_without_port,result);

    }

    
    //now move past [/,?,#]
    raw_url.remove_prefix(boundary_of_hostname);
    //notice we are not adding +1 to go past the delimeter.cause in next step we will need to see this delimeter to determine if next part is a file path(/) or parameter(?) or fragment(#)
    //when pos==npos-->boundary_of_hostname==raw_url.size() so we do raw_url.remove_prefix(raw_url.size());
    //when pos!=npos-->boundary_of_hostname==pos so we do raw_url.remove_prefix(pos) this keeps our delimeter which we can use.
}

void URLParser::host_length_checker(ParsedURL& result){ //it's purpose is to just set the flag that we can use later to return early.
    if (result.full_host_without_port.empty()) return;
    if (result.full_host_without_port.size() > MAX_HOSTNAME_LENGTH){
        result.very_long_hostname=true;
    }
}

void URLParser::detect_address_type(std::string_view full_host_without_port,ParsedURL& result){
    //first discard very long hostname
    if (result.very_long_hostname) return;
    //4 '.' +each within range 0-255 -> numeric ip
    //else domain_name
    //NOTE:how to handle the case where multiple dot's in sequence like example...com?after breaking it down there will be an empty octet.and is_all_digits() already returns false for empty element
    //so ipv4 will never be set in that case.however it will be set as is_domain_name.but later in breakdown_domain() function we will filter this thing and set malformed_host bool to true.
    if (full_host_without_port.find('.')!=std::string_view::npos){
        if (std::count(full_host_without_port.begin(),full_host_without_port.end(),'.')==3){ //maybe ipv4 also maybe some.example.com
            //we will brekdown split by '.' and collect in an array
            std::string_view octet[4]; //an array with 4 octects
            std::string_view full_host_without_port_copy=full_host_without_port; //we need a copy to iterate through the '.' and advance our pointer
            for (size_t i=0;i<4;i++){
                size_t pos=full_host_without_port_copy.find('.');
                if (i==3){
                    octet[i]=full_host_without_port_copy; //cause at that moment only last part will remain
                    break; //to avoid advancing
                }
                octet[i]=full_host_without_port_copy.substr(0,pos);
                full_host_without_port_copy.remove_prefix(pos+1);
            }
            //then for each part of array check if all_digits and within range.
            bool all_valid=true;
            for (auto c:octet){
                if (!is_all_digits(c)) {all_valid=false;break;}
                try{ //cause stoi can throw error in two cases that I have explained in port_extractor function
                    int val=std::stoi(std::string(c));
                    if (val<0 || val>255) {all_valid=false;break;}
                }catch(...){ all_valid=false;break;}
            }
            //if all pass that means confirm ipv4.otherwise we go for domain name
            if (all_valid){
                result.is_ip_address=true;
                result.is_ipv4=true;
            }else{
                result.is_domain_name=true;
            }
        }else{ //not ipv4 for sure
            result.is_domain_name=true;
        }
    }else{
        result.no_dots_in_host=true;
        if (is_all_digits(full_host_without_port)) result.no_dots_bare_ip_in_host=true;
    }
}

void URLParser::port_extractor(std::string_view remaining_part,ParsedURL& result){
    if (is_all_digits(remaining_part)){
        //std::stoi() throws error in two cases std::invalid_argument (e.g. std::stoi("12x3").it's not supposed to happen in our case cause we already calll is_all_digits())
        //and std::out_of_range (e.g std::stoi("99999999999") which is bigger than what int can hold.it might happen in our case)
        //we will catch both cases just for safety
        try {
            result.port=std::stoi(std::string(remaining_part));
            //after extracting the port we will do port range validation here.port analyzing will be at the analyzer but we need to accept only validated ports beforehand for safety.
            //during port analyzing we will just detect unusual ports but right now we will discard ports that are beyond range.
            if (result.port<1 || result.port>65535){
                result.out_of_ranged_port=true;
            }
        }catch(const std::invalid_argument&){ //we catch by reference
            result.malformed_port=true;
        }catch(const std::out_of_range&){
            result.out_of_ranged_port=true;
        }
    }
}

bool URLParser::is_all_digits(std::string_view remaining_part){
    if (remaining_part.empty()) return false;
    return std::all_of(remaining_part.begin(),remaining_part.end(),[](unsigned char ch){
        return ch>='0' && ch<='9'; //strictly checks if each char is only digits in range 0-9
    });
}

void URLParser::domain_breakdown(ParsedURL& result){
    //First I thought this time I can just load the tld_list.txt by using relative path cause it's in the same directory as this cpp file.
    //but our program will look into the directory from where we run our code.that's problem with relative path.
    //for example,if I am in /Users/max/zero_trust/zero_trust_code_base directory and run python3 entry_point.py it will start looking in /Users/max/zero_trust/zero_trust_code_base/ directory.
    //but if I am in /Users/max/zero_trust directory and run python3 zero_trust_code_base/entry_point.py it will look into /Users/max/zero_trust/ directory.
    //so we will load the tld_list.txt file dynamically at the beginning when our program runs.that's why static
    static const std::string tld_list_file_path=[]()->std::string{
        //we will use Dl_info just like we did in traffic_shield_config.h file to find the env file
        //but this time we won't have to travel upwards directories to find the file cause it will reside in same directory.
        Dl_info info;
        //we have to create a dummy lambda function to pass in dladdr function cause we can't pass a member function which results in undefined behaviour
        auto dummy_function=[](){};
        if (!dladdr((void*)&dummy_function,&info) || !info.dli_fname) return ""; //&dummy_function gives the memory adress of dummy_function which is actually a lambda object.(void*) casts that pointer to void* which dladdr expects   
        return (std::filesystem::path(info.dli_fname).parent_path()/"tld_list.txt").string(); //we don't need to include filesystem and dlfcn.h at the top cause traffic_shield_config.h already includes that
    }();
    static const std::unordered_set<std::string> tld_list=[](){
        std::unordered_set<std::string> set;
        std::ifstream file(tld_list_file_path);
        if (!file){
            std::cerr<<"[url_parser.cpp] Failed to open tld_list.txt file"<<std::endl;
            std::exit(1);
        }
        std::string line;
        while(std::getline(file,line)){
            if (line.empty() || line[0]=='#') continue;
            set.insert(line);
        }
        return set;
    }();

    std::string_view remaining=result.full_host_without_port; //make a copy cause we need to advance iterator
    while (!remaining.empty() && result.subdomains.size()<SUBDOMAIN_DEPTH){
        size_t dot_pos=remaining.find('.');
        std::string label;
        //extract the label
        if (dot_pos==std::string_view::npos){
            label=std::string(remaining);
            remaining={}; //we could've do break here but that will leave the while loop.but we need punycode check,lower case and emplace_back logic.
        }else{
            label=std::string(remaining.substr(0,dot_pos));
            remaining.remove_prefix(dot_pos+1);
        }
        //discard empty label.cause empty label indicates there were two dots in sequence("..")
        //also do label length check
        if (label.empty() || label.size()>MAX_LABEL_LENGTH){result.malformed_domain_name=true; return;}

        //punycode check
        if (label.starts_with("xn--")) {result.is_punnycode=true; } //first I returned early in case of punycode but later realized punycode is used in valid domain name.so we just flag it.analyzer will decide further.

        //convert to lowercase.cause domain names are case insensitive per RFC 4343
        //This approach ensures locale-independent behavior for standard ASCII while preventing crashes on platforms where char defaults to signed(we are explicitely casting c to unsigned char to prevent undefinded behaviour if the character has a negative binary value which is common in signed char systems)
        std::transform(label.begin(),label.end(),label.begin(),[](unsigned char c){return std::tolower(c);});
        
        //emplace_back is better than push_back
        result.subdomains.emplace_back(label);
    }
    size_t subdomain_vector_size=result.subdomains.size();
    if (subdomain_vector_size<2){//cause nothing to process here.
        if (subdomain_vector_size==1) result.tld=result.subdomains[0]; //is not strictly necessary i am just recording the value incase analyser wants to work with the tld
        result.malformed_domain_name=true; 
        return;
    }else{
        std::string two_part=result.subdomains[subdomain_vector_size-2]+"."+result.subdomains[subdomain_vector_size-1];
        if (tld_list.count(two_part)){
            if (subdomain_vector_size==2){
                result.malformed_domain_name=true; //cause it has two parts tld and nothing before it.
                return;
            }
            result.tld=two_part; //take the last two part as tld
            result.domain_label=result.subdomains[subdomain_vector_size-3]; //we take the part before tld as domain_label
        }else{
            result.tld=result.subdomains[subdomain_vector_size-1]; //we take the last part as tld
            if (!tld_list.count(result.tld)) result.unknown_tld=true; //we run the check on the last part only
            result.domain_label=result.subdomains[subdomain_vector_size-2]; //take the second last part as domain_label
        }
    }
    result.registered_domain=result.domain_label+"."+result.tld;
}