CXX= g++
#-std=c++17 required for lambda fiunction and node deletion in hashtable.
#fPIC=position independent code.
#O2=optimization level 2.
#Wall=warnings all.
#Wextra=deep dive inspection for warnings.
CXXFLAGS= -std=c++17 -fPIC -Wall -Wextra -O2 
LDFLAGS= -shared -pthread

#targets
TARGET=decision.so
#sources
SOURCE=decision.cpp
HEADER=decision.h
#build all targets
all: $(TARGET)

#build decision shared library
$(TARGET): $(SOURCE) $(HEADER)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(SOURCE) -o $(TARGET)
	@echo "+ Built $(TARGET) successfully!"

#Delete so file
clean:
	rm -f $(TARGET)
	@echo "+ Cleaned build files"


#Test all
test: all
	@echo "+ Built $(TARGET) successfully!"