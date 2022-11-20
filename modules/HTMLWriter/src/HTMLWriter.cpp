#include <any>
#include <chrono>
#include <format>
#include <stdexcept>
#include <tuple>
#include <vector>
#include <iostream>
#include "Config.hpp"
#include "HTMLWriter.hpp"
#include "Utils.hpp"

void HTMLWriter::write(AstPtr ast)
{
	if (ast->type != AST::Type::Root)
		return;
	Buffer buffer;
	buffer.reserve(0x186A0);
	Buffer styleBuffer;
	styleBuffer += "<style>";
	Utils::copyFileContent(styleBuffer, _config["CssPath"].as<Config::String>());
	styleBuffer += "</style>";
	Utils::copyFileContent(buffer, _config["HtmlPrefix"].as<Config::String>());
	Utils::findAndReplace(buffer, "$STYLE$", styleBuffer);
	Utils::findAndReplace(buffer, "$PROJECT_NAME$", _config["ProjectName"].as<Config::String>());
	writeFrontPage(buffer);
	for (auto& us: ast->next)
	{
		parseRoot(buffer, std::move(us));
	}
	Utils::copyFileContent(buffer, _config["HtmlSuffix"].as<Config::String>());
	std::ofstream out(_config["Filename"].as<Config::String>());
	out << buffer;
}

void HTMLWriter::writeUserStories(Buffer& buffer, AstPtr ast)
{
	if (ast->type != AST::Type::UserStories)
		return;
	const Config::Array& h1 = _config["Tags"]["h1"].as<Config::Array>();
	const Config::Object& userStoriesTags = _config["Tags"]["UserStories"].as<Config::Object>();
	const Config::Array& section = _config["Tags"]["Section"].as<Config::Array>();
	const Config::Array& container = userStoriesTags["Container"].as<Config::Array>();
	buffer += h1[0]->as<Config::String>();
	buffer += "User Stories";
	buffer += h1[1]->as<Config::String>();
	auto str = section[0]->as<Config::String>();
	Utils::findAndReplace(str, "$LINK$", "UserStories");
	buffer += str;

	for (auto& usElem: ast->next)
	{
		if (!usElem)
			continue;
		if (usElem->type == AST::Type::Name)
		{
			buffer += container[0]->as<Config::String>();
			auto strName = userStoriesTags["Name"][0].as<Config::String>();
			auto nameCopy = std::any_cast<Config::String>(usElem->value);
			Utils::trim(nameCopy, ' ');
			Utils::findAndReplace(strName, "$LINK$", nameCopy);
			buffer += strName;
			writeString(buffer, std::move(usElem));
			buffer += userStoriesTags["Name"][1].as<Config::String>();
			continue;
		}
		else if (usElem->type == AST::Type::Assignee)
		{
			buffer += userStoriesTags["Assignee"][0].as<Config::String>();
			buffer += "<span class=\"underline\">Assigné(s)</span>: ";
			writeString(buffer, std::move(usElem));
			buffer += userStoriesTags["Assignee"][1].as<Config::String>();
		}
		else if (usElem->type == AST::Type::Status)
		{
			buffer += userStoriesTags["Status"][0].as<Config::String>();
			buffer += "<span class=\"underline\">Statut</span>:";
			writeString(buffer, std::move(usElem));
			buffer += userStoriesTags["Status"][1].as<Config::String>();
		}
		else if (usElem->type == AST::Type::EstimatedTime)
		{
			buffer += userStoriesTags["EstimatedTime"][0].as<Config::String>();
			buffer += "<span class=\"underline\">Temps estimé</span>: ";
			writeInt32(buffer, std::move(usElem));
			buffer += " J/H" + userStoriesTags["EstimatedTime"][1].as<Config::String>();
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
			buffer += "<span class=\"underline\">En tant que</span> ";
			writeString(buffer, std::move(usElem));
			buffer += userStoriesTags["AsA"][1].as<Config::String>();
			continue;
		}
		else if (usElem->type == AST::Type::IWant)
		{
			buffer += userStoriesTags["IWant"][0].as<Config::String>();
			buffer += "<span class=\"underline\">Je veux</span> ";
			writeString(buffer, std::move(usElem));
			buffer += userStoriesTags["IWant"][1].as<Config::String>();
			continue;
		}
		else if (usElem->type == AST::Type::DoDArray)
		{
			writeDoD(buffer, std::move(usElem));
			buffer += container[1]->as<Config::String>();
		}
		else throw std::logic_error("Invalid type in userStories: " + std::to_string(usElem->type));
	}
	buffer += section[1]->as<Config::String>();
}


