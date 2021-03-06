#include "Paths.h"
#include "ThirdParty/ValveFileVDF/vdf_parser.hpp"

#ifdef _WIN32
#include <Windows.h>
#include <winreg.h>
#endif

namespace Freeking
{
	std::filesystem::path Paths::SteamDir()
	{
		static std::filesystem::path dir;

		if (dir.empty())
		{
#ifdef _WIN32
			HKEY key;
			TCHAR value[1024];
			DWORD length = sizeof(value);
			LONG result = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Valve\\Steam", 0, KEY_READ, &key);
			if (result == 0)
			{
				RegQueryValueEx(key, "SteamPath", NULL, NULL, reinterpret_cast<LPBYTE>(&value), &length);
				RegCloseKey(key);

				return value;
			}
#endif
		}

		return dir;
	}

	std::filesystem::path Paths::KingpinDir()
	{
		static std::filesystem::path dir;

		if (dir.empty())
		{
#ifdef _WIN32
			HKEY key;
			TCHAR value[1024];
			DWORD length = sizeof(value);
			LONG result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\kingpin.exe", 0, KEY_READ, &key);
			if (result == 0)
			{
				RegQueryValueEx(key, "Path", NULL, NULL, reinterpret_cast<LPBYTE>(&value), &length);
				RegCloseKey(key);

				dir = value;
			}
			else
			{
				dir = SteamGameDir(38430);
			}
#endif
		}

		return dir;
	}

	std::filesystem::path Paths::SteamGameDir(uint32_t appid)
	{
		const auto& steamPath = SteamDir();
		if (steamPath.empty())
		{
			return {};
		}

		auto vdfPath = steamPath / "steamapps/libraryfolders.vdf";
		if (!std::filesystem::exists(vdfPath))
		{
			return {};
		}

		std::ifstream vdfFile(vdfPath);
		auto vdfRoot = tyti::vdf::read(vdfFile);

		std::vector<std::filesystem::path> libraryFolders;
		libraryFolders.push_back(steamPath);

		for (const auto& attrib : vdfRoot.attribs)
		{	
			const auto& key = attrib.first;
			if (key.size() != 1 || !std::isdigit(key[0]))
			{
				continue;
			}

			libraryFolders.push_back(attrib.second);
		}

		auto appidFilename = "appmanifest_" + std::to_string(appid) + ".acf";

		for (auto libraryFolder : libraryFolders)
		{
			for (const auto& file : std::filesystem::directory_iterator(libraryFolder / "steamapps"))
			{
				if (file.is_directory() ||
					file.path().extension() != ".acf")
				{
					continue;
				}

				if (file.path().filename() == appidFilename)
				{
					std::ifstream acfFile(file.path());
					auto acfRoot = tyti::vdf::read(acfFile);
					auto installDir = acfRoot.attribs["installdir"];
					auto gamePath = file.path().parent_path() /
						std::filesystem::path("common") /
						std::filesystem::path(installDir);

					return gamePath;
				}
			}
		}

		return {};
	}
}
