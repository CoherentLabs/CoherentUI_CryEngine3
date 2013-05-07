/* CoherentUI_Plugin - for licensing and copyright see license.txt */

#include <IPluginBase.h>

#pragma once

struct IPlayerEventListener;

/**
* @brief CoherentUI Plugin Namespace
*/
namespace CoherentUIPlugin
{
    /**
    * @brief plugin CoherentUI concrete interface
    */
    struct IPluginCoherentUI
    {
        /**
        * @brief Get Plugin base interface
        */
        virtual PluginManager::IPluginBase* GetBase() = 0;

        virtual bool InitializeSystem() = 0;
        virtual void ShutdownSystem() = 0;
        virtual IPlayerEventListener* GetPlayerEventListener() const = 0;
    };
};