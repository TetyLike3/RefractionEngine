#include <fstream>
#include <thread>

#include <json.hpp>

#include <Core/FileHandling.h>
#include <Classes/Objects/BasicObject.h>
#include <Classes/ClassSerialiser.h>
#include <Interface/AssetImporter.h>

#include "Project.h"

namespace Refraction::Engine {
	bool SaveProjectData(const std::filesystem::path& projectFilePath, const ProjectData& projectData) {
		auto pathStr = projectFilePath.string();
		if (!(projectFilePath.has_extension() && projectFilePath.extension() == RFCT_PROJECT_EXTENSION)) {
			Log::SError("Invalid project file extension in path " + pathStr);
			return false;
		}
		if (!std::filesystem::exists(projectFilePath.parent_path())) {
			Log::SError("Invalid project directory in path " + pathStr);
			return false;
		}

		auto serialised = Utilities::ClassSerialiser::AppendJSON({}, [&](nlohmann::json& json) {
			json["InitSceneUUID"] = projectData.InitSceneUUID.Serialise();
			json["ActiveCameraUUID"] = projectData.ActiveCamera->GetUUID().Serialise();
			json["Scenes"] = {};
			for (auto& scene : projectData.Scenes) {
				json["Scenes"][scene->GetUUID().Serialise()] = Utilities::ClassSerialiser::Serialise(scene);
				Log::SInfo("Serialised scene with UUID " + scene->GetUUID().AsString());
			}
			json["GlobalObjects"] = {};
			for (auto& globalObj : projectData.GlobalObjects) {
				json["GlobalObjects"][globalObj->GetUUID().Serialise()] = Utilities::ClassSerialiser::Serialise(globalObj);
			}
		});
		
		std::ofstream dataFile(projectFilePath);
		if (!dataFile.is_open()) {
			Log::SError("Could not open path " + pathStr + " for writing.");
			return false;
		}
		dataFile << serialised.dump(RFCT_JSON_INDENT);

		Log::SInfo("Saved project data to " + pathStr);
		return true;
	}

	std::optional<ProjectData> LoadProjectData(const std::filesystem::path& projectFilePath) {
		auto pathStr = projectFilePath.string();
		if (!(std::filesystem::exists(projectFilePath) && std::filesystem::is_regular_file(projectFilePath) && projectFilePath.has_extension() && projectFilePath.extension() == RFCT_PROJECT_EXTENSION)) {
			Log::SWarn("Invalid or missing project file at path " + pathStr);
			return std::nullopt;
		}
		
		std::optional<ProjectData> result = std::nullopt;
		auto contents = FileHandling::ReadFile(projectFilePath);
		Utilities::ClassSerialiser::TryParseJSON(contents, [&](nlohmann::json& json) {
			ProjectData deserialised;
			deserialised.InitSceneUUID = UUID::Deserialise(json["InitSceneUUID"]);
			Log::SInfo("Loaded InitSceneUUID as " + deserialised.InitSceneUUID.AsString());
			for (auto& sceneData : json.at("Scenes")) {
				auto scene = Utilities::ClassSerialiser::DeserialiseObject<Objects::SceneRoot>(sceneData.dump());
				Log::SInfo("Loaded Scene with UUID " + scene->GetUUID().AsString());
				deserialised.Scenes.push_back(scene);
			}

			bool cameraDefined = json.contains("ActiveCameraUUID");
			UUID cameraUUID;
			if (cameraDefined) cameraUUID = UUID::Deserialise(json["ActiveCameraUUID"]);

			for (auto& globalObjData : json.at("GlobalObjects")) {
				auto object = Utilities::ClassSerialiser::DeserialiseObject(globalObjData.dump());
				deserialised.GlobalObjects.push_back(object);
				if (cameraDefined && (object->GetUUID() == cameraUUID)) {
					auto camera = dynamic_pointer_cast<Objects::Camera>(object);
					Objects::Camera::ActiveCamera = camera;
					deserialised.ActiveCamera = camera;
				};
			}

			if (!cameraDefined || !deserialised.ActiveCamera) {
				Log::SWarn("No active camera set, creating new camera");
				auto camera = Common::NewShared<Objects::Camera>();
				Objects::Camera::ActiveCamera = camera;
				deserialised.ActiveCamera = camera;
				deserialised.GlobalObjects.push_back(camera);
			}
			result = std::make_optional(deserialised);
		});

		return result;
	}

