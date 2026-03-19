#include <unordered_map>

#include "BaseTexture.h"

using std::string;

std::unordered_map<string, BaseTexture*> LoadedTextures;

BaseTexture::BaseTexture(string texturePath, string textureType) {
	mSourcePath = texturePath;
	mTextureType = textureType;

	glGenTextures(1, &mTexture);
	glBindTexture(GL_TEXTURE_2D, mTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(mSourcePath.c_str(), &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		Log::Info("Created texture from path " + mSourcePath);
	} else {
		Log::Info("Failed to create texture from path " + mSourcePath);
	}
	stbi_image_free(data);
};

BaseTexture* BaseTexture::LoadTexture(string texturePath, string textureType) {
	if (LoadedTextures.contains(texturePath)) return LoadedTextures.at(texturePath);

	BaseTexture* newTexture = new BaseTexture(texturePath, textureType);
	LoadedTextures[texturePath] = newTexture;

	return newTexture;
}

void BaseTexture::Activate(unsigned int unitOffset) {
	glActiveTexture(GL_TEXTURE0 + unitOffset);
	glBindTexture(GL_TEXTURE_2D, mTexture);
}