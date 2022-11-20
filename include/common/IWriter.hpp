#ifndef IWRITER_HPP_
#define IWRITER_HPP_

#include <string>
#include "AST.hpp"
#include "StructuredData.hpp"
#include "IModule.hpp"

using Buffer = std::string;

class IWriter : public IModule
{
    public:
        virtual ~IWriter() = default;
        virtual void write(AstPtr ast) = 0;
		virtual void writeFunctionalSchema(Buffer& buffer) = 0;
		virtual void writeFrontPage(Buffer &buffer) = 0;
        virtual void writeUserStories(Buffer &buffer, AstPtr ast) = 0;
        virtual void writeString(Buffer &buffer, AstPtr ast) = 0;
		virtual void writeInt32(Buffer &buffer, AstPtr ast) = 0;
        virtual void writeDoD(Buffer &buffer, AstPtr ast) = 0;
        virtual void writeBullets(Buffer &buffer, AstPtr ast) = 0;
        virtual void writeAssignmentTable(Buffer &buffer, AstPtr ast) = 0;
        virtual void writeDeliverablesMap(Buffer &buffer, AstPtr ast) = 0;
		virtual void writeTableOfContent(Buffer &buffer, AstPtr ast) = 0;
		virtual void writeAdvancementReport(Buffer &buffer, AstPtr ast) = 0;
};

#endif /* !IWRITER_HPP_ */
