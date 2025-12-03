# Compiler settings
CC = g++
CFLAGS = -shared -fPIC -Wall
LIBS = -lmagic

# Build the shared library
file_detector.so: file_detector.cpp file_detector.h
	$(CC) $(CFLAGS) file_detector.cpp $(LIBS) -o file_detector.so 

# Remove compiled files
clean:
	rm -f file_detector.so

# Test if library loads
test: file_detector.so
	@echo "Testing file_detector.so..."
	@python3 -c "import ctypes; lib = ctypes.CDLL('./file_detector.so'); print('✓ Library loaded successfully!')"

.PHONY: clean test
