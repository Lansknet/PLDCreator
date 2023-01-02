#ifndef IMODULE_HPP_
#define IMODULE_HPP_

#include <string_view>
#include "StructuredData.hpp"
#include "AST.hpp"

#if defined(_WIN32) || defined(_WIN64)
#define LIB_EXPORT __declspec(dllexport)
#else
#define LIB_EXPORT
#endif

class IModule {
    public:
        virtual ~IModule() = default;

        virtual void Create(const Config::Object &data) = 0;

        virtual void Write(AstPtr ast) = 0;

        [[nodiscard]] virtual const std::string_view GetName() const noexcept = 0;
};
#endif /* !IMODULE_HPP_ */
