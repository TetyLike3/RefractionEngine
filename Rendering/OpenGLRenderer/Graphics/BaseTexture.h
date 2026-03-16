#ifndef BASETEXTURE_H
#define BASETEXTURE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#pragma warning(push, 0)
#include "STB/stb_image.h"
#pragma warning(pop)

#include "../Utilities.h"

#include <string>

class BaseTexture {
public:
	BaseTexture(std::string texturePath, std::string textureType) {
		m_sourcePath = texturePath;
		m_textureType = textureType;

		glGenTextures(1, &m_texture);
		glBindTexture(GL_TEXTURE_2D, m_texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(m_sourcePath.c_str(), &width, &height, &nrChannels, 0);
		if (data) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			Log::Info("Created texture from path " + m_sourcePath);
		} else {
			Log::Info("Failed to create texture from path " + m_sourcePath);
		}
		stbi_image_free(data);
	};

	void Activate(unsigned int unitOffset) {
		glActiveTexture(GL_TEXTURE0 + unitOffset);
		glBindTexture(GL_TEXTURE_2D, m_texture);
	}

	std::string GetTextureType() { return m_textureType; };
	std::string GetSourcePath() { return m_sourcePath; };

private:
	unsigned int m_texture;
	std::string m_sourcePath;
	std::string m_textureType;
};

#endif