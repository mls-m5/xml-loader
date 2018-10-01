
.PHONY: tests

all: xml.o tests

xml.o: src/xmldocument.cpp include/xmldocument.h
	${CXX} -o xml.o src/xmldocument.cpp -Iinclude/ -c
	
tests:
	make -C tests/
	
clean:
	rm -f xml.o
