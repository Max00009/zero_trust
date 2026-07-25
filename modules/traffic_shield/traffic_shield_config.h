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
#include <cstdio> //for std::fopen()
#include <string>
#include <dlfcn.h> //for dladdr() which finds path of current .so file
#include <filesystem> //for filepath manipulation 

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
inline void load_config(const std::string& filepath="config.env"){

}