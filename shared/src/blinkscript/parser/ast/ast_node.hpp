#ifndef ASTNODE_H
#define ASTNODE_H

#include "ast_visitor.hpp"

#include <memory>
#include <string>

namespace SyncBlink
{
    class AstNode
    {
    public:
        virtual ~AstNode(){};
        virtual void accept(AstVisitor& visitor) const = 0;
    };
}

#endif // ASTNODE_H