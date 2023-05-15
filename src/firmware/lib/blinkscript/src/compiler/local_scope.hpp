#ifndef LOCALSCOPE_H
#define LOCALSCOPE_H

#include "program/program.hpp"

#include <cinttypes>
#include <string>
#include <vector>
#include <memory>

namespace SyncBlink
{   
    struct LocalValue 
    {
        uint16_t index;
        std::string identifier;
        uint16_t foreignIndex = 0;
        bool isForeignLocal = false;
    };

    struct LocalScope
    {
    public:
        LocalScope()
        {
        }

        LocalScope(std::shared_ptr<LocalScope> parentScope, bool isForeignScope = false) : parentScope(parentScope), _isForeignScope(isForeignScope)
        {
            if (!_isForeignScope) _localValues = parentScope->_localValues;
        }

        bool isGlobalScope() const
        {
            return parentScope == nullptr;
        }

        bool isForeignScope() const
        {
            return _isForeignScope;
        }

        bool isLocalDefined(std::string identifier) const 
        {
            bool defined = false;
            for(auto iter = _localValues.begin(); iter != _localValues.end(); iter++)
            {
                if(iter->identifier == identifier)
                {
                    defined = true;
                    break;
                }
            }
            return defined;
        }

        void addIdentifier(std::string identifier)
        {
            LocalValue value;
            value.index = (uint16_t)_localValues.size();
            value.identifier = identifier;

            _localValues.push_back(value);
        }

        bool getLocalValue(std::string identifier, LocalValue& value, bool foreignSearch = false)
        {
            bool found = false;
            for(auto iter = _localValues.begin(); iter != _localValues.end(); iter++)
            {
                if(iter->identifier == identifier)
                {
                    found = true;
                    value = *iter;
                    break;
                }
            }
            if (!found && _isForeignScope) found = resolveForeignLocal(identifier, value);
            return found;
        }

        const std::vector<LocalValue>& getLocalValues() const
        {
            return _localValues;
        }

        std::shared_ptr<LocalScope> parentScope;

    private:
        bool resolveForeignLocal(std::string identifier, LocalValue& value)
        {
            bool found = false;
            if (parentScope != nullptr)
            {
                LocalValue parentValue;
                found = parentScope->getLocalValue(identifier, parentValue, true);
                if (found)
                {
                    LocalValue newValue;
                    newValue.index = (uint16_t)_localValues.size();
                    newValue.identifier = parentValue.identifier;
                    newValue.foreignIndex = parentValue.index;
                    newValue.isForeignLocal = true;

                    _localValues.push_back(newValue);
                    value = newValue;
                }
            }
            return found;
        }

        bool _isForeignScope;
        std::vector<LocalValue> _localValues;
    };
}

#endif // LOCALSCOPE_H