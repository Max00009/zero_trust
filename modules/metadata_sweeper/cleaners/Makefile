CC=g++
CFLAGS=-shared -fPIC -Wall -O2#shared,position independent code,warnings,optimization
SOURCES=image_cleaner.cpp
TARGET=libimage_cleaner.so
$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)
clean:
	rm -f $(TARGET)
test: $(TARGET)
	@echo "✓ image_cleaner compiled successfully!"