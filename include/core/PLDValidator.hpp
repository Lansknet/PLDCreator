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

	AstPtr GetAst();

private:
	AstPtr ParseRoot(const Config::Object& object);

	AstPtr ParseTableOfContent(const Config::Object& object);

	AstPtr ParseUserStoriesTableOfContent(const Config::Array& object);

	AstPtr ParseUserStories(const Config::Array& array);

	AstPtr ParseDoD(const Config::Array& array);

	AstPtr ParseBullets(const Config::Array& array);

	AstPtr ParseAssignmentTable(const Config::Object& array);

	AstPtr ParseDeliverablesMap(const Config::Array& array);

	AstPtr ParseAdvancementReport(const Config::Object& object);
	AstPtr _ast{};
};

#endif /* !PLDVALIDATOR_HPP_ */
