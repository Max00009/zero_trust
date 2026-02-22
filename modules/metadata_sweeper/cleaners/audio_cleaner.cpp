//if confused,read REDME.md
#include <string>
#include <sys/stat.h>//for stat
#include <cstdlib> //for system which is required to run shell command inside cpp
extern "C"{
const char* audio_cleaner(const char* filepath){
    //validates input
    if(filepath==NULL ||filepath[0]=='\0'){
        return "ERROR:INVALID_FILEPATH";
    }
    //checks if file exists
    struct stat file_info; //this is where file information of 'filepath' will be stored.needed for stat().stat will return 0 for success and -1 for fialure
    if(stat(filepath,&file_info)!=0){
        return "ERROR:FILE_NOT_FOUND";
    }
    //generate the command
    std::string command = "id3v2 -D \"" + std::string(filepath) + "\" 2>&1";
    //run command
    int result=system(command.c_str());
    //check result and return
    if(result==0){
        return "SUCCESS";
    }else{
        return "ERROR:CLEANING_FAILED";
    }
}
}