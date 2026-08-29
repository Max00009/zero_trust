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
    bool has_credentials= false; // [username:password]@example.com  everything inside [] is credentials.
    std::string_view username;
    std::string_view password;

    //host
    bool has_host=true; //if host is present then '://' is used after scheme name.if host not present then only ':' used.http://[www.example.com]/path everything within [] is host.
    std::string_view full_host; //e.g.  "mail.google.com"
    std::string_view full_host_without_port; //we strip the port if present
    std::vector<std::string> subdomains; //will contain parsed subdomains
    std::string registered_domain; //e.g.  "google.com"   (eTLD+1).
    std::string domain_label;  //e.g.  google"   (name without TLD)
    std::string tld;   //e.g.  "com"  or  "co.uk"

    bool is_domain_name=false; //example.com
    bool is_ip_address=false;
    bool is_ipv4=false; //all digits and dots
    bool is_ipv6=false; //starts with '['
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
    bool very_long_url=false; //we can take a max length from config file and anything lengthier than that will be flagged.
    bool has_null_bytes=false;  //if %00 anywhere.
    bool double_encoding=false; //if % is also encoded(%25xx).
    bool is_punnycode=false;   //xn-- label in host.exploited in Homograph attacks.
    bool has_at_in_host=false;  //if @ present in host name.old trick.
    bool blank_creds=false; //'@' is present but there is blank before that.suspicious.
    bool blank_username=false; //if there is nothing infront of ':' then it's suspicious.
    bool blank_password=false; //i am just keeping a note of it.don't know yet if later i have to consider this.
    bool domain_as_username=false; //this classic attack: http://google.com@evil.com/login.The attacker is using @ to make us think it's going to google.com.
    bool very_long_hostname=false; //we can take a max length from config file and anything lengthier than that will be flagged.
    bool malformed_host=false; //for cases like multiple colons but no '[]' or multiple ':' left after ipv6 address
    bool out_of_ranged_port=false; //if port <0 or >65535
    bool malformed_port=false; //when port contains non-numerical value
    bool no_dots_in_host=false; //https://localhost/path or http://13143/path
    bool no_dots_bare_ip_in_host=false; //http://13143/path (more suspicious than https://localhost/path)
    bool malformed_domain_name=false; //example..com (sequencial dots)
    bool unknown_tld=false; //the tld doesn't match any tld in tld_list.just flagging it.we will still proceed.

    //status
    bool parse_successfull=true;   //false if url is fundamentally malformed.

};

//here we will define a class which will contain various functions needed to parse url.
class URLParser{
public:
    // This is the public function that will be accessible to other files.
    // this function will call other helper functions that I will put in private cause outside function doesn't need access to it.
    static ParsedURL parse(std::string_view raw_url);//our parse function will return ParsedURL object.


private:
    //NOTE:explaination of how we will modify the string even tho we are taking std::string_view.
    //we can't modify the characters inside our string cause std::string_view is read only.
    //HOwever we can change where it points and how long it is.so we can just move the boundaries of the view, not touching the original string.

    //helper functions will live here
    //passing by value is more efficient than passing by reference in case of std::string_view.then why are we passing by reference in most of these fucntions?
    //cause in those fucntions we are modifying by remove_prefix() to advance the cursor.so we need to pass by reference.
    //if we don't need to modify then we can just pass by value(e.g. is_all_digits() function.)
    static void trim_url(std::string_view& raw_url); //this function will remove all white-spaces from our url.We are passing by reference cause we don't want own copy.We want to trim the original copy.
    static void set_scheme(std::string_view& raw_url,ParsedURL& result); //this function will detect the first occurence of ':' and extract the scheme name.
    static void scheme_checker(ParsedURL& result); //this function will check if the scheme is safe and set 'is_scheme_safe' bool accordingly.
    static void detect_host(std::string_view& raw_url,ParsedURL& result); //this function will check if '//' is present after ':'.
    static void credential_extractor(std::string_view& raw_url,ParsedURL& result); //thsi function will check if any credentials present in our url.
    static void username_anomaly_checker(ParsedURL& result); //this function will check if the username mimics any domain name and set the value of domain_as_username bool.
    static void host_extractor(std::string_view& raw_url,ParsedURL& result); //this function will extract the hostname if present.
    static void host_length_checker(ParsedURL& result); //minimalist function to check if hostname exceeds the max limit.
    static void detect_address_type(std::string_view full_host_without_port,ParsedURL& result); //helper function of host_extractor function to check if ip is numeric like 10.48.32.95 or domain like example.com
    static void port_extractor(std::string_view remaining_part,ParsedURL& result); //this function will parse port
    static bool is_all_digits(std::string_view remaining_part); //helper function of port_extractor fucntion needed for port validation
    static void domain_breakdown(ParsedURL& result); //it will breakdown each path of the domain name and store it in a vector
};

#endif