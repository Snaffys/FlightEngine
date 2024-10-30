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