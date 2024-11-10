#ifndef ezXML_H
#define ezXML_H

#include <iostream>
#include <fstream>
#include <string>

struct XMLAttrib
{
	std::string name;
	std::string content;
};

struct XMLNode
{
	std::string tag;
	std::string innerText;
	//std::pair<std::string, std::string> m_attrib;
	
	XMLNode* parent;
};

struct XMLParser
{
	char* buffer;
	size_t length;
	size_t position;
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
	node->tag = "";
	node->innerText = "";
	node->parent = parent;

	return node;
}

inline void DestroyNode(XMLNode* node)
{
	//TODO: Recursively delete all children
	delete node;
}

/*
* move parser position ahead by n bytes, but clamp to length of buffer
*/
inline void ParserConsume(XMLParser* parser, size_t n)
{
	parser->position += n;
	if (parser->position > parser->length)
		parser->position = parser->length - 1;
}

inline XMLNode* ParseNode(XMLParser& parser)
{
	/*
		char arrays for tagOpen, tagClose, innerText
		check for  '<' and consume
		parse tagClose, accumulate tag name in buffer and check for '>' and consume
		check if tagOpen != tagClose, throw mismatch tag error
	*/

	return 0;
}

inline XMLDoc* ParseDocument(XMLDoc* doc)
{
	// copy doc into parsing tracker
	// parse into nodes
	XMLParser parser{
		doc->buf,
		0,
		doc->length
	};

	return doc;
}

inline XMLDoc* LoadDocument(const char* path)
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
	
	if (!length)
	{
		std::cerr << "Failed to parse, file empty: " << path << std::endl;
		return 0;
	}

	if (buf)
	{
		buf[length] = '\0';

		XMLDoc* doc = new XMLDoc();
		doc->buf = _strdup(buf);
		doc->length = length;
		doc->root = nullptr;

		free(buf);
		doc = ParseDocument(doc);
		return doc;
	}
	else 
	{
		std::cerr << "Failed to allocate memory for file: " << path << std::endl;
		return 0;
	}
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