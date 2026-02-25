#compiler and flags
CXX= g++
#-std=c++17 required for lambda fiunction and node deletion in hashtable.
#fPIC=position independent code.
#O2=optimization level 2.
#Wall=warnings all.
#Wextra=deep dive inspection for warnings.
CXXFLAGS= -std=c++17 -fPIC -Wall -Wextra -O2 
LDFLAGS= -shared -pthread

#Targets
TARGET=gateway.so

#Sources
SOURCE=gateway.cpp
HEADER=gateway.h

#link shared_libraries
CACHE_LIB=../cache/cache.so #we have to link cache.so cause our gateway calls cache_fetch() function from cache.cpp
DECISION_LIB=../decision_engine/decision.so #we have to link cache.so cause our gateway calls decision_making() function from decision.cpp
#Build all targets
all: $(TARGET)

#Build gateway shared ibrary
$(TARGET): $(SOURCE) $(HEADER)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(SOURCE) -o $(TARGET) $(CACHE_LIB) $(DECISION_LIB)
	@echo "+ Built $(TARGET) successfully!"

#Delete so file
clean:
	rm -f $(TARGET)
	@echo "+ Cleaned build files"


#Test all
test: all
	@echo "+ Built $(TARGET) successfully!"