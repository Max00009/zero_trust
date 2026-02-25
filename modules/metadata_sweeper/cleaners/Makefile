#compiler
CC=g++
#compiler flags
CFLAGS=-shared -fPIC -Wall -O2#shared,position independent code,warnings,optimization
#source files
IMAGE_SOURCES=image_cleaner.cpp
VIDEO_SOURCES=video_cleaner.cpp
PDF_SOURCES=pdf_cleaner.cpp
AUDIO_SOURCES=audio_cleaner.cpp
#TARGETS
IMAGE_TARGET=libimage_cleaner.so
VIDEO_TARGET=libvideo_cleaner.so
PDF_TARGET=libpdf_cleaner.so
AUDIO_TARGET=libaudio_cleaner.so
# Build all targets
all: $(IMAGE_TARGET) $(VIDEO_TARGET) $(PDF_TARGET) $(AUDIO_TARGET)
# Build image cleaner
$(IMAGE_TARGET): $(IMAGE_SOURCES)
	$(CC) $(CFLAGS) $(IMAGE_SOURCES) -o $(IMAGE_TARGET)
# Build video cleaner
$(VIDEO_TARGET): $(VIDEO_SOURCES)
	$(CC) $(CFLAGS) $(VIDEO_SOURCES) -o $(VIDEO_TARGET)
#Build pdf cleaner
$(PDF_TARGET): $(PDF_SOURCES)
	$(CC) $(CFLAGS) $(PDF_SOURCES) -o $(PDF_TARGET)
$(AUDIO_TARGET): $(AUDIO_SOURCES)
	$(CC) $(CFLAGS) $(AUDIO_SOURCES) -o $(AUDIO_TARGET)
# Clean all compiled files
clean:
	rm -f $(IMAGE_TARGET) $(VIDEO_TARGET) $(PDF_TARGET)
# Test all
test: all
	@echo "+ image_cleaner compiled successfully!"
	@echo "+ video_cleaner compiled successfully!"
	@echo "+ pdf_cleaner compiled successfully!"
	@echo "+ audio_cleaner compiled successfully!"