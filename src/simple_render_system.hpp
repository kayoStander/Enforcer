#pragma once

#include "lve_device.hpp"
#include "lve_frame_info.hpp"
#include "lve_pipeline.hpp"

// std
#include <memory>

namespace lve {
class SimpleRenderSystem {
public:
  SimpleRenderSystem(LveDevice &device, VkRenderPass renderPass,
                     std::vector<VkDescriptorSetLayout> descriptorSetLayouts);
  ~SimpleRenderSystem();

  SimpleRenderSystem(const SimpleRenderSystem &) = delete;
  SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;

  void renderGameObjects(FrameInfo &frameInfo);

private:
  void
  createPipelineLayout(std::vector<VkDescriptorSetLayout> descriptorSetLayouts);
  void createPipeline(VkRenderPass renderPass);

  LveDevice &lveDevice;

  std::unique_ptr<LvePipeline> lvePipeline;
  VkPipelineLayout pipelineLayout;
};
} // namespace lve
