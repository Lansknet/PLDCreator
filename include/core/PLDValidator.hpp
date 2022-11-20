#ifndef PLDVALIDATOR_HPP_
#define PLDVALIDATOR_HPP_

#include <memory>
#include <string>
#include "Config.hpp"
#include "AST.hpp"

class PLDValidator
{
public:
	PLDValidator(const Config::Object& data);

	AstPtr getAst();

private:
	AstPtr parseRoot(const Config::Object& object);

	AstPtr parseTableOfContent(const Config::Object& object);

	AstPtr parseUserStoriesTableOfContent(const Config::Array& object);

	AstPtr parseUserStories(const Config::Array& array);

	AstPtr parseDoD(const Config::Array& array);

	AstPtr parseBullets(const Config::Array& array);

	AstPtr parseAssignmentTable(const Config::Object& array);

	AstPtr parseDeliverablesMap(const Config::Array& array);

	AstPtr parseAdvancementReport(const Config::Object& object);
	AstPtr _ast{};
};

#endif /* !PLDVALIDATOR_HPP_ */
