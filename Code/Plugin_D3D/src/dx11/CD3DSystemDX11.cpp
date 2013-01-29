/* D3D_Plugin - for licensing and copyright see license.txt */

#include <StdAfx.h>
#include "CD3DSystemDX11.h"
#include "d3d11hook.h"

#include <CPluginD3D.h>

//#pragma comment(lib, "dxerr.lib") // not needed atm

D3DPlugin::CD3DSystem11* D3DPlugin::gD3DSystem11 = NULL;

#undef METHOD
#undef INTERFACE

#define INTERFACE ID3D11DeviceContext

#define METHOD GSSetShader
GEN_HOOK_( void, __in_opt ID3D11GeometryShader* pShader, __in_ecount_opt( NumClassInstances )  ID3D11ClassInstance* const* ppClassInstances, UINT NumClassInstances )
{
    CALL_ORGINAL_( , pShader, ppClassInstances, NumClassInstances );

    //FIXME:
    // Note: this is more of a hack since access to swap chain not yet implemented
    // (multiple calls for each frame will come through the listener must handle it by registering a OnPostUpdate and setting a dirty flag)
    if ( pShader == NULL && ppClassInstances == NULL && NumClassInstances == 0 )
    {
        D3DPlugin::gD3DSystem11->OnPostBeginScene();
    }

    rehookVT( This, ID3D11DeviceContext, GSSetShader );
}
#undef METHOD

#define METHOD ClearRenderTargetView
GEN_HOOK_( void, __in  ID3D11RenderTargetView* pRenderTargetView, __in  const FLOAT ColorRGBA[ 4 ] )
{
    CALL_ORGINAL_( , pRenderTargetView, ColorRGBA );
    //D3DPlugin::gD3DSystem11->OnPostBeginScene();
    rehookVT( This, ID3D11DeviceContext, ClearRenderTargetView );
};
#undef METHOD

#define METHOD End
GEN_HOOK_(  void, __in  ID3D11Asynchronous* pAsync )
{
    CALL_ORGINAL_( , pAsync );

    //D3DPlugin::gD3DSystem11->OnPostBeginScene();
    rehookVT( This, ID3D11DeviceContext, ClearRenderTargetView );
    rehookVT( This, ID3D11DeviceContext, End );
}
#undef METHOD

#undef INTERFACE
#define INTERFACE ID3D11Device

#define METHOD CreateTexture2D
GEN_HOOK( __in const D3D11_TEXTURE2D_DESC* pDesc, __in_xcount_opt( pDesc->MipLevels * pDesc->ArraySize ) const D3D11_SUBRESOURCE_DATA* pInitialData, __out_opt ID3D11Texture2D** ppTexture2D )
{
    if ( D3DPlugin::gD3DSystem11->m_nTextureMode == HTM_INJECT )
    {
        goto INJECTTEXTURE;
    }

    CALL_ORGINAL( pDesc, pInitialData, ppTexture2D );

    if ( SUCCEEDED( hr ) )
    {
        switch ( D3DPlugin::gD3DSystem11->m_nTextureMode )
        {
            case HTM_INJECT:
INJECTTEXTURE:
                if ( pDesc && pDesc->ArraySize == 1 && ppTexture2D )
                {
                    ppTexture2D[0] = ( ID3D11Texture2D* )( D3DPlugin::gD3DSystem11->m_pTempTex );
                    ppTexture2D[0]->AddRef();
                }

                hr = S_OK;
                break;

            case HTM_CREATE:
                D3DPlugin::gD3DSystem11->m_pTempTex = NULL;

                if ( pDesc && pDesc->ArraySize == 1 && ppTexture2D )
                {
                    D3DPlugin::gD3DSystem11->m_pTempTex = ( void* )( ppTexture2D[0] );
                }

                break;
        }

        D3DPlugin::gD3DSystem11->m_nTextureMode = HTM_NONE;
    }

    return hr;
}
#undef METHOD

