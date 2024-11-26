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
	std::vector<XMLAttrib> attributes;

	XMLNode* parent;
	XMLNode** children;
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
* Consume whitespace until non-whitespace found.
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

/*
* Using a state machine,
* tokenizes (technically lexes) entire opening tag, 
* resets openingTag to only contain tag name, 
* returns array with XMLAttrib structs as name & value pairs.
* 
* E.g. openingTag: tag_name attrib1="value" attrib2="value
* sets openingTag's value to only tag_name
* returns array of found attributes,
* i.e. attrib1 & attrib2 along with their values as XMLAttrib structs
* 
* TODO: make this into a pure tokenizer, with user defined delimiters?
*/
inline std::vector<XMLAttrib> TokenizeAttributes(XMLParser* parser, XMLString* openingTag)
{
	enum class TokenizerState : uint8_t
	{
		InitialWhitespaceCheck,
		NewToken,
		BuildToken,
		Whitespace,
		CompleteToken,
		EndOfString
	};

	std::vector<XMLAttrib> foundAttributes;

	TokenizerState currState = TokenizerState::InitialWhitespaceCheck;
	TokenizerState nextState = TokenizerState::InitialWhitespaceCheck;
	
	std::string tagName = "";
	std::string currToken = "";
	std::string currAttribName = "";

	// this bool will be used to include whitespaces when tokenizing attribute's value
	bool tokenizingAttributeValue = false;

	auto currChar = openingTag->value.begin();
	while (currChar != openingTag->value.end())
	{
		/*
		* BIG NOTE: 
		* /attr="value"/ and /attr='value'/		both allowed
		* /attr     =  "value"/					allowed
		* /tagName       attr="value"/			allowed
		* 
		* TODO: end of string handling
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
				// and not end of string
				// this is wrong, input string never has < or >
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
				else if (currChar[0] != '=' && !isspace(currChar[0]) && currChar[0] != '"')
				{

					currToken += currChar[0];
					currChar++;
					nextState = TokenizerState::BuildToken;

				}
				// if char is one of the delims (make charset for these)
				else if (currChar[0] == '=' || isspace(currChar[0]))
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

			case TokenizerState::Whitespace:
			{
				// if space, consume it!
				if (isspace(currChar[0]))
				{
					currChar++;
					nextState = TokenizerState::Whitespace;
				}
				// if currToken is empty,
				// build next token
				else if (currToken.empty())
				{
					nextState = TokenizerState::BuildToken;
				}
				// else, have a complete tokenizable string
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
					nextState = TokenizerState::Whitespace;
				}
				// if char is in allowed charset,
				// then tag name has been tokenized
				else if (currChar[0] != '>' && currChar[0] != '=' && currChar[0] != '"' && !isspace(currChar[0]))
				{
					if (tagName.empty())
					{
						tagName = currToken;
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
					currAttribName = currToken;
					currChar++;
					currToken.clear();
					nextState = TokenizerState::NewToken;
				}
				// if char is " (closing),
				// then attribute value has been tokenized
				else if (currChar[0] == '"')
				{
					XMLString* attributeName = new XMLString;
					XMLString* attributeValue = new XMLString;

					attributeName->value = currAttribName;
					attributeName->length = currAttribName.size();

					attributeValue->value = currToken;
					attributeValue->length = currToken.size();

					XMLAttrib foundAttrib{ attributeName, attributeValue };
					foundAttributes.push_back(foundAttrib);

					currAttribName.clear();
					currToken.clear();
					currChar++;
					nextState = TokenizerState::NewToken;
				}
			}
			break;

			case TokenizerState::EndOfString:
			{
				currChar++;
			}
			break;
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
* Accumulate any char,
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
* and call ParseEnding to continue 
* up to '>'
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

	std::vector<XMLAttrib> attributes = TokenizeAttributes(parser, openingTag);

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
	//TODO: nested nodes, note: xml allows nodes to contain text content, as well as child nodes

	XMLNode* node = new XMLNode();
	node->tag = openingTag;
	node->innerText = content;
	node->attributes = attributes;
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

#endif