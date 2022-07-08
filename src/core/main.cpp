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
    using namespace Config;
    StructuredData parser("/home/lansknet/Documents/PLDCreator/config.json");
    const Config::Object &config = parser.getConfig();
    StructuredData pldFile(config["PLDJsonFile"].as<String>());
    ModuleLoader ml(config);
    PLDValidator pld(pldFile.getConfig());
    
    for (auto &mod : ml.getModules())
    {
        if (mod->getName() == config["OutputType"].as<String>())
        {
            mod->create(config);
            mod->write(pld.getAst());
        }
    }
}