#define METHOD GetImmediateContext
GEN_HOOK_(  void, __out ID3D11DeviceContext** ppImmediateContext )
{
    CALL_ORGINAL_( , ppImmediateContext );

    rehookVT( *ppImmediateContext, ID3D11DeviceContext, ClearRenderTargetView );
    rehookVT( *ppImmediateContext, ID3D11DeviceContext, End );
    rehookVT( *ppImmediateContext, ID3D11DeviceContext, GSSetShader );
}
#undef METHOD
#undef INTERFACE

bool GetD3D11DeviceData( INT_PTR* unkdata, int nDatalen, void* pParam )
{
    bool bRet = false;

    return bRet;
}

namespace D3DPlugin
{
    ID3D11Device* FindD3D11Device( INT_PTR nRelativeBase, void* pTrialDevice )
    {
        INT_PTR nModuleOffset = ( INT_PTR )GetModuleHandle( TEXT( "d3d11.dll" ) );

        if ( !nModuleOffset )
        {
            gPlugin->LogWarning( "DX11 not active" );
            return NULL;
        }

        return ( ID3D11Device* )pTrialDevice;
        /*
            // Set Defaults
            static INT_PTR      dxoffset    = 0xC00 + sizeof(INT_PTR);
            static DWORD        dxoffsetlen = sizeof(dxoffset);

            static INT_PTR      dxdata[3];
            static DWORD        dxdatalen   = sizeof(dxdata);

            static bool bFirstCall = true;

        #ifdef _WIN64
            static LPCTSTR sSubKeyData = D3D_DATA SEP D3D_TARGETX64 SEP D3D_TARGETDX11;
            static LPCTSTR sSubKeyOffset = D3D_OFFSET SEP D3D_TARGETX64 SEP D3D_TARGETDX11;
            if(bFirstCall)
            {
                dxdata[0] = 0x000000000001ce3c;
                dxdata[1] = 0x0000000000002f30;
                dxdata[2] = 0x0000000000002f00;
            }
        #else
            static LPCTSTR sSubKeyData = D3D_DATA SEP D3D_TARGETX86 SEP D3D_TARGETDX11;
            static LPCTSTR sSubKeyOffset = D3D_OFFSET SEP D3D_TARGETX86 SEP D3D_TARGETDX11;
            if(bFirstCall)
            {
                dxdata[0] = 0x00006F19;
                dxdata[1] = 0x00006992;
                dxdata[2] = 0x00006969;
            }
        #endif
            bFirstCall = false;

            void* pInterfaceClass = (void*)(nRelativeBase + dxoffset);
            int nFunctioncount = 43;

            // Calculate Offsets of IUnknown Interface VTable
            dxdata[0] += nModuleOffset;
            dxdata[1] += nModuleOffset;
            dxdata[2] += nModuleOffset;
            bool bInterfaceOk = CheckForInterface<IUnknown>(pInterfaceClass, dxdata, dxdatalen, __uuidof(ID3D11Device), nFunctioncount);
            dxdata[0] -= nModuleOffset;
            dxdata[1] -= nModuleOffset;
            dxdata[2] -= nModuleOffset;

            // Offset already found
            if(bInterfaceOk)
                return *(ID3D11Device**)pInterfaceClass;

            // Search for offset
            return FindInterface<ID3D11Device, IUnknown>(
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
                &GetD3D11DeviceData);
        */
    }

