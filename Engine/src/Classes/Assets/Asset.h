#pragma once

#include <string>
#include <map>

#include <json.hpp>

#include <Core/Common.h>
#include <Core/UUID.h>
#include <Core/FileHandling.h>

#define RFCT_ASSET_METADATA_EXTENSION ".rfmeta"

namespace Refraction::Assets {
	enum class MetadataType {
		Asset,
		Image,
		Model,
		Shader,
		COUNT
	};

	constexpr const char* MetadataTypeName[] = {
		"Asset",
		"Image",
		"Model",
		"Shader"
	};

	struct AssetMetadata {
		UUID AssetUUID = UUID::Null();
		MetadataType MetaType = MetadataType::Asset;
		std::filesystem::path SourcePath = ""; // Path to the original source file
		std::filesystem::path AssetPath = ""; // Path to the actual asset file in the project
		std::string AssetType = "";
		uintmax_t FileSize = 0;

		// Returns a deserialised (derived) AssetMetadata object
		static Common::Shared<AssetMetadata> CastedDeserialise(const std::string &data);

		AssetMetadata() = default;
		~AssetMetadata() = default;

		// Returns path to the metadata file
		std::filesystem::path GetPath() const;

		// Serialises metadata into a string
		virtual nlohmann::json Serialise();
		// Loads metadata from the given string
		virtual void Deserialise(std::string data);
	};

	class Asset {
	public:
		Asset() = default;
		virtual ~Asset();

		// Loads asset of the provided UUID from disk
		void LoadAsset(UUIDValue uuid);
		// Saves asset changes to disk
		void Save();
		// Initialises the asset as a volatile asset (no data on disk)
		// Note: Will reset the data of the asset in memory
		void MakeVolatile();

		virtual MetadataType GetMetadataType() { return MetadataType::Asset; }

		inline bool IsVolatile() const { return mVolatile; }
		inline UUIDValue GetUUID() const { return mUUID; }
	protected:
		std::string mDisplayName;
		// Asset does not have a location on disk if true
		bool mVolatile = false;

		// DERIVED METHOD: Load data into memory based on the provided metadata
		virtual void OnLoadAsset(Common::Shared<AssetMetadata> metadata) {}
		// DERIVED METHOD: Re-initialise asset when made volatile
		virtual void OnMakeVolatile() {}
		// DERIVED METHOD: Save any changes to disk
		void OnSave() {}
	private:
		UUIDValue mUUID = 0;
	};
}