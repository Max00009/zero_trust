'''this file is in testing mode right now.a lot of other files will be connected here cause it's like a junction.
now it's in verbose and cli stage.when all done i will make it background daemon and silent.'''
import sys
import os
from utils.file_detector import detect_file_type
from cleaners.image_cleaner import image_cleaner
from cleaners.video_cleaner import video_cleaner
from config import ERROR_FILE_NOT_FOUND

def main():
    filepath=sys.argv[1]
    if not os.path.exists(filepath):
        return ERROR_FILE_NOT_FOUND 
    detected_file_type=detect_file_type(filepath)
    if detected_file_type ==None:
        print("detect file returned None.")
    elif detected_file_type.startswith("image/"):
        print("it detected image and proceeding with next step")
        result=image_cleaner(filepath)
        if result=="SUCCESS":
            print("yeah everything worked.check metadata now.")
    elif detected_file_type.startswith("video/"):
        print("it detected video and proceeding with next step")
        result=video_cleaner(filepath)
        if result=="SUCCESS":
            print("yeah everything worked.check metadata now.")
    else:
        print("detect file returned something that doesn't starts with image or video.")

if __name__=="__main__":
    main()