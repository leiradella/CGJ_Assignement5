////////////////////////////////////////////////////////////////////////////////
//
// Camera Abstraction Class
//
// Copyright (c)2023-24 by Carlos Martinho
//
////////////////////////////////////////////////////////////////////////////////

#include "mglCamera.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace mgl {

///////////////////////////////////////////////////////////////////////// Camera

Camera::Camera(GLuint bindingpoint)
    : ViewMatrix(glm::mat4(1.0f)), ProjectionMatrix(glm::mat4(1.0f)) {
  glGenBuffers(1, &UboId);
  glBindBuffer(GL_UNIFORM_BUFFER, UboId);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, 0, GL_STREAM_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, bindingpoint, UboId);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

Camera::~Camera() {
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
  glDeleteBuffers(1, &UboId);
}

glm::mat4 Camera::getViewMatrix() const { return ViewMatrix; }

void Camera::setViewMatrix(const glm::vec3& eyecoords, const glm::vec3& centercoords, const glm::vec3& upcoords) {
  ViewMatrix = glm::lookAt(eyecoords, centercoords, upcoords);
  setEye(eyecoords);
  setCenter(centercoords);
  setUp(upcoords);
  glBindBuffer(GL_UNIFORM_BUFFER, UboId);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4),
                  glm::value_ptr(ViewMatrix));
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

glm::mat4 Camera::getProjectionMatrix() const { return ProjectionMatrix; }

void Camera::setProjectionMatrix(const glm::mat4 &projectionmatrix) {
  ProjectionMatrix = projectionmatrix;
  glBindBuffer(GL_UNIFORM_BUFFER, UboId);
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4),
                  glm::value_ptr(ProjectionMatrix));
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

glm::vec3 Camera::getEye() const { return eye; }

void Camera::setEye(const glm::vec3& eyecoords) { eye = eyecoords; }

glm::vec3 Camera::getCenter() const { return center; }

void Camera::setCenter(const glm::vec3& centercoords) { center = centercoords; }

glm::vec3 Camera::getUp() const { return up; }

void Camera::setUp(const glm::vec3& upcoords) { up = upcoords; }

////////////////////////////////////////////////////////////////////////////////
}  // namespace mgl
