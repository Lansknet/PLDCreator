#ifndef MODULELOADER_HPP_
#define MODULELOADER_HPP_
#include <string>
#include <vector>

#include "Config.hpp"
#include "LibraryInstance.hpp"

class ModuleLoader {
    public:
        ModuleLoader(const Config::Object &data);
        std::vector<LibraryInstance> &getModules();
        ~ModuleLoader() = default;

    private:
        std::string _modulesPath;
        std::vector<LibraryInstance> _libraries;
};

#endif /* !MODULELOADER_HPP_ */
