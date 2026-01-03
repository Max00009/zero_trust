#include <string>
#include <sys/stat.h>//for stat
#include <cstdlib> //for system which is required to run shell command inside cpp
extern "C"{
const char* pdf_cleaner(const char* filepath,bool change_filesys_date,bool change_internal_date,bool replace_original){
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
    std::string replace_status=replace_original ?"-overwrite_original":"";
    //command1 will erase whatever exiftool can remove
    std::string command_to_remove_metadata= "exiftool -all= "+replace_status+" \"" + std::string(filepath) + "\" 2>&1";
    std::string command1="exiftool ";
    std::string command2="exiftool ";
    //as much i noticed .jpg will not contain any internal file date but just to be safe we can run this
    if(change_internal_date){
        command1+="-CreateDate='1970:01:01 00:00:00' "
                        "-ModifyDate='1970:01:01 00:00:00' "
                        "-TrackCreateDate='1970:01:01 00:00:00' "
                        "-TrackModifyDate='1970:01:01 00:00:00' "
                        "-MediaCreateDate='1970:01:01 00:00:00' "
                        "-MediaModifyDate='1970:01:01 00:00:00' ";
        command1+=replace_status+" \"" + std::string(filepath) + "\" 2>&1";
    }
    //now at the last the file_sys date
    if(change_filesys_date){
        command2+="-FileModifyDate='1970:01:01 00:00:00' ";
        command2+=replace_status+" \"" + std::string(filepath) + "\" 2>&1";
    }
    
    //run command
    int result1=system(command_to_remove_metadata.c_str());
    if(result1!=0){
        return "ERROR:EXIFTOOL_FAILED";
    }
    if(change_internal_date){
        int result2=system(command1.c_str());
        if(result2!=0){
            return "ERROR:INTERNAL_METADATA_CLEANING_FAILED";
        }
    }
    if(change_filesys_date){
        int result3=system(command2.c_str());
        if(result3!=0){
            return "ERROR:FILESYSYSTEM_METADATA_CLEANING_FAILED";
        }
    }
    return "SUCCESS";
}
}