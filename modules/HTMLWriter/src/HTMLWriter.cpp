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

void HTMLWriter::Write(AstPtr ast)
{
	if (ast->type != AST::Type::Root)
		return;
	Buffer buffer;
	buffer.reserve(0x186A0);
	Buffer styleBuffer;
	styleBuffer += "<style>";
	Utils::CopyFileContent(styleBuffer, _config["CssPath"].AsString());
	styleBuffer += "</style>";
	Utils::CopyFileContent(buffer, _config["HtmlPrefix"].AsString());
	Utils::FindAndReplace(buffer, "$STYLE$", styleBuffer);
	Utils::FindAndReplace(buffer, "$PROJECT_NAME$", _config["ProjectName"].AsString());
	WriteFrontPage(buffer);
	for (auto& us: ast->next)
	{
		ParseRoot(buffer, std::move(us));
	}
	Utils::CopyFileContent(buffer, _config["HtmlSuffix"].AsString());
	std::ofstream out(_config["Filename"].AsString());
	out << buffer;
}

void HTMLWriter::WriteUserStories(Buffer& buffer, AstPtr ast)
{
	if (ast->type != AST::Type::UserStories)
		return;
	const Config::Array& h1 = _config["Tags"]["h1"].AsArray();
	const Config::Object& userStoriesTags = _config["Tags"]["UserStories"].AsObject();
	const Config::Array& section = _config["Tags"]["Section"].AsArray();
	const Config::Array& container = userStoriesTags["Container"].AsArray();
	const Config::Array& underline = _config["Tags"]["Underline"].AsArray();
	const Config::String& userStoriesTranslation = _config["Translations"]["UserStories"].AsString();
	const Config::Object& translation = _config["Translations"].AsObject();
	buffer += h1[0]->AsString();
	buffer += userStoriesTranslation;
	buffer += h1[1]->AsString();
	auto str = section[0]->AsString();
	Utils::FindAndReplace(str, "$LINK$", userStoriesTranslation);
	buffer += str;

	for (auto& usElem: ast->next)
	{
		if (!usElem)
			continue;
		if (usElem->type == AST::Type::Name)
		{
			buffer += container[0]->AsString();
			auto strName = userStoriesTags["Name"][0].AsString();
			auto nameCopy = std::any_cast<Config::String>(usElem->value);
			Utils::Trim(nameCopy, ' ');
			Utils::FindAndReplace(strName, "$LINK$", nameCopy);
			buffer += strName;
			WriteString(buffer, std::move(usElem));
			buffer += userStoriesTags["Name"][1].AsString();
			continue;
		}
		else if (usElem->type == AST::Type::Assignee)
		{
			buffer += userStoriesTags["Assignee"][0].AsString();
			buffer += underline[0]->AsString();
			buffer += translation["Assignee"].AsString();
			buffer += underline[1]->AsString();
			WriteString(buffer, std::move(usElem));
			buffer += userStoriesTags["Assignee"][1].AsString();
		}
		else if (usElem->type == AST::Type::Status)
		{
			buffer += userStoriesTags["Status"][0].AsString();
			buffer += underline[0]->AsString();
			buffer += translation["Status"].AsString();
			buffer += underline[1]->AsString();
			WriteString(buffer, std::move(usElem));
			buffer += userStoriesTags["Status"][1].AsString();
		}
		else if (usElem->type == AST::Type::EstimatedTime)
		{
			buffer += userStoriesTags["EstimatedTime"][0].AsString();
			buffer += underline[0]->AsString();
			buffer += translation["EstimatedTime"].AsString();
			buffer += underline[1]->AsString();
			WriteInt32(buffer, std::move(usElem));
			buffer += " J/H" + userStoriesTags["EstimatedTime"][1].AsString();
			continue;
		}
		else if (usElem->type == AST::Type::Description)
		{
			buffer += userStoriesTags["Description"][0].AsString();
			buffer += translation["Description"].AsString();
			buffer += userStoriesTags["Description"][1].AsString();
			buffer += userStoriesTags["DescriptionText"][0].AsString();
			WriteString(buffer, std::move(usElem));
			buffer += userStoriesTags["DescriptionText"][1].AsString();
			continue;
		}
		else if (usElem->type == AST::Type::AsA)
		{
			buffer += userStoriesTags["AsA"][0].AsString();
			buffer += underline[0]->AsString();
			buffer += translation["AsA"].AsString();
			buffer += underline[1]->AsString();
			WriteString(buffer, std::move(usElem));
			buffer += userStoriesTags["AsA"][1].AsString();
			continue;
		}
		else if (usElem->type == AST::Type::IWant)
		{
			buffer += userStoriesTags["IWant"][0].AsString();
			buffer += underline[0]->AsString();
			buffer += translation["IWant"].AsString();
			buffer += underline[1]->AsString();
			WriteString(buffer, std::move(usElem));
			buffer += userStoriesTags["IWant"][1].AsString();
			continue;
		}
		else if (usElem->type == AST::Type::DoDArray)
		{
			WriteDoD(buffer, std::move(usElem));
			buffer += container[1]->AsString();
		}
		else throw std::logic_error("Invalid type in userStories: " + std::to_string(usElem->type));
	}
	buffer += section[1]->AsString();
}


