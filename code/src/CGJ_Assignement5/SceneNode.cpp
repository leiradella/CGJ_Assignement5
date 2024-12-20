#include "SceneNode.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <ctime>
#include <cstdlib>

SceneNode::SceneNode(mgl::Mesh* mesh, mgl::ShaderProgram* Shaders) {
	this->mesh = mesh;
	parent = NULL;
	this->Shaders = Shaders;
	position = { 0.0f, 0.0f, 0.0f };
	rotation = { 0.0f, 0.0f, 0.0f };
	scale = { 1.0f, 1.0f, 1.0f };
}

void SceneNode::draw(GLint ModelMatrixId, GLint viewPosId, glm::vec3 eye) {
	glm::mat4 saved = GlobalMatrix;
	 GlobalMatrix *= 
		 glm::translate(position) * 
		 glm::rotate(glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)) * 
		 glm::rotate(glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)) *
		 glm::rotate(glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)) *
		 glm::scale(scale);
	if (mesh) {
		Shaders->bind();
		glUniformMatrix4fv(ModelMatrixId, 1, GL_FALSE, glm::value_ptr(GlobalMatrix));
		glUniform3fv(viewPosId, 1, glm::value_ptr(eye));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		mesh->draw();
		Shaders->unbind();
	}
	for (int i = 0; i < children.size(); i++) {
		children[i]->draw(ModelMatrixId, viewPosId, eye);
	}
	GlobalMatrix = saved;
}

void SceneNode::addChild(SceneNode* child) {
	children.push_back(child);
	child->parent = this;
}

void SceneNode::setTexure(std::string texFile) {
	std::string texDir = "../assets/";
	std::string texFullName = texDir + texFile;

	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	int width = 512, height = 512, nr_channels;
	//unsigned char *data = stbi_load(texFullName.c_str(), &width, &height, &nr_channels, 4);
	unsigned char* data = createWoodTexture(width, height);
	if (data != NULL) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		printf("Failed to load texture %s\n", texFullName.c_str());
	}
	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);
}

unsigned char* SceneNode::createWoodTexture(int width, int height) {
	unsigned char* noiseData = new unsigned char[width * height * 4]; //RGBA
	std::srand(std::time(nullptr));

	for (int y = 0; y < height; ++y) {
		float distance = 0;
		for (int x = 0; x < width; ++x) {
			//get the current grid coordinates
			float nx = (float)(x) / (float)(width);
			float ny = (float)(y) / (float)(height);
			unsigned char r,g,b,a = 255.0f;

			int check = x;
			if ((check % 32) == 0) {
				distance = 0;
				r = 0;
				g = 0;
				b = 0;
			}
			else {
				distance += 0.1;
				r = (unsigned char)(glm::clamp(139.0f + distance * 50.0f, 0.0f, 255.0f)); // Base brown: 139
				g = (unsigned char)(glm::clamp(69.0f + distance * 30.0f, 0.0f, 255.0f));  // Base greenish tint: 69
				b = (unsigned char)(glm::clamp(19.0f + distance * 20.0f, 0.0f, 255.0f));  // Base dark brown: 19
			}

			int index = ((y * width) + x) * 4;
			noiseData[index+0] = r; //r
			noiseData[index+1] = g; //g
			noiseData[index+2] = b; //b
			noiseData[index+3] = a; //a

		}
	}
	return noiseData;
}

