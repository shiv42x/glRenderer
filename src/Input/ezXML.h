#ifndef ezXML_H
#define ezXML_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

struct XMLNode
{
	std::string m_tag;
	std::string m_innerText;
	//std::pair<std::string, std::string> m_attrib;
	
	XMLNode* parent;
};

struct XMLDoc
{
	const char* buf;
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

inline bool LoadDocument(const char* path, XMLDoc* doc)
{
	std::fstream file(path, std::ios::in | std::ios::binary);

	if (!file)
		std::cout << "Failed to open file: " << path << std::endl;

	file.seekg(0, std::ios::end);
	int length = file.tellg();
	file.seekg(0, std::ios::beg);

	char* buf = (char*)malloc(sizeof(char) * length + 1);
	file.read(buf, length);
	file.close();
		
	if (buf)
	{
		buf[length] = '\0';

		XMLDoc* doc = new XMLDoc();
		doc->buf = _strdup(buf);
		doc->length = length;

		free(buf);
		return true;
	}
	else 
	{
		std::cout << "Failed to allocate memory for file: " << path << std::endl;
		return false;
	}
}

inline XMLDoc* ParseDocument(const char* path)
{
	XMLDoc* doc = new XMLDoc();

	if (!LoadDocument(path, doc))
		std::cout << "Failed to load file: " << path << std::endl;
	
	// lexing trackers
	char lex[256];
	size_t lex_i = 0;
	size_t buf_i = 0;

	while (doc->buf[buf_i] != '\0')
	{
		buf_i++;
	}

	return doc;
}


/*
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
		if (buf[buf_i] == '<')
		{
			if (!curr)


			buf_i++;
			while (buf[buf_i] != '>')
				lex[lex_i++] = buf[buf_i++];
			lex[lex_i] = '\0';

			std::cout << lex << std::endl;
		}
		break;
	}

	std::cout << curr->m_tag << ":" << curr->m_innerText << std::endl;
	std::cout << "buf_i end at: " << buf_i << std::endl;
	free(buf);
	return true;
}
*/


#endif