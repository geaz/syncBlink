#ifndef PROGRAMNODE_H
#define PROGRAMNODE_H

#include "ast_node.hpp"
#include "ast_visitor.hpp"
#include "source/script_source.hpp"

#include <memory>
#include <vector>

namespace SyncBlink
{
    class ProgramAst
    {
    public:
        ProgramAst(std::shared_ptr<ScriptSource> source);

        void addNode(std::unique_ptr<const AstNode> node);
        const std::vector<std::unique_ptr<const AstNode>>& getNodes() const;

    private:
        std::shared_ptr<ScriptSource> _source;
        std::vector<std::unique_ptr<const AstNode>> _nodeList;
    };
}

#endif // PROGRAMNODE_H