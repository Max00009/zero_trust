'''this file is in testing mode right now.a lot of other files will be connected here cause it's like a junction.
now it's in verbose and cli stage.when all done i will make it background daemon and silent.'''
import sys
import os
from utils.file_detector import detect_file_type
from cleaners.image_cleaner import image_cleaner
from cleaners.video_cleaner import video_cleaner
from cleaners.pdf_cleaner import pdf_cleaner
from cleaners.audio_cleaner import audio_cleaner


def main():
    filepath=sys.argv[1]
    if not os.path.exists(filepath):
        print("ERROR:FILE_NOT_FOUND")
        sys.exit(1)
    detected_file_type=detect_file_type(filepath)
    if detected_file_type ==None:
        print("detect file returned None.")
    elif detected_file_type.startswith("image/"):
        print("it detected image and proceeding with next step")
        result=image_cleaner(filepath)
        if result=="SUCCESS":
            print("yeah everything worked.check metadata now.")
        else:
            print(result)
            sys.exit(1)
    elif detected_file_type.startswith("video/"):
        print("it detected video and proceeding with next step")
        result=video_cleaner(filepath)
        if result=="SUCCESS":
            print("yeah everything worked.check metadata now.")
        else:
            print(result)
            sys.exit(1)
    elif detected_file_type.startswith("audio/"):#some recordings in newer model starts with video.video cleaner will handle that.
        print("it detected audio and proceeding with next step")
        result=audio_cleaner(filepath)
        if result=="SUCCESS":
            print("yeah everything worked.check metadata now.")
        else:
            print(result)
            sys.exit(1)
    elif detected_file_type=="application/pdf":#not starts with .exactly same.
        result=pdf_cleaner(filepath)
        if result=="SUCCESS":
            print("yeah everything worked.check metadata now.")
        else:
            print(result)
            sys.exit(1)
    else:
        print("detect file returned something that doesn't starts with image or video or pdf.")
        sys.exit(1)

if __name__=="__main__":
    main()