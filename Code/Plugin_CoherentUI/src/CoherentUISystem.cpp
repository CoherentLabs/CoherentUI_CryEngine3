#include "StdAfx.h"
#include "CoherentUISystem.h"

#include "CoherentInputEventListener.h"
#include "CoherentPlayerEventListener.h"
#include "CoherentSystemEventListener.h"
#include "CoherentViewListener.h"
#include "CPluginCoherentUI.h"
#include "FullscreenTriangleDrawer.h"

#include <Coherent/UI/UISystem.h>
#include <Coherent/UI/View.h>
#include <Coherent/UI/ViewInfo.h>
#include <Coherent/UI/ViewListener.h>
#include <Coherent/UI/License.h>

#include <d3d9.h>
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>


CoherentUIPlugin::CCoherentUISystem* gCoherentUISystem = NULL;
static bool s_NewFrame = false; // Needed for the DX11 hook mechanism of the D3D plugin

namespace CoherentUIPlugin
{

    enum ViewListenerID
    {
        VL_HUD = 0,
        VL_StaticObj,
        VL_BreakableObj,

        VL_Count
    };

    CCoherentUISystem::CCoherentUISystem( void )
        : m_pUISystem( NULL )
    {
    }


    CCoherentUISystem::~CCoherentUISystem( void )
    {
		if (gEnv->IsEditor())
		{	
			UnloadCoherentUIViews();
		}

        gEnv->pGameFramework->UnregisterListener( this );
        ILevelSystem* pLevelSystem = gEnv->pGameFramework->GetILevelSystem();

        if ( pLevelSystem )
        {
            pLevelSystem->RemoveListener( this );
        }

        m_ViewListeners.clear();

        if ( m_pUISystem )
        {
            m_pUISystem->Uninitialize();
        }
    }

    bool CCoherentUISystem::InitializeCoherentUI()
    {
        m_SystemEventsListener.reset( new CCoherentSystemEventListener( this ) );

        gEnv->pGameFramework->RegisterListener( this, "CCoherentUISystem", eFLPriority_HUD );
        gEnv->pGameFramework->GetILevelSystem()->AddListener( this );

        Coherent::UI::SystemSettings settings( L"\\Bin32", false, true, L"coui://cookies.dat", L"cui_cache", L"cui_app_cache", true, false, 9999 );
        m_pUISystem = InitializeUISystem( COHERENT_KEY, settings, m_SystemEventsListener.get(), Coherent::Logging::Debug );

        return m_pUISystem != NULL;
    }

    void CCoherentUISystem::OnSystemReady()
    {
        // You can start creating views from now on.
        // In this example the views are created when a level is loading.
    }

    void CCoherentUISystem::QueueCreateSurface( int width, int height, Coherent::UI::SurfaceResponse* pResponse )
    {
        // Called from main thread
        CryAutoCriticalSection lock( m_PendingCreateSurfaceTasksLock );

        CreateSurfaceTask task;
        task.Width = width;
        task.Height = height;
        task.Response = pResponse;
        m_PendingCreateSurfaceTasks.push_back( task );
    }

    void CCoherentUISystem::ReleaseSurface( Coherent::UI::CoherentHandle surface )
    {
        // Called from main thread
        CryAutoCriticalSection lock( m_SurfacesCollectionLock );

        HandleToSurfaceMap::iterator it = m_Surfaces.find( surface );

        if ( it != m_Surfaces.end() )
        {
            gEnv->pRenderer->RemoveTexture( it->second.CryTextureID );
            m_Surfaces.erase( it );
        }
    }

    void* CCoherentUISystem::GetNativeTextureFromSharedHandle( Coherent::UI::CoherentHandle surface )
    {
        // Called from render thread
        CryAutoCriticalSection lock( m_SurfacesCollectionLock );

        HandleToSurfaceMap::iterator it = m_Surfaces.find( surface );

        if ( it != m_Surfaces.end() )
        {
            return it->second.NativeTexture.pTexPtr;
        }

        return NULL;
    }

