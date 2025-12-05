#include <string>
#include <sys/stat.h>//for stat
#include <cstdlib> //for system which is required to run shell command inside cpp
const char* image_cleaner(const char* filepath){
    //check if exiftool is present.i decided to do this step in the wrapper during import.so it will only happen once.
    // int check=system("which exiftool > /dev/null 2>&1");//we only care about the return value.not the exact output text.that's why we are making the output disappear
    // if(check!=0){
    //     return "ERROR:EXIFTOOL_NOT_FOUND";
    // }
    //validates input
    if(filepath==NULL ||filepath[0]=='\0'){
        return "ERROR:FILE_NOT_FOUND";
    }
    //checks if file exists
    struct stat file_info; //this is where file information of 'filepath' will be stored.needed for stat().stat will return 0 for success and -1 for fialure
    if(stat(filepath,&file_info)!=0){
        return "ERROR:FILE_NOT_FOUND";
    }
    //generate the command
    std::string command = "exiftool -all= -overwrite_original \"" + std::string(filepath) + "\"";
    //run command
    int result=system(command.c_str());
    //check result and return
    if(result==0){
        return "SUCCESS";
    }else{
        return "ERROR:CLEANING_FAILED";
    }
}