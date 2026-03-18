#pragma once

#include "../Graphics/BaseShader.h"
#include "../Graphics/BaseTexture.h"

#include <vector>

struct sVertex {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 texCoord;
};

using std::vector;

class Mesh
{
public:
	vector<sVertex> m_vertices;
	vector<unsigned int> m_indices;
	vector<BaseTexture*> m_textures;

	Mesh(vector<sVertex> vertices, vector<unsigned int> indices, vector<BaseTexture*> textures);
	void Draw(BaseShader &shader);
private:
	unsigned int m_VAO, m_VBO, m_EBO;

	void SetupMesh();
};