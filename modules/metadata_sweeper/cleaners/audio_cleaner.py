'''this is a wrapper file.
it makes easy to call cpp function.
the wrapper will load the .so file,handle the ctype stuff once,convert python strings into C bytes,and provide simple function.
'''
import subprocess #to check exiftool
import ctypes #to load the .so file
import os #to make the relaitve file path

'''instead of hardcoding the file path of (file_detector.so) we have to build the relative file path
in that way we can call our function from anywhere.__file__  will give the current directory'''

current_dir=os.path.dirname(os.path.abspath(__file__)) #current directory
lib_path=os.path.join(current_dir,'libaudio_cleaner.so') #get the relative path 
_lib=ctypes.CDLL(lib_path) #now it will work from anywhere

#function signature configuration
_lib.audio_cleaner.argtypes=[ctypes.c_char_p]
_lib.audio_cleaner.restype=ctypes.c_char_p

def audio_cleaner(filepath):
    filepath_bytes=filepath.encode('utf-8')
    result=(_lib.audio_cleaner(filepath_bytes)).decode('utf-8')
    return result