#pragma once

#include "lve_model.hpp"

// libs
#include <glm/gtc/matrix_transform.hpp>

// std
#include <memory>
#include <unordered_map>

namespace lve {

struct TransformComponent {
  glm::vec3 translation{};
  glm::vec3 scale{1.f, 1.f, 1.f};
  glm::vec3 rotation{};

  // Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
  // Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
  // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
  glm::mat4 mat4();

  glm::mat3 normalMatrix();
};

struct PointLightComponent {
  float lightIntensity = 1.0f;
};

struct fogComponent {
  glm::vec4 color = glm::vec4(
      255.0f, 255.0f, 255.0f,
      1.0f); // later see if starting with {} is equal to start with ()
  float fogStart = 10.0f;
  float fogEnd = 300.0f;
};

class LveGameObject {
public:
  using id_t = unsigned int;
  using Map = std::unordered_map<id_t, LveGameObject>;

  static LveGameObject createGameObject() {
    static id_t currentId = 0;
    return LveGameObject{currentId++};
  }

  static LveGameObject makePointLight(float intensity = 10.f,
                                      float radius = .1f,
                                      glm::vec3 color = glm::vec3(1.f));

  LveGameObject(const LveGameObject &) = delete;
  LveGameObject &operator=(const LveGameObject &) = delete;
  LveGameObject(LveGameObject &&) = default;
  LveGameObject &operator=(LveGameObject &&) = default;

  id_t getId() { return id; }

  glm::vec3 color{};
  TransformComponent transform{};

  std::shared_ptr<LveModel> model{};
  std::unique_ptr<PointLightComponent> pointLight = nullptr;
  std::unique_ptr<fogComponent> fog = nullptr;

private:
  LveGameObject(id_t objId) : id{objId} {}

  id_t id;
};
} // namespace lve
