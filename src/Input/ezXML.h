#ifndef ezXML_H
#define ezXML_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#define strdup _strdup

struct XMLNode
{
	std::string m_tag;
	std::string m_innerText;
	//std::pair<std::string, std::string> m_attrib;
	
	XMLNode* parent;
};

struct XMLDoc
{
	char* buf;
	size_t length;
	XMLNode* root;
};

inline XMLNode* CreateNode(XMLNode* parent)
{
	XMLNode* node = new XMLNode();
	node->m_tag = "";
	node->m_innerText = "";
	node->parent = parent;

	return node;
}

inline void DestroyNode(XMLNode* node)
{
	//TODO: Recursively delete all children
	delete node;
}

inline bool LoadDocument(XMLDoc* doc, const char* path)
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

	doc->buf = strdup(buf);
	doc->length = length;
	doc->root = CreateNode(nullptr);

	// lexing trackers
	char lex[256];
	int lex_i = 0;
	int buf_i = 0;

	XMLNode* curr = nullptr;

	while (buf[buf_i] != '\0')
	{
		
	}

	std::cout << curr->m_tag << ":" << curr->m_innerText << std::endl;
	std::cout << "buf_i end at: " << buf_i << std::endl;
	free(buf);
	return true;
}

#endif