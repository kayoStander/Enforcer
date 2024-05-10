#pragma once

#include "lve_device.hpp"
#include "lve_frame_info.hpp"
#include "lve_pipeline.hpp"

// std
#include <memory>
#include <vector>

namespace lve {
class AtmosphereSystem {
public:
  AtmosphereSystem(LveDevice &device, VkRenderPass renderPass,
                   std::vector<VkDescriptorSetLayout> globalSetLayout);
  ~AtmosphereSystem();

  AtmosphereSystem(const AtmosphereSystem &) = delete;
  AtmosphereSystem &operator=(const AtmosphereSystem &) = delete;

  void render(FrameInfo &frameInfo);

private:
  void createPipelineLayout(std::vector<VkDescriptorSetLayout> globalSetLayout);
  void createPipeline(VkRenderPass renderPass);

  LveDevice &lveDevice;

  std::unique_ptr<LvePipeline> lvePipeline;
  VkPipelineLayout pipelineLayout;
};
} // namespace lve
