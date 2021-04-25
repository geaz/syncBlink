#ifndef STRINGOBJECT_H
#define STRINGOBJECT_H

#include "object.hpp"

#include <memory>
#include <string>

namespace SyncBlink
{
    class StringObj : public Object
    {
    public:
        StringObj(const std::string& string);

        uint32_t getHash() const;
        ObjectType getType() const;
        const std::string& getString() const;

    private:
        uint32_t _hash;
        std::string _string;
    };
}

#endif // STRINGOBJECT_H