#include "keyboardController.hpp"
#include <glm/gtx/rotate_vector.hpp>

#include <limits>

namespace engine
{
    void keyboardController::moveInPlaneXZ(GLFWwindow* window, float dt, gameObject& object)
    {
        //rotations
        glm::vec3 rotate{0.f};
        // the player turn on itself 
        if (glfwGetKey(window, keys.lookRight) == GLFW_PRESS) rotate.y += 1.f; 
        if (glfwGetKey(window, keys.lookLeft) == GLFW_PRESS) rotate.y -= 1.f;

        if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon())
        {
            object.transform.rotation += lookSpeed * dt * glm::normalize(rotate);
        }

        object.transform.rotation.x = glm::clamp(object.transform.rotation.x, -1.5f, 1.5f);
        object.transform.rotation.y = glm::mod(object.transform.rotation.y, glm::two_pi<float>());

        float yaw = object.transform.rotation.y;
        const glm::vec3 forwardDirection{sin(yaw), 0.f, cos(yaw)};
        const glm::vec3 rightDirection{forwardDirection.z, 0.f, -forwardDirection.x};
        const glm::vec3 upDirection{0.f, -1.f, 0.f};

        // motion
        glm::vec3 moveDirection{0.f};
        if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS) moveDirection += forwardDirection;
        if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS) moveDirection -= forwardDirection;
        if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS) moveDirection += rightDirection;
        if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS) moveDirection -= rightDirection;
        if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS) moveDirection += upDirection;
        if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS) moveDirection -= upDirection;

        if (glm::dot(moveDirection, moveDirection) > std::numeric_limits<float>::epsilon())
        {
            object.transform.translation += moveSpeed * dt * glm::normalize(moveDirection);
        }
        
    }


    // compute the new pos of the camera in function of the rotation of the player to stay behind it for example 
    glm::vec3 keyboardController::computePosCam (TransformComponent playerTrans, camera& cam, float dt, GLFWwindow* window) {
        // the camera goes up and down 
        float speed = cam.getRotSpeed();
        float alpha = cam.getAlpha() ;
        glm::vec3 res ; 
        // deal with the change of view : update a boolean in the camera 
        if(glfwGetKey(window, keys.changeView) == GLFW_PRESS) cam.updateBool() ; 

        //alpha is the angle from which the camera looks at the player(in third person view) : pitch of the camera 
        //here we copute the new value of alpha depending on the key pressed
        if (glfwGetKey(window, keys.lookUp) == GLFW_PRESS) alpha += speed * dt ; // you want to see the player from below (we look up to the player)
        if (glfwGetKey(window, keys.lookDown) == GLFW_PRESS) alpha -= speed * dt; // from above 
        // we set the new value to the camera 
        cam.setAlpha(alpha); 

        if(cam.getViewBool()) {
            res = playerTrans.translation + glm::vec3(0.1f* sin(playerTrans.rotation.y), -cam.getCamHeight() , 0.1f* cos(playerTrans.rotation.y)) ; 
        }else {
            //sin(playerTrans.rotation.y), cos(playerTrans.rotation.y) : when we add that to the pos of the player, we put the camera behind it 
            // the cos(alpha) and sin(alpha) deal with the pitch 
            res = playerTrans.translation + glm::vec3(-cam.getCamDist() * cos(cam.getAlpha()) * sin(playerTrans.rotation.y), -cam.getCamHeight() - sin(alpha) , -cam.getCamDist() * cos(alpha) * cos(playerTrans.rotation.y));  
        }
        return res ; 
    }


}