	std::filesystem::path GetProjectFilePath(const std::filesystem::path& projectPath) {
		if (!std::filesystem::exists(projectPath)) {
			Log::SError("Project path " + projectPath.string() + " does not exist");
			return std::filesystem::path();
		}
		if (projectPath.empty()) {
			Log::SError("Project path " + projectPath.string() + " is empty");
			return std::filesystem::path();
		}
		std::string projectName = projectPath.filename().string();
		if (projectName.empty()) {
			Log::SError("Could not get project name from path " + projectPath.string());
			return std::filesystem::path();
		}
		return projectPath / (projectName + RFCT_PROJECT_EXTENSION);
	}

	bool Project::New(const std::filesystem::path& projectPath, bool eraseExisting) {
		auto pathStr = projectPath.string();
		if (!std::filesystem::exists(projectPath)) {
			Log::SWarn("Could not create project, path does not exist");
			return false;
		}
		if (!std::filesystem::is_directory(projectPath)) {
			Log::SWarn(pathStr + " is not a valid directory");
			return false;
		}

		if (std::filesystem::exists(projectPath) && !std::filesystem::is_empty(projectPath)) {
			if (!eraseExisting) {
				if (FileHandling::GetFirstFileOfExtInFolder(projectPath, RFCT_PROJECT_EXTENSION).exists()) {
					// TODO: TEMP: ignore !eraseExisting if there is a project file inside
					auto items = FileHandling::GetItemsInFolder(projectPath);
					for (auto& item : items) {
						if(!std::filesystem::remove_all(item)) Log::SWarn("Failed to erase " + item.path().filename().string());
					}
				} else {
					Log::SError(pathStr + " already exists and eraseExisting is false");
					return false;
				}
			} else {
				std::filesystem::remove_all(projectPath);
			}
		} else if (!std::filesystem::exists(projectPath)) {
			if (!std::filesystem::create_directory(projectPath)) {
				Log::SError("Failed to create project directory at " + pathStr);
				return false;
			}
		}
		if (!std::filesystem::create_directory(projectPath / "Assets")) {
			Log::SError("Failed to create project assets directory at " + pathStr);
			return false;
		}

		Log::SInfo("Creating project at " + pathStr);

		// Placeholder project file
		auto projectFilePath = GetProjectFilePath(projectPath);
		std::ofstream dataFile(projectFilePath);
		if (!dataFile.is_open()) {
			Log::SError("Could not open path " + pathStr + " for writing.");
			return false;
		}
		dataFile << "hi";
		dataFile.close();

		mRootObject = Common::NewShared<Objects::AObject>();
		mProjectPath = projectPath;
		mProjectData = ProjectData{};

		// Import default assets
		auto assetManager = AssetManager::MakeInstance(projectPath).lock();
		if (!assetManager) throw Common::RuntimeError("Failed to instantiate AssetManager");
		AssetImporter::Import(FileHandling::GetResourcesPath() / "textures" / "Basic.png");
		for (auto& shader : FileHandling::GetFoldersInFolder(FileHandling::GetResourcesPath() / "shaders")) {
			AssetImporter::Import(shader.path());
		}

		// Default global objects
		auto camera = Common::NewShared<Objects::Camera>();
		Objects::Camera::ActiveCamera = camera;
		mRootObject->AddChild(camera);
		mProjectData.ActiveCamera = camera;
		mProjectData.GlobalObjects.push_back(camera);

		auto nyenMeshUUID = AssetImporter::Import(FileHandling::GetResourcesPath() / "models/nyen/nyen plush.obj");
		auto testMesh = Common::NewShared<Objects::BasicObject>();
		camera->AddChild(testMesh);
		testMesh->GetComponent<Components::Mesh>()->mModel = assetManager->GetAsset<Assets::Model>(nyenMeshUUID);

		if (!Save()) {
			Log::SError("Failed to create initial save of project data at " + pathStr);
			return false;
		}
		NewScene();
		Log::SInfo("Created project at " + pathStr);
		return true;
	}

	bool Project::NewRemote() {
		return false;
	}

