/* D3D_Plugin - for licensing and copyright see license.txt */

#include <StdAfx.h>
#include "CD3DSystemDX9.h"
#include "d3d9hook.h"

#include <CPluginD3D.h>

//#pragma comment(lib, "dxerr.lib") // not needed atm

D3DPlugin::CD3DSystem9* D3DPlugin::gD3DSystem9 = NULL;

#undef METHOD
#undef INTERFACE

#define INTERFACE IDirect3DDevice9

#define METHOD Present
GEN_HOOK( const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion )
{
    D3DPlugin::gD3DSystem9->OnPrePresent();

    CALL_ORGINAL( pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion );

    D3DPlugin::gD3DSystem9->OnPostPresent();

    return hr;
}
#undef METHOD

#define METHOD BeginScene
GEN_HOOK()
{
    CALL_ORGINAL();

    D3DPlugin::gD3DSystem9->OnPostBeginScene();

    return hr;
}
#undef METHOD

#define METHOD Reset
GEN_HOOK( D3DPRESENT_PARAMETERS* pPresentationParameters )
{
    D3DPlugin::gD3DSystem9->OnPreReset();

    CALL_ORGINAL( pPresentationParameters );

    if ( SUCCEEDED( hr ) )
    {
        D3DPlugin::gD3DSystem9->OnPostReset();
    }

    return hr;
}
#undef METHOD

#define METHOD CreateTexture
GEN_HOOK( UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle )
{
    if ( D3DPlugin::gD3DSystem9->m_nTextureMode == HTM_INJECT )
    {
        goto INJECTTEXTURE;
    }

    CALL_ORGINAL( Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle );

    if ( SUCCEEDED( hr ) )
    {
        switch ( D3DPlugin::gD3DSystem9->m_nTextureMode )
        {
            case HTM_INJECT:
                ( *ppTexture )->Release(); // Scrap CryEngine texture

INJECTTEXTURE:
                *ppTexture = ( IDirect3DTexture9* )D3DPlugin::gD3DSystem9->m_pTempTex; // Inject new texture
                ( *ppTexture )->AddRef(); // Increment reference counter since its now used by CE3
                hr = S_OK;
                break;

            case HTM_CREATE:
                D3DPlugin::gD3DSystem9->m_pTempTex = *ppTexture;
                break;
        }

        D3DPlugin::gD3DSystem9->m_nTextureMode = HTM_NONE;
    }

    return hr;
}
#undef METHOD

bool GetD3D9DeviceData( INT_PTR* unkdata, int nDatalen, void* pParam )
{
    bool bRet = false;

    HWND hWndDummy = CreateWindowEx( NULL, TEXT( "Message" ), TEXT( "DummyWindow" ), WS_MINIMIZE, 0, 0, 8, 8, HWND_MESSAGE, NULL, 0, NULL );

    HMODULE hModule = NULL;
    hModule = GetModuleHandle( "d3d9.dll" );

    typedef IDirect3D9 * ( WINAPI * fDirect3DCreate9 )( unsigned int SDKVersion );
    fDirect3DCreate9 Direct3DCreate9 = ( fDirect3DCreate9 )GetProcAddress( hModule, "Direct3DCreate9" );

    IDirect3D9*          d3d        = NULL;
    IDirect3DDevice9*    p_device   = NULL;
    d3d = Direct3DCreate9( D3D_SDK_VERSION );

    if ( d3d )
    {
        D3DPRESENT_PARAMETERS pp;
        ZeroMemory( &pp, sizeof( D3DPRESENT_PARAMETERS ) );
        pp.SwapEffect       = D3DSWAPEFFECT_DISCARD;
        pp.hDeviceWindow    = hWndDummy;
        pp.Windowed         = TRUE;

        d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_NULLREF, hWndDummy, D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_NOWINDOWCHANGES, &pp, &p_device );

        if ( p_device )
        {
            bRet = true;
        }
    }

    if ( bRet )
    {
        void** vt = getVT( p_device );

        if ( vt )
        {
            memcpy( unkdata, vt, nDatalen );
        }

        else
        {
            bRet = false;
        }
    }

    SAFE_RELEASE( p_device );
    SAFE_RELEASE( d3d );
    DestroyWindow( hWndDummy );

    return bRet;
}

namespace D3DPlugin
{

