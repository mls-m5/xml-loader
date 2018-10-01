#automatically find files from wildcards:
CPP_FILES+= $(wildcard *.cpp)
TEST_CPP_FILES+= $(wildcard *-test.cpp)
TESTS = $(TEST_CPP_FILES:.cpp=)  #find the associated o-filenames
CXXFLAGS = -std=c++11 -I../include/ -Imls-unit-test #add own flags here
LIBS = #libs like -lGL

#make sure to call the rule
all: ${TESTS}
	git submodule update --init #pull all submodules from server

%-test: %-test.cpp
#xml-test: xml-test.cpp ../src/xmldocument.cpp
	${CXX} ${CXXFLAGS} -o $@ $< ${LIBS} `grep -oP '(?<=//OBJECTS=).*' $<`

clean:
	rm -f src/*.o
	rm -f ${TESTS}

