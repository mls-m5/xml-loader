/*
 * xmldocument.h
 *
 *  Created on: 6 okt 2014
 *      Author: Mattias Larsson Sköld
 */

#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <list>

class XmlDom: public std::list<XmlDom> {
public:
	XmlDom(){}
	XmlDom(std::string name): name(name) {}
	XmlDom(std::string name, std::string data): name(name), data(data) {}
	void print(int level = 0, std::ostream *target = 0);

	XmlDom *find(std::string name);
	std::list<XmlDom*> findAll(std::string name);

	void pushValue(std::string name, std::string data){
		push_back(XmlDom(name, data));
	}

	template <class T>
	void pushValue(std::string name, T data){
		std::stringstream ss;
		ss << data;
		pushValue(name, ss.str());
	}

	template <class T>
	T getValue(std::string name, T def = 0){
		auto f = find(name);
		if (f){
			if (f->data.size()){
				std::istringstream ss(f->data);
				T ret;
				ss >> ret;
				return ret;
			}
		}
		return def;
	}

	std::string getString(std::string name, std::string def = ""){
		auto f = find(name);
		if (f){
			return f->data;
		}
		return def;
	}

	std::string name;
	std::string data;
};

class XmlDocument: public XmlDom {
public:
	XmlDocument();
	virtual ~XmlDocument();

	void load(std::istream&);
	void save(std::ostream&);

	bool loadFile(std::string);
	void saveFile(std::string);
};

