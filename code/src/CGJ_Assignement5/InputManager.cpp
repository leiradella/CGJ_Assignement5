#include "InputManager.hpp"

double InputManager::cursorXPos = 0.0f;
double InputManager::cursorYPos = 0.0f;
double InputManager::startCursorXPos = 0.0f;
double InputManager::startCursorYPos = 0.0f;

void InputManager::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    //save mouse button state
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            //if pressed, save the position
            leftMouse= PRESSED;
            glfwGetCursorPos(window, &startCursorXPos, &startCursorYPos);
        }
        else if (action == GLFW_RELEASE) {
            leftMouse= NOT_PRESSED;
            printf("(%f, %f, %f)\n", camera->getEye().x, camera->getEye().y, camera->getEye().z);
        }
    }
}

void InputManager::cursorCallback(GLFWwindow* window, double xpos, double ypos) {
    double deltaX, deltaY;
    glm::vec3 axis(0.0f);
    glm::vec3 eye = camera->getEye();
    glm::vec3 center = camera->getCenter();
    glm::vec3 up = camera->getUp();

    if (leftMouse == PRESSED) {

        //get the variation in cursor coordinates
        glfwGetCursorPos(window, &cursorXPos, &cursorYPos);
        deltaX = cursorXPos - startCursorXPos;
        deltaY = cursorYPos - startCursorYPos;


        glm::vec3 view = glm::normalize(center - eye);
        float pitch = glm::asin(view.y);
        float yaw = glm::atan(-view.x, -view.z);
        radius = glm::length(eye - center);

        //the rotation is contrary to the delta of the cursor, that's why we do -
        const float sensitivity = 0.01f;
        yaw -= (float)deltaX * sensitivity;
        pitch -= (float)deltaY * sensitivity;

        //clamp pitch to prevent rotating over the poles
        pitch = glm::clamp(pitch, glm::radians(-89.0f), glm::radians(89.0f));
        glm::quat qPitch = glm::angleAxis(pitch, glm::vec3(1.0f, 0.0f, 0.0f));
        glm::quat qYaw = glm::angleAxis(yaw, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::quat orientation = glm::normalize(qYaw * qPitch);

        //(we use -z as it is the opengl convention)
        view = radius * glm::rotate(orientation, glm::vec3(0.0f, 0.0f, -1.0f));
        up = glm::rotate(orientation, glm::vec3(0.0f, 1.0f, 0.0f));
        //view = center - eye
        //view - center = -eye
        //center - view = eye
        eye = center - view;

        camera->setViewMatrix(eye, center, up);

        startCursorXPos = cursorXPos;
        startCursorYPos = cursorYPos;
    }
}

void InputManager::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {

    glm::vec3 eye = camera->getEye();
    glm::vec3 center = camera->getCenter();
    glm::vec3 up = camera->getUp();

    //get the new radius based on the offset
    radius = glm::length(eye - center) - (float)(yoffset);

    glm::vec3 viewDir = glm::normalize(eye - center);
    rotX = (float)(glm::atan(viewDir.z, viewDir.x));
    rotY = glm::asin(viewDir.y);

    //prevent zooming too close
    if (radius < 3.0f) {
        radius = 3.0f;
        return;
    }

    viewDir.x = radius * glm::cos(rotY) * glm::cos(rotX);
    viewDir.y = radius * glm::sin(rotY);
    viewDir.z = radius * glm::cos(rotY) * glm::sin(rotX);
    eye = center + viewDir;
    glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), viewDir));
    up = glm::normalize(glm::cross(viewDir, right));
    camera->setViewMatrix(eye, center, up);
}

void InputManager::windowSizeCallback(GLFWwindow* window, int width, int height) {
    float ratio;
    //avoid division by 0
    if (height == 0) { height = 1; }
    glViewport(0, 0, width, height);
    ratio = (float)(width) / height;
    const glm::mat4 newPerspectiveProjection = glm::perspective(glm::radians(30.0f), ratio, 1.0f, 1000.0f);
    const glm::mat4 newOrthogonalProjection = glm::ortho(-width / 200.0f, width / 200.0f, -height / 200.0f, height / 200.0f, 1.0f, 1000.0f);
    if (camera->getProjectionMatrix() == perspectiveProjection) {
        camera->setProjectionMatrix(newPerspectiveProjection);
    }
    else {
        camera->setProjectionMatrix(newOrthogonalProjection);
    }
    orthogonalProjection = newOrthogonalProjection;
    perspectiveProjection = newPerspectiveProjection;
}

void InputManager::setCamera(mgl::Camera *cam) {
    glm::vec3 eye = { 20.0f, 0.0f, 0.0f };
    glm::vec3 center = { 0.0f, 0.0f, 0.0f };
    glm::vec3 up = { 0.0f, 1.0f, 0.0f };

    camera = cam;
    camera->setViewMatrix(eye, center, up);
    camera->setProjectionMatrix(perspectiveProjection);
}

