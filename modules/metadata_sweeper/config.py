'''this file holds all the configurations.
users can edit this file to change behaviour.
'''
#default behaviour
'''
1. exiftool will erase almost all sensitive data after running 'exiftool -all=' command.
2.However Image,Video,Pdf and audio will sometimes contain filesystem metadata(not file metadata) which is saved by os.
exiftool can't remove those.some examples of filestructure metadata:
    File Modification Date/Time,<--we can change it using exiftool to set arbitary values.
    File Access Date/Time,<--this gets changed as a side effect if and only if we change the FileModifyDate at last.good for us.
    File Inode Change Date/Time,
3.There will also remain some internal dates/times after 'exiftool -all=' command as these live in
atoms inside containers.for example-these are the internal MP4 dates:
    Create Date
    Modify Date
    Track Create Date
    Track Modify Date
    Media Create Date
    Media Modify Date
'''
CHANGE_FILESYS_DATE=True #it will change the File Modification Date/Time
CHANGE_INTERNAL_DATE=True #it will change the internal date/time
REPLACE_ORIGINAL=True



#supported formats
SUPPORTED_FORMATS=[
    #image
    'image/jpeg',
    'image/png',
    'image/jpg',
    'image/tiff',
    'image/webp',
    'image/heic',
    #documents
    #video
    #audio
]

#error messages
ERROR_FILE_NOT_FOUND = "ERROR:FILE_NOT_FOUND"
ERROR_UNSUPPORTED_TYPE = "ERROR:UNSUPPORTED_TYPE"
ERROR_EXIFTOOL_FAILED = "ERROR:EXIFTOOL_FAILED"
ERROR_PERMISSION_DENIED = "ERROR:PERMISSION_DENIED"
ERROR_UNKNOWN = "ERROR:UNKNOWN"
ERROR_FILESYSTEM_METADATA_CLEANING_FAILED= "ERROR:FILESYSYSTEM_METADATA_CLEANING_FAILED"
ERROR_INTERNAL_METADATA_CLEANING_FAILED= "ERROR:INTERNAL_METADATA_CLEANING_FAILED"
ERROR_EXIFTOOL_NOT_FOUND= "ERROR:EXIFTOOL_NOT_FOUND"
ERROR_METADATA_CLEANING_FAILED="ERROR:METADATA_CLEANING_FAILED"