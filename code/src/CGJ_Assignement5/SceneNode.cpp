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


	int width = 800, height = 800, nr_channels;
	//unsigned char *data = stbi_load(texFullName.c_str(), &width, &height, &nr_channels, 4);
	unsigned char* data = createPerlinTexture(width, height);
	FILE* fp = fopen("tex.data", "wb");
	fwrite(data, 1, width * height * 4, fp);
	fclose(fp);
	//exit(0);
	if (data != NULL) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		printf("Failed to load texture %s\n", texFullName.c_str());
	}
	//stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);
}

//this is for making the gradiets in the grid for perlin noise
glm::vec2 grad(int x, int y) {
	// Pseudo-random hash function
	uint32_t seed = 2166136261u; // FNV offset basis
	seed ^= (uint32_t)x;
	seed *= 16777619u; // FNV prime
	seed ^= (uint32_t)y;
	seed *= 16777619u;

	// Final mixing for better randomness
	seed ^= (seed >> 15);
	seed *= 2246822519u;
	seed ^= (seed >> 13);
	seed *= 3266489917u;
	seed ^= (seed >> 16);

	// Convert hash into a pseudo-random direction vector
	float angle = (float)(seed & 0xFFFFFF) / 0xFFFFFF * 2.0f * glm::pi<float>();
	return glm::vec2(glm::cos(angle), glm::sin(angle));
}

//this calculates the value of each pixel of the texture using perlin noise
float perlinNoise(float x, float y) {
	
	//////step 1: grid and weights
	//grid coords
	int x0 = (int)x;
	int y0 = (int)y;
	int x1 = x0 + 1;
	int y1 = y0 + 1;
	//weights for interpolation
	float wx = x - (float)x0;
	float wy = y - (float)y0;

	//////step 2: gradients
	////point A (x0, y0)
	glm::vec2 gradient = grad(x0, y0);
	//vector grid -> point
	float dx = x - (float)x0;
	float dy = y - (float)y0;
	//dot product between random gradiant
	float iA = (gradient.x * dx) + (gradient.y * dy);

	////point B (x1, y0)
	gradient = grad(x1, y0);
	dx = x - (float)x1;
	dy = y - (float)y0;
	float iB = (gradient.x * dx) + (gradient.y * dy);

	////point C (x1, y1)
	gradient = grad(x1, y1);
	dx = x - (float)x1;
	dy = y - (float)y1;
	float iC = (gradient.x * dx) + (gradient.y * dy);

	////point D (x0, y1)
	gradient = grad(x0, y1);
	dx = x - (float)x0;
	dy = y - (float)y1;
	float iD = (gradient.x * dx) + (gradient.y * dy);

	//step 3: Interpolate
	float tx = 3 * glm::pow(wx, 2) - 2 * glm::pow(wx, 3);
	float ty = 3 * glm::pow(wy, 2) - 2 * glm::pow(wy, 3);

	float iAB = iA + tx * (iB - iA);
	float iCD = iD + tx * (iC - iD);
	float value = iAB + ty * (iCD - iAB);
	return value;
}

unsigned char* SceneNode::createPerlinTexture(int width, int height) {
	unsigned char* data = new unsigned char[width * height * 4]; //RGBA
	const int GRID_SIZE = 32;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
		
			//harmonics
			float freq = 1;
			float amp = 1;
			float val = 0;
			float totalAmp = 0.0f;	
			
			for (int i = 0; i < 8; i++) {		
				totalAmp += amp;
				val += perlinNoise(x * freq / GRID_SIZE, y * freq / GRID_SIZE) * amp;
				freq *= 2;
				amp /= 2;
			}

			//clamp to prevent numbers going outside the range [-1, 1]
			val /= totalAmp;
			if (val < -1.0f) { val = -1.0f; }
			else if (val > 1.0f) { val = 1.0f; }
			val = (val + 1.0f) * 0.5f * 255.0f;

			int index = ((y * width) + x) * 4;
			data[index+0] = (unsigned char)val; //r
			data[index+1] = (unsigned char)val; //g
			data[index+2] = (unsigned char)val; //b
			data[index+3] = (unsigned char)255; //a

		}
	}
	return data;
}

/*
	//start of the grid block
	int x0 = (int)(x);
	int y0 = (int)(y);
	//end of the grid block
	int x1 = x0 + 1;
	int y1 = y0 + 1;
	//coordinates inside the grid block
	float gx = x - (float)x0;
	float gy = y - (float)y0;

	//////top two cornres
	//top left
	glm::vec2 gradient = { glm::cos(std::rand()), glm::sin(std::rand()) };
	gradient = glm::normalize(gradient);
	float dx = x - (float)x0;
	float dy = y - (float)y0;
	float dot1 = (gradient.x * dx) + (gradient.y * dy);
	//top right
	gradient = { glm::cos(std::rand()), glm::sin(std::rand()) };
	gradient = glm::normalize(gradient);
	dx = x - (float)x1;
	dy = y - (float)y0;
	float dot2 = (gradient.x * dx) + (gradient.y * dy);
	//interpolate both
	float ix1 = (dot2 - dot1) * (3.0f - (gx * 2.0f)) * gx * gx * dot1;

	//////bottom two cornres
	//bottom left
	gradient = { glm::cos(std::rand()), glm::sin(std::rand()) };
	gradient = glm::normalize(gradient);
	dx = x - (float)x0;
	dy = y - (float)y1;
	dot1 = (gradient.x * dx) + (gradient.y * dy);
	//bottom right
	gradient = { glm::cos(std::rand()), glm::sin(std::rand()) };
	gradient = glm::normalize(gradient);
	dx = x - (float)x1;
	dy = y - (float)y1;
	dot2 = (gradient.x * dx) + (gradient.y * dy);
	//interpolate both
	float ix2 = (dot2 - dot1) * (3.0f - (gx * 2.0f)) * gx * gx * dot1;

	//interpolate the 2 results
	float value = (ix2 - ix1) * (3.0f - (gy * 2.0f)) * gy * gy * ix1;
	return value;
*/