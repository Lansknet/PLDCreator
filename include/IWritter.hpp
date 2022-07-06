#include <string>
#include "AST.hpp"

using buffer = std::string;
class IWritter
{
    public:
        virtual void write(buffer &buffer, AstPtr ast) = 0;
        virtual void writeUserStories(buffer &buffer, AstPtr ast) = 0;
        virtual void writeString(buffer &buffer, AstPtr ast) = 0;
        virtual void writeDoD(buffer &buffer, AstPtr ast) = 0;
        virtual void writeBullets(buffer &buffer, AstPtr ast) = 0;
};
