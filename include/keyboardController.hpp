#pragma once

#include "gameWindow.hpp"
#include "gameObject.hpp"
#include "camera.hpp"


namespace engine
{
    class keyboardController
    {
    public:
        struct KeyMappings
        {
            int moveLeft = GLFW_KEY_A;
            int moveRight = GLFW_KEY_D;
            int moveForward = GLFW_KEY_W;
            int moveBackward = GLFW_KEY_S;
            int moveUp = GLFW_KEY_E;
            int moveDown = GLFW_KEY_Q;
            int lookLeft = GLFW_KEY_LEFT;
            int lookRight = GLFW_KEY_RIGHT;
            int lookUp = GLFW_KEY_UP;
            int lookDown = GLFW_KEY_DOWN;
            int changeView = GLFW_KEY_F5 ;
        };

        void moveInPlaneXZ(GLFWwindow* window, float dt, gameObject& object);
        glm::vec3 computePosCam (TransformComponent playerTrans, camera& cam, float dt, GLFWwindow* window);
        glm::vec3 computePosCamMouse(TransformComponent& playerTrans, camera &cam, gameObject &player, GLFWwindow *window);

        KeyMappings keys{};
        float moveSpeed{3.f};
        float lookSpeed{1.5f};
    private:
        double prevMouseX{0.0};
        double prevMouseY{0.0};
    };
    
    
}