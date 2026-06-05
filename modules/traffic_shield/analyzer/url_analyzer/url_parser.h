#ifndef URL_PARSER_H
#define URL_PARSER_H
#include <string>
#include <string_view>
#include <map>
#include <vector>

//we will define a struct that will hold different parts and values of the broken URL.
struct ParsedURL{

    //the raw original url
    std::string_view original_url;

    //scheme
    std::string_view scheme;
    bool is_scheme_safe= false;

    //credentials
    bool has_credentials= false;
    std::string_view username;
    std::string_view password;

    //host
    std::string_view full_host; //e.g.  "mail.google.com"  (no port)
    std::string_view subdomain; //e.g.  "mail"
    std::string registered_domain; //e.g.  "google.com"   (eTLD+1).We have to use std::string here cause we will have to lowercase it.
    std::string_view domain_label;  //e.g.  google"   (name without TLD)
    std::string_view tld;   //e.g.  "com"  or  "co.uk"

    bool is_ip_address=false;
    int subdomain_depth=0;
    int port=-1;    //-1 means port not specified.

    //path
    std::string_view path;//e.g.    "path/to/file.exe"
    std::string decoded_path;  //url decoded path.We have to use std::string here cause we will modify the data.
    std::string_view file_extension;//e.g.  exe,pdf,bat,apk etc.
    std::vector<std::string_view> path_segments;//e.g.  ["path", "to", "file.exe"]

    //query
    std::string_view raw_query;
    std::string decoded_query; //url decoded.We have to use std::string here cause we will modify the data.
    std::map<std::string_view,std::string_view> params; //for (key->value) pairs of query.

    //fragment
    std::string_view fragment;  //amything after #

    //Anomaly flags
    bool has_null_bytes=false;  //if %00 anywhere.
    bool double_encoding=false; //if % is also encoded(%25xx).
    bool is_punnycode=false;   //xn-- label in host.exploited in Homograph attacks.
    bool has_at_in_host=false;  //if @ present in host name.old trick.

    //status
    bool parse_successfull=false;   //false if url is fundamentally malformed.

};

//here we will define a class which will contain various functions needed to parse url.
class URLParser{
public:
    // This is the public function that will be accessible to other files.
    // this function will call other helper functions that I will put in private cause outside function doesn't need access to it.
    static ParsedURL parse(std::string_view raw_url);//our parse function will return ParsedURL object.


private:
    //helper functions will live here

    //NOTE:explain how we will modify the string even tho we are taking std::string_view.
    static void trim_url(std::string_view& raw_url);//this function will remove all white-spaces from our url.

};

#endif