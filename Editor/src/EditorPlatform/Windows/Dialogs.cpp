#define WIN32_LEAN_AND_MEAN

#include <format>
#include <windows.h>
#include <commdlg.h>
#include <ShObjIdl.h>

#include "Dialogs.h"

namespace Refraction::Editor::Dialogs {
	std::filesystem::path SelectFile(const char* ext, const char* title) {
		char buffer[MAX_PATH] = {};

		std::string filter = std::format("{} Files", ext);
		filter.push_back('\0');
		filter += std::format(" * {}", ext);
		filter.push_back('\0');
		filter.push_back('\0');

		OPENFILENAMEA options = {};
		options.lStructSize = sizeof(options);
		options.hwndOwner = nullptr;
		options.lpstrFilter = filter.c_str();
		options.lpstrTitle = title;
		options.nMaxFile = static_cast<DWORD>(sizeof(buffer));
		options.lpstrFile = buffer;
		options.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetOpenFileNameA(&options)) {
			return {buffer};
		}

		return {};
	}
	std::filesystem::path SelectFolder(const std::string& title) {
		IFileDialog* pDialog = nullptr;
		HRESULT hres = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_IFileDialog, reinterpret_cast<void**>(&pDialog));
		if (FAILED(hres) || !pDialog) return {};

		DWORD options;
		pDialog->GetOptions(&options);
		pDialog->SetOptions(options | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM);
		pDialog->SetTitle(std::wstring(title.begin(), title.end()).c_str());

		hres = pDialog->Show(nullptr);
		if (FAILED(hres)) {
			pDialog->Release();
			return {};
		}

		IShellItem* item = nullptr;
		hres = pDialog->GetResult(&item);
		if (FAILED(hres) || !item) {
			pDialog->Release();
			return {};
		}
		PWSTR pszPath = nullptr;
		hres = item->GetDisplayName(SIGDN_FILESYSPATH, &pszPath);
		std::filesystem::path result;

		if (SUCCEEDED(hres)) {
			result = std::filesystem::path(pszPath);
			CoTaskMemFree(pszPath);
		}
		item->Release();
		pDialog->Release();
		return result;
	}
}
