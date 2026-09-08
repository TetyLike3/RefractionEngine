#include <Classes/ClassSerialiser.h>

#include "AssetManager.h"

namespace Refraction::Engine {
	void AssetManager::RegisterAllAssets() {
		Log::SInfo("Registering all assets in project...");
		RecursiveRegisterAssets(mProjectPath / "Assets");
	}

	Common::Ref<Assets::Asset> AssetManager::RegisterAsset(std::filesystem::path metadataPath) {
		auto pathStr = metadataPath.string();
		if (!(std::filesystem::exists(metadataPath) && std::filesystem::is_regular_file(metadataPath) && metadataPath.has_extension() && metadataPath.extension() == RFCT_ASSET_METADATA_EXTENSION)) {
			Log::SError("Invalid metadata file at path " + pathStr);
			return Common::Ref<Assets::Asset>();
		}

		auto meta = LoadMetadata(metadataPath);
		auto asset = Utilities::ClassSerialiser::DeserialiseAsset(meta);
		auto& uuid = meta->AssetUUID;
		if (!mAssetMap.count(uuid)) {
			mAssetMap[uuid] = asset;
			return asset;
		} else {
			Log::SWarn("Asset with UUID " + uuid.AsString() + " already exists, returning existing asset");
			return mAssetMap.at(uuid);
		}
	}

	void AssetManager::UnloadAll() {
		for (auto it = mAssetMap.begin(); it != mAssetMap.end(); ) {
			auto& [uuid, asset] = *it;
			Log::SInfo("Unloading asset with UUID " + UUID::AsString(uuid));
			asset.reset();
			it = mAssetMap.erase(it);
		}

		for (auto it = mMetadataMap.begin(); it != mMetadataMap.end(); ) {
			auto& [uuid, asset] = *it;
			Log::SInfo("Unloading metadata with UUID " + UUID::AsString(uuid));
			asset.reset();
			it = mMetadataMap.erase(it);
		}
	}

	bool AssetManager::IsValidAsset(std::filesystem::path assetPath) {
		auto metaPathOpt = GetMetadataPath(assetPath);
		if (!metaPathOpt) return false;
		auto metaPath = metaPathOpt.value();
		return true;
	}

	std::optional<std::filesystem::path> AssetManager::GetMetadataPath(std::filesystem::path assetPath) {
		auto assetName = assetPath.filename().string();
		// Make sure it's a full path before searching it
		if (assetPath.string().find(mProjectPath.string()) == std::string::npos) assetPath = mProjectPath / "Assets" / assetPath;
		auto files = FileHandling::GetFilesOfExtInFolder(assetPath.parent_path(), RFCT_ASSET_METADATA_EXTENSION);
		for (auto& file : files) {
			if (file.path().filename().string() == assetName) return std::make_optional(file.path());
		}
		return std::nullopt;
	}

	void AssetManager::RecursiveRegisterAssets(std::filesystem::path folder) {
		auto metaFiles = FileHandling::GetFilesOfExtInFolder(folder, RFCT_ASSET_METADATA_EXTENSION);
		for (auto& metaFile : metaFiles) {
			RegisterAsset(metaFile);
		}
		auto folders = FileHandling::GetFoldersInFolder(folder);
		for (auto& child : folders) {
			RecursiveRegisterAssets(child);
		}
	}

	Common::Shared<Assets::AssetMetadata> AssetManager::RecursiveFindMetadataByUUID(std::filesystem::path folder, UUIDValue uuid) {
		auto metaFiles = FileHandling::GetFilesOfExtInFolder(folder, RFCT_ASSET_METADATA_EXTENSION);
		for (auto& metaFile : metaFiles) {
			auto dataStr = FileHandling::ReadFile(metaFile);
			auto meta = Assets::AssetMetadata::CastedDeserialise(dataStr);
			// Return result
			// TODO: Optimise by only doing CastedDeserialise after testing UUID
			if (meta->AssetUUID.AsInt() == uuid) return meta;
		}
		// Continue searching
		auto folders = FileHandling::GetFoldersInFolder(folder);
		for (auto& child : folders) {
			RecursiveFindMetadataByUUID(child, uuid);
		}
		return nullptr;
	}

	Common::Shared<Assets::AssetMetadata> AssetManager::LoadMetadata(std::filesystem::path metadataPath) {
		if (!std::filesystem::exists(metadataPath)) {
			Log::SError("Could not find metadata file at path " + metadataPath.string());
			return nullptr;
		}
		auto dataStr = FileHandling::ReadFile(metadataPath);

		// TODO: Optimise by checking if UUID already exists in map before doing CastedDeserialise
		auto meta = Assets::AssetMetadata::CastedDeserialise(dataStr);

		// Save in memory
		mMetadataMap[meta->AssetUUID] = meta;
		return meta;
	}
}
