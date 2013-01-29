/* D3D_Plugin - for licensing and copyright see license.txt */

#include <StdAfx.h>
#include <CPluginD3D.h>

#include "dx9/CD3DSystemDX9.h"
#include "dx11/CD3DSystemDX11.h"

namespace D3DPlugin
{
    CPluginD3D* gPlugin = NULL;

    CPluginD3D::CPluginD3D()
    {
        gPlugin = this;
    }

    CPluginD3D::~CPluginD3D()
    {
        Release( true );

        gPlugin = NULL;
    }

    bool CPluginD3D::Release( bool bForce )
    {
        bool bRet = true;

        if ( !m_bCanUnload )
        {
            // Should be called while Game is still active otherwise there might be leaks/problems
            bRet = CPluginBase::Release( bForce );

            if ( bRet )
            {
                // Disable hooks and system
                SAFE_RELEASE( m_pDXSystem );

                // Cleanup like this always (since the class is static its cleaned up when the dll is unloaded)
                gPluginManager->UnloadPlugin( GetName() );

                // Allow Plugin Manager garbage collector to unload this plugin
                AllowDllUnload();
            }
        }

        return bRet;
    };

    bool CPluginD3D::Init( SSystemGlobalEnvironment& env, SSystemInitParams& startupParams, IPluginBase* pPluginManager, const char* sPluginDirectory )
    {
        gPluginManager = ( PluginManager::IPluginManager* )pPluginManager->GetConcreteInterface( NULL );
        CPluginBase::Init( env, startupParams, pPluginManager, sPluginDirectory );

#if !defined(D3D_DISABLE_SYSTEM)

        if ( !m_pDXSystem )
        {
            if ( GetModuleHandle( "d3d11.dll" ) && gEnv->pRenderer->GetRenderType() == eRT_DX11 )
            {
                m_pDXSystem = new CD3DSystem11();
            }

            else if ( GetModuleHandle( "d3d9.dll" ) && gEnv->pRenderer->GetRenderType() == eRT_DX9 )
            {
                m_pDXSystem = new CD3DSystem9();
            }
        }

#endif

        if ( !m_pDXSystem )
        {
            gPlugin->LogError( "Couldn't find any suitable renderer" );
        }

        return true;
    }

    const char* CPluginD3D::GetStatus() const
    {
        return "OK";
    }
}