void HTMLWriter::WriteString(Buffer& buffer, AstPtr ast)
{
	const std::string& str = std::any_cast<const std::string&>(ast->value);
	buffer += str;
}


void HTMLWriter::WriteInt32(Buffer& buffer, AstPtr ast)
{
	const Config::Int32& int32 = std::any_cast<const Config::Int32&>(ast->value);
	buffer += std::to_string(int32);
}

void HTMLWriter::WriteDoD(Buffer& buffer, AstPtr ast)
{
	if (ast->type != AST::Type::DoDArray)
		return;
	const Config::Object& dodTag = _config["Tags"]["UserStories"]["DoD"].AsObject();
	const Config::Object& translation = _config["Translations"].AsObject();
	buffer += dodTag["Section"][0].AsString();
	buffer += translation["DoD"].AsString();
	buffer += dodTag["Section"][1].AsString();
	buffer += dodTag["Bullets"]["list"][0].AsString();
	for (auto& dod: ast->next)
	{
		if (dod->type != AST::Type::DoD)
			continue;
		for (auto& dodElement: dod->next)
		{
			if (dodElement->type == AST::Type::Description)
			{
				buffer += dodTag["Bullets"]["listElement"][0].AsString();
				WriteString(buffer, std::move(dodElement));
				buffer += dodTag["Bullets"]["listElement"][1].AsString();
			}
			else if (dodElement->type == AST::Type::Bullets)
			{
				WriteBullets(buffer, std::move(dodElement));
			}
		}
	}
	buffer += dodTag["Bullets"]["list"][1].AsString();
}

void HTMLWriter::WriteBullets(Buffer& buffer, AstPtr ast)
{
	if (ast->type != AST::Type::Bullets)
		return;
	const Config::Object& bulletsTags = _config["Tags"]["UserStories"]["DoD"]["Bullets"].AsObject();
	const std::vector<std::string>& bullets = std::any_cast<const std::vector<std::string>>(ast->value);

	buffer += bulletsTags["list"][0].AsString();
	for (auto& bullet: bullets)
	{
		buffer += bulletsTags["listElement"][0].AsString();
		buffer += bullet;
		buffer += bulletsTags["listElement"][1].AsString();
	}
	buffer += bulletsTags["list"][1].AsString();
}

void HTMLWriter::WriteAssignmentTable(Buffer& buffer, AstPtr ast)
{
	using Assignements = std::unordered_map<std::string, std::vector<std::string>>;
	const Assignements& assignements = std::any_cast<const Assignements&>(ast->value);
	const Config::Object& assignmentTable = _config["Tags"]["AssignmentTable"].AsObject();
	const Config::Array& h1 = _config["Tags"]["h1"].AsArray();
	const Config::Array& section = _config["Tags"]["Section"].AsArray();
	const Config::Object& translation = _config["Translations"].AsObject();

	buffer += h1[0]->AsString();
	buffer += translation["AssignmentTable"].AsString();
	buffer += h1[1]->AsString();
	auto str = section[0]->AsString();
	Utils::FindAndReplace(str, "$LINK$", translation["AssignmentTable"].AsString());
	buffer += str;
	buffer += assignmentTable["Table"][0].AsString();
	for (const auto& [name, elems]: assignements)
	{
		buffer += assignmentTable["Tr"][0].AsString();
		buffer += assignmentTable["Td"][0].AsString();
		buffer += name;
		buffer += assignmentTable["Td"][1].AsString();
		buffer += assignmentTable["Td"][0].AsString();
		for (const auto& assignement: elems)
		{
			auto assigmentStr = assignement;
			Utils::Trim(assigmentStr, ' ');
			buffer += "<a href=\"#" + assigmentStr + "\">" + assignement + "</a><br>";
		}
		buffer += assignmentTable["Td"][1].AsString();
		buffer += assignmentTable["Tr"][1].AsString();
	}
	buffer += assignmentTable["Table"][1].AsString();
	buffer += section[1]->AsString();
}

