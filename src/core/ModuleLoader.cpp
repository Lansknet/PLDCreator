#include <filesystem>
#include "ModuleLoader.hpp"
#include "Config.hpp"

ModuleLoader::ModuleLoader(const Config::Object &data) : _modulesPath(data["ModulesPath"].as<Config::String>())
{
    for (const auto& entry: std::filesystem::directory_iterator(_modulesPath))
	{
		if (entry.is_directory())
			continue;
		_libraries.emplace_back(entry.path().string());
	}
}

std::vector<LibraryInstance>& ModuleLoader::getModules()
{
    return _libraries;
}
