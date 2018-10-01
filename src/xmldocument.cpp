/*
 * xmldocument.cpp
 *
 *  Created on: 6 okt 2014
 *      Author: Mattias Larsson Sköld
 */

#include "xmldocument.h"
#include <sstream>
#include <fstream>
#include <stack>
#include <ctype.h>

using std::istream;
using std::ostream;
using std::stringstream;
using std::string;

#define DEBUG if(0)

class Token: public string{
public:
	enum TokenType {
		Space,
		Word,
		StartTag,
		StartEndTag,
		EndStartTag,
		EndTag,
		Digit,
		Literal,
		CharacterData,
		None
	};
	Token (string str, TokenType type):
		type(type){
		this->string::operator=(str);
	}

	TokenType type;

};

XmlDocument::XmlDocument() {
}

XmlDocument::~XmlDocument() {
}

Token getNextToken(std::istream &stream){
	char c;
	stringstream ss;

	Token::TokenType mode = Token::Space;

	while (!stream.eof()){
		c = stream.get();
		switch (mode){
		case Token::Space:
			if (isspace(c)){
				continue;
			}
			else if(isalpha(c)){
				ss.put(c);
				mode = Token::Word;
				break;
			}
			else if (isdigit(c)){
				ss.put(c);
				mode = Token::Digit;
				break;
			}
			else if (c == '"'){
				ss.put(c);
				mode = Token::Literal;
				break;
			}
			else if (c == -1){
				//Something went wrong, probably end of input
				return Token(ss.str(), mode);
			}
			else{
				ss.put(c);
				switch(c){
				case '<':
					if (stream.peek() == '/'){
						ss.put(stream.get());
						return Token(ss.str(), Token::StartEndTag);
					}
					else{
						mode = Token::StartTag;
					}
					break;
				case '/':
					if (stream.peek() == '>'){
						ss.put(stream.get());
						return Token(ss.str(), Token::EndStartTag);
					}
					break;
				case '>':
					mode = Token::EndTag;
					break;
				}
				return Token(ss.str(), mode);
			}
			break;
		case Token::Digit:
			if (isdigit(c) || c == '.'){
				ss.put(c);
			}
			else{
				stream.unget();
				return Token(ss.str(), mode);
			}
			break;
		case Token::Word:
			if (isdigit(c) || isalpha(c)){
				ss.put(c);
			}
			else{
				stream.unget();
				return Token(ss.str(), mode);
			}
			break;
		case Token::Literal:
			ss.put(c);
			if (c == '"'){
				return Token(ss.str(), mode);
			}
			break;
		}
	}

	return Token(ss.str(), mode);
}

Token parseData(istream &stream){
	stringstream ss;

	while (isspace(stream.peek())){
		stream.get();
	}

	while (stream.peek() != '<' && !stream.eof()){
		auto c = stream.get();
		if (isspace(c)){
			while (isspace(stream.peek())){
				stream.get();
			}
			c = ' ';
		}
		ss.put(c);
	}

	return Token(ss.str(), Token::CharacterData);
}

void XmlDocument::load(std::istream &stream) {
	using std::cout;
	using std::endl;
#define EXPECT_ERROR(x, y) { cout << __FILE__ << ": expected token " << x << " not " << y << ". abort..." << endl; return; }
#define EXPECT_TOKEN(x) if (token.type != x) EXPECT_ERROR(x, token.type);


	XmlDom *currentStructure = 0;
	std::stack<XmlDom*> stack;

	while (!stream.eof()){
		auto token = getNextToken(stream);
		DEBUG cout << token << endl;

		switch (token.type) {
		case Token::StartTag:
		{
			token = getNextToken(stream);
			EXPECT_TOKEN(Token::Word);

			DEBUG cout << "new tag " << token << endl;

			if (currentStructure == 0){
				name = token;
				currentStructure = this;
			}
			else{
				stack.push(currentStructure);
				currentStructure->push_back(XmlDom(token));
				currentStructure = &currentStructure->back();
			}

			token = getNextToken(stream);

			bool inputData = true;
			while (inputData){
				if (token.type == Token::EndStartTag){
					if (!stack.empty()) {
						currentStructure = stack.top();
						stack.pop();
					}
					inputData = false;
				}
				else if (token.type == Token::EndTag){
					inputData = false;
				}
				else if (token.type == Token::Word){
					//Read arguments
					string argumentName = token;
					token = getNextToken(stream);
					if (token.compare("=")){
						EXPECT_ERROR("=", token);
					}
					token = getNextToken(stream);
					auto newStructure = XmlDom(argumentName);
					newStructure.data = token;
					currentStructure->push_back(newStructure);
					token = getNextToken(stream);
				}
				else {
					EXPECT_ERROR("argument", token);
				}
			}

			break;
		}
		case Token::StartEndTag:
		{
			token = getNextToken(stream);
			EXPECT_TOKEN(Token::Word);
			if (token.compare(currentStructure->name)){
				EXPECT_ERROR(currentStructure->name, token);
			}
			token = getNextToken(stream);
			EXPECT_TOKEN(Token::EndTag);
			DEBUG cout << "end tag " << currentStructure->name << endl;
			if (stack.empty()){
				return; //Finished
			}
			else {
				currentStructure = stack.top();
				stack.pop();
			}
		}
		break;
		default:
			//Read data

			token += parseData(stream);
			if (currentStructure){
				currentStructure->data = token;
			}
		}
	}
}

void XmlDocument::save(std::ostream &stream) {
	print(0, &stream);
}

void indent(int level, std::ostream *target){
	for (int i = 0; i < level; ++i){
		*target << "    ";
	}
}

void XmlDom::print(int level, std::ostream *printTarget) {
	using std::endl;

	auto target = printTarget;
	if (!target){
		target = &std::cout;
	}

	indent(level, target);
	*target << "<" << name;

	bool doIndent = size();
	if (size() || !data.empty()){
		*target << ">";
		if (doIndent){
			*target << endl;
		}
		if (!data.empty()){
			if (doIndent){
				indent(level + 1, target);
			}
			*target << data;
			if (doIndent){
				*target << endl;
			}
		}
		for (auto &it: *this){
			it.print(level + 1, target);
		}

		if (doIndent){
			indent(level, target);
		}
		*target << "</" << name << ">" << endl;
	}
	else{
		*target << "/>" << endl;
	}
}

XmlDom* XmlDom::find(std::string name) {
	for (auto &it: *this){
		if (it.name.compare(name) == 0){
			return &it;
		}
	}
	return 0;
}

std::list<XmlDom*> XmlDom::findAll(std::string name) {
	std::list<XmlDom*> retList;
	for (auto &it: *this){
		if (it.name.compare(name) == 0){
			retList.push_back(&it);
		}
	}
	return retList;
}

bool XmlDocument::loadFile(std::string fname) {
	std::ifstream file(fname);
	if (!file.is_open()){
		return false;
	}

	load(file);
	return true;
}

void XmlDocument::saveFile(std::string fname) {
	std::ofstream file(fname);

	save(file);
}
