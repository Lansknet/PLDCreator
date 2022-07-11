/*
** EPITECH PROJECT, 2022
** Documents [SSH: x2024lansknet1202373699000.westeurope.cloudapp.azure.com]
** File description:
** AST
*/

#ifndef AST_HPP_
#define AST_HPP_

#include <vector>
#include <any>
#include <memory>

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
        DoDArray,
        DoD,
        Bullets,
        AssignmentTable
    };
    AST(Type _type) : type(_type), value(), next() {}
    AST(Type _type, std::any any) : type(_type), value(std::move(any)), next() {}
    Type type = Null;
    bool visited = false;
    std::any value;
    std::vector<std::unique_ptr<AST>> next;
};

using AstPtr = std::unique_ptr<AST>;

#endif /* !AST_HPP_ */
