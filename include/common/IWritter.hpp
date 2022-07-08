#ifndef IWRITTER_HPP_
#define IWRITTER_HPP_

#include <string>
#include "AST.hpp"
#include "StructuredData.hpp"
#include "IModule.hpp"

using Buffer = std::string;

class IWritter : public IModule
{
    public:
        virtual ~IWritter() = default;
        virtual void write(AstPtr ast) = 0;
        virtual void writeUserStories(Buffer &buffer, AstPtr ast) = 0;
        virtual void writeString(Buffer &buffer, AstPtr ast) = 0;
        virtual void writeDoD(Buffer &buffer, AstPtr ast) = 0;
        virtual void writeBullets(Buffer &buffer, AstPtr ast) = 0;
};

#endif /* !IWRITTER_HPP_ */
