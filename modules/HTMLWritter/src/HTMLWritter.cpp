#include <any>
#include "Config.hpp"
#include "HTMLWritter.hpp"
#include <stdexcept>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include "IModule.hpp"
#include "dylib.hpp"

//TODO: this function should be moved to another file
void copyFileContent(Buffer &buffer, const std::string &filename)
{
    std::ifstream css(filename);
    std::ostringstream ss;
    ss << css.rdbuf();
    buffer += ss.str();
}

void HTMLWritter::write(AstPtr ast)
{
    if (ast->type != AST::Type::Root)
        return;
    Buffer buffer;
    buffer.reserve(0x186A0);
    const Config::Array &section = _config["Tags"]["UserStories"]["Name"].as<Config::Array>();
    buffer += "<style>";
    copyFileContent(buffer, _config["CssPath"].as<Config::String>());
    buffer += "</style>";
    copyFileContent(buffer, _config["HtmlPrefix"].as<Config::String>());
    buffer += section[0]->as<Config::String>() + "User Stories" + 
        section[1]->as<Config::String>();
    for (auto &us : ast->next)
    {
        writeUserStories(buffer, std::move(us));
    }
    copyFileContent(buffer, _config["HtmlSufix"].as<Config::String>());
    std::ofstream out(_config["Filename"].as<Config::String>());
    out << buffer;
}

void HTMLWritter::writeUserStories(Buffer &buffer, AstPtr ast)
{
    if (ast->type != AST::Type::UserStories)
        return;
    const Config::Object &userStoriesTags = _config["Tags"]["UserStories"].as<Config::Object>();
    for (auto &usElem : ast->next)
    {
        if (usElem->type == AST::Type::Name)
        {
            buffer += userStoriesTags["Name"][0].as<Config::String>();
            writeString(buffer, std::move(usElem)); 
            buffer +=  userStoriesTags["Name"][1].as<Config::String>();
            continue;
        }
        else if (usElem->type == AST::Type::EstimatedTime)
        {
            buffer += userStoriesTags["EstimatedTime"][0].as<Config::String>();
            buffer += "Temps estimé: ";
            writeString(buffer, std::move(usElem));
            buffer += " J/H" + userStoriesTags["EstimatedTime"][0].as<Config::String>();
            continue;
        }
        else if (usElem->type == AST::Type::Description)
        {
            buffer += userStoriesTags["Description"][0].as<Config::String>();
            buffer += "Description" + userStoriesTags["Description"][1].as<Config::String>();
            buffer += userStoriesTags["DescriptionText"][0].as<Config::String>();
            writeString(buffer, std::move(usElem));
            buffer += userStoriesTags["DescriptionText"][1].as<Config::String>();
            continue;
        }
        else if (usElem->type == AST::Type::AsA)
        {
            buffer += userStoriesTags["AsA"][0].as<Config::String>();
            buffer += "En tant que ";
            writeString(buffer, std::move(usElem));
            buffer += userStoriesTags["AsA"][1].as<Config::String>();
            continue;
        }
        else if (usElem->type == AST::Type::IWant)
        {
            buffer += userStoriesTags["IWant"][0].as<Config::String>();
            buffer += "Je veux ";
            writeString(buffer, std::move(usElem));
            buffer += userStoriesTags["IWant"][1].as<Config::String>();
            continue;
        }
        else if (usElem->type == AST::Type::DoDArray)
            writeDoD(buffer, std::move(usElem));
        else throw std::logic_error("Invalid type in userStories: " + std::to_string(usElem->type));
    }
}


void HTMLWritter::writeString(Buffer &buffer, AstPtr ast)
{
    const std::string &str = std::any_cast<const std::string&>(ast->value);
    buffer += str;
}

void HTMLWritter::writeDoD(Buffer &buffer, AstPtr ast)
{
    if (ast->type != AST::Type::DoDArray)
        return;
    const Config::Object &dodTag = _config["Tags"]["UserStories"]["DoD"].as<Config::Object>();
    buffer += dodTag["Section"][0].as<Config::String>();
    buffer += "Definition of Done";
    buffer += dodTag["Section"][1].as<Config::String>();
    for (auto &dod : ast->next)
    {
        if (dod->type != AST::Type::DoD)
            continue;
        for (auto &dodElement : dod->next)
        {
            if(dodElement->type == AST::Type::Description)
            {
                buffer += dodTag["Description"][0].as<Config::String>();
                writeString(buffer, std::move(dodElement));
                buffer += dodTag["Description"][1].as<Config::String>();
            }
            else if (dodElement->type == AST::Type::Bullets)
            {
                writeBullets(buffer, std::move(dodElement));
            }
        }
    }
}

void HTMLWritter::writeBullets(Buffer &buffer, AstPtr ast)
{
    if (ast->type != AST::Type::Bullets)
        return;
    const Config::Object &bulletsTags = _config["Tags"]["UserStories"]["DoD"]["Bullets"].as<Config::Object>();
    const std::vector<std::string> &bullets = std::any_cast<const std::vector<std::string>>(ast->value);

    buffer += bulletsTags["list"][0].as<Config::String>();
    for (auto &bullet : bullets)
    {
        buffer += bulletsTags["listElement"][0].as<Config::String>();
        buffer += bullet;
        buffer += bulletsTags["listElement"][1].as<Config::String>();
    }
    buffer += bulletsTags["list"][1].as<Config::String>();
}

void HTMLWritter::create(const Config::Object &data)
{
    _config = data;
}

const std::string_view HTMLWritter::getName() const noexcept 
{
    return "HTML";
}

extern "C" {
    LIB_EXPORT IModule *loadModule()
    {
        return new HTMLWritter();
    }

    LIB_EXPORT void unloadModule(IModule *module)
    {
        delete module;
    }
}