    IDirect3DDevice9* FindD3D9Device( INT_PTR nRelativeBase, void* pTrialDevice )
    {
        INT_PTR nModuleOffset = ( INT_PTR )GetModuleHandle( TEXT( "d3d9.dll" ) );

        if ( !nModuleOffset )
        {
            gPlugin->LogWarning( "DX9 not active" );
            return NULL;
        }

        // Set Defaults
        static INT_PTR      dxoffset    = 0xC00 + sizeof( INT_PTR );
        static DWORD        dxoffsetlen = sizeof( dxoffset );

        static INT_PTR      dxdata[3];
        static DWORD        dxdatalen   = sizeof( dxdata );

        static bool bFirstCall = true;

        // Note: Those offsets could be deprecated since the find method is deprecated,
        // but it doesn't matter since they are also dynamically detected due DummyDevice just a bit slower..
#ifdef _WIN64
        static LPCTSTR sSubKeyData = D3D_DATA SEP D3D_TARGETX64 SEP D3D_TARGETDX9;
        static LPCTSTR sSubKeyOffset = D3D_OFFSET SEP D3D_TARGETX64 SEP D3D_TARGETDX9;

        if ( bFirstCall )
        {
            dxdata[0] = 0x000000000001ce3c;
            dxdata[1] = 0x0000000000002f30;
            dxdata[2] = 0x0000000000002f00;
        }

#else
        static LPCTSTR sSubKeyData = D3D_DATA SEP D3D_TARGETX86 SEP D3D_TARGETDX9;
        static LPCTSTR sSubKeyOffset = D3D_OFFSET SEP D3D_TARGETX86 SEP D3D_TARGETDX9;

        if ( bFirstCall )
        {
            dxdata[0] = 0x00006F19;
            dxdata[1] = 0x00006992;
            dxdata[2] = 0x00006969;
        }

#endif
        bFirstCall = false;

        void* pInterfaceClass = ( void* )( nRelativeBase + dxoffset );
        int nFunctioncount = 119;

        IDirect3DDevice9* pRet = NULL;

        // Calculate Offsets of IUnknown Interface VTable
        dxdata[0] += nModuleOffset;
        dxdata[1] += nModuleOffset;
        dxdata[2] += nModuleOffset;

        // Check EF_Query/Trial (should always be correct unless CDK decides its private again or faulty)
        if ( pTrialDevice )
        {
            if ( CheckForInterface<IUnknown>( &pTrialDevice, dxdata, dxdatalen, __uuidof( IDirect3DDevice9 ), nFunctioncount ) )
            {
                pRet = static_cast<IDirect3DDevice9*>( pTrialDevice );
            }
        }

        // Check saved/default memory offsets
        if ( !pRet )
        {
            if ( CheckForInterface<IUnknown>( pInterfaceClass, dxdata, dxdatalen, __uuidof( IDirect3DDevice9 ), nFunctioncount ) );

            {
                pRet = *static_cast<IDirect3DDevice9**>( pInterfaceClass );
            }
        }

        dxdata[0] -= nModuleOffset;
        dxdata[1] -= nModuleOffset;
        dxdata[2] -= nModuleOffset;

        // Offset already found
        if ( !pRet )
        {
            // Search for offset
            pRet = FindInterface<IDirect3DDevice9, IUnknown>(
                       nModuleOffset,
                       nRelativeBase,
                       nFunctioncount,
                       0xFFF,
                       sSubKeyData,
                       dxdata,
                       dxdatalen,
                       sSubKeyOffset,
                       dxoffset,
                       dxoffsetlen,
                       &GetD3D9DeviceData );
        }

        return pRet;
    }

    CD3DSystem9::CD3DSystem9()
    {
        m_bD3DHookInstalled = false;
        m_nTextureMode = HTM_NONE;
        m_pTempTex = NULL;

        // since 3.4.0 had EF_Query included again the DX9 search is now deprecated but still included for reference
        m_pDevice = FindD3D9Device( ( INT_PTR )gEnv->pRenderer, gEnv->pRenderer->EF_Query( EFQ_D3DDevice ) );

        if ( m_pDevice )
        {
            gPlugin->LogAlways( "DX9 device found" );
        }

        else
        {
            gPlugin->LogWarning( "DX9 device not found" );
        }

        gD3DSystem9 = this;
    }

    CD3DSystem9::~CD3DSystem9()
    {
        hookD3D( false );

        gD3DSystem9 = NULL;
    }

    void* CD3DSystem9::GetDevice()
    {
        return m_pDevice;
    }

    void CD3DSystem9::hookD3D( bool bHook )
    {
#if defined(D3D_DISABLE_HOOK)
        return;
#endif

        IDirect3DDevice9* pD3DDevice = static_cast<IDirect3DDevice9*>( m_pDevice );

        if ( pD3DDevice )
        {
            if ( bHook && m_bD3DHookInstalled )
            {
                rehookVT( pD3DDevice, IDirect3DDevice9, Present );
                rehookVT( pD3DDevice, IDirect3DDevice9, Reset );
                rehookVT( pD3DDevice, IDirect3DDevice9, BeginScene );
                rehookVT( pD3DDevice, IDirect3DDevice9, CreateTexture );
            }

            else if ( bHook )
            {
                hookVT( pD3DDevice, IDirect3DDevice9, Present );
                hookVT( pD3DDevice, IDirect3DDevice9, Reset );
                hookVT( pD3DDevice, IDirect3DDevice9, BeginScene );
                hookVT( pD3DDevice, IDirect3DDevice9, CreateTexture );

            }

            else if ( m_bD3DHookInstalled )
            {
                unhookVT( pD3DDevice, IDirect3DDevice9, Present, false );
                unhookVT( pD3DDevice, IDirect3DDevice9, Reset, false );
                unhookVT( pD3DDevice, IDirect3DDevice9, BeginScene, false );
                unhookVT( pD3DDevice, IDirect3DDevice9, CreateTexture, false );
            }

            m_bD3DHookInstalled = bHook;
        }
    }

    ITexture* CD3DSystem9::CreateTexture( void** pD3DTextureDst, int width, int height, int numMips, ETEX_Format eTF, int flags )
    {
        gD3DSystem9->m_nTextureMode = HTM_CREATE;

        int iTex = gEnv->pRenderer->SF_CreateTexture( width, height, numMips, NULL, eTF, flags ); // Create Texture
        *pD3DTextureDst = m_pTempTex;
        m_pTempTex = NULL;

        return gEnv->pRenderer->EF_GetTextureByID( iTex );
    }

    ITexture* CD3DSystem9::InjectTexture( void* pD3DTextureSrc, int nWidth, int nHeight, ETEX_Format eTF, int flags )
    {
        gD3DSystem9->m_nTextureMode = HTM_INJECT;
        m_pTempTex = pD3DTextureSrc;
        int iTex = gEnv->pRenderer->SF_CreateTexture( nWidth, nHeight, 0, NULL, eTF, flags ); // Create Dummytexture and replace it to trick CE3 into using our texture.
        m_pTempTex = NULL;

        return gEnv->pRenderer->EF_GetTextureByID( iTex );
    }

}