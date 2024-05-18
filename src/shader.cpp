#include "shader.hpp"
#include <filesystem>
#include <format>
#include <fstream>
#include <fstream>
#include <glad/glad.h>
#include <iostream>

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
{
  this->id = glCreateProgram();
  uint32_t vert = this->compile(GL_VERTEX_SHADER, std::filesystem::current_path() / "shaders" / vertexPath);
  uint32_t frag = this->compile(GL_FRAGMENT_SHADER, std::filesystem::current_path() / "shaders" / fragmentPath);

  glAttachShader(this->id, vert);
  glAttachShader(this->id, frag);
  glLinkProgram(this->id);
  glValidateProgram(this->id);

  glDeleteShader(vert);
  glDeleteShader(frag);
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

uint32_t Shader::compile(uint32_t type, const std::string& filename)
{
  std::stringstream buffer;
  buffer << std::ifstream(filename).rdbuf();
  std::string source = buffer.str();

  uint32_t id = glCreateShader(type);
  const char* src = source.c_str();
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

