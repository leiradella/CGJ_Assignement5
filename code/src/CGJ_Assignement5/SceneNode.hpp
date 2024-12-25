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
	void draw();
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

	//Skybox
	void createTextureSkybox(std::string name, std::string format);

	void setOutline(bool outline) { isOutline = outline; }
	void setSkybox(bool skybox) { isSkybox = skybox; }

	void addMat4Uniform(const std::string& name, const glm::mat4& value) { mat4Uniforms[name] = value; }
	void addVec3Uniform(const std::string& name, const glm::vec3& value) { vec3Uniforms[name] = value; }
	void addFloatUniform(const std::string& name, const float value) { floatUniforms[name] = value; }
	void addIntUniform(const std::string& name, const int value) { intUniforms[name] = value; }

protected:
	SceneNode* parent;
	std::vector<SceneNode*> children;
	mgl::Mesh* mesh;
	mgl::ShaderProgram* Shaders;
	std::map<std::string, glm::mat4> mat4Uniforms;
	std::map<std::string, glm::vec3> vec3Uniforms;
	std::map<std::string, float> floatUniforms;
	std::map<std::string, int> intUniforms;

	//texture
	GLuint texture;
	bool isOutline = false;
	bool isSkybox = false;
	
	//default atributes for each object
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};