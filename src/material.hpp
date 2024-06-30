#include <glm/glm.hpp>

class Material
{
public:
  Material() = default;
  Material(float ambientX,  float ambientY,  float ambientZ,
           float diffuseX,  float diffuseY,  float diffuseZ,
           float specularX, float specularY, float specularZ,
           float shininess)
    : ambient(ambientX, ambientY, ambientZ)
    , diffuse(diffuseX, diffuseY, diffuseZ)
    , specular(specularX, specularY, specularZ)
    , shininess(shininess)
  {}

  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;
  float shininess = 0.0f;
};
