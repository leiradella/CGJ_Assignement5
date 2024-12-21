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
	
	void createTextureImage(std::string texFile);

	//perlin Texture functions
	void createTexturePerlin(int width, int height, float freq = 1.0f, float amp = 1.0f);
	unsigned char* createPerlinNoise(int width, int height, float freq, float amp);

	//wood texture functions (NOT WORKING RIGHT)
	void createTextureWood(int width, int height, float freq = 1.0f, float amp = 1.0f);
	unsigned char* createWood(int width, int height, float freq, float amp);

	//Solid color option
	void createTextureColor(int width, int height, float r, float g, float b);
	unsigned char* createColor(int width, int height, float r, float g, float b);

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