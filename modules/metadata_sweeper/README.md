METADATA SWEEPER
(this whole module can be done with rust more easily and efficiently.i will migrate it to rust.)



PROBELM:
When you take a photo with your phone, it secretly stores:
1. GPS coordinates (where you took it)
2. Phone model (what device you used)
3. Date and time
4. Software version
SENSITIVE_METADATA_TYPES: [
    'EXIF',      # Camera settings, GPS, device info
    'XMP',       # Adobe metadata, copyright
    'IPTC',      # Author, keywords, description
    'ICC',       # Color profile
    'Photoshop', # Adobe Photoshop data
    'Composite', # Calculated metadata
    'Thumbnail', # Embedded preview images
]


TOOLS IN LINUX:	1. exiftool ->to extract metadata.primary tool.
				2. libexif-dev ->maybe used later for optimization(very fast but limited function).
				3. libmagic-dev ->for filetype detection.
                4. id3v2 ->exiftool didn't work for mp3. so had to install it.


WORKFLOW:
1. will monitor for new files
2. will detect the file type
3. call suitable tools
4. clear exif data(except timestamp)
5. save cleaned media
				
entry_point-->file_detector-->suitable cleaner.
for testing we will do entry point a cli and verbose output.but later we will make it background daemon(monitors folder) and sient.



DRAWBACK:
while sharing through social media most of them change a jpg to webp and erase metadata automatically.but they can't do that if photo is sent disguised as pdf or other format.



NOTE:
Some of the test_files was not writtable.I checked and turns out i didn't have write permission.to fix:
    To take ownership:
        ```bash
        sudo chown -R $USER:$USER (file_path)
        ```
        for example:sudo chown -R max1337:max1337 ./test_files/
    To add write permission:
        ```bash
        chmod u+w (file_path)
        ```
    To verify:
        ```bash
        ls -l (file_path)
        ```
However i didn't automate this logic inside my main code cause from the user's perspective he will always have permission.my problem is i am sharing test files from one device to another.