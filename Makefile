PATH := /usr/um/gcc-5.1.0/bin:$(PATH)
LD_LIBRARY_PATH := /usr/um/gcc-5.1.0/lib64
LD_RUN_PATH := /usr/um/gcc-5.1.0/lib64

EXECUTABLE 	= Tracker
CXX			= g++

# list of test drivers (with main()) for development
TESTSOURCES = $(wildcard test*.cpp)
# names of test executables
TESTS       = $(TESTSOURCES:%.cpp=%)

# list of sources used in project
SOURCES 	= $(wildcard *.cpp)
SOURCES     := $(filter-out $(TESTSOURCES), $(SOURCES))
# list of objects used in project
OBJECTS		= $(SOURCES:%.cpp=%.o)

PROJECTFILE = Tracker.cpp

#Default Flags
CXXFLAGS = -std=c++11 -Wall -Werror -Wextra -pedantic -pthread -lpthread -Wconversion -march=native

# make release - will compile "all" with $(CXXFLAGS) and the -O3 flag
#				 also defines NDEBUG so that asserts will not check
release: CXXFLAGS += -O3 -DNDEBUG
release: all

# make debug - will compile "all" with $(CXXFLAGS) and the -g flag
#              also defines DEBUG so that "#ifdef DEBUG /*...*/ #endif" works
debug: CXXFLAGS += -g3 -DDEBUG
debug: clean all

# make profile - will compile "all" with $(CXXFLAGS) and the -pg flag
profile: CXXFLAGS += -pg
profile: clean all
	
# highest target; sews together all objects into executable
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
ifeq ($(EXECUTABLE), executable)
	@echo Edit EXECUTABLE variable in Makefile.
	@echo Using default a.out.
	$(CXX) $(CXXFLAGS) $(OBJECTS)
else
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(EXECUTABLE)
endif

# rule for creating objects
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $*.cpp

# make clean - remove .o files, executables, tarball
clean:
	rm -f $(OBJECTS) $(EXECUTABLE) $(TESTS) $(PARTIAL_SUBMITFILE) $(FULL_SUBMITFILE)

utils_snapshot.o: utils_snapshot.hpp utils_snapshot.cpp
utils_real_time.o: utils_real_time.hpp utils_real_time.cpp
Tracker.o: Tracker.cpp utils_snapshot.o utils_real_time.o

# these targets do not create any files
.PHONY: all release debug profile clean alltests partialsubmit fullsubmit help
# disable built-in rules
.SUFFIXES:
