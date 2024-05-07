#pragma once

#include "lve_camera.hpp"
#include "lve_game_object.hpp"

// lib
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace lve {
#define MAX_LIGHTS 10
struct PointLight {
  glm::vec4 position{}; // ignore w
  glm::vec4 color{};
};
struct GlobalUbo {
  glm::mat4 projection{1.f};
  glm::mat4 view{1.f};
  glm::vec4 ambientColor{1.f, 1.f, 1.f, .02f}; // w = intensity
  PointLight pointLights[MAX_LIGHTS];
  int numLights;
};
struct FrameInfo {
  int frameIndex;
  float frameTime;
  VkCommandBuffer commandBuffer;
  LveCamera &camera;
  VkDescriptorSet globalDescriptorSet;
  LveGameObject::Map &gameObjects;
};
} // namespace lve
