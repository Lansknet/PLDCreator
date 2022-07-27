#include "MarkdownWritter.hpp"
#include "Utils.hpp"
#include <unordered_map>

void MarkdownWritter::write(AstPtr ast)
{
    if (ast->type != AST::Type::Root)
        return;
    Buffer buffer;
    buffer.reserve(0x186A0);
    for (auto &us : ast->next)
    {
        if (us->type == AST::Type::UserStories)
            writeUserStories(buffer, std::move(us));
        else if (us->type == AST::Type::AssignmentTable)
            writeAssignmentTable(buffer, std::move(us));
        else if (us->type == AST::Type::DeliverablesMap)
            writeDeliverablesMap(buffer, std::move(us));
    }
    std::ofstream out(_config["Filename"].as<Config::String>());
    out << buffer;
}

void MarkdownWritter::writeUserStories(Buffer &buffer, AstPtr ast)
{
    if (ast->type != AST::Type::UserStories)
        return;
    buffer += "## User stories\n\n";
    for (auto &usElem : ast->next)
    {
        if (usElem->type == AST::Type::Name)
        {
            buffer += "### ";
            writeString(buffer, std::move(usElem));
            buffer += "\n\n";
            continue;
        }
        else if (usElem->type == AST::Type::EstimatedTime)
        {
            buffer += "**Temps estimé:** ";
            writeString(buffer, std::move(usElem));
            buffer += " J/H\n\n";
            continue;
        }
        else if (usElem->type == AST::Type::Description)
        {
            writeString(buffer, std::move(usElem));
            buffer += "\n\n";
            continue;
        }
        else if (usElem->type == AST::Type::AsA)
        {
            buffer += "**En tant que** ";
            writeString(buffer, std::move(usElem));
            buffer += "\n\n";
            continue;
        }
        else if (usElem->type == AST::Type::IWant)
        {
            buffer += "**Je veux** ";
            writeString(buffer, std::move(usElem));
            buffer += "\n\n";
            continue;
        }
        else if (usElem->type == AST::Type::DoDArray)
            writeDoD(buffer, std::move(usElem));
        else throw std::logic_error("Invalid type in userStories: " + std::to_string(usElem->type));
    }
}

void MarkdownWritter::writeString(Buffer &buffer, AstPtr ast)
{
    const std::string &str = std::any_cast<const std::string&>(ast->value);
    buffer += str;
}

void MarkdownWritter::writeDoD(Buffer &buffer, AstPtr ast)
{
    if (ast->type != AST::Type::DoDArray)
        return;
    buffer += "### Definition of Done\n\n";
    for (auto &dod : ast->next)
    {
        if (dod->type != AST::Type::DoD)
            continue;
        for (auto &dodElement : dod->next)
        {
            if(dodElement->type == AST::Type::Description)
            {
                writeString(buffer, std::move(dodElement));
                buffer += "\n\n";
            }
            else if (dodElement->type == AST::Type::Bullets)
            {
                writeBullets(buffer, std::move(dodElement));
                buffer += "\n\n";
            }
        }
    }
}

void MarkdownWritter::writeBullets(Buffer &buffer, AstPtr ast)
{
    const std::vector<std::string> &bullets = std::any_cast<const std::vector<std::string>>(ast->value);

    for (auto &bullet : bullets)
    {
        buffer += "* ";
        buffer += bullet;
        buffer += "\n\n";
    }
}

void MarkdownWritter::writeAssignmentTable(Buffer &buffer, AstPtr ast)
{
    using Assignements = std::unordered_map<std::string, std::vector<std::string>>;
    const Assignements &assignements = std::any_cast<const Assignements &>(ast->value);

    buffer += "## Tableau d'assignation\n\n";
    buffer += "|  |  |\n";
    buffer += "|--|--|\n";
    for (const auto &[name, elems] : assignements)
    {
        buffer += "| ";
        buffer += name;
        buffer += " | ";
        for (const auto &assignement : elems)
        {
            buffer += assignement + " ";
        }
        buffer += " |\n";
    }
    buffer += "\n\n";
}

void MarkdownWritter::writeDeliverablesMap(Buffer &buffer, AstPtr ast)
{
    using DeliverablesMap = std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::string>>>;
    const DeliverablesMap &deliverables = std::any_cast<DeliverablesMap>(ast->value);
    buffer += "## Carte des livrables\n\n";
    
    for (const auto &[key, value] : deliverables)
    {
        buffer += "### " + key + "\n\n";
        for (const auto &[deliverableKey, deliverableValue] : value)
        {
            buffer += "| " + deliverableKey + " |\n";
            buffer += "|--|\n";
            for (const auto &content : deliverableValue)
            {
                
                buffer += "| " + content + " |\n";
            }
            buffer += "\n\n";
        }
    } 
}

void MarkdownWritter::create(const Config::Object &data)
{
    _config = data;
}

const std::string_view MarkdownWritter::getName() const noexcept 
{
    return "Markdown";
}

extern "C" {
    LIB_EXPORT IModule *loadModule()
    {
        return new MarkdownWritter();
    }

    LIB_EXPORT void unloadModule(IModule *module)
    {
        delete module;
    }
}