void HTMLWriter::writeString(Buffer& buffer, AstPtr ast)
{
	const std::string& str = std::any_cast<const std::string&>(ast->value);
	buffer += str;
}


void HTMLWriter::writeInt32(Buffer& buffer, AstPtr ast)
{
	const Config::Int32& int32 = std::any_cast<const Config::Int32&>(ast->value);
	buffer += std::to_string(int32);
}

void HTMLWriter::writeDoD(Buffer& buffer, AstPtr ast)
{
	if (ast->type != AST::Type::DoDArray)
		return;
	const Config::Object& dodTag = _config["Tags"]["UserStories"]["DoD"].as<Config::Object>();
	buffer += dodTag["Section"][0].as<Config::String>();
	buffer += "Definition of Done";
	buffer += dodTag["Section"][1].as<Config::String>();
	buffer += dodTag["Bullets"]["list"][0].as<Config::String>();
	for (auto& dod: ast->next)
	{
		if (dod->type != AST::Type::DoD)
			continue;
		for (auto& dodElement: dod->next)
		{
			if (dodElement->type == AST::Type::Description)
			{
				buffer += dodTag["Bullets"]["listElement"][0].as<Config::String>();
				writeString(buffer, std::move(dodElement));
				buffer += dodTag["Bullets"]["listElement"][1].as<Config::String>();
			}
			else if (dodElement->type == AST::Type::Bullets)
			{
				writeBullets(buffer, std::move(dodElement));
			}
		}
	}
	buffer += dodTag["Bullets"]["list"][1].as<Config::String>();
}

void HTMLWriter::writeBullets(Buffer& buffer, AstPtr ast)
{
	if (ast->type != AST::Type::Bullets)
		return;
	const Config::Object& bulletsTags = _config["Tags"]["UserStories"]["DoD"]["Bullets"].as<Config::Object>();
	const std::vector<std::string>& bullets = std::any_cast<const std::vector<std::string>>(ast->value);

	buffer += bulletsTags["list"][0].as<Config::String>();
	for (auto& bullet: bullets)
	{
		buffer += bulletsTags["listElement"][0].as<Config::String>();
		buffer += bullet;
		buffer += bulletsTags["listElement"][1].as<Config::String>();
	}
	buffer += bulletsTags["list"][1].as<Config::String>();
}

void HTMLWriter::writeAssignmentTable(Buffer& buffer, AstPtr ast)
{
	using Assignements = std::unordered_map<std::string, std::vector<std::string>>;
	const Assignements& assignements = std::any_cast<const Assignements&>(ast->value);
	const Config::Object& assignmentTable = _config["Tags"]["AssignmentTable"].as<Config::Object>();
	const Config::Array& h1 = _config["Tags"]["h1"].as<Config::Array>();
	const Config::Array& section = _config["Tags"]["Section"].as<Config::Array>();

	buffer += h1[0]->as<Config::String>();
	buffer += "Tableau d'assignation";
	buffer += h1[1]->as<Config::String>();
	auto str = section[0]->as<Config::String>();
	Utils::findAndReplace(str, "$LINK$", "AssignmentTable");
	buffer += str;
	buffer += assignmentTable["Table"][0].as<Config::String>();
	for (const auto& [name, elems]: assignements)
	{
		buffer += assignmentTable["Tr"][0].as<Config::String>();
		buffer += assignmentTable["Td"][0].as<Config::String>();
		buffer += name;
		buffer += assignmentTable["Td"][1].as<Config::String>();
		buffer += assignmentTable["Td"][0].as<Config::String>();
		for (const auto& assignement: elems)
		{
			auto assigmentStr = assignement;
			Utils::trim(assigmentStr, ' ');
			buffer += "<a href=\"#" + assigmentStr + "\">" + assignement + "</a><br>";
		}
		buffer += assignmentTable["Td"][1].as<Config::String>();
		buffer += assignmentTable["Tr"][1].as<Config::String>();
	}
	buffer += assignmentTable["Table"][1].as<Config::String>();
	buffer += section[1]->as<Config::String>();
}

