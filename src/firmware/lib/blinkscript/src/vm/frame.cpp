#include "frame.hpp"

#include "program/model/string_hash.hpp"

namespace SyncBlink
{
    Frame::Frame()
    {
    }

    Frame::Frame(std::shared_ptr<Frame> parentFrame) : _parentFrame(parentFrame)
    {
    }

    bool Frame::add(uint32_t hash, Value value)
    {
        return _variables.set(hash, value);
    }

    bool Frame::set(uint32_t hash, Value value)
    {
        bool result = false;

        Value foundValue;
        if (_variables.get(hash, &foundValue))
        {
            _variables.set(hash, value);
            result = true;
        }
        else if (_parentFrame != nullptr)
            result = _parentFrame->set(hash, value);

        return result;
    }

    Value Frame::get(uint32_t hash)
    {
        Value foundValue;
        if (_variables.get(hash, &foundValue))
            return foundValue;
        else if (_parentFrame != nullptr)
            return _parentFrame->get(hash);
        else
            return Value();
    }

    bool Frame::addSet(const std::string& identifier, Value value)
    {
        uint32_t hash = stringHash(identifier);
        bool result = set(hash, value);
        if (!result)
            result = add(hash, value);
        return result;
    }

    Value Frame::get(const std::string& identifier)
    {
        uint32_t hash = stringHash(identifier);
        return get(hash);
    }

    std::shared_ptr<Frame> Frame::getParentFrame() const
    {
        return _parentFrame;
    }
}