/*okay so here is the deal.
till now I had 'config.py' file and our entry_point.py was fetching config values from it.
but there was two issues because only python can read values directly from config.py:
    1. I had to create a global variable in subsequent cpp files.then in the entry_point.py i had to pass the config value as an argument
    before assigning that value to that global variable.all this hassle just to use that config value in the cpp code.
    2. another issue is I need some config values inside cpp code that is not called from our entry_point.py.so how can we pass the config value in that cpp code?


so I decided to switch from config.py to config.env cause unlike config.py which is just a python file and can be read only by python code
config.env is just a text file.any language can read it-->then load in the process environment-->and then later get the value from process environment table.
what is process environment?
Every running program (process) has a built-in key-value store maintained by the operating system called the process environment. It's separate from any file, separate from any language.

so there are two distinct stages- 
    stage 1: we load the key=value pairs into the process environment.
    stage 2: we fetch values from that process environmnet table.
different language do these steps differently.

for example,

[python]
    stage 1
        load_dotenv("config.env")
        # reads config.env file
        # puts each KEY=VALUE into os.environ
        # which IS the process environment table
    stage 2
        thread_count=os.environ.get("MAX_THREAD_COUNT")
        # fetch value of MAX_THREAD_COUNT from the process environment table

[cpp]
    stage 1
        via setenv() it loads the KEY=VALUE pair in process environment table
    stage 2
        via std::getenv() it fetch value from the process environment table

okay so first I thought in the entry_point.py file I will use load_dotenv() to load the config.env file(to be accurate read the config.env file and load key=value pair) in the process environment table
every .so file loaded by Python inherits the same process environment. They all see the same variables. No special setup needed in any of them.
and now that the process environment table is set we can fetch those values in any files using `os.environ.get()`(if it's python) and `std::getenv()` (if it's cpp)

but that has two issues:
    1. in that scenario we have to make sure that load_dotenv() always runs before any cpp shared library(.so) is loaded using ctypes.CDLL()
    because those shared library is nothing but compiled cpp code where we will use std::getenv() to fecth values from process environmnet table.
    so if the process environment table is not loaded yet then it will crash.However it's not a critical issue cause i can just make sure that it's loaded before '.so' files.
    2. the critical issue is-during testing I might have to run the cpp files solo/individually.in that case the env file will not be loaded cause the entry_point.py is not being ran.
    and as we are trying to fetch values without loading them in process environment table our program will crash.

so I think I have to do something hybrid.I will load env file at the beginning of entry_point.py file 
but I will also load them in my individual cpp code with a parameter(0 as third argument in setenv()) to prevent overwriting in case our python code has already loaded the values in the table.
I don't know right now if I should also do something for other way around like if cpp has loaded the values later other file don't overwrite it.
*/

//in cpp we need to create a custom function to load the KEY=VALUE pair from the env file in the process environment table.cause there is no direct method like load_dotenv() in cpp.
//here I will keep those functions.I am keeping this at traffic_shield/ directory cause multiple files within this module will call this functions.


#pragma once
#include <fstream> //for std::ifstream()
#include <string>
#include <dlfcn.h> //for dladdr() which finds path of current .so file
#include <filesystem> //for filepath manipulation
#include <iostream>
#include <cstdlib> //for setenv() and getenv()
#include <cctype> //for std::isdigit
#include "analyzer/utils/utils.h" //I encountered a problem with this include.let me explain it below.
//the above include filepath is relative to the calling code.for example if example/foo.cpp is calling the get_config_bool() which requires is_same() function from utils.h
//the compiler will look into example/analyzer/utils/utils.h which doesn't exist.
//the simplest solution is to add the -I flag in MAkefile of foo.cpp with absolute path of traffic_shield/ where compiler can find our .h file
//-I flag tells the compiler "When resolving #include paths, also look in this directory"
//So now when compiler sees #include "analyzer/utils/utils.h", it searches:
//1. current file's directory     → example/analyzer/utils/utils.h  NOT FOUND
//2. traffic_shield/ (from -I)    → traffic_shield/analyzer/utils/utils.h  FOUND

/*
after solving that problem another doubt came to my mind.could we just use the same -I flag to point traffic_shield/ directory where traffic_shield_config.env lives instead of creating the hectic find_config_env() function?
the answer is no cause these two are different problems.
.env file is needed at runtime.compiler has nothing to do with it cause the compilation is already done.
.h file is needed at compile time when compiler is building our code and it needs to find header files.so -I flag tells the compiler where to look for them.
when our program is running it has no idea what MAX_THREAD_COUNT should be — that value was never in our source code. It's a configuration decision that can change without recompiling.
it needs to read the actual value from somewhere. That somewhere is the .env file.
compiler doesn't read the env file.it justs reads our source code,finds header files and translates our code into machine code.
env file is read by our running program.missing env file will cause runtime error.any change to env file doesn't require recompilation.
*/




