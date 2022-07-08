#include <any>
#include <exception>
#include <memory>
#include <stdexcept>
#include <iostream>
#include <vector>
#include "Config.hpp"
#include "PLDValidator.hpp"
#include "AST.hpp"
#include "nlohmann/json_fwd.hpp"

PLDValidator::PLDValidator(const Config::Object &data)
{
    _ast = parseRoot(data);
}

AstPtr PLDValidator::getAst()
{
    return std::move(_ast);
}

AstPtr PLDValidator::parseRoot(const Config::Object &object)
{
    AstPtr ast = std::make_unique<AST>(AST::Type::Root);
    for (auto &[key, value] : object)
    {
        if (key == "userStories")
            ast->next.push_back(parseUserStories(value->as<Config::Array>()));
        else throw std::runtime_error("Invalid key in root: " + key);
    }
    return ast;
}

AstPtr PLDValidator::parseUserStories(const Config::Array &array)
{
    //TODO: this function should be refactored, or AST struct should be changed
    AstPtr ast = std::make_unique<AST>(AST::Type::UserStories);
    for (auto &us : array)
    {
        for (auto &[key, value] : us->as<Config::Object>())
        {
            if (key == "Name")
            {
                auto any = std::make_any<Config::String>(value->as<Config::String>());
                ast->next.push_back(std::make_unique<AST>(AST::Type::Name, std::move(any)));
            }
            else if (key == "Description")
            {
                auto any = std::make_any<Config::String>(value->as<Config::String>());
                ast->next.push_back(std::make_unique<AST>(AST::Type::Description, std::move(any)));
            }
            else if (key == "EstimatedTime")
            {
                auto any = std::make_any<Config::String>(value->as<Config::String>());
                ast->next.push_back(std::make_unique<AST>(AST::Type::EstimatedTime, std::move(any)));
            }
            else if (key == "AsA")
            {
                auto any = std::make_any<Config::String>(value->as<Config::String>());
                ast->next.push_back(std::make_unique<AST>(AST::Type::AsA, std::move(any)));
            }
            else if (key == "IWant")
            {
                auto any = std::make_any<Config::String>(value->as<Config::String>());
                ast->next.push_back(std::make_unique<AST>(AST::Type::IWant, std::move(any)));
            }
            else if (key == "DoD")
            {
                ast->next.push_back(parseDoD(value->as<Config::Array>()));
            }
            else throw std::logic_error("Invalid key in userStories: " + value->as<Config::String>());
        }
    }
    return ast;
}

AstPtr PLDValidator::parseDoD(const Config::Array &array)
{
    AstPtr ast = std::make_unique<AST>(AST::Type::DoDArray);
    for (auto &dod : array)
    {
        AstPtr astDod = std::make_unique<AST>(AST::Type::DoD);
        for (auto &[key, value] : dod->as<Config::Object>())
        {
            if (key == "Description")
            {
                astDod->next.push_back(std::make_unique<AST>(AST::Type::Description, std::make_any<Config::String>(value->as<Config::String>())));
            }
            else if (key == "Bullets")
            {
                astDod->next.push_back(parseBullets(value->as<Config::Array>()));
            }
            else throw std::logic_error("Invalid key in DoD: " + value->as<Config::String>());
        }
        ast->next.push_back(std::move(astDod));
    }
    return ast;
}

AstPtr PLDValidator::parseBullets(const Config::Array &array)
{
    AstPtr ast = std::make_unique<AST>(AST::Type::Bullets);
    std::vector<Config::String> bullets;
    for (auto &value : array)
    {
        bullets.push_back(value->as<Config::String>());
    }
    ast->value = std::make_any<std::vector<Config::String>>(std::move(bullets));
    return ast;
}
