#ifndef HTMLWRITER_HPP_
#define HTMLWRITER_HPP_

#include "Config.hpp"
#include "IWriter.hpp"

class HTMLWriter : public IWriter
{
public:
	HTMLWriter() : _config()
	{
	}

	void Write(AstPtr ast) final;

	void WriteFunctionalSchema(Buffer& buffer) final;

	void WriteFrontPage(Buffer& buffer) final;

	void WriteUserStories(Buffer& buffer, AstPtr ast) final;

	void WriteString(Buffer& buffer, AstPtr ast) final;

	void WriteInt32(Buffer& buffer, AstPtr ast) final;

	void WriteDoD(Buffer& buffer, AstPtr ast) final;

	void WriteBullets(Buffer& buffer, AstPtr ast) final;

	void WriteAssignmentTable(Buffer& buffer, AstPtr ast) final;

	void WriteDeliverablesMap(Buffer& buffer, AstPtr ast) final;

	void WriteTableOfContent(Buffer& buffer, AstPtr ast) final;

	void WriteAdvancementReport(Buffer& buffer, AstPtr ast) final;

	virtual void Create(const Config::Object& data) final;

	[[nodiscard]] virtual const std::string_view GetName() const noexcept final;

private :
	void ParseRoot(Buffer& buffer, AstPtr ast);

	Config::Object _config;
};

#endif /* !HTMLWRITER_HPP_ */
