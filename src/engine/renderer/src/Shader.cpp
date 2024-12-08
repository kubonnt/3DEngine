#include "../include/Shader.h"

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& cachePath)
	: loadedFromCache(true), vertexShaderPath(vertexPath), fragmentShaderPath(fragmentPath)
{
	std::string vertexHashPath = cachePath + "_vertex.hash";
	std::string fragmentHashPath = cachePath + "_fragment.hash";

	bool vertexChanged = ShaderSourceChanged(vertexPath, vertexHashPath);
	bool fragmentChanged = ShaderSourceChanged(fragmentPath, fragmentHashPath);

	if (vertexChanged || fragmentChanged || !LoadShadersFromBinary(cachePath))
	{
		if (vertexChanged)
		{
			CompileAndAttachShader(GL_VERTEX_SHADER, vertexPath);
		}
		if (fragmentChanged)
		{
			CompileAndAttachShader(GL_FRAGMENT_SHADER, fragmentPath);
		}

		if (LinkShaderProgram())
		{
			SaveShadersToBinary(cachePath);
			std::cout << "Shader compiled from source.\n" << vertexPath << fragmentPath << std::endl;
			loadedFromCache = false;
		}
		else
		{
			std::cerr << "Shader failed to compile and link from source.\n";
		}
		// Load shader sources and create program
		//std::string vertexSource = LoadShaderSource(vertexPath);
		//std::string fragmentSource = LoadShaderSource(fragmentPath);
		//CreateShaderProgram(vertexSource, fragmentSource);
		//SaveShaderBinary(cachePath);
		//std::cout << "Shader compiled from source.\n";
	} 
	else
	{
		std::cout << "Shader loaded from cache.\n" << cachePath << std::endl;;
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

	std::string source = buffer.str();
	source.erase(std::remove(source.begin(), source.end(), '\r'), source.end()); // Normalize line endings

	return source;
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

bool Shader::ShaderSourceChanged(const std::string& shaderPath, const std::string& hashPath)
{
	std::hash<std::string> hasher;

	// Load shader source and calculate hash
	std::string shaderSource = LoadShaderSource(shaderPath);
	size_t shaderHash = hasher(shaderSource);

	// Try to read the existing hash from the binary hash file
	size_t oldHash = 0;
	std::ifstream hashFile(hashPath, std::ios::binary);
	if (hashFile.is_open()) 
	{
		hashFile.read(reinterpret_cast<char*>(&oldHash), sizeof(size_t));
	}

	// Compare the old hash with the new one
	if (shaderHash != oldHash) 
	{
		// Save the new hash to the binary hash file
		std::ofstream hashOutFile(hashPath, std::ios::binary);
		if (!hashOutFile.is_open()) 
		{
			std::cerr << "Failed to open file for writing shader hash: " << hashPath << std::endl;
		}
		else 
		{
			hashOutFile.write(reinterpret_cast<const char*>(&shaderHash), sizeof(size_t));
		}
		return true; // Shader source changed
	}

	return false; // No change in shader source
}

bool Shader::LinkShaderProgram()
{
	ID = glCreateProgram();

	// Attach compiled or loaded shaders
	for (const auto& [shaderType, shaderID] : compiledShaderIDs)
	{
		glAttachShader(ID, shaderID);
	}

	glLinkProgram(ID);

	GLint success = 0;
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetProgramInfoLog(ID, 512, nullptr, infoLog);
		std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;

		// Recompile the shaders if linking failed
		for (const auto& [shaderType, shaderID] : compiledShaderIDs)
		{
			glDeleteShader(shaderID);
		}
		compiledShaderIDs.clear();

		std::cout << "Attempting to recompile shaders due to linking failure.\n";

		// Recompile shaders from source
		CompileAndAttachShader(GL_VERTEX_SHADER, vertexShaderPath);  // Replace with your path variables
		CompileAndAttachShader(GL_FRAGMENT_SHADER, fragmentShaderPath);

		glLinkProgram(ID);
		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(ID, 512, nullptr, infoLog);
			std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED after recompilation\n" << infoLog << std::endl;
			return false;
		}
	}

	// Clean up individual shader objects after linking
	for (const auto& [shaderType, shaderID] : compiledShaderIDs)
	{
		glDeleteShader(shaderID);
	}

	compiledShaderIDs.clear();

	return true;
}

void Shader::CompileAndAttachShader(GLenum shaderType, const std::string& shaderPath)
{
	std::string shaderSource = LoadShaderSource(shaderPath);
	unsigned int shaderID = CompileShader(shaderType, shaderSource);
	compiledShaderIDs[shaderType] = shaderID;
}

