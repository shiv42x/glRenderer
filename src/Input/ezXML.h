#ifndef ezXML_H
#define ezXML_H

#include <iostream>
#include <filesystem>
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
	FILE* xml_doc = fopen_s(path, "rb");

	if (!xml_doc)
	{
		std::cerr << "Error opening " << path;
		return false;
	}

	fseek(xml_doc, 0, SEEK_END);
	int size = ftell(xml_doc);
	fseek(xml_doc, 0, SEEK_SET);
	
	// + 1 for null terminating char
	char* buf = (char*)malloc(size + 1);
	fread(buf, 1, size, xml_doc);
	fclose(xml_doc);

	std::cout << buf << std::endl;
}


#endif