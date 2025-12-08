#compiler
CC=g++
#compiler flags
CFLAGS=-shared -fPIC -Wall -O2#shared,position independent code,warnings,optimization
#source files
IMAGE_SOURCES=image_cleaner.cpp
VIDEO_SOURCES=video_cleaner.cpp
#TARGETS
IMAGE_TARGET=libimage_cleaner.so
VIDEO_TARGET=libvideo_cleaner.so
# Build all targets
all: $(IMAGE_TARGET) $(VIDEO_TARGET)
# Build image cleaner
$(IMAGE_TARGET): $(IMAGE_SOURCES)
	$(CC) $(CFLAGS) $(IMAGE_SOURCES) -o $(IMAGE_TARGET)
# Build video cleaner
$(VIDEO_TARGET): $(VIDEO_SOURCES)
	$(CC) $(CFLAGS) $(VIDEO_SOURCES) -o $(VIDEO_TARGET)
# Clean all compiled files
clean:
	rm -f $(IMAGE_TARGET) $(VIDEO_TARGET)
# Test both
test: all
	@echo "✓ image_cleaner compiled successfully!"
	@echo "✓ video_cleaner compiled successfully!"