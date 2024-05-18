#pragma once
#include <cstdint>
#include <string>

class Shader {
public:
  // the program ID
  uint32_t id;

  // constructor reads and builds the shader
  Shader(const std::string &vertexPath, const std::string &fragmentPath);
  // use/activate the shader
  void use();
  // utility uniform functions
  void setBool(const std::string &name, bool value) const;
  void setInt(const std::string &name, int32_t value) const;
  void setFloat(const std::string &name, float value) const;
private:
  uint32_t compile(uint32_t type, const std::string& source);
};
