#include "BaseModel.h"

void BaseModel::DrawModel() {
	m_pShader->Activate();
	m_pShader->setUniformMat4("modelTransform", m_pTransform->GetTransform());

	for (auto& mesh : m_meshes) 
		mesh.Draw(*m_pShader);
	//Log::Info("Drawn for model" + sourcePath);
}

void BaseModel::LoadModel(std::string path) {
	Log::Info("Loading model " + path);

	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		Log::Info("MODEL LOAD FAILED | " + std::string(import.GetErrorString()));
		return;
	}
	sourcePath = path.substr(0, path.find_last_of("/"));
	Log::Info("Parsing scene data...");
	ProcessNode(scene->mRootNode, scene);
}

void BaseModel::ProcessNode(aiNode* node, const aiScene* scene) {
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		m_meshes.push_back(ProcessMesh(mesh, scene));
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		ProcessNode(node->mChildren[i], scene);
	}
}

Mesh BaseModel::ProcessMesh(aiMesh* mesh, const aiScene* scene) {
	std::vector<sVertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<BaseTexture*> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		sVertex vertex;

		aiVector3D importPos = mesh->mVertices[i];
		vertex.pos = glm::vec3(importPos.x, importPos.y, importPos.z);

		aiVector3D importNormal = mesh->mNormals[i];
		vertex.normal = glm::vec3(importNormal.x, importNormal.y, importNormal.z);

		if (mesh->HasTextureCoords(0)) {
			aiVector3D importTexCoord = mesh->mTextureCoords[0][i];
			vertex.texCoord = glm::vec2(importTexCoord.x, importTexCoord.y);
		} else
			vertex.texCoord = glm::vec2(0.0f);

		vertices.push_back(vertex);
	}


	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		vector<BaseTexture*> diffuseMaps = LoadMaterialTextures(material,
			aiTextureType_DIFFUSE, REFRACT_TEXTURE_TYPE_DIFFUSE);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		vector<BaseTexture*> specularMaps = LoadMaterialTextures(material,
			aiTextureType_SPECULAR, REFRACT_TEXTURE_TYPE_SPECULAR);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}

	return Mesh(vertices, indices, textures);
}

vector<BaseTexture*> BaseModel::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName) {
	vector<BaseTexture*> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);

		std::string fullPath = sourcePath + "/" + str.C_Str();

		bool skip = false;
		for (unsigned int j = 0; j < m_textures.size(); j++)
		{
			if (std::strcmp(m_textures[j]->GetSourcePath().data(), fullPath.data()) == 0)
			{
				textures.push_back(m_textures[j]);
				skip = true;
				break;
			}
		}

		if (!skip) {
			BaseTexture* texture = BaseTexture::LoadTexture(fullPath, typeName);
			textures.push_back(texture);
			m_textures.push_back(texture);
		}
	}
	return textures;
}
