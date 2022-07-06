#include <any>
#include <exception>
#include <memory>
#include <stdexcept>
#include <iostream>
#include "Parser.hpp"
#include "AST.hpp"
#include "nlohmann/json_fwd.hpp"

Parser::Parser(const json &json)
{
    if (!json.is_object())
        throw std::logic_error("JSON: file should be a JSON object");
    _ast = parseJsonObject(json);
    _ast->type = AST::Type::Root;
}

AstPtr Parser::getAst()
{
    return std::move(_ast);
}

AstPtr Parser::parseJsonObject(const json &object)
{
    AstPtr ast = std::make_unique<AST>(AST::Type::Object);
    for (auto &[key, value] : object.items())
    {
        if (value.is_array() && key == "userStories")
                ast->next.push_back(parseUserStories(value));
        else throw std::logic_error("Unkow key" + key);

    }
    return std::move(ast);
}

AstPtr Parser::parseUserStories(const json &userStoriesObj)
{
    AstPtr ast = std::make_unique<AST>(AST::Type::UserStories);
    int i = 0;
    for (auto &us : userStoriesObj)
    {
        for (auto &[key, value]: us.items())
        {
            if (key == "Name")
                ast->next.push_back(parseString(key, value, AST::Type::Name));
            else if (key == "Description")
                ast->next.push_back(parseString(key, value, AST::Type::Description));
            else if (key == "EstimatedTime")
                ast->next.push_back(parseString(key, value, AST::Type::EstimatedTime));
            else if (key == "AsA")
                ast->next.push_back(parseString(key, value, AST::Type::AsA));
            else if (key == "IWant")
                ast->next.push_back(parseString(key, value, AST::Type::IWant));
            else if (key == "DoD")
                ast->next.push_back(parseDoD(value));
            else throw std::logic_error("Unknow key : " + key);
        }
    }
    return std::move(ast);
}

AstPtr Parser::parseDoD(const json &array)
{
    if (!array.is_array())
        throw std::logic_error("Wrong type, expected array" + array.dump());
    AstPtr ast = std::make_unique<AST>(AST::Type::DoD);
    for (auto &dod : array)
    {
        if (!dod.is_object())
            continue;
        for (auto &[key, value] : dod.items())
        {
            if (key == "Description")
                ast->next.push_back(parseString("Description", value, AST::Type::Description));
            else if (key == "bullets")
                ast->next.push_back(parseBullets(value));
        }
    }
    return std::move(ast);
}

AstPtr Parser::parseBullets(const json &array)
{
    if (!array.is_array())
        throw std::logic_error("Wrong type, expected array" + array.dump());
    AstPtr ast = std::make_unique<AST>(AST::Type::Bullets);
    ast->value = std::make_any<std::vector<std::string>>(array.get<std::vector<std::string>>());
    return std::move(ast);
}

AstPtr Parser::parseString(const std::string &key, const json &primitive, AST::Type type)
{
    AstPtr ast = std::make_unique<AST>(type);

    if (!primitive.is_string())
        throw std::logic_error("The value must be a string : " + key);
    ast->value = std::make_any<std::string>(primitive.get<std::string>());
    return std::move(ast);
}