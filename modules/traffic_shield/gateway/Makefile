#i had problems compiling and then loading gateway.so during entry_point.py startup.
#i have to clean this code later.
#compiler and flags
CXX= g++
#-std=c++17 required for lambda function and node deletion in hashtable.
#fPIC=position independent code.
#O2=optimization level 2.
#Wall=warnings all.
#Wextra=deep dive inspection for warnings.
CXXFLAGS= -std=c++17 -fPIC -Wall -Wextra -O2 -I../cache -I../decision #-I for include path.so compiler can find header files.
#when entry_point.py was loading gateway.so it couldn't find cache.so and decision.so
#cause at runtime it was looking ../cache/ and ../decision/ WITH RESPECT TO it's own location.
#so we have to explicitely tell to look from gateway.so's location.we will use RPATH for that.
#first $ escapes second $.ORIGIN=where gateway.so is located.
#-L is for linking path.
LDFLAGS= -shared -pthread \
		-Wl,-rpath,'$$ORIGIN/../cache' \
		-Wl,-rpath,'$$ORIGIN/../decision_engine' \
		-L../cache -L../decision_engine \
		-lcache -ldecision

#Targets
TARGET=libgateway.so

#Sources
SOURCE=gateway.cpp
HEADER=gateway.h

#link shared_libraries
#instead of linking dynamically i could have compile all together but i thought may be later i have to work with cache separately.that's why i am compiling everything separate.
#CACHE_LIB=../cache/libcache.so #we have to link cache.so cause our gateway calls cache_fetch() function from cache.cpp
#DECISION_LIB=../decision_engine/libdecision.so #we have to link cache.so cause our gateway calls decision_making() function from decision.cpp
#Build all targets
all: $(TARGET)

#Build gateway shared ibrary
$(TARGET): $(SOURCE) $(HEADER)
	$(CXX) $(CXXFLAGS) $(SOURCE) -o $(TARGET) $(LDFLAGS)
	@echo "+ Built $(TARGET) successfully!"

#Delete so file
clean:
	rm -f $(TARGET)
	@echo "+ Cleaned build files"


#Test all
test: all
	@echo "+ Built $(TARGET) successfully!"