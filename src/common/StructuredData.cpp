#include <iostream>
#include <fstream>
#include <stdexcept>
#include "StructuredData.hpp"
#include "Config.hpp"

StructuredData::StructuredData(const std::string& file)
{
    std::ifstream i(file);
    _config = GetObject(json::parse(i));
}

const Config::Object& StructuredData::GetConfig() const
{
	return _config;
}

Config::Object StructuredData::GetObject(const json& object)
{
	if (!object.is_object())
		throw std::logic_error("JSON: file should be a JSON object");
	Config::Object config;
	for (auto& [key, value]: object.items())
	{
		if (value.is_object())
			config.push_back({ key, std::make_shared<Config::Node>(GetObject(value)) });
		else if (value.is_array())
			config.push_back({ key, std::make_shared<Config::Node>(GetArray(value)) });
		else if (value.is_string())
			config.push_back({ key, std::make_shared<Config::Node>(value.get<Config::String>()) });
		else if (value.is_primitive())
			config.push_back({ key, GetPrimitive(value)});
		else throw std::logic_error("Unknow value type: " + value.dump());
	}
	return config;
}

Config::Array StructuredData::GetArray(const json& array)
{
	if (!array.is_array())
		throw std::logic_error("JSON: file should be a JSON array");
	Config::Array config;
	for (auto& value: array)
	{
		if (value.is_object())
			config.push_back(std::make_shared<Config::Node>(GetObject(value)));
		else if (value.is_array())
			config.push_back(std::make_shared<Config::Node>(GetArray(value)));
		else if (value.is_string())
			config.push_back(std::make_shared<Config::Node>(value.get<Config::String>()));
		else
			throw std::logic_error("Unknow value type: " + value.dump());
	}
	return config;
}

Config::NodePtr StructuredData::GetPrimitive(const json& primitive)
{
	if (primitive.is_string())
		return std::make_shared<Config::Node>(primitive.get<Config::String>());
	else if (primitive.is_number_integer())
		return std::make_shared<Config::Node>(primitive.get<Config::Int32>());
	else if (primitive.is_number_unsigned())
		return std::make_shared<Config::Node>(primitive.get<Config::UInt32>());
	else if (primitive.is_number_float())
		return std::make_shared<Config::Node>(primitive.get<Config::Double>());
	else if (primitive.is_boolean())
		return std::make_shared<Config::Node>(primitive.get<Config::Bool>());
	else
		throw std::logic_error("Unknown value type: " + primitive.dump());
}