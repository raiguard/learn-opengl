#include "shader.hpp"
#include <format>
#include <fstream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
{
  this->id = glCreateProgram();
  uint32_t vert = this->compile(GL_VERTEX_SHADER, vertexPath);
  uint32_t frag = this->compile(GL_FRAGMENT_SHADER, fragmentPath);

  glAttachShader(this->id, vert);
  glAttachShader(this->id, frag);
  glLinkProgram(this->id);
  glValidateProgram(this->id);

  glDeleteShader(vert);
  glDeleteShader(frag);
}

Shader::~Shader()
{
  glDeleteProgram(this->id);
}

void Shader::use()
{
  glUseProgram(this->id);
}

void Shader::setBool(const std::string& name, bool value) const
{
  glUniform1i(glGetUniformLocation(this->id, name.c_str()), (int32_t)value);
}

void Shader::setInt(const std::string& name, int32_t value) const
{
  glUniform1i(glGetUniformLocation(this->id, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
  glUniform1f(glGetUniformLocation(this->id, name.c_str()), value);
}

void Shader::setMat4(const std::string& name, glm::mat4 value) const
{
  glUniformMatrix4fv(glGetUniformLocation(this->id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setVec3(const std::string& name, float x, float y, float z) const
{
  glUniform3f(glGetUniformLocation(this->id, name.c_str()), x, y, z);
}

void Shader::setVec3(const std::string& name, glm::vec3 value) const
{
  glUniform3fv(glGetUniformLocation(this->id, name.c_str()), 1, glm::value_ptr(value));
}

uint32_t Shader::compile(uint32_t type, const std::string& filename)
{
  std::stringstream buffer;
  buffer << std::ifstream(filename).rdbuf();
  std::string source = buffer.str();

  uint32_t id = glCreateShader(type);
  const char *src = source.c_str();
  glShaderSource(id, 1, &src, nullptr);
  glCompileShader(id);

  int result;
  glGetShaderiv(id, GL_COMPILE_STATUS, &result);
  if (!result)
  {
    int length;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
    std::string message;
    message.resize(length);
    glGetShaderInfoLog(id, length, &length, message.data());
    std::cout << std::format("Failed to compile shader {}:\n\t{}", filename, message) << std::endl;
    exit(-1);
  }

  return id;
}
