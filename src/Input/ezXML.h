#ifndef ezXML_H
#define ezXML_H

#include <iostream>
#include <fstream>
#include <string>

struct XMLString
{
	std::string value;
	size_t length;
};

struct XMLAttrib
{
	XMLString* name;
	XMLString* content;
};

struct XMLNode
{
	XMLString* tag;
	XMLString* innerText;
	//std::pair<std::string, std::string> m_attrib;
	
	XMLNode* parent;
};

struct XMLParser
{
	char* buf;
	size_t length;
	size_t position;
};

struct XMLDoc
{
	char* buf;
	size_t length;
	XMLNode* root;
};


inline XMLNode* GetDocRoot(XMLDoc* doc)
{
	return doc->root;
}

inline void FreeNode(XMLNode* node)
{
	//TODO: Recursively delete all children
	delete node;
}

/*
* Move parser position ahead by n, 
* but clamp to length of buffer
*/
inline void ParserConsume(XMLParser* parser, size_t n)
{
	parser->position += n;
	// clamp to length of buf
	if (parser->position > parser->length)
		parser->position = parser->length - 1;
}

/*
* Accumulate any char except whitespace (not implemented yet),
* parse closing of tag ('>') and return accumulated
*/
inline XMLString* ParseEnding(XMLParser* parser)
{
	//TODO: skip whitespace
	size_t start = parser->position;
	size_t length = 0;

	while (start + length < parser->length)
	{
		if ((parser->buf[parser->position] == '>'))
			break;
		else
		{
			ParserConsume(parser, 1);
			length++;
		}
	}
	
	// consume '>'
	if (parser->buf[parser->position] != '>')
	{
		std::cout << "Missing '>'" << std::endl;
		return 0; //TODO: add error handling
	}
	ParserConsume(parser, 1);

	XMLString* tagName = new XMLString();
	tagName->value = std::string(&parser->buf[start], length);
	tagName->length = length;
	return tagName;
}

/*
* Parse opening tag ('<')
*/
inline XMLString* ParseOpening(XMLParser* parser)
{
	//TODO: add skip whitespace

	// consume '<'
	if (parser->buf[parser->position] != '<')
	{
		std::cout << "Missing '<'" << std::endl;
		return 0; //TODO: add error handling
	}
	ParserConsume(parser, 1);

	return ParseEnding(parser);
}

/*
* Parse content between tags
*/
inline XMLString* ParseContent(XMLParser* parser)
{
	size_t start = parser->position;
	size_t length = 0;

	while (start + length < parser->length)
	{
		if ((parser->buf[parser->position] == '<'))
			break;
		else
		{
			ParserConsume(parser, 1);
			length++;
		}
	}

	if (parser->buf[parser->position] != '<')
	{
		//TODO: add error handling
		std::cout << "Expected '<' but reached EOF." << std::endl;
		return 0;
	}

	XMLString* content = new XMLString();
	content->value = std::string(&parser->buf[start], length);
	content->length = length;
	return content;
}

/*
* Parse closing tag ('</')
*/
inline XMLString* ParseClosing(XMLParser* parser)
{
	if (parser->position + 1 > parser->length)
	{
		std::cout << "Root tag not closed" << std::endl;
		return 0; //TODO: add error handling
	}

	if (parser->buf[parser->position] != '<'
		|| parser->buf[parser->position + 1] != '/')
	{
		// split this case into two
		std::cout << "Closing tag missing '<' or '/'." << std::endl;
		return 0; //TODO: add error handling
	}
	ParserConsume(parser, 2);
	
	return ParseEnding(parser);
}

inline XMLNode* ParseNode(XMLParser* parser)
{
	/*
		check for  '<' and consume
		parse tagClose, accumulate tag name in buffer and check for '>' and consume
		check if opening != close, throw mismatch tag error1
	*/

	XMLString* openingTag = 0;
	XMLString* content = 0;
	XMLString* closingTag = 0;

	openingTag = ParseOpening(parser);
	if (!openingTag)
		std::cout << "ParseOpening() returned null." << std::endl; //TODO: add error handling

	//TODO: get attributes
	content = ParseContent(parser);
	if (!content)
	{
		std::cout << "ParseContent() returned null." << std::endl; //TODO: add error handling
		return 0;
	}

	closingTag = ParseClosing(parser);
	if (!closingTag)
	{
		std::cout << "ParseClosing() returned null." << std::endl; //TODO: add error handling
		return 0;
	}

	// add special method to compare XMLStrings
	if (openingTag->value != closingTag->value)
	{
		//TODO: add error handling
		std::cout << "Tag mismatch: '" << openingTag->value << "' and '" << closingTag->value << "'" << std::endl;
		return 0;
	}

	//create node and return
	//TODO: nested nodes
	XMLNode* node = new XMLNode();
	node->tag = openingTag;
	node->innerText = content;
	return node;
}

inline XMLDoc* ParseDocument(XMLDoc* doc)
{
	XMLParser parser{
		doc->buf,
		doc->length,
		0
	};
	XMLNode* parent = ParseNode(&parser);

	if (!parent)
	{
		//TODO: add error handling
		std::cout << "ParseNode() returned null." << std::endl;
		return 0;
	}

	doc->root = parent;
	return doc;
}

inline XMLDoc* LoadDocument(const char* path)
{
	std::fstream file(path, std::ios::in | std::ios::binary);

	if (!file)
		std::cout << "Failed to open file: " << path << std::endl;  //TODO: add error handling
	
	file.seekg(0, std::ios::end);
	int length = file.tellg();
	file.seekg(0, std::ios::beg);

	char* buf = (char*)malloc(sizeof(char) * length + 1);
	file.read(buf, length);
	file.close();
	
	if (!length)
	{
		std::cerr << "Failed to parse, file empty: " << path << std::endl;
		return 0; //TODO: add error handling
	}

	if (buf)
	{
		buf[length] = '\0';

		XMLDoc* doc = new XMLDoc();
		doc->buf = _strdup(buf);
		doc->length = length;
		doc->root = 0;

		free(buf);
		doc = ParseDocument(doc);
		return doc;
	}
	else 
	{
		std::cerr << "Failed to allocate memory for file: " << path << std::endl;
		return 0; //TODO: add error handling
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