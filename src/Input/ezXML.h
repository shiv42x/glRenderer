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
	int length = file.tellg();
	file.seekg(0, std::ios::beg);
	
	char* buf = (char*)malloc(sizeof(char) * length + 1);
	file.read(buf, length);
	file.close();

	if (!buf)
	{
		std::cout << "Failed to allocate memory for file: " << path << std::endl;
		return false;
	}

	buf[length] = '\0';
	std::cout << buf << std::endl;

	free(buf);
	return true;
}

#endif