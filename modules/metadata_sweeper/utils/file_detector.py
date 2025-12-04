'''this is a wrapper file.
it makes easy to call cpp function.
the wrapper will load the .so file,handle the ctype stuff once,convert python strings into C bytes,and provide simple function.
'''

import ctypes #to load the .so file
import os #to make the relaitve file path

'''instead of hardcoding the file path of (file_detector.so) we have to build the relative file path
in that way we can call our function from anywhere.__file__  will give the current directory'''

current_dir=os.path.dirname(os.path.abspath(__file__)) #current directory
lib_path=os.path.join(current_dir,'libfile_detector.so') #get the relative path 
_lib=ctypes.CDLL(lib_path) #now it will work from anywhere

'''const char* detect_file_type(const char* filepath);
this cpp function takes const char* type as argument in input and output
in ctypes terms: const char*=ctypes.c_char_p
'''
_lib.detect_file_type.argtypes=[ctypes.c_char_p]#what it tales.with list 
_lib.detect_file_type.restype=ctypes.c_char_p#what it returns.no .singlr type

def detect_file_type(filepath):
    filepath_bytes=filepath.encode('utf-8')
    result=(_lib.detect_file_type(filepath_bytes)).decode('utf-8')
    if result.startswith('ERROR:'):
        return None
    return result

