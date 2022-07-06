#include "AST.hpp"
#include "nlohmann/json_fwd.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include "Parser.hpp"
#include "HTMLWritter.hpp"
#include <queue>

using json = nlohmann::ordered_json;

int main(int argc, const char** argv)
{
    std::ifstream i(argv[1]);
    auto j = nlohmann::ordered_json::parse(i);
    Parser p(j);
    auto ast = p.getAst();
    std::unique_ptr<IWritter> writter = std::make_unique<HTMLWritter>();
    buffer b;
    writter->write(b, std::move(ast));
    std::cout << b << std::endl;
    
}