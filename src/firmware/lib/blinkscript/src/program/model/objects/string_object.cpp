#include "string_object.hpp"
#include "program/model/string_hash.hpp"

namespace SyncBlink
{
    StringObj::StringObj(const std::string& string) : _string(string), _hash(stringHash(string))
    {
    }

    uint32_t StringObj::getHash() const
    {
        return _hash;
    }

    ObjectType StringObj::getType() const
    {
        return ObjectType::STRING;
    }

    const std::string& StringObj::getString() const
    {
        return _string;
    }
}