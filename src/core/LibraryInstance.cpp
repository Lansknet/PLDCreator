#include "LibraryInstance.hpp"
#include "IModule.hpp"

LibraryInstance::LibraryInstance(const std::string &libPath) : _libraryInstance(libPath, false)
{
    _moduleInstance = _libraryInstance.get_function<IModule *()>("loadModule")();
}

LibraryInstance::~LibraryInstance()
{
    _libraryInstance.get_function<void(IModule*)>("unloadModule")(_moduleInstance);
    _moduleInstance = nullptr;
}

IModule* LibraryInstance::operator->()
{
    return _moduleInstance;
}