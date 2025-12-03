//this file uses libmagic to detect file type
#include <magic.h>//for libmagic
#include <string>//for string handling
#include <cstring>//for strncpy
#include <sys/stat.h>
#include "file_detector.h"

// Main function
const char* detect_file_type(const char* filepath) {
    //Check if file exists
    if (filepath==NULL||filepath[0]=='\0'){
        return "ERROR:FILE_NOT_FOUND";
    }
    struct stat file_info; //this is where file information of 'filepath' will be stored.needed for stat().stat will return 0 for success and -1 for fialure
    if(stat(filepath,&file_info)!=0){
        return "ERROR:FILE_NOT_FOUND";
    }

    //Initialize libmagic.this creates the empty magic_t handle
    magic_t magic=magic_open(MAGIC_MIME_TYPE);//returns the magic mime type e.g. image/jpeg
    if(magic==NULL){
        return "ERROR:UNKNOWN";
    }
    //Load magic database inisde that handle.NULL means default database at /usr/share/misc/magic.mgc
    if((magic_load(magic,NULL))!=0){
        magic_close(magic);
        return "ERROR:UNKNOWN";
    };
    //Detect file type
    const char* mime_type=magic_file(magic,filepath);//load the mime_type inside mime_type
    if(mime_type==NULL){
        magic_close(magic);
        return "ERROR:UNKNOWN";
    }
    //we can't return mime_type cause it's a pointer whose memory will be freed.so we will copy the contents 
    //and then return that.we will use static char cause normal variable dies after finction ends but static lives until program dies
    static char result[256];
    strncpy(result,mime_type,255);
    result[255]='\0';
    
    //close the hamdle
    magic_close(magic);

    return result;    
}