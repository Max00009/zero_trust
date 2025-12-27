#include <string>
#include <sys/stat.h>//for stat
#include <cstdlib> //for system which is required to run shell command inside cpp
extern "C"{
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
    //command1 will erase whatever exiftool can remove
    std::string command_to_remove_metadata= "exiftool -all= -overwrite_original \"" + std::string(filepath) + "\" 2>&1";
    //command2 will zero out remaining file structure metadata.
    //well turns out i can't chanage FileAccessDate and FileInodeChangeDate using exiftool.
    std::string command_to_change_internal_metadata="exiftool "
                                                    "-CreateDate='1970:01:01 00:00:00' "
                                                    "-ModifyDate='1970:01:01 00:00:00' "
                                                    "-TrackCreateDate='1970:01:01 00:00:00' "
                                                    "-TrackModifyDate='1970:01:01 00:00:00' "
                                                    "-MediaCreateDate='1970:01:01 00:00:00' "
                                                    "-MediaModifyDate='1970:01:01 00:00:00' "
                                                    "-overwrite_original \"" + std::string(filepath) + "\" 2>&1";
    std::string command_to_change_filesystem_metadata= "exiftool "
                                    "-FileModifyDate='1970:01:01 00:00:00' "
                                    "-overwrite_original \"" + std::string(filepath) + "\" 2>&1";
    //run command
    int result1=system(command_to_remove_metadata.c_str());
    int result3=system(command_to_change_internal_metadata.c_str());
    int result2=system(command_to_change_filesystem_metadata.c_str());//at last
    //check result and return
    if(result1==0 && result2==0 && result3==0){
        return "SUCCESS";
    }else if(result1!=0){
        return "ERROR:EXIFTOOL_FAILED";
    }else if(result2!=0){
        return "ERROR:FILESYSYSTEM_METADATA_CLEANING_FAILED";
    }else if(result3!=0){
        return "ERROR:INTERNAL_METADATA_CLEANING_FAILED";
    }
    return "ERROR:UNKNOWN_ERROR";
}
}