//first we need to find the config.env file respective to the file where we calling load_config()
//because remember config.env will be relative to where the program runs from,not relative to where config.h lives
//different cpp files might call load_config() function and for different files the relative path will be different.so we have to take a dynamic approach.
//we will create a finction that will dynamically build the relative path between calling .so file(cause our calling cpp file will eventually be compiled into .so file) and the traffic_shield_config.env file
inline std::string find_config_env(){
    Dl_info info; //dl_info is a datatype.It is used to store symbolic information about a memory address when calling the dladdr() function.
    if(!dladdr((void*)find_config_env,&info) || !info.dli_fname) return ""; //this line calls the dladdr() function to populate info struct and also runs a success check.
    //breakdown of this part `dladdr((void*)find_config_env,&info)`
    //dladdr takes any address in memory and tells us which shared library file that address belongs to and where that library is on disk.
    //so we have to give it any address that belongs to our target so file.one way to do that is give address of any function defined in that so file.
    //we can give address of any function but here we are giving address of find_config_env.
    //NOTE there is no () after find_config_env cause in C++, a function name without () is not a function call — it's the address of that function in memory.and that's what we need.
    //dladdr expects const void* — a generic pointer type. But find_config_env is a function pointer, which has its own specific type.that's why we need to typecast it to (void*).
    //and we will also pass the info variable(which is dl_info datatype) by reference where dlladdr will store the output information and later we can extract what we need from there.

    std::filesystem::path so_file_path{info.dli_fname}; //path to current so file.we are just initializing so_file_path variable.
    std::filesystem::path so_file_dir=so_file_path.parent_path(); //directory of current so file

    //now we have to search upwards directory for 'traffic_chield_config.env' file.we know for sure that our config.env file will be at the root directory of respective module.
    //so we will search each directory and if not found move to parent directory and search there.but we have to makesure we don't beyond up the root directory of our current module.
    std::filesystem::path curr_dir=so_file_dir;
    while (true){
        //check for traffic_shield_config.env in current directory
        std::filesystem::path target_file_path = curr_dir / "traffic_shield_config.env";
        if (std::filesystem::exists(target_file_path)){
            //that means we found it and we just need to return the filepath string
            return target_file_path.string();
        }
        //if not found first check if we have reached the root of our MODULE cause we don't want to move past that
        if (curr_dir.filename()=="traffic_shield") return ""; //filename() will return the last component of path.

        //if somehow traffic_shield check fails somehow,we need another check to prevent infinite looping.
        std::filesystem::path parent=curr_dir.parent_path();
        if (parent==curr_dir) return ""; //cause caling parent_path() on '/' will return '/' itself.

        //now move one directory up
        curr_dir=parent;
    }
}


//if you are wondering why I used inline I have already explained it in url_analyzer/utils/utils.h file
inline void load_config(){
    std::string filepath=find_config_env();
    if (filepath.empty()){ //if find_config_env() returns empty string
        std::cerr<< "[config] ERROR: could not find traffic_shield_config.env.Make sure it exists in the traffic_shield root directory.\n";
        return;
    }

    //now let's open that file
    std::ifstream file(filepath); //in one line - creates an object of std::ifstream class.calls constructor with filepath argument.constructor automatically opens the file.
    if(!file.is_open()){
        std::cerr<<"[config] ERROR: failed to open"<<filepath<<"file"<<std::endl;
        return;
    }

    std::string line;
    while (std::getline(file,line)){
        //we need to skip empty lines and comments
        if (line.empty() || line[0]=='#') continue;
        //we also have to skip inline comments i.e. where comment starts at middle of a line.e.g CACHE_TIME_TO_LIVE=604800 #in seconds
        auto comment=line.find('#');
        if (comment!=std::string::npos){
            line=line.substr(0,comment);
        }
        //after filtering out all comments now we have to find '=' inside those remaining line
        auto eq=line.find('=');
        if (eq==std::string::npos) continue; //if there is a line which is not comment but also doesn't contain '=' then just skip that

        //now extract the key,value pair
        std::string key=line.substr(0,eq);
        std::string value=line.substr(eq+1); //no length indicates take everything remaining

        //now we need to trim whitespaces from key and value.for that we will create a lambda function
        auto trim=[](std::string& s){
            size_t start=s.find_first_not_of(" \t\r\n"); //we are looking for first char that is not space,tabs,carriage return or new line.look carefully ' '(space) is present at the beginning.
            size_t end=s.find_last_not_of(" \t\r\n"); //we are looking for last char that is not space,tabs,carriage return or new line.look carefully ' '(space) is present at the beginning.
            s=(start==std::string::npos)?"":s.substr(start,end-start+1); //if we don't find the start non-whitespace character then we just take an empty string as result.in next step we will check if it's empty
        };
        trim(key);
        trim(value);

        //check if key or value is empty string
        if (key.empty()) continue; //if key is empty then it's always invalid.
        //incase of empty value that might be meaningful i.e. the settings exist but has no value. however in my config file every key needs to have a value.
        //so I will also check if value is empty and if it is then give an error.
        //we can later fallback to default value in case value is empty but I don't like the idea of hardcoding default values in case of each key.
        if (value.empty()){
            std::cerr<<"[config] ERROR: No value found for "<<key<<" parameter.Set the value in traffic_shield_config.env file"<<std::endl;
            continue; //we continue with next key
        }

        //now that our key,value pair is reday we have to load it in process environment
        //setenv() strictly expects const char* as first two arguments:int setenv(const char *name, const char *value, int overwrite).that's why we have to use c_str() member function.
        if((setenv(key.c_str(),value.c_str(),0))!=0){//0 means don't overwrite if already loaded
            std::cerr<<"[config] ERROR: Failed to load "<<key<<"="<<value<<" in process environment"<<std::endl;
            continue;
        }
    }
}

