////////////////////////////////////////////////////////////////////////////////
//
// Camera Abstraction Class
//
// Copyright (c)2023-24 by Carlos Martinho
//
////////////////////////////////////////////////////////////////////////////////

#ifndef MGL_CAMERA_HPP
#define MGL_CAMERA_HPP

#include <GL/glew.h>

#include <glm/glm.hpp>

namespace mgl {

class Camera;

///////////////////////////////////////////////////////////////////////// Camera

class Camera {
 private:
  glm::mat4 ViewMatrix;
  glm::mat4 ProjectionMatrix;
  glm::vec3 eye = { 5.0f, 5.0f, 5.0f };
  glm::vec3 center = { 0.0f, 0.0f, 0.0f };
  glm::vec3 up = { 0.0f, 1.0f, 0.0f };

  //if you want to set these outside the class, use getEye, getCenter,
  //getUp and setViewMatrix
  void setEye(const glm::vec3& eyecoords);
  void setCenter(const glm::vec3& centercoords);
  void setUp(const glm::vec3& upcoords);

 public:
	 GLuint UboId;
  explicit Camera(GLuint bindingpoint);
  virtual ~Camera();
  glm::mat4 getViewMatrix() const;
  void setViewMatrix(const glm::vec3 &eyecoords, const glm::vec3& centercoords, const glm::vec3& upcoords);
  glm::mat4 getProjectionMatrix() const;
  void setProjectionMatrix(const glm::mat4 &projectionmatrix);
  glm::vec3 getEye() const;
  glm::vec3 getCenter() const;
  glm::vec3 getUp() const;
};

////////////////////////////////////////////////////////////////////////////////
}  // namespace mgl

#endif /* MGL_CAMERA_HPP */