void HTMLWriter::WriteDeliverablesMap(Buffer& buffer, AstPtr ast)
{
	using DeliverablesMap = std::unordered_map<std::string, std::vector<std::string>>;
	const DeliverablesMap& deliverables = std::any_cast<DeliverablesMap>(ast->value);
	const Config::Object& deliverablesTable = _config["Tags"]["DeliverablesMap"].AsObject();
	const Config::Array& h1 = _config["Tags"]["h1"].AsArray();
	const Config::Array& section = _config["Tags"]["Section"].AsArray();
	const Config::Object& translation = _config["Translations"].AsObject();
	buffer += h1[0]->AsString();
	buffer += translation["DeliverablesMap"].AsString();
	buffer += h1[1]->AsString();
	auto str = section[0]->AsString();
	Utils::FindAndReplace(str, "$LINK$", translation["DeliverablesMap"].AsString());
	buffer += str;
	for (const auto& [key, value]: deliverables)
	{
		buffer += deliverablesTable["TitleContainer"][0].AsString();
		buffer += deliverablesTable["Title"][0].AsString();
		buffer += key;
		buffer += deliverablesTable["Title"][1].AsString();
		buffer += deliverablesTable["TitleContainer"][1].AsString();
		buffer += deliverablesTable["Subtitle1Container"][0].AsString();
		for (const auto& elem: value)
		{
			buffer += deliverablesTable["Subtitle1"][0].AsString();
			buffer += elem;
			buffer += deliverablesTable["Subtitle1"][1].AsString();
		}
		buffer += deliverablesTable["Subtitle1Container"][1].AsString();
	}
	buffer += section[1]->AsString();
}

void HTMLWriter::Create(const Config::Object& data)
{
	_config = data;
}

const std::string_view HTMLWriter::GetName() const noexcept
{
	return "HTML";
}

void HTMLWriter::ParseRoot(Buffer& buffer, AstPtr ast)
{
	if (ast->type == AST::Type::UserStories)
		WriteUserStories(buffer, std::move(ast));
	else if (ast->type == AST::Type::AssignmentTable)
		WriteAssignmentTable(buffer, std::move(ast));
	else if (ast->type == AST::Type::DeliverablesMap)
		WriteDeliverablesMap(buffer, std::move(ast));
	else if (ast->type == AST::Type::AdvancementReport)
		WriteAdvancementReport(buffer, std::move(ast));
	else if (ast->type == AST::Type::Root)
	{
		for (auto& child: ast->next)
			ParseRoot(buffer, std::move(child));
	}
	else if (ast->type == AST::Type::TableOfContent)
	{
		WriteTableOfContent(buffer, std::move(ast));
		WriteFunctionalSchema(buffer);
	}
	else throw std::runtime_error("Unknown AST type");
}

void HTMLWriter::WriteTableOfContent(Buffer& buffer, AstPtr ast)
{
	if (ast->type != AST::Type::TableOfContent)
		return;
	const Config::Array& h1 = _config["Tags"]["h1"].AsArray();
	const Config::Object& tableOfContent = _config["Tags"]["TableOfContent"].AsObject();
	const Config::Object& translation = _config["Translations"].AsObject();
	auto schema = tableOfContent["Content"][0].AsString();
	buffer += h1[0]->AsString();
	buffer += translation["TableOfContent"].AsString();
	buffer += h1[1]->AsString();
	Utils::FindAndReplace(schema, "$LINK$", "FunctionalSchema");
	buffer += schema;
	buffer += translation["FunctionalSchema"].AsString();
	buffer += tableOfContent["Content"][1].AsString();
	for (auto& child: ast->next)
	{
		const auto& value = std::any_cast<const std::string&>(child->value);
		auto contentDiv = tableOfContent["Content"][0].AsString();
		Utils::FindAndReplace(contentDiv, "$LINK$", value);
		buffer += contentDiv;
		buffer += translation[value].AsString();
		buffer += tableOfContent["Content"][1].AsString();
		if (value == "UserStories")
		{
			for (const auto& us: child->next)
			{
				auto indentedContent = tableOfContent["IndentedContent"][0].AsString();
				auto usTitle = std::any_cast<const std::string&>(us->value);

				Utils::Trim(usTitle, ' ');
				Utils::FindAndReplace(indentedContent, "$LINK$", usTitle);
				buffer += indentedContent;
				buffer += std::any_cast<const std::string&>(us->value);
				buffer += tableOfContent["IndentedContent"][1].AsString();

			}
		}
	}
}

