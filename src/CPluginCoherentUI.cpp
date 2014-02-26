/* CoherentUI_Plugin - for licensing and copyright see license.txt */

#include <StdAfx.h>
#include <CPluginCoherentUI.h>
#include "CoherentUISystem.h"

namespace CoherentUIPlugin
{
    CPluginCoherentUI* gPlugin = NULL;
    D3DPlugin::IPluginD3D* gD3DSystem = NULL;
    void* gD3DDevice = NULL;

    CPluginCoherentUI::CPluginCoherentUI()
    {
        gPlugin = this;
    }

    CPluginCoherentUI::~CPluginCoherentUI()
    {
        Release( true );

        gPlugin = NULL;
    }

    bool CPluginCoherentUI::Release( bool bForce )
    {
        bool bRet = true;
        bool bWasInitialized = m_bIsFullyInitialized; // Will be reset by base class so backup

        if ( !m_bCanUnload )
        {
            // Note: Type Unregistration will be automatically done by the Base class (Through RegisterTypes)
            // Should be called while Game is still active otherwise there might be leaks/problems
            bRet = CPluginBase::Release( bForce );

            if ( bRet )
            {
                ShutdownSystem();

                if ( gD3DSystem )
                {
                    PluginManager::safeReleasePlugin( "D3D", gD3DSystem );
                }

                // Cleanup like this always (since the class is static its cleaned up when the dll is unloaded)
                gPluginManager->UnloadPlugin( GetName() );

                // Allow Plugin Manager garbage collector to unload this plugin
                AllowDllUnload();
            }
        }

        return bRet;
    };

    bool CPluginCoherentUI::Init( SSystemGlobalEnvironment& env, SSystemInitParams& startupParams, IPluginBase* pPluginManager, const char* sPluginDirectory )
    {
        gPluginManager = ( PluginManager::IPluginManager* )pPluginManager->GetConcreteInterface( NULL );
        CPluginBase::Init( env, startupParams, pPluginManager, sPluginDirectory );

        if ( gEnv && gEnv->pSystem && !gEnv->pSystem->IsQuitting() )
        {
            gCoherentUISystem = new CCoherentUISystem();
        }

        return true;
    }

    bool CPluginCoherentUI::RegisterTypes( int nFactoryType, bool bUnregister )
    {
        // Note: Autoregister Flownodes will be automatically registered by the Base class
        bool bRet = CPluginBase::RegisterTypes( nFactoryType, bUnregister );

        using namespace PluginManager;
        eFactoryType enFactoryType = eFactoryType( nFactoryType );

        if ( bRet )
        {
            if ( gEnv && gEnv->pSystem && !gEnv->pSystem->IsQuitting() )
            {
                // CVars
                if ( gEnv->pConsole && ( enFactoryType == FT_All || enFactoryType == FT_CVar ) )
                {
                    if ( !bUnregister )
                    {
                        // TODO: Register CVars here if you have some
                        // ...
                    }

                    else
                    {
                        // TODO: Unregister CVars here if you have some
                        // ...
                    }
                }

                // CVars Commands
                if ( gEnv->pConsole && ( enFactoryType == FT_All || enFactoryType == FT_CVarCommand ) )
                {
                    if ( !bUnregister )
                    {
                        // TODO: Register CVar Commands here if you have some
                        // ...
                    }

                    else
                    {
                        // TODO: Unregister CVar Commands here if you have some
                        // ...
                    }
                }

                // Game Objects
                if ( gEnv->pGame && gEnv->pGame->GetIGameFramework() && ( enFactoryType == FT_All || enFactoryType == FT_GameObjectExtension ) )
                {
                    if ( !bUnregister )
                    {
                        // TODO: Register Game Object Extensions here if you have some
                        // ...
                    }
                }
            }
        }

        return bRet;
    }

    const char* CPluginCoherentUI::ListCVars() const
    {
        return "..."; // TODO: Enter CVARs/Commands here if you have some
    }

    const char* CPluginCoherentUI::GetStatus() const
    {
        return "OK";
    }

    bool CPluginCoherentUI::CheckDependencies() const
    {
        bool bRet = CPluginBase::CheckDependencies();

        if ( bRet )
        {
            bRet = PluginManager::safeGetPluginConcreteInterface<D3DPlugin::IPluginD3D*>( "D3D" );
        }

        return bRet;
    }

    bool CPluginCoherentUI::InitDependencies()
    {
        if ( gEnv && gEnv->pSystem && !gEnv->pSystem->IsQuitting() && gCoherentUISystem )
        {
            gD3DSystem = PluginManager::safeUsePluginConcreteInterface<D3DPlugin::IPluginD3D*>( "D3D" );

            if ( gD3DSystem )
            {
                gD3DDevice = gD3DSystem->GetDevice();
                gD3DSystem->RegisterListener( gCoherentUISystem );
            }
        }

        return CPluginBase::InitDependencies();
    }

    bool CPluginCoherentUI::InitializeSystem()
    {
        if ( gCoherentUISystem )
        {
            return gCoherentUISystem->InitializeCoherentUI();
        }

        else
        {
            if ( gEnv && gEnv->pSystem && !gEnv->pSystem->IsQuitting() )
            {
                gCoherentUISystem = new CCoherentUISystem();

                if ( gD3DSystem )
                {
                    gD3DSystem->RegisterListener( gCoherentUISystem );
                }

                return true;
            }
        }

        return false;
    }

    void CPluginCoherentUI::ShutdownSystem()
    {
        if ( gCoherentUISystem )
        {
            if ( gD3DSystem )
            {
                gD3DSystem->UnregisterListener( gCoherentUISystem );
            }

            delete gCoherentUISystem;
            gCoherentUISystem = NULL;
        }
    }

    IPlayerEventListener* CPluginCoherentUI::GetPlayerEventListener() const
    {
        if ( gCoherentUISystem )
        {
            return gCoherentUISystem->GetPlayerEventListener();
        }

        return NULL;
    }
}