    IPlayerEventListener* CCoherentUISystem::GetPlayerEventListener() const
    {
        return m_PlayerEventListener.get();
    }

	bool CCoherentUISystem::RaycastClosestViewListenersGeometry(const Vec3& origin, const Vec3& dir, int& outX, int& outY, CCoherentViewListener*& pViewListener)
	{
		float minDist = std::numeric_limits<float>::max();
		int viewX;
		int viewY;
		int viewHitIndex = -1;

		for (int i = 0, count = (int)m_ViewListeners.size(); i < count; ++i)
		{
			CCoherentViewListener* pListener = m_ViewListeners[i].get();

			float t;
			int x, y;
			if (pListener->RaycastGeometry(origin, dir, t, x, y))
			{
				if (t < minDist)
				{
					minDist = t;
					viewX = x;
					viewY = y;

					viewHitIndex = i;
				}
			}
		}

		if (viewHitIndex != -1)
		{
			outX = viewX;
			outY = viewY;
			pViewListener = m_ViewListeners[viewHitIndex].get();

			return true;
		}

		return false;
	}

    // ID3DListener methods
    void CCoherentUISystem::OnPrePresent()
    {
        if ( !m_FullscreenDrawer )
        {
            m_FullscreenDrawer.reset( new CFullscreenTriangleDrawer() );
        }

        if ( m_ViewListeners.size() > VL_HUD && m_InputEventsListener && m_InputEventsListener->ShouldDrawCoherentUI() )
        {
            void* pHUDTexture = m_ViewListeners[VL_HUD]->GetTexture();

            if ( pHUDTexture )
            {
                m_FullscreenDrawer->Draw( pHUDTexture );
            }
        }
    }

    void CCoherentUISystem::OnPreReset()
    {
        for ( size_t i = 0; i < m_ViewListeners.size(); ++i )
        {
            if ( i == VL_HUD )
            {
                m_ViewListeners[i]->ReleaseTexture();
            }

            else
            {
                // Textures set on objects cannot be released immediately
                // as this will cause internal changes for some materials
                // and lead to a crash.
                m_ViewListeners[i]->SetTexture( NULL, 0 );
            }
        }

        m_FullscreenDrawer.reset(); // Clear resources for Fullscreen drawing
    }

    void CCoherentUISystem::OnPostReset()
    {
        m_FullscreenDrawer.reset( new CFullscreenTriangleDrawer() );

        if ( m_ViewListeners.size() > VL_HUD && m_ViewListeners[VL_HUD]->GetView() )
        {
            m_ViewListeners[VL_HUD]->GetView()->Resize( gEnv->pRenderer->GetWidth(), gEnv->pRenderer->GetHeight() );
        }
    }

    void CCoherentUISystem::OnPostBeginScene()
    {
        if ( !s_NewFrame )
        {
            return;
        }

        s_NewFrame = false;
		ExecutePendingCreateSurfaceTasks();

        if ( m_pUISystem )
        {
            // Note that calling any ViewListener's OnDraw will be called in this stack.
            // Since we're in the render thread, it's safe to do drawing in the OnDraw method.
            m_pUISystem->FetchSurfaces();
        }
    }

    // IGameFramework methods
    void CCoherentUISystem::OnPostUpdate( float fDeltaTime )
    {
        s_NewFrame = true;
        SetTexturesForListeners();

        if ( m_pUISystem )
        {
            UpdateHUD();
            m_pUISystem->Update();
        }
    }

    void CCoherentUISystem::OnActionEvent( const SActionEvent& event )
    {
		if (gEnv->IsEditor() && event.m_event == eAE_inGame)
		{
			LoadCoherentUIViews();
		}

        // Since CPlayer is not an iterface class, we cannot register
        // a listener from within the Coherent UI DLL;
        // The registration is done in CGame::OnActionEvent, when the
        // player enters a level.
    }