void HTMLWriter::WriteAdvancementReport(Buffer& buffer, AstPtr ast)
{
	using Assignements = std::unordered_map<std::string, std::vector<std::string>>;
	const Assignements& assignements = std::any_cast<const Assignements&>(ast->value);
	const Config::Object& assignmentTable = _config["Tags"]["AssignmentTable"].AsObject();
	const Config::Array& h1 = _config["Tags"]["h1"].AsArray();
	const Config::Array& section = _config["Tags"]["Section"].AsArray();
	const Config::Object& translation = _config["Translations"].AsObject();


	buffer += h1[0]->AsString();
	buffer += translation["AdvancementReport"].AsString();
	buffer += h1[1]->AsString();
	auto str = section[0]->AsString();
	Utils::FindAndReplace(str, "$LINK$", "AdvancementReport");
	buffer += str;
	buffer += assignmentTable["Table"][0].AsString();
	for (const auto& [name, elems]: assignements)
	{
		buffer += assignmentTable["Tr"][0].AsString();
		buffer += assignmentTable["Td"][0].AsString();
		buffer += name;
		buffer += assignmentTable["Td"][1].AsString();
		buffer += assignmentTable["Td"][0].AsString();
		for (const auto& assignement: elems)
		{
			buffer += assignement + "<br>";
		}
		buffer += assignmentTable["Td"][1].AsString();
		buffer += assignmentTable["Tr"][1].AsString();
	}
	buffer += assignmentTable["Table"][1].AsString();
	buffer += section[1]->AsString();
}

void HTMLWriter::WriteFrontPage(Buffer& buffer)
{
	const Config::Object& frontPage = _config["Tags"]["FrontPage"].AsObject();
	const Config::Array& sprintName = frontPage["SprintName"].AsArray();
	const Config::Array& image = frontPage["Image"].AsArray();
	const Config::Array& title = frontPage["Title"].AsArray();
	const Config::Array& description = frontPage["Description"].AsArray();
	const Config::Array& lastUpdate = frontPage["LastUpdate"].AsArray();
	const Config::Array& container = frontPage["Container"].AsArray();
	const Config::Object& translation = _config["Translations"].AsObject();

	buffer += sprintName[0]->AsString();
	buffer += _config["FrontPage"]["Sprint"].AsString();
	buffer += sprintName[1]->AsString();
	buffer += container[0]->AsString();
	buffer += image[0]->AsString();
	Utils::CopyImageContent(buffer, _config["FrontPage"]["Image"].AsString());
	buffer += image[1]->AsString();
	buffer += title[0]->AsString();
	buffer += _config["FrontPage"]["Title"].AsString();
	buffer += title[1]->AsString();
	buffer += description[0]->AsString();
	buffer += _config["FrontPage"]["Description"].AsString();
	buffer += description[1]->AsString();
	buffer += lastUpdate[0]->AsString();
	buffer += translation["LastUpdate"].AsString();
	buffer += lastUpdate[1]->AsString();
	auto time = std::chrono::zoned_time{ "Europe/Paris", std::chrono::system_clock::now() };
	Utils::FindAndReplace(buffer, "$DATE$", std::format("{:%d-%m-%Y}", time));
	Utils::FindAndReplace(buffer, "$TIME$", std::format("{:%Hh%M}", time));
	buffer += container[1]->AsString();
}

void HTMLWriter::WriteFunctionalSchema(Buffer& buffer)
{
	const Config::Array& section = _config["Tags"]["Section"].AsArray();
	const Config::Array& h1 = _config["Tags"]["h1"].AsArray();
	const Config::Object& translation = _config["Translations"].AsObject();
	buffer += h1[0]->AsString();
	buffer += translation["FunctionalSchema"].AsString();
	buffer += h1[1]->AsString();
	auto str = section[0]->AsString();
	Utils::FindAndReplace(str, "$LINK$", "FunctionalSchema");
	buffer += str;
	buffer += R"(<img alt="Schéma fonctionnel" class="Schema" src="data:image/png;base64,)";
	Utils::CopyImageContent(buffer, (_config["FrontPage"]["Schema"].AsString()));
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