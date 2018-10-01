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
#include <vector>

class XmlDom: public std::vector<XmlDom> {
public:
	XmlDom(){}
	XmlDom(std::string name): name(name) {}
	XmlDom(std::string name, std::string data): name(name), data(data) {}
	void print(int level = 0, std::ostream *target = 0);

	XmlDom *find(std::string name);
	std::vector<XmlDom*> findAll(std::string name);

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

	std::string *attributeRef(std::string name) {
		for (auto &it: attributes) {
			if (it.first == name) {
				return &it.second;
			}
		}
		return nullptr;
	}

	std::string attribute(std::string name, std::string def) {
		auto attr = attributeRef(name);
		if (attr) {
			return *attr;
		}
		else {
			return def;
		}
	}

	void setAttribute(std::string name, std::string value) {
		auto ref = attributeRef(name);
		if (ref) {
			*ref = value;
		}
		else {
			attributes.push_back({name, value});
		}
	}

	template <class T>
	T convertAttribute(std::string name, T def = 0) {
		auto attr = attributeRef(name);
		if (!attr) {
			return def;
		}
		std::istringstream ss(*attr);
		T ret;
		ss >> ret;
		return ret;
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
	std::vector<std::pair<std::string, std::string>> attributes;
};

class XmlDocument: public XmlDom {
public:
	XmlDocument();
	virtual ~XmlDocument();

	void load(std::istream&);
	void save(std::ostream&);

	bool loadFile(std::string);
	void saveFile(std::string);

	std::string version = "1.0";
	std::string encoding = "";
	std::string standalone = "";
};

