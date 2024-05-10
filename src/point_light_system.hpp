#pragma once

#include "lve_device.hpp"
#include "lve_frame_info.hpp"
#include "lve_pipeline.hpp"

// std
#include <memory>
#include <vector>

namespace lve {
class PointLightSystem {
public:
  PointLightSystem(LveDevice &device, VkRenderPass renderPass,
                   std::vector<VkDescriptorSetLayout> descriptorSetLayouts);
  ~PointLightSystem();

  PointLightSystem(const PointLightSystem &) = delete;
  PointLightSystem &operator=(const PointLightSystem &) = delete;

  void update(FrameInfo &frameInfo, GlobalUbo &ubo);
  void render(FrameInfo &frameInfo);

private:
  void
  createPipelineLayout(std::vector<VkDescriptorSetLayout> descriptorSetLayouts);
  void createPipeline(VkRenderPass renderPass);

  LveDevice &lveDevice;

  std::unique_ptr<LvePipeline> lvePipeline;
  VkPipelineLayout pipelineLayout;
};
} // namespace lve