    // ILevelSystemListener methods
    void CCoherentUISystem::OnLoadingStart( ILevelInfo* pLevel )
    {
		// The editor executes OnLoadingStart multiple times without
		// corresponding OnUnloadComplete calls;
		// We will create the views when the player enters Game mode
		if (!gEnv->IsEditor())
		{	
			LoadCoherentUIViews();
		}        
    }

    void CCoherentUISystem::OnUnloadComplete( ILevel* pLevel )
    {
		if (!gEnv->IsEditor())
		{	
			UnloadCoherentUIViews();
		}
    }

    // Helper methods

	void CCoherentUISystem::LoadCoherentUIViews()
	{
		m_ViewListeners.resize( VL_Count );
        m_ViewListeners[VL_HUD].reset( new CCoherentViewListener() );

        m_ViewListeners[VL_StaticObj].reset( new CCoherentViewListener() );
		m_ViewListeners[VL_StaticObj]->SetCollisionMesh("curve.obj");
		m_ViewListeners[VL_StaticObj]->SetEngineObjectAndMaterialNames("CoherentStaticEntity", "materials/placeholder_1024_576");

        m_ViewListeners[VL_BreakableObj].reset( new CCoherentViewListener() );
		m_ViewListeners[VL_BreakableObj]->SetEngineObjectAndMaterialNames("CoherentBreakableEntity", "materials/placeholder_1024_768");

        m_InputEventsListener.reset( new CCoherentInputEventListener() );
        gEnv->pInput->AddEventListener( m_InputEventsListener.get() );

        m_PlayerEventListener.reset( new CCoherentPlayerEventListener( m_ViewListeners[VL_HUD].get() ) );

        CreateViewsForListeners();
	}

	void CCoherentUISystem::UnloadCoherentUIViews()
	{
		gEnv->pInput->RemoveEventListener( m_InputEventsListener.get() );
        m_InputEventsListener.reset();
        m_PlayerEventListener.reset();

        m_ViewListeners.clear();
	}

    void CCoherentUISystem::UpdateHUD()
    {
        CCoherentViewListener* pHUDListener = NULL;
        pHUDListener = ( m_ViewListeners.size() > VL_HUD ? m_ViewListeners[VL_HUD].get() : NULL );

        if ( pHUDListener )
        {
            CCamera& camera = gEnv->pSystem->GetViewCamera();
            Vec3 viewDir = camera.GetViewdir();
            float rotation = cry_atan2f( viewDir.y, viewDir.x ) * 180.0f / 3.14159f;
            // Adjust rotation so it is the same as in the game
            rotation = -rotation - 135.0f;
            Coherent::UI::View* pView = pHUDListener->GetView();

            if ( pView && pHUDListener->IsReadyForBindings() )
            {
                pView->TriggerEvent( "SetAbsoluteCompassRotation", rotation );
            }
        }
    }

    void CCoherentUISystem::ExecutePendingCreateSurfaceTasks()
    {
        // Called from render thread
        CryAutoCriticalSection lock( m_PendingCreateSurfaceTasksLock );

        IDirect3DDevice9* device = static_cast<IDirect3DDevice9*>( gD3DDevice );
        ERenderType renderType = gEnv->pRenderer->GetRenderType();

        for ( size_t i = 0, count = m_PendingCreateSurfaceTasks.size(); i < count; ++i )
        {
            const CreateSurfaceTask& task = m_PendingCreateSurfaceTasks[i];

            Coherent::UI::CoherentHandle createdHandle( 0 );
            TexturePair texturePair;

            if ( renderType == eRT_DX9 )
            {
                createdHandle = CreateSharedTextureDX9( task, &texturePair );
            }

            else if ( renderType == eRT_DX11 )
            {
                createdHandle = CreateSharedTextureDX11( task, &texturePair );
            }

            if ( !( createdHandle == Coherent::UI::CoherentHandle( 0 ) ) )
            {
                CryAutoCriticalSection lock( m_SurfacesCollectionLock );

                m_Surfaces.insert( std::make_pair( createdHandle, texturePair ) );
            }

            task.Response->Signal( createdHandle );
        }

        m_PendingCreateSurfaceTasks.clear();
    }

