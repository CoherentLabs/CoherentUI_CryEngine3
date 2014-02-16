#include "StdAfx.h"
#include "CoherentUISystem.h"

#include "CoherentInputEventListener.h"
#include "CoherentPlayerEventListener.h"
#include "CoherentSystemEventListener.h"
#include "CoherentViewListener.h"
#include "CoherentHUDViewListener.h"
#include "CPluginCoherentUI.h"
#include "FullscreenTriangleDrawer.h"
#include "ViewConfig.h"

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

namespace CoherentUIPlugin
{
    CCoherentUISystem::CCoherentUISystem( void )
        : m_pUISystem( NULL )
    {
        m_PlayerEventListener.reset( new CCoherentPlayerEventListener() );
    }


    CCoherentUISystem::~CCoherentUISystem( void )
    {
        // unregister listeners
        if ( m_pUISystem && gEnv )
        {
            if ( gEnv->pGame && gEnv->pGame->GetIGameFramework() )
            {
                gEnv->pGame->GetIGameFramework()->UnregisterListener( this );
            }

            if ( gEnv->pSystem && m_InputEventsListener )
            {
                gEnv->pInput->RemoveEventListener( m_InputEventsListener.get() );
            }
        }

        // delete view listeners
        for ( View::const_iterator iter = m_Views.begin(); iter != m_Views.end(); ++iter )
        {
            CCoherentViewListener* pListener = iter->first;
            delete pListener;
        }

        m_Views.clear();

        m_HudViewListener.reset();

        if ( m_pUISystem )
        {
            m_pUISystem->Uninitialize();
        }
    }

    bool CCoherentUISystem::InitializeCoherentUI()
    {
        // register listeners
        m_SystemEventsListener.reset( new CCoherentSystemEventListener( this ) );
        m_InputEventsListener.reset( new CCoherentInputEventListener() );

        if ( gEnv->pGame && gEnv->pGame->GetIGameFramework() )
        {
            gEnv->pGame->GetIGameFramework()->RegisterListener( this, "CCoherentUISystem", FRAMEWORKLISTENERPRIORITY_HUD );
        }
        if ( gEnv->pInput )
        {
            gEnv->pInput->AddEventListener( m_InputEventsListener.get() );
        }

        std::string sPath( gPluginManager->GetPluginDirectory( PLUGIN_NAME ) );
        sPath += "\\host";
        std::wstring sPathW;
        sPathW.assign( sPath.begin(), sPath.end() );

        Coherent::UI::SystemSettings settings( sPathW.c_str(), false, true, L"coui://cookies.dat",
            L"cui_cache", L"cui_app_cache", true, false, 9999, false, NULL );
        m_pUISystem = InitializeUISystem( COHERENT_UI_SDK_VER, COHERENT_KEY, settings,
            m_SystemEventsListener.get(), Coherent::Logging::Debug, NULL, &m_PakFileHandler );

        return m_pUISystem != NULL;
    }

    bool CCoherentUISystem::IsReady()
    {
        if ( m_SystemEventsListener )
        {
            return m_SystemEventsListener.get()->IsReady();
        }
        return false;
    }

    void CCoherentUISystem::OnSystemReady()
    {
        // You can start creating views from now on.
        // In this example the views are created when a level is loading.
    }

    void CCoherentUISystem::OnError( const Coherent::UI::SystemError& error )
    {
        CryLogAlways( error.Error );
    }

    CCoherentViewListener* CCoherentUISystem::GetViewListener( int id )
    {
        if ( m_HudViewListener )
        {
            Coherent::UI::View* pHudView = m_HudViewListener->GetView();
            if ( pHudView != nullptr && pHudView->GetId() == id )
            {
                return m_HudViewListener.get();
            }
        }

        for ( View::const_iterator iter = m_Views.begin(); iter != m_Views.end(); ++iter )
        {
            Coherent::UI::View* pView = iter->first->GetView();
            if ( pView->GetId() == id )
            {
                return iter->first;
            }
        }
        return NULL;
    }

    CCoherentViewListener* CCoherentUISystem::CreateView( ViewConfig* pConfig )
    {
        CCoherentViewListener* pViewListener = new CCoherentViewListener();

        if ( !pConfig->CollisionMesh.empty() )
        {
            pViewListener->SetCollisionMesh( pConfig->CollisionMesh.c_str() );
        }

        string entityName;

        if ( pConfig->Entity )
        {
            entityName = pConfig->Entity->GetName();
        }

        pViewListener->SetEngineObjectAndMaterialNames( entityName, pConfig->MaterialName.c_str() );

        m_pUISystem->CreateView( pConfig->ViewInfo, pConfig->Url.c_str(), pViewListener );

        m_Views.insert( View::value_type( pViewListener, pConfig ) );
        return pViewListener;
    }

