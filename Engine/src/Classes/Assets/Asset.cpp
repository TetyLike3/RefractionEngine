#include <json.hpp>
#include <utility>

#include <Core/FileHandling.h>
#include <Classes/ClassSerialiser.h>
#include <Classes/ClassHeaders.h>
#include <Interface/AssetManager.h>

#include "Asset.h"

namespace Refraction::Assets {
	Common::Shared<AssetMetadata> AssetMetadata::CastedDeserialise(const std::string& data) {
		MetadataType metaType;
		Utilities::ClassSerialiser::TryParseJSON(data, [&](nlohmann::json& json) {
			for (int i = 0; i < (int)MetadataType::COUNT; i++) {
				if (json["MetadataType"] == MetadataTypeName[i]) {
					metaType = (MetadataType)i;
					break;
				}
			}
		});

		Common::Shared<AssetMetadata> result;
		switch (metaType) {
		default: case MetadataType::Asset:
			result = Common::NewShared<AssetMetadata>();
			break;
		case MetadataType::Image:
			result = Common::NewShared<ImageMetadata>();
			break;
		case MetadataType::Model:
			result = Common::NewShared<ModelMetadata>();
			break;
		case MetadataType::Shader:
			result = Common::NewShared<ShaderMetadata>();
			break;
		}

		result->Deserialise(data);
		return result;
	}

	std::filesystem::path AssetMetadata::GetPath() const {
		if (AssetPath.empty()) return "";
		return AssetPath.stem().string() + RFCT_ASSET_METADATA_EXTENSION;
	}

	nlohmann::json AssetMetadata::Serialise() {
		return Utilities::ClassSerialiser::AppendJSON({}, [&](nlohmann::json& json) {
			json["AssetType"] = AssetType;
			json["MetadataType"] = MetadataTypeName[(int)MetaType];
			json["AssetUUID"] = AssetUUID.Serialise();
			json["SourcePath"] = SourcePath.string();
			json["AssetPath"] = AssetPath.string();
			json["FileSize"] = FileSize;
		});
	}

	void AssetMetadata::Deserialise(std::string data) {
		Utilities::ClassSerialiser::TryParseJSON(std::move(data), [&](nlohmann::json& json) {
			AssetUUID = UUID::Deserialise(json["AssetUUID"]);
			for (int i = 0; i < (int)MetadataType::COUNT; i++) {
				if (json["MetadataType"] == MetadataTypeName[i]) {
					MetaType = (MetadataType)i;
					break;
				}
			}
			AssetType = json["AssetType"].get<std::string>();
			SourcePath = std::filesystem::path(json["SourcePath"].get<std::string>());
			AssetPath = std::filesystem::path(json["AssetPath"].get<std::string>());
			FileSize = json["FileSize"].get<uintmax_t>();
		});
	}

	Asset::~Asset() = default;

	void Asset::LoadAsset(UUIDValue uuid) {
		Common::Ref<AssetMetadata> metaWeak;
		Engine::AssetManager::Try([&](const auto& assetManager) {
			metaWeak = assetManager->FetchMetadata(uuid);
		});

		if (auto meta = metaWeak.lock()) {
			mUUID = uuid;
			if (!std::filesystem::exists(meta->SourcePath) && !std::filesystem::exists(meta->AssetPath)) throw std::runtime_error("Failed to load asset with provided metadata, no asset path exists.");

			// Derived object actually loads asset
			OnLoadAsset(meta);
		} else {
			throw Common::RuntimeError("Failed to load asset with UUID " + UUID::AsString(uuid) + ", could not fetch metadata");
		}
	}

	void Asset::Save() {
		OnSave();
	}

	void Asset::MakeVolatile() {
		mVolatile = true;
		mUUID = UUID();

		OnMakeVolatile();
	}
}
