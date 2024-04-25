#include "lve_model.hpp"

#include <cassert>
#include <cstddef>
#include <cstring>
#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

//tip: only use stagingbuffer for static things

namespace lve{

	std::vector<VkVertexInputBindingDescription> lveModel::Vertex::getBindingDescriptions(){
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescriptions;
	};

	std::vector<VkVertexInputAttributeDescription> lveModel::Vertex::getAttributeDescriptions(){
		std::vector<VkVertexInputAttributeDescription> attributeDescription(2);
		attributeDescription[0].binding = 0;
		attributeDescription[0].location = 0;
		attributeDescription[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescription[0].offset = offsetof(Vertex,position);
		
		attributeDescription[1].binding = 0;
		attributeDescription[1].location = 1;
		attributeDescription[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescription[1].offset = offsetof(Vertex,color);
		return attributeDescription;
	};

	lveModel::lveModel(lveDevice &device,const lveModel::Builder &builder)
	:lvedevice{device}{
		createVertexBuffers(builder.vertices);
		createIndexBuffers(builder.indices);
	};

	lveModel::~lveModel(){
		vkDestroyBuffer(lvedevice.device(),vertexBuffer,nullptr);
		vkFreeMemory(lvedevice.device(),vertexBufferMemory,nullptr);

		if (hasIndexBuffer){
			vkDestroyBuffer(lvedevice.device(),indexBuffer,nullptr);
			vkFreeMemory(lvedevice.device(),indexBufferMemory,nullptr);
		}
	}

	std::unique_ptr<lveModel> lveModel::createModelFromFile(
			lveDevice &device,
			const std::string &filePath
			){
		Builder builder{};
		builder.loadModel(filePath);
		std::cout << "vertexCount > " << builder.vertices.size() << std::endl;
		return std::make_unique<lveModel>(device,builder);
	}

	void lveModel::draw(VkCommandBuffer commandBuffer){
		if (hasIndexBuffer){
			vkCmdDrawIndexed(commandBuffer,indexCount,1,0,0,0);
		}else{
			vkCmdDraw(commandBuffer,vertexCount,1,0,0);
		}
	}

	void lveModel::bind(VkCommandBuffer commandBuffer){
		VkBuffer buffers[] = {vertexBuffer};
		VkDeviceSize offsets[] = {0};
		vkCmdBindVertexBuffers(commandBuffer,0,1,buffers,offsets);
		
		if (hasIndexBuffer){
			vkCmdBindIndexBuffer(commandBuffer,indexBuffer,0,VK_INDEX_TYPE_UINT32);
		}
	}

	void lveModel::createVertexBuffers(const std::vector<Vertex> &vertices){
		vertexCount = static_cast<uint32_t>(vertices.size());
		assert(vertexCount >= 3 && "vertex count must be at least 3");

		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		lvedevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer,
			stagingBufferMemory);

		void *data;
		vkMapMemory(lvedevice.device(),stagingBufferMemory,0,bufferSize,0,&data);
		memcpy(data,vertices.data(),static_cast<size_t>(bufferSize));
		vkUnmapMemory(lvedevice.device(),stagingBufferMemory);

		lvedevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			vertexBuffer,
			vertexBufferMemory);

		lvedevice.copyBuffer(stagingBuffer,vertexBuffer,bufferSize);

		vkDestroyBuffer(lvedevice.device(),stagingBuffer,nullptr);
		vkFreeMemory(lvedevice.device(),stagingBufferMemory,nullptr);

	}

	void lveModel::createIndexBuffers(const std::vector<uint32_t> &indices){
		indexCount = static_cast<uint32_t>(indices.size());
		hasIndexBuffer = indexCount > 0;

		if (!hasIndexBuffer) {return;}

		VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		lvedevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer,
			stagingBufferMemory);

		void *data;
		vkMapMemory(lvedevice.device(),stagingBufferMemory,0,bufferSize,0,&data);
		memcpy(data,indices.data(),static_cast<size_t>(bufferSize));
		vkUnmapMemory(lvedevice.device(),stagingBufferMemory);

		lvedevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			indexBuffer,
			indexBufferMemory);

		lvedevice.copyBuffer(stagingBuffer,indexBuffer,bufferSize);

		vkDestroyBuffer(lvedevice.device(),stagingBuffer,nullptr);
		vkFreeMemory(lvedevice.device(),stagingBufferMemory,nullptr);
	}

	void lveModel::Builder::loadModel(const std::string &filePath){
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn,err;

		if (!tinyobj::LoadObj(&attrib,&shapes,&materials,&warn,&err,filePath.c_str())){
			throw std::runtime_error(warn + err);
		}

		vertices.clear();
		indices.clear();

		for (const auto &shape : shapes){
			for (const auto &index : shape.mesh.indices){
				Vertex vertex{};

				if (index.vertex_index >= 0){
					vertex.position = {
						attrib.vertices[3*index.vertex_index+0],
						attrib.vertices[3*index.vertex_index+1],
						attrib.vertices[3*index.vertex_index+2]
					};

					auto colorIndex = 3 * index.vertex_index + 2;
					if (colorIndex < attrib.colors.size()){
						vertex.color = {
							attrib.colors[colorIndex-0],
							attrib.colors[colorIndex-1],
							attrib.colors[colorIndex-2]
						};
					}else{
						vertex.color = {1.f,1.f,1.f};
					}
				}

				if (index.normal_index >= 0){
					vertex.normal = {
						attrib.normals[3*index.normal_index+0],
						attrib.normals[3*index.normal_index+1],
						attrib.normals[3*index.normal_index+2]
					};
				}
				
				if (index.texcoord_index >= 0){
					vertex.uv = {
						attrib.texcoords[3*index.texcoord_index+0],
						attrib.texcoords[3*index.texcoord_index+1]
					};
				}

				vertices.push_back(vertex);
			}
		}
	}
}
