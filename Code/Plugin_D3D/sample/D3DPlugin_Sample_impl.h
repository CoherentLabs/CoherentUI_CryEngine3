// This sample will make your rendering window green, but you are free
// to do something more meaningfull ;)

#include <IPluginManager.h>
#include <IPluginD3D.h>
#include <d3d9.h>
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>

class CMyD3D
    : private D3DPlugin::ID3DEventListener
{
protected:
	bool bDX9;

	union device    // Declare union type
	{
		void* ptr;
		IDirect3DDevice9*	dx9;
		//ID3D11Device*		dx11;
	} m_pDevice;

    PluginManager::IPluginBase* m_pD3DPlugin;
    D3DPlugin::IPluginD3D* m_pD3DSystem;
	IDirect3DStateBlock9* m_pStateBlock;

	void OnPrePresent() {
		if(bDX9)
		{
			// Save current D3D State [Optional: if you don't modify it]
			m_pStateBlock->Capture();

			// Apply custom rendering
			// Make window green (TODO: Remove this and add your own stuff)
			//m_pDevice.dx9->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(200,255,0), 1.0f, 0);

			// Restore D3D State for next frame
			m_pStateBlock->Apply();
		}
	};

	void OnPostPresent() {};
	void OnPreReset() {
		SAFE_RELEASE(m_pStateBlock);
	};
	void OnPostReset() {};
	void OnPostBeginScene() {};

public:
	CMyD3D() {
		m_pDevice.ptr	= NULL;
		m_pStateBlock	= NULL;

        m_pD3DPlugin = gPluginManager->GetPluginByName("D3D");
        m_pD3DSystem = static_cast<D3DPlugin::IPluginD3D*>(m_pD3DPlugin ? m_pD3DPlugin->GetConcreteInterface() : NULL);

		if(m_pD3DSystem)
		{
			// Initialize the device
			m_pDevice.ptr = m_pD3DSystem->GetDevice();
			bDX9 = true;

			if(bDX9 && m_pDevice.ptr)
			{ // Create D3D State [Optional: if you don't modify it]
				m_pDevice.dx9->CreateStateBlock(D3DSBT_ALL, &m_pStateBlock);
			}

			// the listeners will be called renderer thread.
			m_pD3DSystem->RegisterListener(this);
		}
	}

	~CMyD3D() {
		if(m_pD3DSystem)
			m_pD3DSystem->UnregisterListener(this);

		SAFE_RELEASE(m_pStateBlock);
	}
};
