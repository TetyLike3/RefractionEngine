#include "Mesh.h"

Mesh::Mesh(vector<sVertex> vertices, vector<unsigned int> indices, vector<BaseTexture> textures) {
	m_vertices = vertices;
	m_indices = indices;
	m_textures = textures;

	SetupMesh();
}

void Mesh::Draw(BaseShader& shader) {
	unsigned int diffuseIndex = 1;
	unsigned int specularIndex = 1;
	for (unsigned int i = 0; i < m_textures.size(); i++) {
		m_textures[i].Activate(i);
		std::string number;
		std::string name = m_textures[i].GetTextureType();
		if (name == "texDiffuse")
			number = std::to_string(diffuseIndex++);
		else if (name == "texSpecular")
			number = std::to_string(specularIndex++);

		shader.setUniformInt(name + number, i);
	}
	glActiveTexture(GL_TEXTURE0);

	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::SetupMesh() {
	// Create buffers
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_EBO);

	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(sVertex), &m_vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);

	// Load vertex data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(sVertex), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(sVertex), (void*)offsetof(sVertex, normal));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(sVertex), (void*)offsetof(sVertex, texCoord));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}