#ifndef BASETEXTURE_H
#define BASETEXTURE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#pragma warning(push, 0)
#include "STB/stb_image.h"
#pragma warning(pop)

#include "../Utilities.h"
#include "Log.h"

#include <string>

#define REFRACT_TEXTURE_TYPE_DIFFUSE "texDiffuse"
#define REFRACT_TEXTURE_TYPE_SPECULAR "texSpecular"


class BaseTexture {
public:
	// Creates a texture (or returns a pointer to the existing texture)
	static BaseTexture* LoadTexture(std::string texturePath, std::string textureType);
	void Activate(unsigned int unitOffset);

	std::string GetTextureType() { return m_textureType; };
	std::string GetSourcePath() { return m_sourcePath; };

private:
	BaseTexture(std::string texturePath, std::string textureType);

	unsigned int m_texture;
	std::string m_sourcePath;
	std::string m_textureType;
};

#endif