#include "Config.hpp"
#include "PLDValidator.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <queue>
#include "StructuredData.hpp"
#include "ModuleLoader.hpp"

int main(int argc, const char** argv)
{
	try
	{
		using namespace Config;
		StructuredData parser("config.json");
		const Config::Object& config = parser.getConfig();
		StructuredData pldFile(config["PLDJsonFile"].as<String>());
		ModuleLoader ml(config);
		PLDValidator pld(pldFile.getConfig());

		for (auto& mod: ml.getModules())
		{
			if (mod->getName() == config["OutputType"].as<String>())
			{
				mod->create(config);
				mod->write(pld.getAst());
			}
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
}