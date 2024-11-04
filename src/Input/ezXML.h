#ifndef ezXML_H
#define ezXML_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

struct XMLNode
{
	std::string m_tagName;
	std::string m_innerText;
	std::pair<std::string, std::string> m_attrib;
	
	XMLNode* parent;
};

bool LoadDocument(const char* path)
{
	std::fstream xml_doc(path);

	if (!xml_doc)
	{
		std::cerr << "Unable to open file " << path << std::endl;
		return false;
	}

	char ch;
	while (xml_doc.get(ch))
	{
		std::cout << ch;
	}
	std::cout << std::endl;
	xml_doc.close();
	return true;
}

#endif