	bool Project::Open(const std::filesystem::path& projectFilePath) {
		auto pathStr = projectFilePath.string();
		if (!std::filesystem::exists(projectFilePath) || !std::filesystem::is_regular_file(projectFilePath) || projectFilePath.extension() != RFCT_PROJECT_EXTENSION) {
			Log::SError("Attempt to open invalid project path at " + pathStr);
			return false;
		}

		// Close any active project
		if (IsLoaded()) Close();

		auto projectFolderPath = projectFilePath.parent_path();
		mProjectPath = projectFolderPath;

		auto assetManager = AssetManager::MakeInstance(mProjectPath).lock();
		if (!assetManager) throw Common::RuntimeError("Failed to instantiate AssetManager");

		// Create new root
		mRootObject = Common::NewShared<Objects::AObject>();

		AssetManager::Try([&](Common::Shared<AssetManager> assetManager) {
			assetManager->RegisterAllAssets();
		});

		auto actualProjectFilePath = GetProjectFilePath(projectFolderPath);
		auto projectData = LoadProjectData(actualProjectFilePath);
		mProjectData = projectData.value_or(ProjectData{});

		if (!projectData) Log::SWarn("Failed to load project data at " + pathStr);

		for (auto& scene : mProjectData.Scenes) {
			mRootObject->AddChild(scene);
		}
		for (auto& globalObj : mProjectData.GlobalObjects) {
			mRootObject->AddChild(globalObj);
		}

		// Open init scene
		if (mProjectData.InitSceneUUID != UUID::Null()) {
			if (!OpenScene(mProjectData.InitSceneUUID)) {
				Log::SWarn("Failed to open specified InitScene, using first scene found instead");
				if (mProjectData.Scenes.size() > 0) {
					mProjectData.InitSceneUUID = mProjectData.Scenes[0]->GetUUID();
					OpenScene(mProjectData.InitSceneUUID);
				}
			}
		}

		Log::SInfo("Opened project at " + pathStr);
		return true;
	}

	bool Project::Save() {
		if (!IsLoaded()) {
			Log::SWarn("Attempt to save project when one isn't loaded");
			return false;
		}

		bool success = true;
		auto projectFilePath = GetProjectFilePath(mProjectPath);
		if (!SaveProjectData(projectFilePath, mProjectData)) {
			success = false;
			Log::SError("Failed to save project data at " + projectFilePath.string());
		} else Log::SInfo("Saved project data at " + projectFilePath.string());

		return success;
	}

	void Project::Close() {
		if (!IsLoaded()) {
			Log::SWarn("Attempt to close project when one isn't loaded");
			return;
		}

		Log::SInfo("Closing project at " + mProjectPath.string());

		Objects::Camera::ActiveCamera = nullptr;
		for (auto& scene : mProjectData.Scenes) {
			scene.reset();
		}
		for (auto& globalObject : mProjectData.GlobalObjects) {
			globalObject.reset();
		}
		mProjectPath.clear();
		mProjectData = ProjectData{};

		AssetManager::Try([&](Common::Shared<AssetManager> assetManager) {
			assetManager->UnloadAll();
		});
		Log::SInfo("Closed project successfully");
	}