    Coherent::UI::CoherentHandle CCoherentUISystem::CreateSharedTextureDX9( const CreateSurfaceTask& task, TexturePair* outTexturePair )
    {
        IDirect3DTexture9* pD3DTex = nullptr;
        // Create a shared texture
        HANDLE result = 0;
        IDirect3DDevice9* pDevice = static_cast<IDirect3DDevice9*>( gD3DDevice );
        HRESULT hr = pDevice->CreateTexture( task.Width, task.Height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pD3DTex, &result );

        if ( FAILED( hr ) )
        {
            CryLogAlways( "Unable to create shared texture with DirectX9 renderer!" );
        }

        ITexture* pCryTex = gD3DSystem->InjectTexture( pD3DTex, task.Width, task.Height, eTF_A8R8G8B8, 0 );
        // The native texture has one more reference after InjectTexture

        if ( outTexturePair )
        {
            outTexturePair->CryTextureID = pCryTex->GetTextureID();
            outTexturePair->NativeTexture.pTexDX9 = pD3DTex;
        }

        SAFE_RELEASE( pD3DTex );

        return Coherent::UI::CoherentHandle( result );
    }

    Coherent::UI::CoherentHandle CCoherentUISystem::CreateSharedTextureDX11( const CreateSurfaceTask& task, TexturePair* outTexturePair )
    {
        // Create shared texture
        D3D11_TEXTURE2D_DESC desc;
        memset( &desc, 0, sizeof( desc ) );
        desc.Width = task.Width;
        desc.Height = task.Height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
        desc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;

        ID3D11Device* pDevice = static_cast<ID3D11Device*>( gD3DDevice );

        ID3D11Texture2D* pD3DTex = NULL;
        HRESULT hr = pDevice->CreateTexture2D( &desc, NULL, &pD3DTex );

        if ( FAILED( hr ) )
        {
            return Coherent::UI::CoherentHandle( 0 );
        }

        IDXGIResource* pTempResource = NULL;
        hr = pD3DTex->QueryInterface( __uuidof( IDXGIResource ), ( void** )&pTempResource );

        if ( FAILED( hr ) )
        {
            SAFE_RELEASE( pD3DTex );
            return Coherent::UI::CoherentHandle( 0 );
        }

        HANDLE result;
        hr = pTempResource->GetSharedHandle( &result );
        pTempResource->Release();

        if ( FAILED( hr ) )
        {
            SAFE_RELEASE( pD3DTex );
            return Coherent::UI::CoherentHandle( 0 );
        }

        ITexture* pCryTex = gD3DSystem->InjectTexture( pD3DTex, task.Width, task.Height, eTF_A8R8G8B8, 0 );
        // The native texture has one more reference after InjectTexture

        if ( outTexturePair )
        {
            outTexturePair->CryTextureID = pCryTex->GetTextureID();
            outTexturePair->NativeTexture.pTexDX11 = pD3DTex;
        }

        SAFE_RELEASE( pD3DTex );

        return Coherent::UI::CoherentHandle( result );
    }

