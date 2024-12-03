#pragma once 

#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <glm.hpp>
#include <glad/glad.h>
#include <gtc/type_ptr.hpp>


class Shader
{
public:
	Shader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& cachePath);
	void Use() const;

	void SetMat4(const std::string& name, const glm::mat4& value) const;
	void SetVec3(const std::string& name, const glm::vec3& value) const;
	void SetFloat(const std::string& name, float value) const;

	~Shader();

	bool WasLoadedFromCache() const;

private:
	unsigned int ID;

	std::string LoadShaderSource(const std::string& filepath);
	unsigned int CompileShader(unsigned int type, const std::string& source);
	void CompileAndAttachShader(GLenum shaderType, const std::string& shaderPath);
	bool LinkShaderProgram();
	void CreateShaderProgram(const std::string& vertexSource, const std::string& fragmentSource);

	bool LoadShaderBinary(const std::string& binaryPath); // Depracated
	bool LoadShadersFromBinary(const std::string& binaryPath);
	void SaveShaderBinary(const std::string& binaryPath); // Depracated
	void SaveShadersToBinary(const std::string& binaryPath);
	bool ShaderSourceChanged(const std::string& shaderPath, const std::string& hashPath);

	mutable std::unordered_map<std::string, int> uniformLocationCache;
	std::unordered_map<GLenum, unsigned int> compiledShaderIDs;

	int GetUniformLocation(const std::string& name) const;

	std::string vertexShaderPath;
	std::string fragmentShaderPath;

	bool loadedFromCache;
};
