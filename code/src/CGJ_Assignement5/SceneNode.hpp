#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include < vector >
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

#include "../mgl/mgl.hpp"

static glm::mat4 GlobalMatrix(1.0f);

class SceneNode {
public:
	SceneNode(mgl::Mesh* m, mgl::ShaderProgram* Shaders);
	void addChild(SceneNode* child);
	void draw(GLint ModelMatrixId, GLint viewPosId, glm::vec3 eye);
	std::vector<SceneNode*> getChildren() { return children; };

	void setPosition(glm::vec3 pos) { position = pos; }
	glm::vec3 getPosition() { return position; }
	void setRotation(glm::vec3 rot) { rotation = rot; }
	glm::vec3 getRotation() { return rotation; }
	void setScale(glm::vec3 scale) { this->scale = scale; }
	glm::vec3 getScale() { return scale; }
	
	void setTexure(std::string texFile);



protected:
	SceneNode* parent;
	std::vector<SceneNode*> children;
	mgl::Mesh* mesh;
	mgl::ShaderProgram* Shaders;

	//texture
	GLuint texture;
	
	//default atributes for each object
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};