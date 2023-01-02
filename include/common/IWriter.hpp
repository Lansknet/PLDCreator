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
        virtual void Write(AstPtr ast) = 0;
		virtual void WriteFunctionalSchema(Buffer& buffer) = 0;
		virtual void WriteFrontPage(Buffer &buffer) = 0;
        virtual void WriteUserStories(Buffer &buffer, AstPtr ast) = 0;
        virtual void WriteString(Buffer &buffer, AstPtr ast) = 0;
		virtual void WriteInt32(Buffer &buffer, AstPtr ast) = 0;
        virtual void WriteDoD(Buffer &buffer, AstPtr ast) = 0;
        virtual void WriteBullets(Buffer &buffer, AstPtr ast) = 0;
        virtual void WriteAssignmentTable(Buffer &buffer, AstPtr ast) = 0;
        virtual void WriteDeliverablesMap(Buffer &buffer, AstPtr ast) = 0;
		virtual void WriteTableOfContent(Buffer &buffer, AstPtr ast) = 0;
		virtual void WriteAdvancementReport(Buffer &buffer, AstPtr ast) = 0;
};

#endif /* !IWRITER_HPP_ */
