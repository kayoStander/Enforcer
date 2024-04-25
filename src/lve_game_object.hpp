#pragma once

#include "lve_model.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <memory>

namespace lve{

	struct transformComponent{
		glm::vec3 translation{};
		glm::vec3 scale{1.f,1.f,1.f};
		glm::vec3 rotation{};

		// extrinsic <- (right to left)
		// intrinsic -> (left to right)
		glm::mat4 mat4(){
			   const float c3 = glm::cos(rotation.z);
			   const float s3 = glm::sin(rotation.z);
			   const float c2 = glm::cos(rotation.x);
			   const float s2 = glm::sin(rotation.x);
			   const float c1 = glm::cos(rotation.y);
			   const float s1 = glm::sin(rotation.y);
			   return glm::mat4{
			{
				scale.x * (c1 * c3 + s1 * s2 * s3),
				scale.x * (c2 * s3),
				scale.x * (c1 * s2 * s3 - c3 * s1),
				0.0f,
			},
			{
				scale.y * (c3 * s1 * s2 - c1 * s3),
				scale.y * (c2 * c3),
				scale.y * (c1 * c3 * s2 + s1 * s3),
				0.0f,
			},
			{
				scale.z * (c2 * s1),
				scale.z * (-s2),
				scale.z * (c1 * c2),
				0.0f,
			},
			{translation.x, translation.y, translation.z, 1.0f}};
		}
	};

	class lveGameObject{
		public:
			using id_t = unsigned int;

			static lveGameObject createGameObject(){
				static id_t currentID = 0;
				return lveGameObject{currentID++};
			}

			lveGameObject(const lveGameObject&)=delete;
			lveGameObject &operator=(const lveGameObject&)=delete;
			lveGameObject(lveGameObject&&)=default;
			lveGameObject &operator=(lveGameObject&&)=default;

			id_t getId(){return id;}

			std::shared_ptr<lveModel> model{};
			glm::vec3 color{};
			transformComponent transform{};
		private:
			lveGameObject(id_t objId):id{objId}{};

			id_t id;
	};
};
