#include <vector>
#include <any>
#include <memory>

#pragma once

struct AST
{
    public:
    enum Type{
        Null = -1,
        Root,
        Object,
        UserStories,
        Name,
        Description,
        EstimatedTime,
        AsA,
        IWant,
        DoD,
        Bullets
    };
    AST(Type _type) : type(_type) {}
    Type type = Null;
    bool visited = false;
    std::any value;
    std::vector<std::unique_ptr<AST>> next;
};

using AstPtr = std::unique_ptr<AST>;
