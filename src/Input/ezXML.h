#ifndef ezXML_H
#define ezXML_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

struct XMLNode
{
	std::string tag;
	std::string value;
	std::pair<std::string, std::string> attrib;
	std::vector<XMLNode> children;
};

#endif