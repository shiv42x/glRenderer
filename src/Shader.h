#pragma once

#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

class Shader
{
private:
	std::string m_vertexPath;
    std::string m_fragPath;
	unsigned int m_rendererID;
    std::unordered_map<std::string, int> m_uniformLocationCache;

    void CheckCompileErrors(unsigned int shader, std::string type);

public:
    Shader(const std::string& vertexPath, const std::string& fragPath);
    ~Shader();

	void Bind() const;
	void Unbind() const;

	// uniforms
    int GetUniformLocation(const std::string& name);
    void setBool(const std::string& name, bool value);
    void setInt(const std::string& name, int value);
    void setFloat(const std::string& name, float value);
    void setVec2(const std::string& name, const glm::vec2& value);
    void setVec2(const std::string& name, float x, float y);
    void setVec3(const std::string& name, const glm::vec3& value);
    void setVec3(const std::string& name, float x, float y, float z);
    void setVec4(const std::string& name, const glm::vec4& value);
    void setVec4(const std::string& name, float x, float y, float z, float w);
    void setMat2(const std::string& name, const glm::mat2& mat);
    void setMat3(const std::string& name, const glm::mat3& mat);
    void setMat4(const std::string& name, const glm::mat4& mat);
};