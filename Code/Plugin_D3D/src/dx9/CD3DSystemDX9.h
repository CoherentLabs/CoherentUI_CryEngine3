/* D3D_Plugin - for licensing and copyright see license.txt */

#pragma once

#include <CPluginD3D.h>

namespace D3DPlugin
{
    class CD3DSystem9 :
        public IPluginD3DEx,
        private ID3DEventListener
    {
        private:
            bool m_bD3DHookInstalled;
            std::vector<ID3DEventListener*> m_vecQueue;

            void hookD3D( bool bHook );
        public:
            int m_nTextureMode;
            void* m_pTempTex;
            void* m_pDevice;

            CD3DSystem9();
            virtual ~CD3DSystem9();

            void Release()
            {
                delete this;
            }

            PluginManager::IPluginBase* GetBase()
            {
                return gPlugin->GetBase();
            };

            void ActivateEventDispatcher( bool bActivate )
            {
                hookD3D( bActivate );
            };

            void* GetDevice();
            eD3DType GetType()
            {
                return m_pDevice ? D3D_DX9 : D3D_NONE;
            };

            void* GetSwapChain()
            {
                return NULL;
            }; // DX11 only
            void* GetDeviceContext()
            {
                return NULL;
            }; // DX11 only

            ITexture* CreateTexture( void** pD3DTextureDst, int width, int height, int numMips, ETEX_Format eTF, int flags );
            ITexture* InjectTexture( void* pD3DTextureSrc, int nWidth, int nHeight, ETEX_Format eTF, int flags );

        public:
            DECLARE_REGISTER_LISTENER( m_vecQueue );
            DECLARE_UNREGISTER_LISTENER( m_vecQueue );
            DECLARE_BROADCAST_EVENT( m_vecQueue, OnPrePresent );
            DECLARE_BROADCAST_EVENT( m_vecQueue, OnPostPresent );
            DECLARE_BROADCAST_EVENT( m_vecQueue, OnPreReset );
            DECLARE_BROADCAST_EVENT( m_vecQueue, OnPostReset );
            DECLARE_BROADCAST_EVENT( m_vecQueue, OnPostBeginScene );
    };

    extern CD3DSystem9* gD3DSystem9;
};