	void Project::ProcessRemoteMessage(std::string message) {
		if (!IsRemote()) return; // Not a remote project so this shouldn't run

		Utilities::ClassSerialiser::TryParseJSON(message, [&](nlohmann::json json) {
			if (!json.contains("Command")) {
				Log::Editor.Warn("Unable to process remote message");
				return;
			}
			RemoteProjectCommand cmd = json.at("Command").get<RemoteProjectCommand>();

			switch (cmd) {
			case RemoteProjectCommand::AddObject:
			{
				if (!json.contains("ParentUUID")) {
					Log::Editor.Warn("No Parent UUID provided with command, ignoring message");
					return;
				}
				if (!json.contains("SerialisedObject")) {
					Log::Editor.Warn("No valid serialised data provided with command, ignoring message");
					return;
				}
				auto uuid = UUID::FromExisting(json.at("ParentUUID").get<uint64_t>(), true);
				auto serialised = json.at("SerialisedObject");

				// Get target with given UUID
				Objects::AObject* parent = nullptr;
				for (auto& obj : mProjectData.GlobalObjects) {
					if (obj->GetUUID() == uuid) {
						parent = obj.get();
						break;
					}
					parent = Objects::AObject::GetInstanceWithUUID(uuid, obj.get());
					if (parent) break;
				}
				if (!parent) {
					for (auto& scene : mProjectData.Scenes) {
						parent = Objects::AObject::GetInstanceWithUUID(uuid, scene.get());
						if (parent) break;
					}
				}

				// Ignore if no target found
				if (!parent) break;

				// Add new object
				auto newObj = Utilities::ClassSerialiser::DeserialiseObject(serialised.dump());
				newObj->mParent = parent;

				Log::Editor.Info("Successfully added object " + uuid.AsString() + " from remote message");
				break;
			}
			case RemoteProjectCommand::AddComponent:
			{
				if (!json.contains("ParentUUID")) {
					Log::Editor.Warn("No Parent UUID provided with command, ignoring message");
					return;
				}
				if (!json.contains("SerialisedComponent")) {
					Log::Editor.Warn("No valid serialised data provided with command, ignoring message");
					return;
				}
				auto uuid = UUID::FromExisting(json.at("ParentUUID").get<uint64_t>(), true);
				auto serialised = json.at("SerialisedComponent");

				// Get target with given UUID
				Objects::AObject* parent = nullptr;
				for (auto& obj : mProjectData.GlobalObjects) {
					if (obj->GetUUID() == uuid) {
						parent = obj.get();
						break;
					}
					parent = Objects::AObject::GetInstanceWithUUID(uuid, obj.get());
					if (parent) break;
				}
				if (!parent) {
					for (auto& scene : mProjectData.Scenes) {
						parent = Objects::AObject::GetInstanceWithUUID(uuid, scene.get());
						if (parent) break;
					}
				}

				// Ignore if no target found
				if (!parent) break;

				// Add new component
				auto newComp = Utilities::ClassSerialiser::DeserialiseComponent(serialised.dump());
				newComp->mParent = parent;

				Log::Editor.Info("Successfully added component " + uuid.AsString() + " from remote message");
				break;
			}
			case RemoteProjectCommand::UpdateObject:
			{
				if (!json.contains("UUID")) {
					Log::Editor.Warn("No UUID provided with command, ignoring message");
					return;
				}
				if (!json.contains("SerialisedObject")) {
					Log::Editor.Warn("No valid serialised data provided with command, ignoring message");
					return;
				}
				auto uuid = UUID::FromExisting(json.at("UUID").get<uint64_t>(), true);
				std::string serialised = json.at("SerialisedObject");

				// Get target with given UUID
				Objects::AObject* target = nullptr;
				for (auto& obj : mProjectData.GlobalObjects) {
					if (obj->GetUUID() == uuid) {
						target = obj.get();
						break;
					}
					target = Objects::AObject::GetInstanceWithUUID(uuid, obj.get());
					if (target) break;
				}
				if (!target) {
					for (auto& scene : mProjectData.Scenes) {
						target = Objects::AObject::GetInstanceWithUUID(uuid, scene.get());
						if (target) break;
					}
				}

				// Ignore if no target found
				if (!target) break;

				// Update object
				target->Deserialise(serialised);

				Log::Editor.Info("Successfully updated object " + uuid.AsString() + " from remote message");
				break;
			}
			case RemoteProjectCommand::UpdateComponent:
			{
				if (!json.contains("UUID")) {
					Log::Editor.Warn("No UUID provided with command, ignoring message");
					return;
				}
				if (!json.contains("SerialisedComponent")) {
					Log::Editor.Warn("No valid serialised data provided with command, ignoring message");
					return;
				}
				auto uuid = UUID::FromExisting(json.at("UUID").get<uint64_t>(), true);
				std::string serialised = json.at("SerialisedComponent");

				// Get target with given UUID
				Objects::AObject* targetParent = nullptr;
				Components::AComponent* target = nullptr;
				for (auto& obj : mProjectData.GlobalObjects) {
					targetParent = Objects::AObject::GetInstanceWithUUID(uuid, obj.get());
					if (targetParent) break;
				}
				if (!target) {
					for (auto& scene : mProjectData.Scenes) {
						targetParent = Objects::AObject::GetInstanceWithUUID(uuid, scene.get());
						if (targetParent) break;
					}
				}

				// Ignore if no target parent found (therefore component doesn't exist)
				if (!targetParent) break;

				for (auto& comp : *targetParent->GetComponents()) {
					if (comp->GetUUID() != uuid) continue;
					target = comp.get();
					break;
				}

				// Update object
				target->Deserialise(serialised);

				Log::Editor.Info("Successfully updated component " + uuid.AsString() + " from remote message");
				break;
			}
			case RemoteProjectCommand::RemoveInstance:
			{
				if (!json.contains("UUID")) {
					Log::Editor.Warn("No UUID provided with command, ignoring message");
					return;
				}
				auto uuid = UUID::FromExisting(json.at("UUID").get<uint64_t>(), true);

				// Get target with given UUID
				Objects::AObject* target = nullptr;
				for (size_t i = 0; i < mProjectData.GlobalObjects.size(); i++) {
					auto& obj = mProjectData.GlobalObjects[i];
					// Test if this object is the target
					if (obj->GetUUID() == uuid) {
						mProjectData.GlobalObjects.erase(std::next(mProjectData.GlobalObjects.begin(), i - 1));
						break;
					}
					// Test descendants
					target = Objects::AObject::GetInstanceWithUUID(uuid, obj.get());
					if (target) break;
				}

				if (!target) {
					for (size_t i = 0; i < mProjectData.Scenes.size(); i++) {
						auto& obj = mProjectData.Scenes[i];
						// Test if this object is the target
						if (obj->GetUUID() == uuid) {
							mProjectData.Scenes.erase(std::next(mProjectData.Scenes.begin(), i - 1));
							break;
						}
						// Test descendants
						target = Objects::AObject::GetInstanceWithUUID(uuid, obj.get());
						if (target) break;
					}
				}

				// Ignore if no target found
				if (!target) break;

				target->RemoveChild(uuid);

				break;
			}
			}
		});
	}