    void CCoherentUISystem::CreateViewsForListeners()
    {
		CRY_ASSERT( m_ViewListeners.size() == VL_Count && m_pUISystem );

#if defined(COHERENT_UI_LIMITED_VERSION)
#error The Starter version of Coherent UI supports the creation of a single view only! \
	To avoid warnings and unexpected black objects, comment all but one of the calls \
	to "CreateView" in this function. When done remove this error directive to compile successfully.
#endif

        Coherent::UI::ViewInfo info;
        info.Width = gEnv->pRenderer->GetWidth();
        info.Height = gEnv->pRenderer->GetHeight();
        info.UsesSharedMemory = true;
        info.IsTransparent = true;
        if (m_ViewListeners[VL_HUD]->GetView() == nullptr)
        {
            m_pUISystem->CreateView(info, L"coui://Bin32/TestPages/hud/hud.html", m_ViewListeners[VL_HUD].get());
        }

        info.Width = 1024;
        info.Height = 576;
        info.IsTransparent = false;

        if ( m_ViewListeners[VL_StaticObj]->GetView() == nullptr )
        {
            m_pUISystem->CreateView( info, L"http://www.google.co.uk", m_ViewListeners[VL_StaticObj].get() );
        }

        info.Width = 1024;
        info.Height = 768;

        if ( m_ViewListeners[VL_BreakableObj]->GetView() == nullptr )
        {
            m_pUISystem->CreateView( info, L"http://neography.com/experiment/circles/solarsystem/", m_ViewListeners[VL_BreakableObj].get() );
        }
    }

    void CCoherentUISystem::SetTexturesForListeners()
    {
        CCoherentViewListener* pListener = NULL;

        // Create HUD texture
        pListener = (m_ViewListeners.size() > VL_HUD ? m_ViewListeners[VL_HUD].get() : NULL);
        if (pListener && pListener->GetTexture() == NULL)
        {
            void* pD3DTextureDst = NULL;
            ITexture* pCryTex = gD3DSystem->CreateTexture(
                (void**)&pD3DTextureDst,
                gEnv->pRenderer->GetWidth(),
                gEnv->pRenderer->GetHeight(),
                1,
                eTF_A8R8G8B8,
                FT_USAGE_DYNAMIC
                );

            pListener->SetTexture(pD3DTextureDst, pCryTex->GetTextureID());
        }

        // Create textures for entities
        pListener = ( m_ViewListeners.size() > VL_StaticObj ?  m_ViewListeners[VL_StaticObj].get() : NULL );

        if ( pListener && pListener->GetTexture() == NULL )
        {
            ChangeEntityDiffuseTextureForMaterial( pListener, pListener->GetEngineObjectName(), pListener->GetOverriddenMaterialName() );
        }

        pListener = ( m_ViewListeners.size() > VL_BreakableObj ? m_ViewListeners[VL_BreakableObj].get() : NULL );

        if ( pListener && pListener->GetTexture() == NULL )
        {
            ChangeEntityDiffuseTextureForMaterial( pListener, pListener->GetEngineObjectName(), pListener->GetOverriddenMaterialName() );
        }
    }

    void CCoherentUISystem::ChangeEntityDiffuseTextureForMaterial( CCoherentViewListener* pViewListener, const char* entityName, const char* materialName )
    {
        IMaterial* pMaterial = gEnv->p3DEngine->GetMaterialManager()->FindMaterial( materialName );
        IEntity* pEntity = gEnv->pEntitySystem->FindEntityByName( entityName );

        if ( pEntity && !pMaterial )
        {
            pMaterial = pEntity->GetMaterial();
        }

        if ( pMaterial )
        {
            STexSamplerRT& sampler = pMaterial->GetShaderItem().m_pShaderResources->GetTexture( EFTT_DIFFUSE )->m_Sampler;

            // Create a new texture and scrap the old one
            void* pD3DTextureDst = NULL;
            ITexture* pCryTex = gD3DSystem->CreateTexture(
                                    ( void** )&pD3DTextureDst,
                                    sampler.m_pITex->GetWidth(),
                                    sampler.m_pITex->GetHeight(),
                                    1,
                                    eTF_A8R8G8B8,
                                    FT_USAGE_DYNAMIC
                                );

            int oldTextureID = sampler.m_pITex->GetTextureID();
            gEnv->pRenderer->RemoveTexture( oldTextureID );
            sampler.m_pITex = pCryTex;

            pViewListener->SetTexture( pD3DTextureDst, pCryTex->GetTextureID() );
        }
    }

}
