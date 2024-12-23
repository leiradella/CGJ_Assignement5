#include "SceneNode.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <ctime>
#include <cstdlib>

SceneNode::SceneNode(mgl::Mesh* mesh, mgl::ShaderProgram* Shaders) {
	this->mesh = mesh;
	parent = NULL;
	texture = 0;
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
		if (isOutline) { glDepthMask(GL_FALSE); }
		glUniformMatrix4fv(ModelMatrixId, 1, GL_FALSE, glm::value_ptr(GlobalMatrix));
		glUniform3fv(viewPosId, 1, glm::value_ptr(eye));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		mesh->draw();
		Shaders->unbind();
		if (isOutline) { glDepthMask(GL_TRUE); }
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

void SceneNode::createTextureImage(std::string texFile) {
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
	unsigned char *data = stbi_load(texFullName.c_str(), &width, &height, &nr_channels, 4);
	if (data != NULL) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else {
		printf("Failed to load texture %s\n", texFullName.c_str());
		glBindTexture(GL_TEXTURE_2D, 0);
	}
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
	//vector: grid -> point
	float dx = x - (float)x0;
	float dy = y - (float)y0;
	//dot product between random gradiant and the grid point vector
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

unsigned char* SceneNode::createPerlinNoise(int width, int height, float freq, float amp) {
	unsigned char* data = new unsigned char[width * height * 4]; //RGBA
	const int GRID_SIZE = 32;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			float val = 0.0f;

			//harmonics
			float frequency = freq;
			float amplitude = amp;
			for (int i = 0; i < 8; i++) {		
				val += perlinNoise(x * frequency / GRID_SIZE, y * frequency / GRID_SIZE) * amplitude;
				frequency *= 2;
				amplitude /= 2;
			}

			//clamp to prevent numbers going outside the range [-1, 1]
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

void SceneNode::createTexturePerlin(int width, int height, float freq, float amp) {
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	unsigned char* data = createPerlinNoise(width, height, freq, amp);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
}

unsigned char* SceneNode::createWood(int width, int height, float freq, float amp) {
	unsigned char* data = new unsigned char[width * height * 4]; // RGBA

	float ringFactor = 50.0f;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			// Radial distance
			float dx = (x - width / 2.0f) / (float)width;
			float dy = (y - height / 2.0f) / (float)height;
			float baseDist = sqrt(dx * dx + dy * dy) * ringFactor;

			// Distort the rings using freq
			float distortion = perlinNoise(x * freq / width, y * freq / height) * amp;

			// Combine base distance and distortion
			float dist = baseDist + distortion;

			// Create sine wave for rings (amp controls thickness via `smoothstep`)
			float woodValue = sin(dist) * 0.5f + 0.5f; // Normalize to [0, 1]
			woodValue = glm::smoothstep(0.5f - amp * 0.1f, 0.5f + amp * 0.1f, woodValue);

			// Create a color gradient
			float gradientFactor = (float)y / height; // Vertical gradient
			unsigned char red = (unsigned char)((woodValue * 200.0f + 55.0f) * (1.0f - gradientFactor));
			unsigned char green = (unsigned char)((woodValue * 140.0f + 30.0f) * (1.0f - gradientFactor));
			unsigned char blue = (unsigned char)((woodValue * 90.0f + 20.0f) * gradientFactor); // Blue increases with gradient

			// Assign color to texture data
			int index = (y * width + x) * 4;
			data[index + 0] = red;    // R
			data[index + 1] = green;  // G
			data[index + 2] = blue;   // B
			data[index + 3] = 255;    // A
		}
	}

	return data;
}

void SceneNode::createTextureWood(int width, int height, float freq, float amp) {
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	unsigned char* data = createWood(width, height, freq, amp);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
}

void SceneNode::createTextureColor(int width, int height, float r, float g, float b) {
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	unsigned char* data = createColor(width, height, r, g, b);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
}

unsigned char* SceneNode::createColor(int width, int height, float r, float g, float b) {
	unsigned char* data = new unsigned char[width * height * 4]; // RGBA

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int index = (y * width + x) * 4;
			data[index + 0] = r;    // R
			data[index + 1] = g;  // G
			data[index + 2] = b;   // B
			data[index + 3] = 255;    // A
		}
	}
	return data;
}