void HTMLWriter::writeDeliverablesMap(Buffer& buffer, AstPtr ast)
{
	using DeliverablesMap = std::unordered_map<std::string, std::vector<std::string>>;
	const DeliverablesMap& deliverables = std::any_cast<DeliverablesMap>(ast->value);
	const Config::Object& deliverablesTable = _config["Tags"]["DeliverablesMap"].as<Config::Object>();
	const Config::Array& h1 = _config["Tags"]["h1"].as<Config::Array>();
	const Config::Array& section = _config["Tags"]["Section"].as<Config::Array>();

	buffer += h1[0]->as<Config::String>();
	buffer += "Carte des livrables";
	buffer += h1[1]->as<Config::String>();
	auto str = section[0]->as<Config::String>();
	Utils::findAndReplace(str, "$LINK$", "DeliverablesMap");
	buffer += str;
	for (const auto& [key, value]: deliverables)
	{
		buffer += deliverablesTable["TitleContainer"][0].as<Config::String>();
		buffer += deliverablesTable["Title"][0].as<Config::String>();
		buffer += key;
		buffer += deliverablesTable["Title"][1].as<Config::String>();
		buffer += deliverablesTable["TitleContainer"][1].as<Config::String>();
		buffer += deliverablesTable["Subtitle1Container"][0].as<Config::String>();
		for (const auto& elem: value)
		{
			buffer += deliverablesTable["Subtitle1"][0].as<Config::String>();
			buffer += elem;
			buffer += deliverablesTable["Subtitle1"][1].as<Config::String>();
		}
		buffer += deliverablesTable["Subtitle1Container"][1].as<Config::String>();
	}
	buffer += section[1]->as<Config::String>();
}

void HTMLWriter::create(const Config::Object& data)
{
	_config = data;
}

const std::string_view HTMLWriter::getName() const noexcept
{
	return "HTML";
}

void HTMLWriter::parseRoot(Buffer& buffer, AstPtr ast)
{
	if (ast->type == AST::Type::UserStories)
		writeUserStories(buffer, std::move(ast));
	else if (ast->type == AST::Type::AssignmentTable)
		writeAssignmentTable(buffer, std::move(ast));
	else if (ast->type == AST::Type::DeliverablesMap)
		writeDeliverablesMap(buffer, std::move(ast));
	else if (ast->type == AST::Type::AdvancementReport)
		writeAdvancementReport(buffer, std::move(ast));
	else if (ast->type == AST::Type::Root)
	{
		for (auto& child: ast->next)
			parseRoot(buffer, std::move(child));
	}
	else if (ast->type == AST::Type::TableOfContent)
	{
		writeTableOfContent(buffer, std::move(ast));
		writeFunctionalSchema(buffer);
	}
	else throw std::runtime_error("Unknown AST type");
}

void HTMLWriter::writeTableOfContent(Buffer& buffer, AstPtr ast)
{
	if (ast->type != AST::Type::TableOfContent)
		return;
	const Config::Array& h1 = _config["Tags"]["h1"].as<Config::Array>();
	const Config::Object& tableOfContent = _config["Tags"]["TableOfContent"].as<Config::Object>();
	buffer += h1[0]->as<Config::String>();
	buffer += "Table des matières";
	buffer += h1[1]->as<Config::String>();
	auto schema = tableOfContent["Content"][0].as<Config::String>();
	Utils::findAndReplace(schema, "$LINK$", "FunctionalSchema");
	buffer += schema;
	buffer += "Schéma fonctionnel";
	buffer += tableOfContent["Content"][1].as<Config::String>();
	for (auto& child: ast->next)
	{
		const auto& value = std::any_cast<const std::string&>(child->value);
		auto str = tableOfContent["Content"][0].as<Config::String>();
		Utils::findAndReplace(str, "$LINK$", value);
		buffer += str;
		buffer += value;
		buffer += tableOfContent["Content"][1].as<Config::String>();
		if (value == "UserStories")
		{
			for (const auto& us: child->next)
			{
				auto indentedContent = tableOfContent["IndentedContent"][0].as<Config::String>();
				auto usTitle = std::any_cast<const std::string&>(us->value);

				Utils::trim(usTitle, ' ');
				Utils::findAndReplace(indentedContent, "$LINK$", usTitle);
				buffer += indentedContent;
				buffer += std::any_cast<const std::string&>(us->value);
				buffer += tableOfContent["IndentedContent"][1].as<Config::String>();

			}
		}
	}
}

