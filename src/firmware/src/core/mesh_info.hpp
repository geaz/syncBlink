#ifndef MESHINFO_H
#define MESHINFO_H

#include "core/config/config.hpp"
#include "core/script_list.hpp"
#include "core/script.hpp"
#include "core/message/messages/mesh_connection.hpp"
#include "core/network/get_id.hpp"

#include <map>
#include <tuple>
#include <string>

namespace SyncBlink
{
    class MeshInfo
    {
    public:
        MeshInfo(Config& config, ScriptList& scriptList) : _config(config), _scriptList(scriptList)
        {
        }

        std::tuple<uint64_t, NodeInfo> getLocalNodeInfo() const
        {
            NodeInfo info;
            info.isStation = _config.Values[F("is_hub")];
            info.isAnalyzer = _config.Values[F("is_analyzer")];
            info.isNode = _config.Values[F("is_node")];
            info.parentId = 0;
            info.ledCount = _config.Values[F("led_count")];
            info.majorVersion = VERSIONMAJOR;
            info.minorVersion = VERSIONMINOR;

            std::string nodeLabel = _config.Values[F("name")];
            info.nodeLabel = nodeLabel;
            
            return std::make_tuple(SyncBlink::getId(), info);
        }

        void addNode(uint64_t nodeId, NodeInfo nodeInfo)
        {
            _connectedNodes[nodeId] = nodeInfo;
        }

        void removeNode(uint64_t nodeId)
        {
            auto iter = _connectedNodes.begin();
            auto endIter = _connectedNodes.end();
            for (; iter != endIter;)
            {
                if (iter->first == nodeId || iter->second.parentId == nodeId)
                {
                    iter = _connectedNodes.erase(iter);
                }
                else
                {
                    ++iter;
                }
            }
        }

        std::map<uint64_t, NodeInfo>& getConnectedNodes()
        {
            return _connectedNodes;
        }

        std::tuple<size_t, size_t> getTotalNodesAndLeds()
        {
            size_t totalNodes = 0;
            size_t totalLeds = 0;
            for (auto& node : _connectedNodes)
            {
                totalNodes++;
                totalLeds += node.second.ledCount;
            }
            return std::make_tuple(totalNodes, totalLeds);
        }

        Script getActiveScript()
        {
            Script script;
            const char* activeScriptName = _config.Values[F("active_script")];
            if (activeScriptName != nullptr)
            {
                script = _scriptList.get(activeScriptName);
            }

            if (!script.IsCompiled)
            {
                Serial.println(F("[MeshInfo] Currently active script not found! Falling back ..."));

                std::vector<Script> scriptList = _scriptList.getList();
                for (size_t i = 0; i < scriptList.size(); i++)
                {
                    Script script = scriptList[i];
                    if (script.IsCompiled)
                    {
                        setActiveScript(script.Name);
                        break;
                    }
                }
            }
            return script;
        }

        void setActiveScript(const std::string& scriptName)
        {
            if (scriptName.length() > 0)
            {
                Serial.printf_P(PSTR("[MeshInfo] Saving active script (%s) ...\n"), scriptName.c_str());
                _config.Values[F("active_script")] = scriptName.c_str();
                _config.save();
            }
        }

        void setActiveAnalyzer(const uint64_t& analyzerId) 
        {
            _activeAnalzyerId = analyzerId;
        }

        uint64_t getActiveAnalyzer() const 
        {
            return _activeAnalzyerId;
        }      

        void setLightMode(bool lightMode)
        {
            _lightMode = lightMode;
        }

        bool getLightMode() const
        {
            return _lightMode;
        }

    private:
        Config& _config;
        ScriptList& _scriptList;

        bool _lightMode;
        uint64_t _activeAnalzyerId;
        std::map<uint64_t, NodeInfo> _connectedNodes;
    };
}

#endif // SCRIPTLIST_H