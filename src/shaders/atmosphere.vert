#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec4 fragColor; // fogFactor
layout(location = 1) out vec3 cameraPosWorld;

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
  vec4 fogColor; // aligned as 16
  float fogStart;
  float fogEnd;
} push;

void main(){

  vec3 cameraPosWorld = ubo.inverseView[3].xyz;
  float distanceCameraPosWorld = sqrt(dot(cameraPosWorld.x,push.fogStart));
  vec4 finalColor = vec4(0.0,0.0,0.0,1.0);//mix());  ...

  fragColor = push.fogColor;

  gl_Position = vec4(position,1.0);
}
