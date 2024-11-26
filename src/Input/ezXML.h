#ifndef ezXML_H
#define ezXML_H

#include <vector>
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
	XMLString* value;
};

struct XMLNode
{
	XMLString* tag;
	XMLString* innerText;
	XMLNode* parent;
	std::vector<XMLAttrib> attributes;
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
* Consume whitespace until non-whitespace found
*/
inline void ConsumeWhitespace(XMLParser* parser)
{
	while (isspace(parser->buf[parser->position]))
	{
		// clamp to length of buffer
		if (parser->position + 1 >= parser->length)
			return;
		else
			parser->position++;
	}
}

inline std::vector<XMLAttrib> TokenizeOpeningTag(XMLParser* parser, XMLString* openingTag)
{
	std::vector<XMLAttrib> foundAttributes;
	
	enum class TokenizerState : uint8_t
	{
		InitialWhitespaceCheck,
		NewToken,
		BuildToken,
		ConsumeWhitespace,
		CompleteToken,
		EndOfString
	};

	TokenizerState currState = TokenizerState::InitialWhitespaceCheck;
	TokenizerState nextState = TokenizerState::InitialWhitespaceCheck;
	
	std::string tagName = "";
	std::string currToken = "";
	std::string prevToken = "";

	// this bool will be used to include whitespace when tokenizing attribute's value
	bool tokenizingAttributeValue = false;

	auto currChar = openingTag->value.begin();
	while (currChar != openingTag->value.end())
	{
		/*
		* BIG NOTE: 
		* attr="value" and attr='value' both allowed
		* attr     =  "value"				 allowed
		* tagName       attr="value"         allowed
		* 
		* TODO: end of string handling
		* TODO: multiple attributes
		* TODO: allow single quotes
		* TODO: change to use permitted character set lookups
		*/
		switch (currState)
		{
			case TokenizerState::InitialWhitespaceCheck:
			{
				// change isspace to lookup set of whitespaces
				if (isspace(currChar[0]))
				{	
					// handle error
					std::cout << "InitialWhitespaceCheck: XML not well formed. Char: " << currChar[0] << std::endl;
					std::exit(-1);
				}
				else
					nextState = TokenizerState::NewToken;
			}
			break;

			case TokenizerState::NewToken:
			{
				// if char is in allowed charset
				if (currChar[0] != '>')
				{
					nextState = TokenizerState::BuildToken;
				}
				else
				{
					// handle error
					std::cout << "NewToken: XML not well formed. Char: " << currChar[0] << std::endl;
					std::exit(-1);
				}
			}
			break;

			case TokenizerState::BuildToken:
			{
				// if tokenizingAttributeValue, add everything except "
				if (tokenizingAttributeValue)
				{
					currToken += currChar[0];
					currChar++;

					// if reached end of value, denoted by closing ",
					// not tokenizing attribute value anymore,
					// and handle complete attribute value token
					if (currChar[0] == '"')
					{
						tokenizingAttributeValue = false;
						nextState = TokenizerState::CompleteToken;
					}
				} 
				// if currChar is opening ",
				// tick forward, now tokenizing attribute value,
				// and move to building attribute value token
				else if (currChar[0] == '"')
				{
					currChar++;
					tokenizingAttributeValue = true;
					nextState = TokenizerState::BuildToken;
				}
				// if char is in allowed charset
				else if (currChar[0] != '>' && currChar[0] != '=' && !isspace(currChar[0]) && currChar[0] != '"')
				{

					currToken += currChar[0];
					currChar++;
					nextState = TokenizerState::BuildToken;

				}
				// if char is one of the delims (make charset for these)
				else if (currChar[0] == '>' || currChar[0] == '=' || isspace(currChar[0]))
				{
					nextState = TokenizerState::CompleteToken;
				}
				else
				{
					// handle error
					std::cout << "BuildToken: XML not well formed. Char: " << currChar[0] << std::endl;
					std::exit(-1);
				}
			}
			break;

			case TokenizerState::ConsumeWhitespace:
			{
				if (isspace(currChar[0]))
				{
					currChar++;
					nextState = TokenizerState::ConsumeWhitespace;
				}
				// if whitespace just cleared,
				// and currChar is quote,
				// then the case encountered is: 
				// attrib= "value" (1 or more whitespace between = and opening quote (which is allowed by xml spec)
				else if (currChar[0] == '"')
				{
					nextState = TokenizerState::BuildToken;
				}
				else
				{
					nextState = TokenizerState::CompleteToken;
				}
			}
			break;

			case TokenizerState::CompleteToken:
			{
				// skip all whitespace
				// but not when tokenizing attribute's value, since we preserve whitespace there
				if (isspace(currChar[0]) && !tokenizingAttributeValue)
				{
					nextState = TokenizerState::ConsumeWhitespace;
				}
				// if char is in allowed charset,
				// then tag name has been tokenized
				else if (currChar[0] != '>' && currChar[0] != '=' && currChar[0] != '"' && !isspace(currChar[0]))
				{
					if (tagName.empty())
					{
						tagName = currToken;
						prevToken = currToken;
						//currChar++;
						currToken.clear();
						nextState = TokenizerState::NewToken;
					}
					else
					{
						// potentially hanging token i.e. not tag_name, nor attribute
						std::cout << "CompleteToken: XML not well formed. Char: " << currChar[0] << std::endl;
						std::exit(-1);
					}
				}
				// if char is =,
				// then attribute name has been tokenized
				else if (currChar[0] == '=')
				{
					// save attribute name, so we can set it later
					// ...when we have its value
					prevToken = currToken;
					currChar++;
					currToken.clear();
					nextState = TokenizerState::NewToken;
				}
				// if char is ",
				// then attribute value has been tokenized
				else if (currChar[0] == '"')
				{
					XMLString* attributeName = new XMLString;
					XMLString* attributeValue = new XMLString;

					attributeName->value = prevToken;
					attributeName->length = prevToken.size();

					attributeValue->value = currToken;
					attributeValue->length = currToken.size();

					XMLAttrib foundAttrib{ attributeName, attributeValue };
					foundAttributes.push_back(foundAttrib);

					nextState = TokenizerState::EndOfString;
				}
			}
			break;

			case TokenizerState::EndOfString:
			{
				currChar++;
			}
		}

		currState = nextState;
	}

	
	openingTag->value = tagName;
	openingTag->length = tagName.size();
	return foundAttributes;
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
	std::vector<XMLAttrib> attributes = TokenizeOpeningTag(parser, openingTag);
	if (attributes.size() != 0)
	{
		std::cout << openingTag->value << std::endl;
		std::cout << attributes[0].name->value << ":" << attributes[0].value->value << std::endl;


		std::exit(-1);
	}

	//TODO: check for self-closing tag

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