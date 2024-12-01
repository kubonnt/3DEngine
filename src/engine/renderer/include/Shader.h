#pragma once 

#include <string>
#include <unordered_map>
#include <glm.hpp>

class Shader
{
public:
	Shader(const std::string& vertexPath, const std::string& fragmentPath);
	void Use() const;

	void SetMat4(const std::string& name, const glm::mat4& value) const;

	~Shader();

private:
	unsigned int ID;

	std::string LoadShaderSource(const std::string& filepath);
	unsigned int CompileShader(unsigned int type, const std::string& source);
	void CreateShaderProgram(const std::string& vertexSource, const std::string& fragmentSource);

	mutable std::unordered_map<std::string, int> uniformLocationCache;

	int GetUniformLocation(const std::string& name) const;
};
