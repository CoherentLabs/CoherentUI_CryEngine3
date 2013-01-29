/* D3D_Plugin - for licensing and copyright see license.txt */

#include <IPluginBase.h>

#pragma once

/**
* @brief D3D Plugin Namespace
*/
namespace D3DPlugin
{
    enum eD3DType
    {
        D3D_NONE,
        D3D_DX9,
        D3D_DX11,
    };

    struct ID3DEventListener
    {
        virtual void OnPrePresent() = 0;
        virtual void OnPostPresent() = 0;
        virtual void OnPreReset() = 0;
        virtual void OnPostReset() = 0;
        virtual void OnPostBeginScene() = 0;
    };

    /**
    * @brief plugin D3D concrete interface
    */
    struct IPluginD3D
    {
        /**
        * @brief Get Plugin base interface
        */
        virtual PluginManager::IPluginBase* GetBase() = 0;

        virtual void ActivateEventDispatcher( bool bActivate = true ) = 0;
        virtual void RegisterListener( ID3DEventListener* item ) = 0;
        virtual void UnregisterListener( ID3DEventListener* item ) = 0;

        virtual void* GetSwapChain() = 0; // DX11 only
        virtual void* GetDeviceContext() = 0; // DX11 only
        virtual void* GetDevice() = 0;

        virtual eD3DType GetType() = 0;

        virtual ITexture* CreateTexture( void** pD3DTextureDst, int width, int height, int numMips, ETEX_Format eTF, int flags ) = 0;
        virtual ITexture* InjectTexture( void* pD3DTextureSrc, int nWidth, int nHeight, ETEX_Format eTF, int flags ) = 0;
    };
};