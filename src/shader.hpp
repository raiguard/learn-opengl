#pragma once
#include <cstdint>
#include <string>
#include <glm/glm.hpp>

class Shader {
public:
  Shader(const std::string &vertexPath, const std::string &fragmentPath);
  ~Shader();

  void use();
  void setBool(const std::string& name, bool value) const;
  void setInt(const std::string& name, int32_t value) const;
  void setFloat(const std::string& name, float value) const;

  void setMat4(const std::string& name, glm::mat4 value) const;
  void setVec3(const std::string& name, float x, float y, float z) const;

  uint32_t id;
private:
  uint32_t compile(uint32_t type, const std::string &source);
};
