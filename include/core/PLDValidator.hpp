#ifndef PLDVALIDATOR_HPP_
#define PLDVALIDATOR_HPP_

#include <memory>
#include <string>
#include "Config.hpp"
#include "AST.hpp"

class PLDValidator
{
    public:
        PLDValidator(const Config::Object &data);
        AstPtr getAst();
    private:
        AstPtr parseRoot(const Config::Object &object);
        AstPtr parseUserStories(const Config::Array &array);
        AstPtr parseDoD(const Config::Array &array);
        AstPtr parseBullets(const Config::Array &array);
        AstPtr parseAssignmentTable(const Config::Array &array);
        AstPtr _ast {};
};

#endif /* !PLDVALIDATOR_HPP_ */