//here we will define helper fucntions to read a value from process environment table.
//NOTE:in load_config we continued with next line when the value was empty or it failed to load the key=pair value in process environment table.
//      that's fine but in these helper functions we have to enforce the "must have a value" rule.otherwise the main function where the value is being fetched will crash.
//      so in these functions if we don't find a key or if the key has no value we will exit loudly.


//to check if all digits
//we are going to use atoi().but there is a problem.atoi() returns 0 silently incase of non-numeric value like "abc".
//atoi will also return 0 if the value is literally 0.
//so we have to check if all the digits are numerical.if one non-numerical found we will just throw error and exit.
inline bool is_all_digits(const char* s){
    bool has_negative_at_start=false;
    for (size_t i=0;s[i]!='\0';i++){
        if (i==0 && s[i]=='-'){
            has_negative_at_start=true;
            continue; //otherwise it will return false for '-' so negative values(maybe in future) will be discarded.
        }
        if (!std::isdigit(s[i])){
            return false;
        }
    }
    if (has_negative_at_start && s[1]=='\0') return false; //this will discard the only '-' case where no other digits after '-'
    return true;
}

//to get size_t config values
inline size_t get_config_size(const char* key){
    const char* value=std::getenv(key);
    if (!value || value[0]=='\0'){
        std::cerr<<"[config] ERROR: required config value '"<<key<<"' is missing or empty."<<std::endl;
        std::exit(1);
    }
    if (!is_all_digits(value)){
        std::cerr<<"[config] ERROR: '"<<key<<"' has a non-numerical value."<<std::endl;
        std::exit(1);
    }
    //this function returns size_t.if atoi returns a negative value typecasting that to size_t will turn it into large positive value.
    //so we have to discard negative value in this case.
    if (value[0]=='-'){
        std::cerr<<"[config] ERROR: '"<<key<<"' cannot have a negative value."<<std::endl;
        std::exit(1);
    }
    return static_cast<size_t>(std::atoi(value)); //atoi to change the string into int.and then we are typecasting to size_t.
}

//to get int config values
inline int get_config_int(const char* key){
    const char* value=std::getenv(key);
    if (!value || value[0]=='\0'){
        std::cerr<<"[config] ERROR: required config value '"<<key<<"' is missing or empty."<<std::endl;
        std::exit(1);
    }
    if (!is_all_digits(value)){
        std::cerr<<"[config] ERROR: '"<<key<<"' has a non-numerical value."<<std::endl;
        std::exit(1);
    }
    return std::atoi(value);
}

//to get bool values
inline bool get_config_bool(const char* key){
    const char* value=std::getenv(key);
    if (!value || value[0]=='\0'){
        std::cerr<<"[config] ERROR: required config value '"<<key<<"' is missing or empty."<<std::endl;
        std::exit(1);
    }
    std::string bool_value{value};
    return bool_value=="1" || is_same(bool_value,"true")||is_same(bool_value,"yes"); //to handle case insensitive case
}

//if later we need some other datatype value from config file we have to create another function for that.
