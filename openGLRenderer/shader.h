#ifndef SHADER_H
#define SHADER_H

//necessary includes
#include <glad/glad.h>
#include <iostream>
#include<fstream>
#include <sstream>
#include<string>
#include <glm/glm.hpp>
/*
* TODO: transfer this function code to c++ file to avoid setting off the ODR for multiple includes
*/
class Shader {
public:
	//program id
	unsigned int ID;
	
	//constructor
	Shader(const char* vertexPath, const char* fragmentPath) {
		//retrieve vertex and fragment source code from file path
		std::string vertexCode;
		std::string fragmentCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		//ensure ifstreams can throw excpetions
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try {
			//open file
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			std::stringstream vShaderStream, fShaderStream;
			//read file's buffer content to stream
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			//close file handlers
			vShaderFile.close();
			fShaderFile.close();
			//convert stream to string
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch (std::ifstream::failure e) {
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
		}
		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();

		//compile shaders
		unsigned int vertex, fragment;
		int success;
		char infoLog[512];

		//we pass glvertexshader because we want a vertex shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		//1st arg: shader object to compile, 2nd arg: how many strings as source code, 3rd: soruce code.
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		//make sure that all worked
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		//Fragment shader time
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		//double check
		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(fragment, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT_SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
		//create and link program
		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		glLinkProgram(ID);
		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(ID, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER_PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}
		//clean up shaders, since they're linked we can delete
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}
	//use program duh
	void use() {
		glUseProgram(ID);
	}
	//utility uniform functions
	void setBool(const std::string& name, bool value) const {
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}
	void setInt(const std::string& name, int value) const {
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
	void setFloat(const std::string& name, float value) const {
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}
	void setVec3(const std::string& name, float x, float y, float z) const
	{
		glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
	}
	void setVec3(const std::string& name, glm::vec3 vec) const
	{
		glUniform3f(glGetUniformLocation(ID, name.c_str()), vec.x, vec.y, vec.z);
	}
	void setMat4(const std::string& name, glm::mat4 mat) const {
		//this gl function is expecting a continguous array of floats, which is why we point to the arrays first value
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
};
#endif
