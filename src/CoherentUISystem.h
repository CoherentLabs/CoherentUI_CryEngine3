#pragma once

#include <IPluginD3D.h>
#include <IGameFramework.h>
#include <ILevelSystem.h>
#include <Coherent/UI/CoherentTypes.h>
#include <Coherent/UI/UISystem.h>

#include <PakFileHandler.h>

namespace Coherent
{
    namespace UI
    {
        class UISystem;
        class SurfaceResponse;
        struct ViewInfo;
    }
}

struct IDirect3DTexture9;
struct ID3D11Texture2D;
struct IPlayerEventListener;
struct ViewConfig;

namespace CoherentUIPlugin
{

    class CCoherentInputEventListener;
    class CCoherentPlayerEventListener;
    class CCoherentSystemEventListener;
    class CCoherentViewListener;
    class CFullscreenTriangleDrawer;
    struct ViewConfig;

    class CCoherentUISystem :
        public D3DPlugin::ID3DEventListener,
        public IGameFrameworkListener
    {
        public:
            CCoherentUISystem( void );
            ~CCoherentUISystem( void );

            bool InitializeCoherentUI();
            void OnSystemReady();
            void OnError( const Coherent::UI::SystemError& error );

            CCoherentViewListener* CreateView( ViewConfig* pConfig );
            void DeleteView( CCoherentViewListener* pViewListener );
            CCoherentViewListener* CreateHUDView( std::wstring path );
            void DeleteHUDView();

            Coherent::UI::View* CCoherentUISystem::GetView( int id );

            void QueueCreateSurface( int width, int height, Coherent::UI::SurfaceResponse* pResponse );
            void ReleaseSurface( Coherent::UI::CoherentHandle surface );
            void* GetNativeTextureFromSharedHandle( Coherent::UI::CoherentHandle surface );
            IPlayerEventListener* GetPlayerEventListener() const;
            bool RaycastClosestViewListenersGeometry( const Vec3& origin, const Vec3& dir, int& outX, int& outY, CCoherentViewListener*& ppViewListener );
            void ShowMap( bool show );

            // ID3DListener methods
            virtual void OnPrePresent() COHERENT_OVERRIDE;
            virtual void OnPostPresent()  COHERENT_OVERRIDE {}
            virtual void OnPreReset() COHERENT_OVERRIDE;
            virtual void OnPostReset() COHERENT_OVERRIDE;
            virtual void OnPostBeginScene() COHERENT_OVERRIDE;

            // IGameFrameworkListener methods
            virtual void OnPostUpdate( float fDeltaTime ) COHERENT_OVERRIDE;
            virtual void OnPreRender() COHERENT_OVERRIDE {}
            virtual void OnSaveGame( ISaveGame* pSaveGame ) COHERENT_OVERRIDE {}
            virtual void OnLoadGame( ILoadGame* pLoadGame ) COHERENT_OVERRIDE {}
            virtual void OnLevelEnd( const char* nextLevel ) COHERENT_OVERRIDE {}
            virtual void OnActionEvent( const SActionEvent& event ) COHERENT_OVERRIDE;

        private:
            struct CreateSurfaceTask
            {
                int Width;
                int Height;
                Coherent::UI::SurfaceResponse* Response;
            };

            // Shared textures specific structure
            struct TexturePair
            {
                union DirectXTexture
                {
                    IDirect3DTexture9*  pTexDX9;
                    ID3D11Texture2D*    pTexDX11;
                    void*               pTexPtr;
                } NativeTexture;
                int CryTextureID;
            };

        private:
            void UpdateHUD();

            void SetTexturesForListeners();
            void ChangeEntityDiffuseTextureForMaterial( CCoherentViewListener* pViewListener, const char* entityName, const char* materialName );

            void ExecutePendingCreateSurfaceTasks();
            Coherent::UI::CoherentHandle CreateSharedTextureDX9( const CreateSurfaceTask& task, TexturePair* outTexturePair );
            Coherent::UI::CoherentHandle CreateSharedTextureDX11( const CreateSurfaceTask& task, TexturePair* outTexturePair );

        private:
            boost::scoped_ptr<CCoherentSystemEventListener> m_SystemEventsListener;
            boost::scoped_ptr<CCoherentInputEventListener> m_InputEventsListener;
            boost::scoped_ptr<CCoherentPlayerEventListener> m_PlayerEventListener;

            boost::scoped_ptr<CCoherentViewListener> m_HudViewListener;

            typedef std::map<CCoherentViewListener*, ViewConfig*> View;
            View m_Views;

            Coherent::UI::UISystem* m_pUISystem;

            std::vector<CreateSurfaceTask> m_PendingCreateSurfaceTasks;
            CryCriticalSection m_PendingCreateSurfaceTasksLock;

            boost::scoped_ptr<CFullscreenTriangleDrawer> m_FullscreenDrawer;

            // Shared texture specific member variable
            typedef std::map<Coherent::UI::CoherentHandle, TexturePair> HandleToSurfaceMap;
            HandleToSurfaceMap m_Surfaces;
            CryCriticalSection m_SurfacesCollectionLock;

            // CryPak File Handler
            PakFileHandler m_PakFileHandler;
    };

}

extern CoherentUIPlugin::CCoherentUISystem* gCoherentUISystem; //!< Global internal Coherent UI System Pointer
