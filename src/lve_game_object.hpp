#pragma once

#include "lve_model.hpp"

#include <memory>

namespace lve{

	struct transform2DComponent{
		glm::vec2 translation{};
		glm::vec2 scale{1.f,1.f};
		float rotation;

		glm::mat2 mat2() {
			const float s = glm::sin(rotation);
			const float c = glm::cos(rotation);
			glm::mat2 rotationMat{{c,s},{-s,c}};
	
			glm::mat2 scaleMat{{scale.x,.0f},{.0f,scale.y}};
			return rotationMat * scaleMat;
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
			transform2DComponent transform2D{};
		private:
			lveGameObject(id_t objId):id{objId}{};

			id_t id;
	};
};
