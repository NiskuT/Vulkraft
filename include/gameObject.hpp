#pragma once

#include "engineModel.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <memory>
#include <unordered_map>

namespace engine
{
    struct TransformComponent
    {
        glm::vec3 translation{};
        glm::vec3 scale{1.f, 1.f, 1.f};
        glm::vec3 rotation{};

        glm::mat4 mat4();
        glm::mat3 normalMatrix();
    };

    class gameObject
    {
    public:
        using id_t = unsigned int;
        using Map = std::unordered_map<id_t, gameObject>;

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
        TransformComponent transform{};

    private:
        gameObject(id_t objId) : id(objId) {}

        id_t id;
    };
}