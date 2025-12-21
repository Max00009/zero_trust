'''this file holds all the configurations.
users can edit this file to change behaviour.
'''
#default behaviour
'''exiftool will erase almost all sensitive data.But Image,Video,Pdf and audio will sometimes contain filestructure metadata(not file metadata) which is saved by os.
exiftool can't remove those.
some examples of filestructure metadata:
    File Modification Date/Time,<--we can change it using exiftool to set arbitary values.
    File Access Date/Time,
    File Inode Change Date/Time,
    Track Create Date,
    Track Modify Date,
    Media Create Date,
    Media Modify Date etc.
'''
DELETE_ALL_DATE_TIME=False
REPLACE_ORIGINAL=True

#exiftool path
EXIFTOOL_PATH="exiftool"#we will let the system find it automatically

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
ERROR_ZERO_OUT_FAILED ="ERROR:ZEROING OUT FAILED"
ERROR_EXIFTOOL_NOT_FOUND="ERROR:EXIFTOOL_NOT_FOUND"