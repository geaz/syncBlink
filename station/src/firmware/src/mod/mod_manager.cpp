#include <EEPROM.h>
#include <LittleFS.h>
#include "mod_manager.hpp"
#include "station_context.hpp"

namespace SyncBlink
{
    Mod ModManager::get(const std::string& modName)
    {
        Mod mod;
        mod.Name = modName;
        mod.Exists = true;

        std::string path = "/mods/" + modName;
        if(LittleFS.exists(path.c_str()))
        {
            File file = LittleFS.open(path.c_str(), "r");
            while (file.available()) {
                mod.Content = file.readString().c_str();
            }
            file.close();
        }
        else
        {
            mod.Exists = false;
        }        

        return mod;
    }

    std::vector<std::string> ModManager::getList()
    {
        std::vector<std::string> modList;

        Dir dir = LittleFS.openDir("mods");
        while (dir.next())
        {
            modList.push_back(dir.fileName().c_str());
        }

        return modList;
    }

    void ModManager::add(const std::string& modName)
    {
        File file = LittleFS.open(("/mods/" + modName).c_str(), "w");
        file.close();
    }

    void ModManager::save(const std::string& modName, const std::string& modContent)
    {
        File file = LittleFS.open(("/mods/" + modName).c_str(), "w");
        file.print(modContent.c_str());
        file.close();
        
        if(getActiveModName() == modName)
        {
            for(auto event : activeModChangedEvents.getEventHandlers()) event.second();
        }
    }

    void ModManager::remove(const std::string& modName)
    {
        LittleFS.remove(("/mods/" + modName).c_str());
    }

    std::string ModManager::getActiveModName()
    {
        int i = 0;
        char data[SyncBlink::ModRomLength];
        char curChar = char(EEPROM.read(i + SyncBlink::ModRomStart));

        while (i < SyncBlink::ModRomLength && curChar != '\0')
        {
            curChar = char(EEPROM.read(i + SyncBlink::ModRomStart));
            data[i] = curChar;
            i++;
        }

        std::string activeMod(data);
        #ifdef DEBUG_MODMANAGER
        Serial.printf("[MODMANAGER] Active MOD: %s\n", activeMod.c_str());
        #endif

        Mod mod = get(activeMod);
        if(!mod.Exists) 
        {
            Serial.println("Currently active mod not found! Falling back ...");
            std::vector<std::string> modList = getList();
            if(modList.size() > 0)
            {
                saveActiveModName(getList().front());
                activeMod = getActiveModName();
            }
        }
        return activeMod;
    }

    void ModManager::saveActiveModName(const std::string& modName)
    {
        if (modName.length() > 0) 
        {
            #ifdef DEBUG_MODMANAGER
            Serial.println("[MODMANAGER] Clearing active Mod ...");
            #endif
            for (int i = SyncBlink::ModRomStart; i < SyncBlink::ModRomEnd; ++i) { EEPROM.write(i, 0); }

            #ifdef DEBUG_MODMANAGER
            Serial.printf("[MODMANAGER] Saving active Mod (%s) ...\n", modName.c_str());
            #endif
            for (uint i = 0; i < modName.length(); ++i) EEPROM.write(i + SyncBlink::ModRomStart, modName[i]);

            EEPROM.commit();
            for(auto event : activeModChangedEvents.getEventHandlers()) event.second();
        }
    }

    AudioAnalyzerSource ModManager::getActiveSource()
    {
        uint rawSource = EEPROM.read(SyncBlink::SourceRom);
        if(rawSource != 0 && rawSource != 1) {
            #ifdef DEBUG_MODMANAGER
            Serial.println("[MODMANAGER] Currently active source not valid! Falling back ...");
            #endif
            saveActiveSource(AudioAnalyzerSource::Station);
        }
        return static_cast<AudioAnalyzerSource>(EEPROM.read(SyncBlink::SourceRom));
    }

    void ModManager::saveActiveSource(AudioAnalyzerSource source)
    {
        #ifdef DEBUG_MODMANAGER
        Serial.printf("[MODMANAGER] Saving active Source (%d) ...\n", source);
        #endif
        EEPROM.write(SyncBlink::SourceRom, static_cast<uint>(source));
        EEPROM.commit();
    }
}