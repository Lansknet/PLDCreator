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

	void write(AstPtr ast) final;

	void writeFunctionalSchema(Buffer& buffer) final;

	void writeFrontPage(Buffer& buffer) final;

	void writeUserStories(Buffer& buffer, AstPtr ast) final;

	void writeString(Buffer& buffer, AstPtr ast) final;

	void writeInt32(Buffer& buffer, AstPtr ast) final;

	void writeDoD(Buffer& buffer, AstPtr ast) final;

	void writeBullets(Buffer& buffer, AstPtr ast) final;

	void writeAssignmentTable(Buffer& buffer, AstPtr ast) final;

	void writeDeliverablesMap(Buffer& buffer, AstPtr ast) final;

	void writeTableOfContent(Buffer& buffer, AstPtr ast) final;

	void writeAdvancementReport(Buffer& buffer, AstPtr ast) final;

	virtual void create(const Config::Object& data) final;

	[[nodiscard]] virtual const std::string_view getName() const noexcept final;

private :
	void parseRoot(Buffer& buffer, AstPtr ast);

	Config::Object _config;
};

#endif /* !HTMLWRITER_HPP_ */
