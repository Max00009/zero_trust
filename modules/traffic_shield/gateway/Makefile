#compiler and flags
CXX= g++
CXXFLAGS= -std=c++11 -fPIC -Wall -O2 #-std=c++11 required for lambda fiunction.
LDFLAGS= -shared -pthread

#Targets
TARGET=gateway.so

#Sources
SOURCE=gateway.cpp
HEADER=gateway.h

#link shared_libraries
CACHE_LIB=../cache/cache.so
#Build all targets
all: $(TARGET)

#Build gateway shared ibrary
$(TARGET): $(SOURCE) $(HEADER)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(SOURCE) -o $(TARGET)
	@echo "✓ Built $(TARGET) successfully!"

#Delete so file
clean:
	rm -f $(TARGET)
	@echo "✓ Cleaned build files"


#Test all
test: all
	@echo "✓ Built $(TARGET) successfully!"