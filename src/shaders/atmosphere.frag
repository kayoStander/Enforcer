#version 450

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec3 cameraPosWorld;

layout(location = 0) out vec4 outColor;

struct PointLight {
  vec4 position; // ignore w 
  vec4 color;
};

layout(set = 0, binding = 0) uniform GlobalUbo {
  mat4 projection;
  mat4 view;
  mat4 inverseView;
  vec4 ambientLightColor; // w is intensity
  PointLight pointLights[10];
  int numLights;
} ubo;

layout(push_constant) uniform Push {
  vec4 fogColor; 
  float fogStart;
  float fogEnd;
} push;

void main(){
  //float fogFactor = (pixel,push.fogEnd/(push.fogStart,push.fogEnd);
  //fogFactor = clamp(fogFactor,0,1);
  outColor = fragColor;
}
