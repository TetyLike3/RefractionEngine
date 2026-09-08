#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include "STB/stb_image.h"

#include "OpenGLTexture.h"

namespace Refraction::Engine::Platform {
	Common::Shared<ATexture> OpenGLTexture::GetFromPath(const std::filesystem::path& path) {
		stbi_set_flip_vertically_on_load(true);
		int w, h, c;
		if (unsigned char* data = stbi_load(path.string().c_str(), &w, &h, &c, 0)) {
			auto texStruct = TextureStructure{
				.Width = w,
				.Height = h,
				.Format = TextureFormat::RGB8,
				.MipsEnabled = true
			};
			auto newTex = Common::NewShared<OpenGLTexture>(texStruct);
			newTex->Activate(0);

			// Enable wrapping (repeat)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			Log::Render.Info("Created texture from path " + path.string());
			stbi_image_free(data);

			return newTex;
		} else {
			Log::Render.Error("Failed to create texture from path " + path.string() + ": " + stbi_failure_reason());
			stbi_image_free(data);
		}
		return nullptr;
	}

	Common::Shared<ATexture> OpenGLTexture::GetFromID(unsigned int id) {
		if (!glIsTexture(id)) return nullptr;

		auto newTexture = Common::NewShared<OpenGLTexture>(id);

		return newTexture;
	}

	OpenGLTexture::OpenGLTexture(const TextureStructure& texStruct) {
		OpenGLTexture::Generate();
		OpenGLTexture::Regenerate(texStruct);
	}

	OpenGLTexture::OpenGLTexture(const unsigned int id) {
		if (!glIsTexture(id)) throw Common::RuntimeError("Failed to create reference to texture, ID " + std::to_string(id) + " is invalid");
		mBufferID = id;
	}

	void OpenGLTexture::Generate() {
		glGenTextures(1, &mBufferID);
	}

	void OpenGLTexture::Activate(unsigned int unitOffset) {
		glActiveTexture(GL_TEXTURE0 + unitOffset);
		glBindTexture(GL_TEXTURE_2D, mBufferID);
	}

	void OpenGLTexture::Regenerate(const TextureStructure& texStruct) {
		GLint iFmt = 0;
		GLenum fmt = 0, type = 0;

		switch (texStruct.Format) {
		default: case TextureFormat::NONE:
			break;
		case TextureFormat::R8:
			iFmt = GL_R8;
			fmt = GL_R;
			type = GL_UNSIGNED_BYTE;
			break;
		case TextureFormat::RGB8:
			iFmt = GL_RGB8;
			fmt = GL_RGB;
			type = GL_UNSIGNED_BYTE;
			break;
		case TextureFormat::RGB16F:
			iFmt = GL_RGB16F;
			fmt = GL_RGB;
			type = GL_FLOAT;
			break;
		case TextureFormat::RGB32F:
			iFmt = GL_RGB32F;
			fmt = GL_RGB;
			type = GL_FLOAT;
			break;
		case TextureFormat::RGBA8:
			iFmt = GL_RGBA8;
			fmt = GL_RGBA;
			type = GL_UNSIGNED_BYTE;
			break;
		case TextureFormat::RGBA16F:
			iFmt = GL_RGBA16F;
			fmt = GL_RGBA;
			type = GL_FLOAT;
			break;
		case TextureFormat::RGBA32F:
			iFmt = GL_RGBA32F;
			fmt = GL_RGBA;
			type = GL_FLOAT;
			break;
		}

		Activate(0);

		glTexImage2D(GL_TEXTURE_2D, 0, iFmt, texStruct.Width, texStruct.Height, 0, fmt, type, nullptr);
		if (texStruct.MipsEnabled) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
		mStructure = texStruct;
	}

	void OpenGLTexture::Unload() {
		if (!glIsTexture(mBufferID)) return;
		glDeleteTextures(1, &mBufferID);
	}

	Math::Vector2 OpenGLTexture::GetSize() {
		int w, h;
		glBindTexture(GL_TEXTURE_2D, mBufferID);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
		return {w, h};
	}
}

