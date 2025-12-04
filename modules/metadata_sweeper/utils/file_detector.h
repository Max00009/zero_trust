//i am keeping this header file in case in future inside other modules i want to directly call functions from 
//file_detector.cpp .but i might just use python wrapper to access the function.i am just keeping both options open
#ifndef FILE_DETECTOR_H
#define FILE_DETECTOR_H

#ifdef __cplusplus //makes the header work in both c and cpp
extern "C"{//this is to prevent mangled function name so python can call it as it is
#endif
    const char* detect_file_type(const char* filepath);
#ifdef __cplusplus
}
#endif
#endif