#include "../include/Shader.h"
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <gtc/type_ptr.hpp>

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& binaryPath) 
	: loadedFromCache(false)
{
	bool sourceChanged = ShaderSourceChanged(vertexPath, fragmentPath);
	std::cout << "ShaderSourceChanged returned: " << sourceChanged << std::endl;

	if (sourceChanged || !LoadShaderBinary(binaryPath))
	{
		// Load shader sources and create program
		std::string vertexSource = LoadShaderSource(vertexPath);
		std::string fragmentSource = LoadShaderSource(fragmentPath);
		CreateShaderProgram(vertexSource, fragmentSource);
		SaveShaderBinary(binaryPath);
		std::cout << "Shader compiled from source.\n";
	} else
	{
		loadedFromCache = true;
		std::cout << "Shader loaded from cache.\n";
	}
}

Shader::~Shader()
{
	glDeleteProgram(ID);
}

std::string Shader::LoadShaderSource(const std::string& filepath)
{
	std::ifstream file(filepath);
	if (!file.is_open())
	{
		std::cerr << "Failed to open shader file: " << filepath << std::endl;
		return "";
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
	unsigned int shader = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(shader, 1, &src, nullptr);
	glCompileShader(shader);

	int success = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(shader, 512, nullptr, infoLog);
		std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	return shader;
}

void Shader::CreateShaderProgram(const std::string& vertexSource, const std::string& fragmentSource)
{
	unsigned int vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource);
	unsigned int fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);

	int success = 0;
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetProgramInfoLog(ID, 512, nullptr, infoLog);
		std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

bool Shader::LoadShaderBinary(const std::string& binaryPath)
{
	std::cout << "Loading shader cache from: " << binaryPath << std::endl;
	std::ifstream binaryFile(binaryPath, std::ios::binary);
	if (!binaryFile.is_open())
	{
		return false;
	}

	// Get the size of the file
	binaryFile.seekg(0, std::ios::end);
	std::streamsize size = binaryFile.tellg();
	binaryFile.seekg(0, std::ios::beg);

	// Read the binary data
	std::vector<char> buffer(size);
	if (!binaryFile.read(buffer.data(), size))
	{
		std::cerr << "Failed to read shader binary file: " << binaryPath << std::endl;
		return false;
	}

	GLenum format;
	binaryFile.read(reinterpret_cast<char*>(&format), sizeof(format));

	// Creat the program and load the binary
	ID = glCreateProgram();
	glProgramBinary(ID, format, buffer.data(), size);

	// Check if loading was successful
	GLint success;
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(ID, 512, nullptr, infoLog);
		std::cerr << "ERROR::SHADER::BINARY::LINKING_FAILED\n" << infoLog << std::endl;
		return false;
	}

	return true;
}

void Shader::SaveShaderBinary(const std::string& binaryPath)
{
	std::cout << "Saving shader cache to: " << binaryPath << std::endl;
	GLint binaryLength;
	glGetProgramiv(ID, GL_PROGRAM_BINARY_LENGTH, &binaryLength);
	std::vector<char> binary(binaryLength);

	GLenum format;
	glGetProgramBinary(ID, binaryLength, nullptr, &format, binary.data());

	std::ofstream binaryFile(binaryPath, std::ios::binary);
	if (!binaryFile.is_open()) {
		std::cerr << "Failed to open file for writing shader binary: " << binaryPath << std::endl;
		return;
	}

	// Write the format and binary data to the file
	binaryFile.write(reinterpret_cast<char*>(&format), sizeof(format));
	binaryFile.write(binary.data(), binaryLength);
}

bool Shader::ShaderSourceChanged(const std::string& vertexPath, const std::string& fragmentPath)
{
	std::hash<std::string> hasher;

	// Get hash of vertex shader source
	std::string vertexSource = LoadShaderSource(vertexPath);
	size_t vertexHash = hasher(vertexSource);

	// Get hash of fragment shader source
	std::string fragmentSource = LoadShaderSource(fragmentPath);
	size_t fragmentHash = hasher(fragmentSource);

	// Combine hashes for a simple cache validation mechanism
	size_t combinedHash = vertexHash ^ (fragmentHash << 1);

	// Check if there is a hash file, or if the hash is different
	std::ifstream hashFile("shader_hash.hash");
	if (!hashFile.is_open()) {
		std::ofstream newHashFile("shader_hash.hash");
		newHashFile << combinedHash;
		return true;  // No hash file means shader needs recompiling
	}

	size_t oldHash;
	hashFile >> oldHash;

	if (oldHash != combinedHash) {
		std::ofstream newHashFile("shader_hash.txt");
		newHashFile << combinedHash;
		return true;  // Hash mismatch means shader source has changed
	}

	return false; // Hash match means shader source is unchanged
}

void Shader::Use() const
{
	glUseProgram(ID);
}

void Shader::SetMat4(const std::string& name, const glm::mat4& mat) const
{
	GLint location = glGetUniformLocation(ID, name.c_str());
	if (location == -1) 
	{
		std::cerr << "Warning: Uniform '" << name << "' not found in the shader." << std::endl;
	}
	glUniformMatrix4fv(location, 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetVec3(const std::string& name, const glm::vec3& value) const 
{
	glUniform3fv(GetUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::SetFloat(const std::string& name, float value) const 
{
	glUniform1f(GetUniformLocation(name), value);
}

int Shader::GetUniformLocation(const std::string& name) const
{
	auto it = uniformLocationCache.find(name);
	if (it != uniformLocationCache.end())
	{
		return it->second; // Return the cached uniform location
	}
	int location = glGetUniformLocation(ID, name.c_str());
	uniformLocationCache.insert({ name, location }); // Insert the new location into the cache

	return location;
}

bool Shader::WasLoadedFromCache() const
{
	return loadedFromCache;
}