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
		const Config::Object& config = parser.GetConfig();
		StructuredData pldFile(config["PLDJsonFile"].as<String>());
		ModuleLoader ml(config);
		PLDValidator pld(pldFile.GetConfig());

		for (auto& mod: ml.GetModules())
		{
			if (mod->GetName() == config["OutputType"].as<String>())
			{
				mod->Create(config);
				mod->Write(pld.GetAst());
			}
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
}