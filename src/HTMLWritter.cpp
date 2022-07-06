#include <any>
#include "HTMLWritter.hpp"
#include <vector>
#include <fstream>
#include <sstream>

void copyFileContent(buffer &buffer, const std::string &filename)
{
    std::ifstream css(filename);
    std::ostringstream ss;
    ss << css.rdbuf();
    buffer += ss.str();
}

void HTMLWritter::write(buffer &buffer, AstPtr ast)
{
    if (ast->type != AST::Type::Root)
        return;
    buffer += "<style>";
    copyFileContent(buffer, "/home/lansknet/Documents/PLDCreator/style.css");
    buffer += "</style>";
    copyFileContent(buffer, "/home/lansknet/Documents/PLDCreator/prefix");
    buffer += "<h1>User Stories</h1>";
    for (auto &us : ast->next)
    {
        writeUserStories(buffer, std::move(us));
    }
    copyFileContent(buffer, "/home/lansknet/Documents/PLDCreator/sufix");
}

void HTMLWritter::writeUserStories(buffer &buffer, AstPtr ast)
{
    if (ast->type != AST::Type::UserStories)
        return;
    for (auto &usElem : ast->next)
    {
        if (usElem->type == AST::Type::Name)
        {
            buffer += "<h2>";
            writeString(buffer, std::move(usElem)); 
            buffer +=  "</h2>";
            continue;
        }
        else if (usElem->type == AST::Type::EstimatedTime)
        {
            buffer += "<p>Temps estimé: ";
            writeString(buffer, std::move(usElem));
            buffer += " J/H</p>";
            continue;
        }
        else if (usElem->type == AST::Type::Description)
        {
            buffer += "<h3>Description</h3><p>";
            writeString(buffer, std::move(usElem));
            buffer += "</p>";
            continue;
        }
        else if (usElem->type == AST::Type::AsA)
        {
            buffer += "<p>En tant que ";
            writeString(buffer, std::move(usElem));
            buffer += "</p>";
            continue;
        }
        else if (usElem->type == AST::Type::IWant)
        {
            buffer += "<p>Je veux ";
            writeString(buffer, std::move(usElem));
            buffer += "</p>";
            continue;
        }
        if (usElem->type == AST::Type::DoD)
            writeDoD(buffer, std::move(usElem));
        else writeString(buffer, std::move(usElem));
    }
}


void HTMLWritter::writeString(buffer &buffer, AstPtr ast)
{
    const std::string &str = std::any_cast<const std::string&>(ast->value);
    buffer += str;
}

void HTMLWritter::writeDoD(buffer &buffer, AstPtr ast)
{
    if (ast->type != AST::Type::DoD)
        return;
    buffer += "<h3>Definition of Done</h3>";
    for (auto &dodElem : ast->next)
    {
        if (dodElem->type == AST::Type::Description)
        {
            buffer += "<p>";
            writeString(buffer, std::move(dodElem));
            buffer += "</p>";
        }
        else if (dodElem->type == AST::Type::Bullets)
        {
            writeBullets(buffer, std::move(dodElem));
        }
    }
}

void HTMLWritter::writeBullets(buffer &buffer, AstPtr ast)
{
    if (ast->type != AST::Type::Bullets)
        return;
    buffer += "<ul>";
    const std::vector<std::string> &bullets = std::any_cast<const std::vector<std::string>>(ast->value);
    for (auto &bullet : bullets)
    {
        buffer += "<li>";
        buffer += bullet;
        buffer += "</li>";
    }
    buffer += "</ul>";
}
