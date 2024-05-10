#include "atmosphere_system.hpp"
#include "lve_frame_info.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <cassert>
#include <stdexcept>

namespace lve {

struct AtmosphereConstantData {
  glm::vec4 fogColor; // aligned as 16
  float fogStart;
  float fogEnd;
};

AtmosphereSystem::AtmosphereSystem(
    LveDevice &device, VkRenderPass renderPass,
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts)
    : lveDevice{device} {
  createPipelineLayout(descriptorSetLayouts);
  createPipeline(renderPass);
}

AtmosphereSystem::~AtmosphereSystem() {
  vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr);
}

void AtmosphereSystem::createPipelineLayout(
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts) {
  VkPushConstantRange pushConstantRange{};
  pushConstantRange.stageFlags =
      VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
  pushConstantRange.offset = 0;
  pushConstantRange.size = sizeof(AtmosphereConstantData);

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount =
      static_cast<uint32_t>(descriptorSetLayouts.size());
  pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
  pipelineLayoutInfo.pushConstantRangeCount = 1;
  pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
  if (vkCreatePipelineLayout(lveDevice.device(), &pipelineLayoutInfo, nullptr,
                             &pipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
  }
}

void AtmosphereSystem::createPipeline(VkRenderPass renderPass) {
  assert(pipelineLayout != nullptr &&
         "Cannot create pipeline before pipeline layout");

  PipelineConfigInfo pipelineConfig{};
  LvePipeline::defaultPipelineConfigInfo(pipelineConfig);
  pipelineConfig.renderPass = renderPass;
  pipelineConfig.pipelineLayout = pipelineLayout;
  lvePipeline = std::make_unique<LvePipeline>(
      lveDevice, "shaders/atmosphere.vert.spv", "shaders/atmosphere.frag.spv",
      pipelineConfig);
}

void AtmosphereSystem::render(FrameInfo &frameInfo) {
  lvePipeline->bind(frameInfo.commandBuffer); // go look here later

  vkCmdBindDescriptorSets(frameInfo.commandBuffer,
                          VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1,
                          &frameInfo.globalDescriptorSet, 0,
                          nullptr); // aq q muda o descriptorset ó n esquece
  for (auto &kv : frameInfo.gameObjects) {
    auto &obj = kv.second;
    if (obj.fog == nullptr) {
      continue;
    }
    AtmosphereConstantData push{};
    push.fogColor = obj.fog->color;
    push.fogStart = obj.fog->fogStart;
    push.fogEnd = obj.fog->fogEnd;

    vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout,
                       VK_SHADER_STAGE_VERTEX_BIT |
                           VK_SHADER_STAGE_FRAGMENT_BIT,
                       0, sizeof(AtmosphereConstantData), &push);
    obj.model->bind(frameInfo.commandBuffer);
    obj.model->draw(frameInfo.commandBuffer);
  }
}

} // namespace lve
