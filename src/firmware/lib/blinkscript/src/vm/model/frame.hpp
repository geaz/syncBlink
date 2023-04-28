#ifndef FRAME_H
#define FRAME_H

#include "value.hpp"
#include "symbol_table.hpp"

#include <map>
#include <string>
#include <memory>

namespace SyncBlink
{    
    class Frame
    {
    public:
        Frame();
        Frame(std::shared_ptr<Frame> parentFrame);

        bool add(uint32_t hash, Value value);
        bool set(uint32_t hash, Value value);
        Value get(uint32_t hash);

        bool addSet(const std::string& identifier, Value value);
        Value get(const std::string& identifier);

        std::shared_ptr<Frame> getParentFrame() const;

    private:  
        SymbolTable _variables;
        std::shared_ptr<Frame> _parentFrame = nullptr;     
    };
}

#endif // FRAME_H