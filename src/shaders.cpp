#include <fstream>
#include <glad/glad.h>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>

#include "../headers/shaders.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace Shaders {
	std::string loadSourceFromFile(const char* fp) {
		std::string src;
		std::ifstream fh;

		fh.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try {
			fh.open(fp);
			std::stringstream sstr;
			sstr << fh.rdbuf();
			fh.close();

			src = sstr.str();
			return src;
		} catch (std::ifstream::failure err) {
			printf("[ERROR] Unable to read file \"%s\\n", fp);
			exit(1);
		}
	}

	unsigned int compile(GLenum shadertype, const char* shaderSource) {
		unsigned int shader;
		shader = glCreateShader(shadertype);
		glShaderSource(shader, 1, &shaderSource, NULL);
		glCompileShader(shader);

		int success;
		char infolog[512];
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader, 512, NULL, infolog);
			printf("[%s] Shader Compilation Error:\n%s\n", shaderSource, infolog);
		}

		return shader;
	}

	unsigned int compileFromFile(GLenum shadertype, const char* file) {
		std::string src = loadSourceFromFile(file);
		unsigned int shader = compile(shadertype, src.c_str());
		return shader;
	}

	unsigned int createProgram(std::vector<unsigned int> shaders) {
		unsigned int shaderProg;
		shaderProg = glCreateProgram();

		// Link the shaders
		for(unsigned int s : shaders) 
			glAttachShader(shaderProg, s);

		glLinkProgram(shaderProg);

		// Check for errors
		int success;
		char infolog[512];
		glGetProgramiv(shaderProg, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shaderProg, 512, NULL, infolog);
			printf("Shader Program Linking Error:\n%s\n", infolog);
		}

		// Delete the shaders when we are done
		for(unsigned int s : shaders)
			glDeleteShader(s);

		return shaderProg;
	}

	Shader::Shader(const char* vertPath, const char* fragPath) {
		unsigned int vertShader = compileFromFile(GL_VERTEX_SHADER, vertPath);
		unsigned int fragShader = compileFromFile(GL_FRAGMENT_SHADER, fragPath);
		std::vector<unsigned int> shaders({vertShader, fragShader});

		// Shader program ID
		id = Shaders::createProgram(shaders);
	}

	void Shader::use() { glUseProgram(id); }

	void Shader::setInt(const std::string &name, int val) const {
		glUniform1i(uniLocation(name), val);
	}

	void Shader::setFloat(const std::string &name, float val) const {
		glUniform1f(uniLocation(name), val);
	}

	void Shader::setBool(const std::string &name, bool val) const {
		glUniform1i(uniLocation(name), (int)val);
	}

	void Shader::setMat4(const std::string &name, glm::mat4 val) const {
		glUniformMatrix4fv(uniLocation(name), 1, GL_FALSE, glm::value_ptr(val));
	}

	void Shader::setVec3(const std::string &name, glm::vec3 val) const {
		glUniform3f(uniLocation(name), val[0], val[1], val[2]);
	}

	unsigned int Shader::uniLocation(const std::string &name) const {
		return glGetUniformLocation(id, name.c_str());
	}
}
