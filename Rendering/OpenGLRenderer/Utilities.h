#pragma once

#include <string>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <chrono>
#include <iterator>
#include <format>

namespace fs = std::filesystem;


class Utilities
{
public:
	static std::string EngineWorkingDirectory;
	static std::vector<char> ReadFile(const std::string& filename);
	static bool ReadFile(const std::string& filename, std::string& buffer); // Writes contents of file to provided string

	static std::vector<fs::directory_entry> getFilesInFolder(fs::path folderPath);
	static std::vector<fs::directory_entry> getFilesOfExtInFolder(fs::path folderPath, std::string ext);
	static fs::directory_entry getFirstFileOfExtInFolder(fs::path folderPath, std::string ext);
	static std::vector<fs::directory_entry> getFoldersInFolder(fs::path folderPath);

	static std::string calculateFPS(double deltaTime, int precision = -1);
private:
	static std::string m_lastFilePrinted;
	static std::string m_lastMessagePrinted;
};