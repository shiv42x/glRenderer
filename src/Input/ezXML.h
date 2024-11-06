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

inline bool LoadDocument(const char* path)
{
	std::fstream file(path, std::ios::in | std::ios::binary);

	if (!file)
	{
		std::cout << "Failed to open file: " << path << std::endl;
		return false;
	}

	file.seekg(0, std::ios::end);
	int size = file.tellg();
	file.seekg(0, std::ios::beg);
	
	// 1 byte for null terminator
	char* buf = (char*)malloc(sizeof(char) * size + 1);
	file.read(buf, size);
	file.close();
	buf[size] = '\0';

	std::cout << buf << std::endl;
	return true;
}
#endif