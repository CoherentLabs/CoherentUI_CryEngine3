/* D3D_Plugin - for licensing and copyright see license.txt */

#pragma once

#include <Game.h>

#include <IPluginManager.h>
#include <IPluginBase.h>
#include <CPluginBase.hpp>

#include <IPluginD3D.h>

#define PLUGIN_NAME "D3D"
#define PLUGIN_CONSOLE_PREFIX "[" PLUGIN_NAME " " PLUGIN_TEXT "] " //!< Prefix for Logentries by this plugin

/**
* @brief small listener helper for event broadcasting
*/
#define DECLARE_BROADCAST_EVENT(QUEUE, METHOD, ...) \
    void METHOD() \
    { \
        if(m_bD3DHookInstalled) \
            for(auto iterQueue = QUEUE.begin(); iterQueue!=QUEUE.end(); ++iterQueue) \
                (*iterQueue)->METHOD(__VA_ARGS__); \
    }

/**
* @brief small listener helper for registering listeners
*/
#define DECLARE_REGISTER_LISTENER(QUEUE) \
    void RegisterListener( ID3DEventListener* item )\
    {\
        QUEUE.push_back( item );\
        \
        if ( !m_bD3DHookInstalled && QUEUE.size() > 0 )\
        {\
            hookD3D( true );\
        }\
    }

/**
* @brief small listener helper for unregistering listeners
*/
#define DECLARE_UNREGISTER_LISTENER(QUEUE) \
    void UnregisterListener( ID3DEventListener* item )\
    {\
        for ( auto iterQueue = QUEUE.begin(); iterQueue != QUEUE.end(); ++iterQueue )\
        {\
            if ( ( *iterQueue ) == item )\
            {\
                iterQueue = QUEUE.erase( iterQueue );\
                \
                if ( iterQueue == QUEUE.end() )\
                {\
                    break;\
                }\
            }\
        }\
    }

namespace D3DPlugin
{
    /**
    * @brief Small helper to clean up the correct system type (but outside the main interface)
    */
    struct IPluginD3DEx :
        public IPluginD3D
    {
        public:
            /**
            * @brief Releases the system
            */
            virtual void Release() = 0;
    };

    /**
    * @brief Provides information and manages the resources of this plugin
    */
    class CPluginD3D :
        public PluginManager::CPluginBase
    {
        private:
            IPluginD3DEx* m_pDXSystem;

        public:
            CPluginD3D();
            ~CPluginD3D();

            // IPluginBase
            bool Release( bool bForce = false );

            int GetInitializationMode() const
            {
                return int( PluginManager::IM_Default );
            };

            bool Init( SSystemGlobalEnvironment& env, SSystemInitParams& startupParams, IPluginBase* pPluginManager, const char* sPluginDirectory );

            const char* GetVersion() const
            {
                return "1.9.0.0";
            };

            const char* GetName() const
            {
                return PLUGIN_NAME;
            };

            const char* GetCategory() const
            {
                return "Framework";
            };

            const char* ListAuthors() const
            {
                return "Hendrik Polczynski <hendrikpolczyn at gmail dot com>";
            };

            const char* GetStatus() const;

            const char* GetCurrentConcreteInterfaceVersion() const
            {
                return "1.0";
            };

            void* GetConcreteInterface( const char* sInterfaceVersion )
            {
                return static_cast < IPluginD3D* > ( m_pDXSystem );
            };

            PluginManager::IPluginBase* GetBase()
            {
                return static_cast<PluginManager::IPluginBase*>( this );
            };
    };

    extern CPluginD3D* gPlugin;
}

/**
* @brief This function is required to use the Autoregister Flownode without modification.
* Include the file "CPluginD3D.h" in front of flownode.
*/
inline void GameWarning( const char* sFormat, ... ) PRINTF_PARAMS( 1, 2 );
inline void GameWarning( const char* sFormat, ... )
{
    va_list ArgList;
    va_start( ArgList, sFormat );
    D3DPlugin::gPlugin->LogV( ILog::eWarningAlways, sFormat, ArgList );
    va_end( ArgList );
};
