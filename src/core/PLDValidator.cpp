#include <any>
#include <exception>
#include <memory>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <unordered_map>
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
        if (key == "UserStories")
            ast->next.push_back(parseUserStories(value->as<Config::Array>()));
        else if (key == "AssignmentTable")
            ast->next.push_back(parseAssignmentTable(value->as<Config::Array>()));
        else if (key == "DeliverablesMap")
            ast->next.push_back(parseDeliverablesMap(value->as<Config::Array>()));
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

AstPtr PLDValidator::parseAssignmentTable(const Config::Array &array)
{
    AstPtr ast = std::make_unique<AST>(AST::Type::AssignmentTable);
    using Assignements = std::unordered_map<std::string, std::vector<std::string>>;
    Assignements assignements;
    for (auto assignmentTable : array)
    {
        for (const auto &[key, value] : assignmentTable->as<Config::Object>())
        {
            auto elements = value->as<Config::Array>();
            std::vector<std::string> vec;
            for (auto element : elements)
            {
                assignements[key].push_back(element->as<Config::String>());
            }
        }
    }
    ast->value = std::make_any<Assignements>(assignements);
    return ast;
}

AstPtr PLDValidator::parseDeliverablesMap(const Config::Array &array)
{
    AstPtr ast = std::make_unique<AST>(AST::Type::DeliverablesMap);
    using DeliverablesMap = std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::string>>>;
    DeliverablesMap deliverables;
    for (const auto &elem : array)
    {
        for (const auto &[delivrableName, value] : elem->as<Config::Object>())
        {
            for (const auto& delivrableElement : value->as<Config::Object>())
            {
                std::vector<std::string> s;
                for (const auto &delivrableValue : delivrableElement.second->as<Config::Array>())
                {
                    s.push_back(delivrableValue->as<Config::String>());
                }
                deliverables[delivrableName][delivrableElement.first] = s;
            }

        }
    }
    ast->value = std::make_any<DeliverablesMap>(deliverables);
    return ast;
}