#include <memory>
#include <string>
#include <nlohmann/json.hpp>
#include "AST.hpp"
#include "nlohmann/json_fwd.hpp"

#pragma once
using json = nlohmann::ordered_json;
using jsonObject = json::object_t;

class Parser
{
    public:
        Parser(const json &json);
        AstPtr getAst();
    private:
        AstPtr parseJsonObject(const json &object);
        AstPtr parseJsonArray(const std::string &name, const json::array_t &array);
        AstPtr parseJsonPrimitive(const std::string &name, const json &primitive);

        AstPtr parseUserStories(const json &primitive);
        AstPtr parseDoD(const json &primitive);
        AstPtr parseBullets(const json &primitive);
        AstPtr parseString(const std::string &key, const json &primitive, AST::Type type);
        AstPtr _ast {};
};
