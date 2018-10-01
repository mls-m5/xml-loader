
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


TEST_CASE("attributes") {
	XmlDocument xmlDocument;

	istringstream testStream(R"_(
<root width="100" height="200">
	<child width="50" height="40"/>
</root>
)_");

	xmlDocument.load(testStream);

	ASSERT_EQ(xmlDocument.name, "root");
	ASSERT_EQ(xmlDocument.attribute("width", ""), "100");
	ASSERT_EQ(xmlDocument.convertAttribute<int>("width"), 100);
	ASSERT_EQ(xmlDocument.attribute("height", ""), "200");
	ASSERT_EQ(xmlDocument.convertAttribute<int>("height"), 200);

	auto child = xmlDocument.find("child");
	ASSERT(child, "could not find child element");
	ASSERT_EQ(child->attribute("width", ""), "50");
	ASSERT_EQ(child->attribute("height", ""), "40");
}


TEST_CASE("Non alphabetical characters in node names") {
	XmlDocument xmlDocument;

	istringstream testStream(R"_(
<root_node width="100" height="200">
	<second-node/>
	<third:node/>
</root_node>
)_");

	xmlDocument.load(testStream);

	ASSERT(xmlDocument.name == "root_node", "could not find root_node");
	ASSERT(xmlDocument.find("second-node"), "could not find second-node");
	ASSERT(xmlDocument.find("third:node"), "could not find third:node");
}

TEST_CASE("Handle header") {
	XmlDocument xmlDocument;

	istringstream testStream(R"_(
<?xml version="1.0" encoding="utf-8" standalone="yes"?>
<root>Content</root>
)_");

	xmlDocument.load(testStream);

	ASSERT_EQ(xmlDocument.name, "root");
	ASSERT_EQ(xmlDocument.data, "Content");

	ASSERT_EQ(xmlDocument.version, "1.0");
	ASSERT_EQ(xmlDocument.encoding, "utf-8");
	ASSERT_EQ(xmlDocument.standalone, "yes");

}

TEST_SUIT_END