bool Shader::LoadShadersFromBinary(const std::string& binaryPath)
{
	GLint numFormats = 0;
	glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &numFormats);
	if (numFormats == 0)
	{
		std::cerr << "No supported program binary formats found on this system." << std::endl;
		return false; // Indicates shader binary is not supported
	}

	std::ifstream binaryFile(binaryPath, std::ios::binary);
	if (!binaryFile.is_open()) 
	{
		std::cerr << "Failed to open shader cache binary file: " << binaryPath << std::endl;
		return false;
	}

	GLenum format;
	binaryFile.read(reinterpret_cast<char*>(&format), sizeof(format));

	GLint binaryLength;
	binaryFile.read(reinterpret_cast<char*>(&binaryLength), sizeof(binaryLength));

	std::vector<char> binary(binaryLength);
	if (!binaryFile.read(binary.data(), binaryLength)) 
	{
		std::cerr << "Failed to read shader binary data from cache." << std::endl;
		return false;
	}

	ID = glCreateProgram();
	glProgramBinary(ID, format, binary.data(), binaryLength);

	// Check if loading was successful
	GLint success = 0;
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetProgramInfoLog(ID, 512, nullptr, infoLog);
		std::cerr << "ERROR::SHADER::BINARY::LINKING_FAILED\n" << infoLog << std::endl;
		return false;
	}

	std::cout << "Shader program successfully loaded from binary.\n";
	return true;

	/*
	GLint numFormats = 0;
	glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &numFormats);
	if (numFormats == 0)
	{
		std::cerr << "No supported program binary formats found on this system." << std::endl;
		return false; // Indicates shader binary is not supported
	}

	std::ifstream binaryFile(binaryPath, std::ios::binary);
	if (!binaryFile.is_open()) {
		std::cerr << "Failed to open shader cache binary file: " << binaryPath << std::endl;
		return false;
	}

	while (binaryFile.peek() != EOF)
	{
		GLenum shaderType;
		binaryFile.read(reinterpret_cast<char*>(&shaderType), sizeof(shaderType));

		GLenum format;
		binaryFile.read(reinterpret_cast<char*>(&format), sizeof(format));

		GLint binaryLength;
		binaryFile.read(reinterpret_cast<char*>(&binaryLength), sizeof(binaryLength));

		std::vector<char> binary(binaryLength);
		if (!binaryFile.read(binary.data(), binaryLength)) {
			std::cerr << "Failed to read shader binary data from cache." << std::endl;
			return false;
		}

		unsigned int shaderID = glCreateShader(shaderType);
		glShaderBinary(1, &shaderID, format, binary.data(), binaryLength);

		// Check for errors after loading the binary
		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			std::cerr << "OpenGL Error after glShaderBinary: " << error << std::endl;
			return false;
		}

		compiledShaderIDs[shaderType] = shaderID;
	}

	// Link the program again to ensure all uniforms are correctly registered
	if (!LinkShaderProgram()) {
		std::cerr << "Failed to link shader program from binary. Falling back to source compilation." << std::endl;
		return false;
	}

	return true;
	*/
}

void Shader::SaveShadersToBinary(const std::string& binaryPath)
{
	GLint binaryLength;
	glGetProgramiv(ID, GL_PROGRAM_BINARY_LENGTH, &binaryLength);
	std::vector<char> binary(binaryLength);

	GLenum format;
	glGetProgramBinary(ID, binaryLength, nullptr, &format, binary.data());

	std::ofstream binaryFile(binaryPath, std::ios::binary);
	if (!binaryFile.is_open()) {
		std::cerr << "Failed to open file for writing shader cache: " << binaryPath << std::endl;
		return;
	}

	// Write the format and binary data to the file
	binaryFile.write(reinterpret_cast<const char*>(&format), sizeof(format));
	binaryFile.write(reinterpret_cast<const char*>(&binaryLength), sizeof(binaryLength));
	binaryFile.write(binary.data(), binaryLength);

	std::cout << "Shader binary successfully saved to: " << binaryPath << std::endl;

	/*
	for (const auto& [shaderType, shaderID] : compiledShaderIDs)
	{
		GLint binaryLength;
		glGetShaderiv(shaderID, GL_PROGRAM_BINARY_LENGTH, &binaryLength);
		std::vector<char> binary(binaryLength);

		GLenum format;
		glGetProgramBinary(shaderID, binaryLength, nullptr, &format, binary.data());

		binaryFile.write(reinterpret_cast<const char*>(&shaderType), sizeof(shaderType));
		binaryFile.write(reinterpret_cast<const char*>(&format), sizeof(format));
		binaryFile.write(reinterpret_cast<const char*>(&binaryLength), sizeof(binaryLength));
		binaryFile.write(binary.data(), binaryLength);
	}
	*/

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
		return it->second;
	}

	int location = glGetUniformLocation(ID, name.c_str());
	if (location == -1)
	{
		std::cerr << "Warning: Uniform '" << name << "' not found in the shader." << std::endl;
	}

	uniformLocationCache[name] = location;
	return location;
}

bool Shader::WasLoadedFromCache() const
{
	return loadedFromCache;
}