    void CCoherentUISystem::DeleteView( CCoherentViewListener* pViewListener )
    {
        View::iterator it = m_Views.find( pViewListener );

        if ( it != m_Views.end() )
        {
            m_Views.erase( it );
        }

        delete pViewListener;
    }

    CCoherentViewListener* CCoherentUISystem::CreateHUDView( std::wstring path )
    {
        m_HudViewListener.reset( new CCoherentHUDViewListener() );

        CRY_ASSERT( m_pUISystem );

        Coherent::UI::ViewInfo info;
        info.Width = gEnv->pRenderer->GetWidth();
        info.Height = gEnv->pRenderer->GetHeight();
        info.UsesSharedMemory = false;
        info.IsTransparent = true;
        info.SupportClickThrough = true;

        if ( m_HudViewListener->GetView() == nullptr )
        {
            m_pUISystem->CreateView( info, path.c_str(), m_HudViewListener.get() );
        }

        m_PlayerEventListener.get()->AddViewListener( m_HudViewListener.get() );
        return m_HudViewListener.get();
    }

    void CCoherentUISystem::DeleteHUDView()
    {
        m_PlayerEventListener.get()->RemoveViewListener( m_HudViewListener.get() );
        m_HudViewListener.reset();
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

    bool CCoherentUISystem::RaycastClosestViewListenersGeometry( const Vec3& origin, const Vec3& dir, int& outX, int& outY, CCoherentViewListener*& pViewListener )
    {
        // check hud view
        if ( m_HudViewListener )
        {
            float t;
            int x, y;
            if ( m_HudViewListener->RaycastGeometry( origin, dir, t, x, y ) )
            {
                outX = x;
                outY = y;
                pViewListener = m_HudViewListener.get();

                return true;
            }
        }
        

        // check other views
        float minDist = std::numeric_limits<float>::max();
        int viewX;
        int viewY;
        CCoherentViewListener* pHitListener = NULL;

        for ( View::const_iterator iter = m_Views.begin(); iter != m_Views.end(); ++iter )
        {
            CCoherentViewListener* pListener = iter->first;

            float t;
            int x, y;

            if ( pListener->RaycastGeometry( origin, dir, t, x, y ) )
            {
                if ( t < minDist )
                {
                    minDist = t;
                    viewX = x;
                    viewY = y;

                    pHitListener = pListener;
                }
            }
        }

        if ( pHitListener != NULL )
        {
            outX = viewX;
            outY = viewY;
            pViewListener = pHitListener;

            return true;
        }

        return false;
    }

    void CCoherentUISystem::SetCUIInput( bool enabled )
    {
        // set input to coherent ui
        if ( m_InputEventsListener )
        {
            m_InputEventsListener->SetPlayerInput( !enabled );
        }
    }

    // ID3DListener methods
    void CCoherentUISystem::OnPrePresent()
    {
        if ( !m_FullscreenDrawer )
        {
            m_FullscreenDrawer.reset( new CFullscreenTriangleDrawer() );
        }

        if ( m_HudViewListener && m_InputEventsListener && m_InputEventsListener->ShouldDrawCoherentUI() )
        {
            void* pHUDTexture = m_HudViewListener->GetTexture();

            if ( pHUDTexture )
            {
                m_FullscreenDrawer->Draw( pHUDTexture );
            }
        }
    }

    void CCoherentUISystem::OnPreReset()
    {
        if ( m_HudViewListener )
        {
            m_HudViewListener->ReleaseTexture();
        }

        for ( View::const_iterator iter = m_Views.begin(); iter != m_Views.end(); ++iter )
        {
            // Textures set on objects cannot be released immediately
            // as this will cause internal changes for some materials
            // and lead to a crash.
            iter->first->SetTexture( NULL, 0 );
        }

        m_FullscreenDrawer.reset(); // Clear resources for Fullscreen drawing
    }

    void CCoherentUISystem::OnPostReset()
    {
        m_FullscreenDrawer.reset( new CFullscreenTriangleDrawer() );

        if ( m_HudViewListener && m_HudViewListener->GetView() )
        {
            m_HudViewListener->GetView()->Resize( gEnv->pRenderer->GetWidth(), gEnv->pRenderer->GetHeight() );
        }
    }

    void CCoherentUISystem::OnPostBeginScene()
    {
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
        SetTexturesForListeners();

        if ( m_pUISystem )
        {
            UpdateHUD();
            m_pUISystem->Update();
        }
    }

    void CCoherentUISystem::OnActionEvent( const SActionEvent& event )
    {
    }

    // Helper methods

    void CCoherentUISystem::UpdateHUD()
    {
        static Vec3 lastPosition = Vec3Constants<Vec3::value_type>::fVec3_Zero;
        static float lastRotation = 0;

        CCoherentViewListener* pHUDListener = NULL;
        pHUDListener = ( m_HudViewListener ? m_HudViewListener.get() : NULL );

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
                if ( rotation != lastRotation )
                {
                    pView->TriggerEvent( "SetAbsoluteCompassRotation", rotation );
                    // Adjust the rotation for the map, too...
                    pView->TriggerEvent( "SetPlayerRotationOnMap", rotation - 45.0f );

                    lastRotation = rotation;
                }

                Vec3 cameraPosition = camera.GetPosition();

                if ( ( cameraPosition - lastPosition ).GetLengthSquared() > VEC_EPSILON )
                {
                    pView->TriggerEvent( "SetPlayerPositionOnMap", cameraPosition.x, cameraPosition.y );

                    lastPosition = cameraPosition;
                }
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
        // The shared texture's format for DX11 must be DXGI_FORMAT_B8G8R8A8_UNORM.
        // There is no corresponding ETEX_Format and after injecting the created
        // texture, COM errors occur.
        // TODO: Find a way to fool CryEngine into accepting a DXGI_FORMAT_B8G8R8A8_UNORM texture.

        // Create shared texture
        D3D11_TEXTURE2D_DESC desc;
        memset( &desc, 0, sizeof( desc ) );
        desc.Width = task.Width;
        desc.Height = task.Height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
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

        // TODO: Test code; this is valid for HUD textures only since CE3 does not need to have an internal texture
        ITexture* pCryTex = NULL; // gD3DSystem->InjectTexture(pD3DTex, task.Width, task.Height, eTF_A8R8G8B8, 0);
        // The native texture has one more reference after InjectTexture

        if ( outTexturePair )
        {
            outTexturePair->CryTextureID = pCryTex ? pCryTex->GetTextureID() : -1;
            outTexturePair->NativeTexture.pTexDX11 = pD3DTex;
        }

        // TODO: Test code; if InjectTexture was called, we should release 1 reference
        //SAFE_RELEASE( pD3DTex );

        return Coherent::UI::CoherentHandle( result );
    }

    void CCoherentUISystem::SetTexturesForListeners()
    {
        CCoherentViewListener* pListener = NULL;

        // Create HUD texture
        pListener = ( m_HudViewListener ? m_HudViewListener.get() : NULL );

        if ( pListener && pListener->GetTexture() == NULL )
        {
            void* pD3DTextureDst = NULL;
            // TODO: Test code; create a texture without letting CE3 know so we can use a custom format
            /*
            ITexture* pCryTex = gD3DSystem->CreateTexture(
                                    &pD3DTextureDst,
                                    gEnv->pRenderer->GetWidth(),
                                    gEnv->pRenderer->GetHeight(),
                                    1,
                                    eTF_A8R8G8B8,
                                    FT_USAGE_DYNAMIC
                                );

            pListener->SetTexture( pD3DTextureDst, pCryTex->GetTextureID() );
            */
            D3D11_TEXTURE2D_DESC desc;
            ZeroMemory(&desc, sizeof(desc));
            desc.Width = gEnv->pRenderer->GetWidth();
            desc.Height = gEnv->pRenderer->GetHeight();
            desc.ArraySize = 1;
            desc.MipLevels = 1;
            desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
            desc.SampleDesc.Count = 1;
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

            ID3D11Device* pDevice = static_cast<ID3D11Device*>(gD3DDevice);
            HRESULT r = pDevice->CreateTexture2D(&desc, NULL, (ID3D11Texture2D**)&pD3DTextureDst);

            pListener->SetTexture(pD3DTextureDst, -1);

        }

        // Create textures for entities
        for ( View::const_iterator iter = m_Views.begin(); iter != m_Views.end(); ++iter )
        {
            pListener = iter->first;

            if ( pListener && pListener->GetTexture() == NULL )
            {
                ChangeEntityDiffuseTextureForMaterial( pListener, pListener->GetEngineObjectName(), pListener->GetOverriddenMaterialName() );
            }
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
                                    &pD3DTextureDst,
                                    sampler.m_pITex->GetWidth(),
                                    sampler.m_pITex->GetHeight(),
                                    1,
                                    eTF_A8R8G8B8,
                                    FT_USAGE_DYNAMIC
                                );

            int oldTextureID = sampler.m_pITex->GetTextureID();
            gEnv->pRenderer->RemoveTexture( oldTextureID );
            sampler.m_pITex = pCryTex;
            pCryTex->AddRef();

            pViewListener->SetTexture( pD3DTextureDst, pCryTex->GetTextureID() );
        }
    }
}