void HTMLWriter::writeAdvancementReport(Buffer& buffer, AstPtr ast)
{
	using Assignements = std::unordered_map<std::string, std::vector<std::string>>;
	const Assignements& assignements = std::any_cast<const Assignements&>(ast->value);
	const Config::Object& assignmentTable = _config["Tags"]["AssignmentTable"].as<Config::Object>();
	const Config::Array& h1 = _config["Tags"]["h1"].as<Config::Array>();
	const Config::Array& section = _config["Tags"]["Section"].as<Config::Array>();

	buffer += h1[0]->as<Config::String>();
	buffer += "Rapport d'avancement";
	buffer += h1[1]->as<Config::String>();
	auto str = section[0]->as<Config::String>();
	Utils::findAndReplace(str, "$LINK$", "AdvancementReport");
	buffer += str;
	buffer += assignmentTable["Table"][0].as<Config::String>();
	for (const auto& [name, elems]: assignements)
	{
		buffer += assignmentTable["Tr"][0].as<Config::String>();
		buffer += assignmentTable["Td"][0].as<Config::String>();
		buffer += name;
		buffer += assignmentTable["Td"][1].as<Config::String>();
		buffer += assignmentTable["Td"][0].as<Config::String>();
		for (const auto& assignement: elems)
		{
			buffer += assignement + "<br>";
		}
		buffer += assignmentTable["Td"][1].as<Config::String>();
		buffer += assignmentTable["Tr"][1].as<Config::String>();
	}
	buffer += assignmentTable["Table"][1].as<Config::String>();
	buffer += section[1]->as<Config::String>();
}

void HTMLWriter::writeFrontPage(Buffer& buffer)
{
	buffer += "<h2 class=\"sprintName\">" + _config["FrontPage"]["Sprint"].as<Config::String>() + "</h2>";
	buffer += "<div class=\"FrontPage\">";
	buffer += R"(<img alt="Logo" id="logoCenter" src="data:image/png;base64,)";
	Utils::copyImageContent(buffer, _config["FrontPage"]["Image"].as<Config::String>());
	buffer += "\"/>";
	buffer += "<h2>" + _config["FrontPage"]["Title"].as<Config::String>() + "</h2>";
	buffer += "<br>";
	buffer += "<h3>" + _config["FrontPage"]["Description"].as<Config::String>() + "</h3>";
	buffer += "<p>Dernière mise à jour le $DATE$ à $TIME$</p>";
	auto time = std::chrono::zoned_time{ "Europe/Paris", std::chrono::system_clock::now() };
	Utils::findAndReplace(buffer, "$DATE$", std::format("{:%d-%m-%Y}", time));
	Utils::findAndReplace(buffer, "$TIME$", std::format("{:%Hh%M}", time));
	buffer += "</div>";
}

void HTMLWriter::writeFunctionalSchema(Buffer& buffer)
{
	const Config::Array& section = _config["Tags"]["Section"].as<Config::Array>();
	const Config::Array& h1 = _config["Tags"]["h1"].as<Config::Array>();
	buffer += h1[0]->as<Config::String>();
	buffer += "Schéma fonctionnel";
	buffer += h1[1]->as<Config::String>();
	auto str = section[0]->as<Config::String>();
	Utils::findAndReplace(str, "$LINK$", "FunctionalSchema");
	buffer += str;
	buffer += R"(<img alt="Schéma fonctionnel" class="Schema" src="data:image/png;base64,)";
	Utils::copyImageContent(buffer, (_config["FrontPage"]["Schema"].as<Config::String>()));
	buffer += "\" />";
}

extern "C" {
LIB_EXPORT IModule* loadModule()
{
	return new HTMLWriter();
}

LIB_EXPORT void unloadModule(IModule* module)
{
	delete module;
}
}