#pragma once

#include <vector>
#include <string>
#include <filesystem>

namespace Refraction::FileHandling {
	std::filesystem::path GetWorkingDirectory();
	std::filesystem::path GetResourcesPath();
	void SetResourcesPath(std::filesystem::path path);

	// Returns a string of the contents of a file
	extern std::string ReadFile(const std::string& filename);
	// Returns a string of the contents of a file
	std::string ReadFile(const std::filesystem::path& filePath);

	extern bool DoesFileExist(const std::string& path);
	extern std::vector<std::filesystem::directory_entry> GetFilesInFolder(std::filesystem::path folderPath);
	extern std::vector<std::filesystem::directory_entry> GetFilesOfExtInFolder(std::filesystem::path folderPath, std::string ext);
	extern std::filesystem::directory_entry GetFirstFileOfExtInFolder(std::filesystem::path folderPath, std::string ext);
	extern std::vector<std::filesystem::directory_entry> GetFoldersInFolder(std::filesystem::path folderPath);
	// Returns all files and folders inside the given folder
	extern std::vector<std::filesystem::directory_entry> GetItemsInFolder(std::filesystem::path folderPath);
};
