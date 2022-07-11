#include <filesystem>
#include "ModuleLoader.hpp"
#include "Config.hpp"

ModuleLoader::ModuleLoader(const Config::Object &data) : _modulesPath(data["ModulesPath"].as<Config::String>())
{
	using std::filesystem::directory_iterator;
    using fp = bool (*)( const std::filesystem::path&);
    std::size_t s = std::count_if(directory_iterator(_modulesPath), directory_iterator{}, (fp)std::filesystem::is_regular_file);
	_libraries.reserve(s);
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
