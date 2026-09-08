#include <fstream>

#include <Classes/ClassHeaders.h>
#include <Interface/Project.h>

#include "AssetImporter.h"

namespace Refraction::Engine {
	UUIDValue AssetImporter::Import(std::filesystem::path sourcePath) {
		if (!sourcePath.has_filename()) throw Common::RuntimeError("Cannot import anything other than a file");
		auto currentProject = Project::GetInstance().lock();
		if (!currentProject) throw Common::RuntimeError("Cannot import without an active project");
		auto importPath = currentProject->GetFilePath().parent_path() / "Assets";

		auto assetManager = AssetManager::GetInstance().lock();
		if (!assetManager) throw Common::RuntimeError("Failed to get AssetManager instance");
		auto dstName = importPath / sourcePath.filename();

		// Check for existing asset
		auto existing = assetManager->GetAsset(dstName).lock();
		if (existing) {
			Log::SWarn("There is already an asset at " + dstName.string() + ", returning existing asset");
			return existing->GetUUID();
		}

		// Import
		Log::SInfo("Importing file from " + sourcePath.string() + " to " + dstName.string());
		std::filesystem::copy(sourcePath, dstName);
		if (!std::filesystem::exists(dstName)) throw Common::RuntimeError("Failed to copy asset during import");

		// Make metadata file
		Assets::AssetMetadata meta{};
		meta.MetaType = Assets::MetadataType::Asset;
		meta.AssetUUID = UUID();
		meta.AssetPath = dstName;
		meta.SourcePath = sourcePath;
		meta.FileSize = std::filesystem::file_size(dstName);

		auto extension = sourcePath.extension();
		if (extension == ".png" || extension == ".jpg" || extension == ".jpeg") {
			meta.MetaType = Assets::MetadataType::Image;
			meta.AssetType = typeid(Assets::Image).name();
		} else if (extension == ".obj" || extension == ".fbx") {
			meta.MetaType = Assets::MetadataType::Model;
			meta.AssetType = typeid(Assets::Model).name();
		} else if (extension.empty()) {
			auto files = FileHandling::GetFilesInFolder(sourcePath);
			if (files.size() == 2) {
				// Assume it's a shader
				if (FileHandling::GetFirstFileOfExtInFolder(sourcePath, ".frag").exists() && FileHandling::GetFirstFileOfExtInFolder(sourcePath, ".vert").exists()) {
					meta.MetaType = Assets::MetadataType::Shader;
					meta.AssetType = typeid(Assets::Shader).name();
				}
			}
		}
		// Define as regular asset (this prevents crashing but is a problem)
		if (meta.AssetType.empty()) {
			Log::SWarn("Asset at " + dstName.string() + " not specialised. May experience unexpected behaviour.");
			meta.AssetType = typeid(Assets::Asset).name();
		}

		auto metaPath = meta.GetPath(); // GetPath is valid because we set AssetPath
		// Make sure it's a full path before writing to it
		auto projectPath = currentProject->GetFilePath().parent_path();
		if (metaPath.string().find(projectPath.string()) == std::string::npos) {
			metaPath = projectPath / "Assets" / metaPath;
		}
		auto serialised = meta.Serialise();
		std::ofstream dataFile(metaPath);
		if (!dataFile.is_open()) throw Common::RuntimeError(std::format("Could not open {} for writing", metaPath.string()));
		dataFile << serialised.dump(RFCT_JSON_INDENT);
		dataFile.close();

		// Erase UUID before registering or the deserialisation will cause issues
		meta.AssetUUID.Reset();

		// Register asset
		UUIDValue uuid = 0;
		AssetManager::Try([&](Common::Shared<AssetManager> manager) {
			auto asset = manager->RegisterAsset(metaPath);
			uuid = asset.lock()->GetUUID();
		});
		return uuid;
	}
}
