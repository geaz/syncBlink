#include "program_ast.hpp"

namespace SyncBlink
{
    void ProgramAst::addNode(std::unique_ptr<const AstNode> node)
    {
        _nodeList.push_back(std::move(node));
    }

    const std::vector<std::unique_ptr<const AstNode>>& ProgramAst::getNodes() const
    {
        return _nodeList;
    }
}