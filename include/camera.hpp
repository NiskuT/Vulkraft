#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace engine
{
    class camera
    {
    public:
        void setOrthographicProjection(float left, float right, float bottom, float top, float near, float far);
        void setPerspectiveProjection(float fovy, float aspectRatio, float near, float far);

        void setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3(0.0f, -1.0f, 0.0f));
        void setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3(0.0f, -1.0f, 0.0f));
        void setViewYXZ(glm::vec3 position, glm::vec3 rotation);

        void setAlpha (float a) {alphaCam = glm::clamp(a, -0.5f, 1.5f);} // we clamp alpha so the camera doesn't go under the floor, and in front of the player 
        
        void updateBool () {firstBool = !firstBool; }

        const glm::mat4& getProjectionMatrix() const { return projectionMatrix; }
        const glm::mat4& getViewMatrix() const { return viewMatrix; }
        const glm::mat4& getViewInverseMatrix() const { return viewInverseMatrix; }
        const glm::vec3 getPosition() const { return glm::vec3(viewInverseMatrix[3]); }

        const float getCamHeight () const { return camHeight ; }
        const float getCamDist () const { return camDist ; }
        const float getAlpha () const {return alphaCam ;}
        const float getRotSpeed () const { return ROT_SPEED ;}
        const float getMoveSpeed () const { return MOVE_SPEED ;}
        const float getViewBool () const { return firstBool ; }



    private:
        glm::mat4 projectionMatrix{1.0f};
        glm::mat4 viewMatrix{1.0f};
        glm::mat4 viewInverseMatrix{1.0f};

        const glm::vec3 Pos = glm::vec3{0.f}; // definition for Third person view
        const float camHeight = 0.5 ; 
        const float camDist = 1.5 ; 
        //camera pitch limits  
        float alphaCam = 0.0f; // this is the angle from which we see the player (third person view, look at)
        // Rotation and motion speed
        const float ROT_SPEED = glm::radians(120.0f);
        const float MOVE_SPEED = 2.0f;
        // bool for the third/first person view 
        bool firstBool = true ; 

    };
    
} // namespace engine