    CD3DSystem11::CD3DSystem11()
    {
        m_bD3DHookInstalled = false;
        m_nTextureMode = HTM_NONE;
        m_pTempTex = NULL;

        // Note: Seach for DX11 isn't yet implemented, but would be easy
        // (since 3.4.0 was the first DX11 release and had EF_Query included again its now no problem anymore)
        m_pDevice = FindD3D11Device( ( INT_PTR )gEnv->pRenderer, gEnv->pRenderer->EF_Query( EFQ_D3DDevice ) );

        m_pDeviceCtx = NULL;

        if ( m_pDevice )
        {
            ID3D11Device*           pDevice = ( ID3D11Device* )m_pDevice;
            pDevice->GetImmediateContext( ( ID3D11DeviceContext** )&m_pDeviceCtx );

            if ( m_pDeviceCtx )
            {
                ( ( ID3D11DeviceContext* )( m_pDeviceCtx ) )->Release();
                gPlugin->LogAlways( "DX11 device context found" );
            }

            else
            {
                gPlugin->LogWarning( "DX11 device context not found" );
            }

            gPlugin->LogAlways( "DX11 device found" );
        }

        else
        {
            gPlugin->LogWarning( "DX11 device not found" );
        }

        /*
            ID3D11Debug*            pDbgDevice = NULL;
            HRESULT hr = pDevice->QueryInterface(__uuidof(ID3D11Debug), (void**)&pDbgDevice);

            IDXGISwapChain*         pSwapChain = NULL;
            if(pDbgDevice)
                hr = pDbgDevice->GetSwapChain(&pSwapChain);
        */
        gD3DSystem11 = this;
    }

    CD3DSystem11::~CD3DSystem11()
    {
        hookD3D( false );

        gD3DSystem11 = NULL;
    }

    void* CD3DSystem11::GetDevice()
    {
        return m_pDevice;
    }

    void CD3DSystem11::hookD3D( bool bHook )
    {
#if defined(D3D_DISABLE_HOOK)
        return;
#endif

        if ( m_pDevice && m_pDeviceCtx )
        {
            if ( bHook && m_bD3DHookInstalled )
            {
                rehookVT( m_pDevice, ID3D11Device, CreateTexture2D );
                rehookVT( m_pDevice, ID3D11Device, GetImmediateContext );
                rehookVT( m_pDeviceCtx, ID3D11DeviceContext, ClearRenderTargetView );
                rehookVT( m_pDeviceCtx, ID3D11DeviceContext, End );
                rehookVT( m_pDeviceCtx, ID3D11DeviceContext, GSSetShader );
            }

            else if ( bHook )
            {
                hookVT( m_pDevice, ID3D11Device, CreateTexture2D );
                hookVT( m_pDevice, ID3D11Device, GetImmediateContext );
                hookVT( m_pDeviceCtx, ID3D11DeviceContext, ClearRenderTargetView );
                hookVT( m_pDeviceCtx, ID3D11DeviceContext, End );
                hookVT( m_pDeviceCtx, ID3D11DeviceContext, GSSetShader );
            }

            else if ( m_bD3DHookInstalled )
            {
                unhookVT( m_pDevice, ID3D11Device, CreateTexture2D );
                unhookVT( m_pDevice, ID3D11Device, GetImmediateContext );
                unhookVT( m_pDeviceCtx, ID3D11DeviceContext, ClearRenderTargetView );
                unhookVT( m_pDeviceCtx, ID3D11DeviceContext, End );
                unhookVT( m_pDeviceCtx, ID3D11DeviceContext, GSSetShader );
            }

            m_bD3DHookInstalled = bHook;
        }
    }

    ITexture* CD3DSystem11::CreateTexture( void** pD3DTextureDst, int width, int height, int numMips, ETEX_Format eTF, int flags )
    {
        gD3DSystem11->m_nTextureMode = HTM_CREATE;
        int iTex = gEnv->pRenderer->SF_CreateTexture( width, height, numMips, NULL, eTF, flags ); // Create Texture
        *pD3DTextureDst = m_pTempTex;
        m_pTempTex = NULL;

        return gEnv->pRenderer->EF_GetTextureByID( iTex );
    }

    ITexture* CD3DSystem11::InjectTexture( void* pD3DTextureSrc, int nWidth, int nHeight, ETEX_Format eTF, int flags )
    {
        gD3DSystem11->m_nTextureMode = HTM_INJECT;
        m_pTempTex = pD3DTextureSrc;
        int iTex = gEnv->pRenderer->SF_CreateTexture( nWidth, nHeight, 1, NULL, eTF, flags ); // Create Dummytexture and replace it to trick CE3 into using our texture.
        m_pTempTex = NULL;

        return gEnv->pRenderer->EF_GetTextureByID( iTex );
    }

}