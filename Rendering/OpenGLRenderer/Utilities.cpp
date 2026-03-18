#include "Utilities.h"


using std::string, std::vector, fs::directory_entry, fs::directory_iterator, fs::path;

string Utilities::m_lastFilePrinted = "";
string Utilities::m_lastMessagePrinted = "";



// Needs a NUL terminator, otherwise operations like shader compilation will find junk characters at the end of the file
const char* fileTerminator = "\0";

vector<char> Utilities::ReadFile(const string& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file!");
	};

	size_t fileSize = (size_t)file.tellg();
	vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();
	buffer.insert(buffer.end(), *fileTerminator);

	return buffer;
};

bool Utilities::ReadFile(const string& filename, string& buffer) {
	vector<char> charBuf;
	try {
		charBuf = ReadFile(filename);
	} catch (...) {
		return false;
	}
	buffer = string(charBuf.begin(), charBuf.end());
	return true;
}

vector<directory_entry> Utilities::getFilesInFolder(path folderPath)
{
	vector<directory_entry> files;
	for (const auto& file : directory_iterator(folderPath))
	{
		files.push_back(file);
	};
	return files;
};

vector<directory_entry> Utilities::getFilesOfExtInFolder(path folderPath, string ext)
{
	vector<directory_entry> files;
	for (const auto& file : directory_iterator(folderPath))
	{
		if (file.path().extension() == ext)
		{
			files.push_back(file);
		};
	};
	return files;
}
directory_entry Utilities::getFirstFileOfExtInFolder(path folderPath, string ext)
{
	for (const auto& file : directory_iterator(folderPath))
	{
		if (file.path().extension() == ext)
		{
			return file;
		};
	};
	return directory_entry();
};

vector<directory_entry> Utilities::getFoldersInFolder(path folderPath)
{
	vector<directory_entry> folders;
	for (const auto& folder : fs::recursive_directory_iterator(folderPath))
	{
		if (!folder.is_directory()) continue;
		folders.push_back(folder);
	};
	return folders;
};

string Utilities::calculateFPS(double deltaTime, int precision) {
	double fps = 100 / deltaTime;
	if (precision < 0) return std::to_string(fps);
	if (precision == 0) return std::to_string(trunc(fps));

	string fpsString = std::to_string(fps);
	return fpsString.substr(0, fpsString.find(".") + precision + 1);
}