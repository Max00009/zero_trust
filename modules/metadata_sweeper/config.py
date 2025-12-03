'''this file holds all the configurations.
users can edit this file to change behaviour.
'''
#default behaviour
KEEP_TIMESTAMP=False
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