	Common::Ref<Objects::SceneRoot> Project::NewScene() {
		Log::SInfo("Creating a new scene");
		auto newScene = Common::NewShared<Objects::SceneRoot>();
		mRootObject->AddChild(newScene);
		mProjectData.Scenes.push_back(newScene);

		// Instantiate default objects/components
		///

		auto assetManager = AssetManager::GetInstance().lock();
		if (!assetManager) throw Common::RuntimeError("Failed to create scene, no AssetManager instance");

		auto nyenMeshUUID = AssetImporter::Import(FileHandling::GetResourcesPath() / "models/nyen/nyen plush.obj");
		auto nyenObj = Common::NewShared<Objects::BasicObject>();
		nyenObj->mInstanceName = "Nyen";
		nyenObj->GetComponent<Components::Mesh>()->mModel = assetManager->GetAsset<Assets::Model>(nyenMeshUUID);
		nyenObj->GetComponent<Components::APhysics>()->mAngularVelocity = Math::Vector3(0, 64, 0);
		newScene->AddChild(nyenObj);

		auto backpackMeshUUID = AssetImporter::Import(FileHandling::GetResourcesPath() / "models/survivalBackpack/backpack.obj");
		auto backpackObj = Common::NewShared<Objects::BasicObject>();
		backpackObj->mInstanceName = "Backpack";
		backpackObj->GetComponent<Components::Mesh>()->mModel = assetManager->GetAsset<Assets::Model>(backpackMeshUUID);
		backpackObj->mTransform = Math::Transform::FromLookAt(Math::Vector3(0, 14, 10), Math::Vector3::Zero());
		newScene->AddChild(backpackObj);

		Log::SInfo("Successfully created a new scene with UUID " + newScene->GetUUID().AsString());
		mActiveScene = newScene;
		// Autoset as initScene if none is defined
		if (mProjectData.InitSceneUUID == UUID::Null()) {
			mProjectData.InitSceneUUID = newScene->GetUUID();
			// Add baseplate for convenience
			auto baseplateMeshUUID = AssetImporter::Import(FileHandling::GetResourcesPath() / "models/Basic/Cube.obj");
			auto baseplate = Common::NewShared<Objects::AObject>();
			baseplate->mInstanceName = "Baseplate";
			auto comp = baseplate->AddComponent<Components::Mesh>();
			comp->mModel = assetManager->GetAsset<Assets::Model>(baseplateMeshUUID);
			comp->mTransform.Translate(Math::Vector3(0, -8, 0));
			comp->mTransform.mScale = Math::Vector3(128, 8, 128);
			newScene->AddChild(baseplate);
		}
		return mActiveScene;
	}

	bool Project::OpenScene(UUID sceneUUID) {
		Common::Shared<Objects::SceneRoot> targetScene;
		for (auto& scene : mProjectData.Scenes) {
			if (scene->GetUUID().AsInt() == sceneUUID.AsInt()) {
				targetScene = scene;
				break;
			}
		}
		if (!targetScene) {
			Log::SError("Invalid Scene UUID provided (" + sceneUUID.AsString() + ")");
			return false;
		}
		Log::SInfo("Opening scene with UUID " + sceneUUID.AsString());

		mActiveScene = targetScene;
		return true;
	}
}