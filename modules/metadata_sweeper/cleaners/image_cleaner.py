'''this is a wrapper file.
it makes easy to call cpp function.
the wrapper will load the .so file,handle the ctype stuff once,convert python strings into C bytes,and provide simple function.
'''
import subprocess #to check exiftool
import ctypes #to load the .so file
import os #to make the relaitve file path

#now we will check if exiftool is installed ONNLY ONCE when module is loaded
try:
    subprocess.run(['which','exiftool'],
                    stdout=subprocess.DEVNULL,#we are hiding text output
                    stderr=subprocess.DEVNULL,#and error message.cause we only need to check if it's installed
                    check=True)
    _exiftool_available=True
except subprocess.CalledProcessError:
    _exiftool_available=False

'''instead of hardcoding the file path of (file_detector.so) we have to build the relative file path
in that way we can call our function from anywhere.__file__  will give the current directory'''

current_dir=os.path.dirname(os.path.abspath(__file__)) #current directory
lib_path=os.path.join(current_dir,'libimage_cleaner.so') #get the relative path 
_lib=ctypes.CDLL(lib_path) #now it will work from anywhere

#function signature configuration
_lib.image_cleaner.argtypes=[ctypes.c_char_p]
_lib.image_cleaner.restype=ctypes.c_char_p

def image_cleaner(filepath):
    if _exiftool_available!=True:
        return "ERROR:EXIFTOOL_NOT_FOUND"
    filepath_bytes=filepath.encode('utf-8')
    result=(_lib.image_cleaner(filepath_bytes)).decode('utf-8')
    if result.startswith('ERROR:'):
        return None
    return result