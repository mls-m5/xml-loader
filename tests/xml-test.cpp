
//OBJECTS=../src/xmldocument.cpp


#include "unittest.h"
#include "xmldocument.h"
#include "istream"
#include <fstream>
#include <sstream>

using namespace::std;

TEST_SUIT_BEGIN

TEST_CASE("load"){

	XmlDocument xmlDocument;

	ifstream file("test-def.xml");

	ASSERT(file.is_open(), "File is not open");

	xmlDocument.load(file);

	xmlDocument.print();
}

TEST_CASE("save"){
	XmlDocument xmlDocument;

	xmlDocument.name = "root";
	xmlDocument.push_back(XmlDom("synth"));

	xmlDocument.print();
}

TEST_CASE("find-all"){
	XmlDocument xmlDocument;

	stringstream ss;

	ss << "<root>" << endl;

	for (int i = 0; i < 5; ++i){
		ss << "<controller>7</controller>" << endl;
	}

	for (int i = 0; i < 10; ++i){
		ss << "<osc/>" << endl;
	}

	ss << "</root>" << endl;

	xmlDocument.load(ss);

	auto ret = xmlDocument.findAll("osc");

	ASSERT(ret.size() == 10, "could not find right number of elements, found " << ret.size());

	ret = xmlDocument.findAll("controller");

	ASSERT(ret.front()->data[0] = '7', "fel värde på data");
}

TEST_CASE("file-access"){
	XmlDocument xmlDocument;
	xmlDocument.name = "root";
	xmlDocument.saveFile("test-out.xml");

	XmlDocument xmlDocument2;
	ASSERT(xmlDocument2.loadFile("test-out.xml"), "file not loaded");
}

TEST_SUIT_END

