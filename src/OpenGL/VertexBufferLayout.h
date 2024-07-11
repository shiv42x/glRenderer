#pragma once

#include <vector>

#define GL_UNSIGNED_INT 0x1405
#define GL_FLOAT 0x1406
#define GL_FALSE 0

struct VertexBufferElement
{
	unsigned int type;
	unsigned int count;
	unsigned char normed;

	static unsigned int GetSizeOfType(unsigned int type)
	{
		switch (type)
		{
			case GL_FLOAT:			return 4;
			case GL_UNSIGNED_INT:	return 4;
		}
		return 0;
	}
};

class VertexBufferLayout
{
private:
	std::vector<VertexBufferElement> m_elements;
	unsigned int m_stride;

public:
	VertexBufferLayout()
		: m_stride(0) {}

	void Push(unsigned int type, unsigned int count)
	{
		m_elements.push_back({ type, count, GL_FALSE });
		m_stride += count * VertexBufferElement::GetSizeOfType(type);
	}

	inline const std::vector<VertexBufferElement> GetElements() const { return m_elements; }
	inline unsigned int GetStride() const { return m_stride; }
};