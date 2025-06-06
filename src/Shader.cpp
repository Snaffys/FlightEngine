#include <Shader.h>

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
	vertexShader = compileShader(GL_VERTEX_SHADER, vertexPath);
	fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentPath);

	id = glCreateProgram();
	glAttachShader(id, vertexShader);
	glAttachShader(id, fragmentShader);
	glLinkProgram(id);

	glGetShaderiv(id, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(id, 512, NULL, infoLog);
		printf("Shaders linking failed\n%s\n", infoLog);
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}


Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath) {
	vertexShader = compileShader(GL_VERTEX_SHADER, vertexPath);
	fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentPath);
	geometryShader = compileShader(GL_GEOMETRY_SHADER, geometryPath);

	id = glCreateProgram();
	glAttachShader(id, vertexShader);
	glAttachShader(id, fragmentShader);
	glAttachShader(id, geometryShader);
	glLinkProgram(id);

	glGetShaderiv(id, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(id, 512, NULL, infoLog);
		printf("Shaders linking failed\n%s\n", infoLog);
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteShader(geometryShader);
}

void Shader::Activate() {
	glUseProgram(id);
}

int Shader::compileShader(const GLuint type, const char* filename) {
	// Get pointer to the array that contains GLSL code
	const char* shaderSource = getFileContents(filename);
	
	// Create and compile the shader
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &shaderSource, NULL);
	glCompileShader(shader);
	
	// Check for errors
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		printf(type == GL_VERTEX_SHADER ? "Vertex " : GL_FRAGMENT_SHADER ? "Fragment " : "Geometry ");
		printf("shader %s compilation failed\n%s\n", filename, infoLog);
	}
	return shader;
}

char* Shader::getFileContents(const char* filename) {
	std::ifstream stream(filename, std::ios::binary);
	if (stream) {
		stream.seekg(0, std::ios::end);
		int arrSize = stream.tellg() * sizeof(char) + 1;
		char* contents = (char*)malloc(arrSize);
		contents[arrSize - 1] = '\0';
		stream.seekg(0, std::ios::beg);
		stream.read(contents, arrSize);
		stream.close();
		return(contents);
	}
	printf("Error in getting %s content", filename);
	exit(1);
}


void Shader::setInt(const GLchar* name, int value) {
	glUniform1i(glGetUniformLocation(id, name), value);
}

void Shader::setFloat(const GLchar* name, float value) {
	glUniform1f(glGetUniformLocation(id, name), value);
}

void Shader::setVec2(const GLchar* name, const glm::vec2& values) {
	glUniform2fv(glGetUniformLocation(id, name), 1, &values[0]);
}

void Shader::setVec3(const GLchar* name, const glm::vec3& values) {
	glUniform3fv(glGetUniformLocation(id, name), 1, &values[0]);
}

void Shader::setVec4(const GLchar* name, const glm::vec4& values) {
	glUniform4fv(glGetUniformLocation(id, name), 1, &values[0]);
}

void Shader::setVec3(const GLchar* name, float firstValue, float secondValue, float thirdValue) {
	glUniform3f(glGetUniformLocation(id, name), firstValue, secondValue, thirdValue);
}

void Shader::setMat4(const GLchar* name, const glm::mat4& mat) {
	glUniformMatrix4fv(glGetUniformLocation(id, name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setBool(const GLchar* name, bool value) {
	glUniform1i(glGetUniformLocation(id, name), value);
}