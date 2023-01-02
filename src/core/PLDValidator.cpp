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

PLDValidator::PLDValidator(const Config::Object& data)
{
	_ast = ParseRoot(data);
}

AstPtr PLDValidator::GetAst()
{
	return std::move(_ast);
}

AstPtr PLDValidator::ParseRoot(const Config::Object& object)
{
	AstPtr ast = std::make_unique<AST>(AST::Type::Root);
	for (auto& [key, value]: object)
	{
		if (key == "UserStories")
			ast->next.push_back(ParseUserStories(value->as<Config::Array>()));
		else if (key == "AssignmentTable")
			ast->next.push_back(ParseAssignmentTable(value->as<Config::Object>()));
		else if (key == "DeliverablesMap")
			ast->next.push_back(ParseDeliverablesMap(value->as<Config::Array>()));
		else if (key == "AdvancementReport")
			ast->next.push_back(ParseAdvancementReport(value->as<Config::Object>()));
		else if (key.starts_with("Sprint"))
		{
			ast->next.push_back(ParseRoot(value->as<Config::Object>()));
			ast->next.back()->next.insert(ast->next.back()->next.begin(),
					ParseTableOfContent(object.begin()->second->as<Config::Object>()));
		}
		else throw std::runtime_error("Invalid key in root: " + key);
	}
	return ast;
}

AstPtr PLDValidator::ParseTableOfContent(const Config::Object& object)
{
	AstPtr ast = std::make_unique<AST>(AST::Type::TableOfContent);
	for (auto& [key, value]: object)
	{
		auto any = std::any(key);
		if (key == "UserStories")
			ast->next.push_back(ParseUserStoriesTableOfContent(value->as<Config::Array>()));
		else ast->next.push_back(std::make_unique<AST>(AST::Type::TableOfContentEntry, std::move(any)));
	}
	return ast;
}

AstPtr PLDValidator::ParseUserStoriesTableOfContent(const Config::Array & object)
{
	AstPtr ast = std::make_unique<AST>(AST::Type::TableOfContentEntry);
	ast->value = std::string("UserStories");
	for (const auto& us: object)
	{
		const auto& usObject = us->as<Config::Object>();
		const auto& str = usObject["Title"].as<std::string>();
		ast->next.push_back(std::make_unique<AST>(AST::Type::TableOfContentEntry, std::any(usObject["Title"].as<std::string>())));
	}
	return ast;
}

AstPtr PLDValidator::ParseUserStories(const Config::Array& array)
{
	//TODO: this function should be refactored, or AST struct should be changed
	AstPtr ast = std::make_unique<AST>(AST::Type::UserStories);
	for (auto& us: array)
	{
		for (auto& [key, value]: us->as<Config::Object>())
		{
			if (key == "Title")
			{
				auto any = std::make_any<Config::String>(value->as<Config::String>());
				ast->next.push_back(std::make_unique<AST>(AST::Type::Name, std::move(any)));
			}
			else if (key == "Description")
			{
				auto any = std::make_any<Config::String>(value->as<Config::String>());
				ast->next.push_back(std::make_unique<AST>(AST::Type::Description, std::move(any)));
			}
			else if (key == "Assignee")
			{
				auto any = std::make_any<Config::String>(value->as<Config::String>());
				ast->next.push_back(std::make_unique<AST>(AST::Type::Assignee, std::move(any)));
			}
			else if (key == "Status")
			{
				auto any = std::make_any<Config::String>(value->as<Config::String>());
				ast->next.push_back(std::make_unique<AST>(AST::Type::Status, std::move(any)));
			}
			else if (key == "EstimatedTime")
			{
				auto any = std::make_any<Config::Int32>(value->as<Config::Int32>());
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
				ast->next.push_back(ParseDoD(value->as<Config::Array>()));
			}
			else throw std::logic_error("Invalid key in userStories: " + value->as<Config::String>());
		}
	}
	return ast;
}

AstPtr PLDValidator::ParseDoD(const Config::Array& array)
{
	AstPtr ast = std::make_unique<AST>(AST::Type::DoDArray);
	for (auto& dod: array)
	{
		AstPtr astDod = std::make_unique<AST>(AST::Type::DoD);
		for (auto& [key, value]: dod->as<Config::Object>())
		{
			if (key == "Description")
			{
				astDod->next.push_back(std::make_unique<AST>(AST::Type::Description,
						std::make_any<Config::String>(value->as<Config::String>())));
			}
			else if (key == "Bullets")
			{
				astDod->next.push_back(ParseBullets(value->as<Config::Array>()));
			}
			else throw std::logic_error("Invalid key in DoD: " + value->as<Config::String>());
		}
		ast->next.push_back(std::move(astDod));
	}
	return ast;
}

AstPtr PLDValidator::ParseBullets(const Config::Array& array)
{
	AstPtr ast = std::make_unique<AST>(AST::Type::Bullets);
	std::vector<Config::String> bullets;
	for (auto& value: array)
	{
		bullets.push_back(value->as<Config::String>());
	}
	ast->value = std::make_any<std::vector<Config::String>>(std::move(bullets));
	return ast;
}

AstPtr PLDValidator::ParseAssignmentTable(const Config::Object& array)
{
	AstPtr ast = std::make_unique<AST>(AST::Type::AssignmentTable);
	using Assignements = std::unordered_map<std::string, std::vector<std::string>>;
	Assignements assignements;
	for (const auto &elem : array)
	{
		const auto& [key, values] = elem;

		for (const auto& element: values->as<Config::Array>())
		{
			assignements[key].push_back(element->as<Config::String>());
		}
	}
	ast->value = std::make_any<Assignements>(assignements);
	return ast;
}

AstPtr PLDValidator::ParseDeliverablesMap(const Config::Array& array)
{
	AstPtr ast = std::make_unique<AST>(AST::Type::DeliverablesMap);
	using DeliverablesMap = std::unordered_map<std::string, std::vector<std::string>>;
	DeliverablesMap deliverables;
	for (const auto& elem: array)
	{
		for (const auto& [delivrableName, value]: elem->as<Config::Object>())
		{
				std::vector<std::string> s;
			for (const auto& delivrableElement: value->as<Config::Array>())
			{
				s.push_back(delivrableElement->as<Config::String>());
				deliverables[delivrableName] = s;
			}

		}
	}
	ast->value = std::make_any<DeliverablesMap>(deliverables);
	return ast;
}

AstPtr PLDValidator::ParseAdvancementReport(const Config::Object& object)
{
	AstPtr ast = std::make_unique<AST>(AST::Type::AdvancementReport);
	using Assignements = std::unordered_map<std::string, std::vector<std::string>>;
	Assignements assignements;
	for (const auto &elem : object)
	{
		const auto& [key, values] = elem;

		for (const auto& element: values->as<Config::Array>())
		{
			assignements[key].push_back(element->as<Config::String>());
		}
	}
	ast->value = std::make_any<Assignements>(assignements);
	return ast;
}
