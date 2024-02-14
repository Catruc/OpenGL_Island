#include "Camera.hpp"

namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraUpDirection = cameraUp;
    
        
        this->cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        this->cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection));
}



    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        //TODO
        return glm::lookAt(cameraPosition, cameraPosition + cameraFrontDirection, glm::vec3(0.0f, 1.0f, 0.0f));
        //return glm::mat4();
    }

    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed) {
        //TODO
        
        switch (direction) {
            case MOVE_FORWARD:
                cameraPosition += speed * cameraFrontDirection;
                break;
            case MOVE_BACKWARD:
                cameraPosition -= speed * cameraFrontDirection;
                break;
            case MOVE_RIGHT:
                cameraPosition += speed * cameraRightDirection;
                break;
            case MOVE_LEFT:
                cameraPosition -= speed * cameraRightDirection;
                break;
            case MOVE_UP:
                cameraPosition += speed*cameraUpDirection;
                break;
            case MOVE_DOWN:
                cameraPosition -= speed*cameraUpDirection;
                break;
            
        }
    }

    void Camera::lookAtPoint(glm::vec3 target) {
            cameraFrontDirection = glm::normalize(target - cameraPosition);
            // Recompute the Right and Up vectors
            cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection));
    
    }
        // Optionally recalculate cameraUpDirection if you want the camera's up vector to be dynamic
        // cameraUpDirection = glm::normalize(glm::cross(cameraRightDirection, cameraFrontDirection));

    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float pitch, float yaw) {
        glm::vec3 frontCamera;
        frontCamera.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        frontCamera.y = sin(glm::radians(pitch));
        frontCamera.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFrontDirection = glm::normalize(frontCamera);
        cameraRightDirection = glm::normalize(glm::cross(this->cameraFrontDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
    }
}
