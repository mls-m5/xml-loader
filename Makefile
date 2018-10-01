
.PHONY: tests

all: xml.o xml.d.o tests

xml.o: src/xmldocument.cpp include/xmldocument.h
	${CXX} -o xml.o src/xmldocument.cpp -Iinclude/ -c
	
xml.d.o: src/xmldocument.cpp include/xmldocument.h
	${CXX} -o xml.d.o src/xmldocument.cpp -Iinclude/ -c -g
	
tests:
	make -C tests/
	
clean:
	rm -f xml.o
