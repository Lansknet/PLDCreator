#ifndef LIBRARYINSTANCE_HPP_
#define LIBRARYINSTANCE_HPP_

#include <string>
#include <memory>
#include "IModule.hpp"
#include "dylib.hpp"

class LibraryInstance {
    public:
        LibraryInstance(const std::string &libPath);
        LibraryInstance(LibraryInstance&&) = default;
        LibraryInstance(const LibraryInstance&) = delete;
        ~LibraryInstance();
        IModule* operator->();
        LibraryInstance &operator=(const LibraryInstance&) = delete;
    protected:
    private:
        dylib _libraryInstance;
        IModule* _moduleInstance;
};

#endif /* !LIBRARYINSTANCE_HPP_ */
