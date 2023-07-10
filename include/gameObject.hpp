#pragma once

#include "engineModel.hpp"

#include <memory>

namespace engine
{
    struct Transform2dComponent
    {
        glm::vec2 translation{};
        glm::vec2 scale{1.f, 1.f};
        float rotation;

        glm::mat2 mat2() 
        { 
            glm::mat2 rotationMat{{glm::cos(rotation), glm::sin(rotation)}, {-glm::sin(rotation), glm::cos(rotation)}};
            glm::mat2 scaleMat = glm::mat2(scale.x, 0.f, 0.f, scale.y);
            return rotationMat*scaleMat; 
        }
    };

    class gameObject
    {
    public:
        using id_t = unsigned int;

        static gameObject createGameObject()
        {
            static id_t currentId = 0;
            return gameObject(currentId++);
        }

        id_t getId() const { return id; }

        gameObject(const gameObject&) = delete;
        gameObject& operator=(const gameObject&) = delete;
        gameObject(gameObject&&) = default;
        gameObject& operator=(gameObject&&) = default;

        std::shared_ptr<engineModel> model{};
        glm::vec3 color{};
        Transform2dComponent transform2d{};

    private:
        gameObject(id_t objId) : id(objId) {}

        id_t id;
    };
}