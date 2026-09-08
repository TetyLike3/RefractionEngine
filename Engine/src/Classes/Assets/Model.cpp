
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <Classes/ClassSerialiser.h>
#include <Interface/AssetManager.h>

#include "Model.h"

namespace Refraction::Assets {
	void ProcessNode(std::string sourcePath, std::vector<Common::Ref<Assets::Material>>& materials, std::vector<Common::Shared<Engine::Platform::AMeshFragment>>& fragments, aiNode* node, const aiScene* scene);
	Common::Shared<Engine::Platform::AMeshFragment> ProcessMesh(std::string& sourcePath, std::vector<Common::Ref<Assets::Material>>& materials, aiMesh* mesh, const aiScene* scene);
	std::vector<Common::Ref<Assets::Image>> LoadMaterialTextures(std::string& sourcePath, aiMaterial* mat, aiTextureType type, std::string typeName);

	void ProcessNode(std::string sourcePath, std::vector<Common::Ref<Assets::Material>>& materials, std::vector<Common::Shared<Engine::Platform::AMeshFragment>>& fragments, aiNode* node, const aiScene* scene) {
		for (unsigned int i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			fragments.push_back(ProcessMesh(sourcePath, materials, mesh, scene));
		}

		for (unsigned int i = 0; i < node->mNumChildren; i++) {
			ProcessNode(sourcePath, materials, fragments, node->mChildren[i], scene);
		}
	}

	Common::Shared<Engine::Platform::AMeshFragment> ProcessMesh(std::string& sourcePath, std::vector<Common::Ref<Assets::Material>>& materials, aiMesh* mesh, const aiScene* scene) {
		std::vector<Engine::sVertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Common::Shared<Assets::Image>> diffuseMaps;
		std::vector<Common::Shared<Assets::Image>> specularMaps;

		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			Engine::sVertex vertex;

			aiVector3D importPos = mesh->mVertices[i];
			vertex.pos = Math::Vector3(importPos.x, importPos.y, importPos.z);

			aiVector3D importNormal = mesh->mNormals[i];
			vertex.normal = Math::Vector3(importNormal.x, importNormal.y, importNormal.z);

			if (mesh->HasTextureCoords(0)) {
				aiVector3D importTexCoord = mesh->mTextureCoords[0][i];
				vertex.texCoord = Math::Vector2(importTexCoord.x, importTexCoord.y);
			} else
				vertex.texCoord = Math::Vector2(0.0f);

			vertices.push_back(vertex);
		}


		for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}

		return Engine::Platform::AMeshFragment::MakeMeshFragment(vertices, indices, materials[mesh->mMaterialIndex]);
	}

	std::vector<Common::Ref<Assets::Image>> LoadMaterialTextures(std::string& sourcePath, aiMaterial* mat, aiTextureType type, std::string typeName) {
		std::vector<Common::Ref<Assets::Image>> textures;
		Engine::AssetManager::Try([&](Common::Shared<Engine::AssetManager> assetManager) {
			for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
				aiString str;
				mat->GetTexture(type, i, &str);

				std::string fullPath = sourcePath + "/" + str.C_Str();

				auto texture = assetManager->GetAsset<Assets::Image>(fullPath);
				textures.push_back(texture);
			}
		});
		return textures;
	}

	nlohmann::json ModelMetadata::Serialise() {
		return Utilities::ClassSerialiser::AppendJSON(AssetMetadata::Serialise(), [&](nlohmann::json& json) {
			json["VertexCount"] = VertexCount;
			json["PolyCount"] = PolyCount;
		});
	}

	void ModelMetadata::Deserialise(std::string data) {
		AssetMetadata::Deserialise(data);
		Utilities::ClassSerialiser::TryParseJSON(data, [&](nlohmann::json& json) {
			if (json.contains("VertexCount")) VertexCount = json.at("VertexCount").get<int>();
			if (json.contains("PolyCount")) PolyCount = json.at("PolyCount").get<int>();
		});
	}

	void Model::OnLoadAsset(Common::Shared<AssetMetadata> metadata) {
		auto meta = Common::AsA<ModelMetadata>(metadata);
		if (!meta) {
			Log::SError("Metadata cast failed");
			return;
		}

		Assimp::Importer import;
		const aiScene* scene = import.ReadFile(meta->AssetPath.string(), aiProcess_Triangulate | aiProcess_FlipUVs);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			Log::SError("MODEL LOAD FAILED | " + std::string(import.GetErrorString()));
			return;
		}

		auto importSourcePath = meta->AssetPath.string().substr(0, meta->AssetPath.string().find_last_of("/"));

		// Create materials
		Log::SInfo("Parsing materials...");
		if (scene->mNumMaterials > 0) {
			for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
				auto importMat = scene->mMaterials[i];

				Common::Ref<Assets::Material> matWeak;
				Engine::AssetManager::Try([&](Common::Shared<Engine::AssetManager> assetManager) {
					matWeak = assetManager->MakeVolatile<Assets::Material>();
				});
				if (matWeak.expired()) continue;
				auto mat = matWeak.lock();

				auto diffuseMaps = LoadMaterialTextures(importSourcePath, importMat, aiTextureType_DIFFUSE, RFCT_TEXTURE_TYPE_DIFFUSE);
				if (diffuseMaps.size() > 0) mat->mDiffuse = diffuseMaps[0];
				else {
					Log::SWarn("Imported material does not associate with any diffuse textures, using default texture.");
				}
				auto specularMaps = LoadMaterialTextures(importSourcePath, importMat, aiTextureType_SPECULAR, RFCT_TEXTURE_TYPE_SPECULAR);
				if (specularMaps.size() > 0) mat->mSpecular = specularMaps[0];
				else {
					Log::SWarn("Imported material does not associate with any specular textures, using default texture.");
				}
				mMaterials.push_back(matWeak);
			}
		} else { // Create default material
			Common::Ref<Assets::Material> matWeak;
			Engine::AssetManager::Try([&](Common::Shared<Engine::AssetManager> assetManager) {
				matWeak = assetManager->MakeVolatile<Assets::Material>();
			});
			mMaterials.push_back(matWeak);
		}

		// Load meshes
		Log::SInfo("Parsing mesh data...");
		ProcessNode(importSourcePath, mMaterials, mFragments, scene->mRootNode, scene);
	}
}

