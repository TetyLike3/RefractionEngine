#pragma once

#include "BaseBuffer.h"

class GBuffer : public BaseBuffer {
public:
	enum GBUFFER_TEXTURE_TYPE {
		GBUFFER_TEXTURE_POSITION,
		GBUFFER_TEXTURE_DIFFUSE,
		GBUFFER_TEXTURE_NORMAL,
		GBUFFER_TEXTURE_TEXCOORD,
		GBUFFER_iTEXTURECOUNT
	};

	GBuffer();
	~GBuffer();

	bool Init(unsigned int viewWidth, unsigned int viewHeight);

	void BindForWrite();
	void BindForRead();
	void SetReadBuffer(GBUFFER_TEXTURE_TYPE texType);
private:
	GLuint m_fbo;
	GLuint m_textures[GBUFFER_iTEXTURECOUNT];
	GLuint m_depthTexture;
	GLuint m_finalTexture;
};