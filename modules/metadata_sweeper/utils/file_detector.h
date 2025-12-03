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