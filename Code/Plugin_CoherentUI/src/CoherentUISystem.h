#pragma once

#include <IPluginD3D.h>
#include <IGameFramework.h>
#include <ILevelSystem.h>
#include <Coherent/UI/CoherentTypes.h>

namespace Coherent
{
	namespace UI
	{
		class UISystem;
		class SurfaceResponse;
	}
}

struct IDirect3DTexture9;
struct ID3D11Texture2D;
struct IPlayerEventListener;

namespace CoherentUIPlugin
{

	class CCoherentInputEventListener;
	class CCoherentPlayerEventListener;
	class CCoherentSystemEventListener;
	class CCoherentViewListener;
	class CFullscreenTriangleDrawer;

	class CCoherentUISystem :
		public D3DPlugin::ID3DEventListener,
		public IGameFrameworkListener,
		public ILevelSystemListener
	{
		public:
			CCoherentUISystem( void );
			~CCoherentUISystem( void );

			bool InitializeCoherentUI();
			void OnSystemReady();
			void QueueCreateSurface( int width, int height, Coherent::UI::SurfaceResponse* pResponse );
			void ReleaseSurface( Coherent::UI::CoherentHandle surface );
			void* GetNativeTextureFromSharedHandle( Coherent::UI::CoherentHandle surface );
			IPlayerEventListener* GetPlayerEventListener() const;
			bool RaycastClosestViewListenersGeometry(const Vec3& origin, const Vec3& dir, int& outX, int& outY, CCoherentViewListener*& ppViewListener);

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

			// ILevelSystemListener methods
			virtual void OnLevelNotFound( const char* levelName ) COHERENT_OVERRIDE {}
			virtual void OnLoadingStart( ILevelInfo* pLevel ) COHERENT_OVERRIDE;
			virtual void OnLoadingComplete( ILevel* pLevel ) COHERENT_OVERRIDE {}
			virtual void OnLoadingError( ILevelInfo* pLevel, const char* error ) COHERENT_OVERRIDE {}
			virtual void OnLoadingProgress( ILevelInfo* pLevel, int progressAmount ) COHERENT_OVERRIDE {}
			virtual void OnUnloadComplete( ILevel* pLevel ) COHERENT_OVERRIDE;

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

			void LoadCoherentUIViews();
			void UnloadCoherentUIViews();

			void CreateViewsForListeners();
			void SetTexturesForListeners();
			void ChangeEntityDiffuseTextureForMaterial( CCoherentViewListener* pViewListener, const char* entityName, const char* materialName );

			void ExecutePendingCreateSurfaceTasks();
			Coherent::UI::CoherentHandle CreateSharedTextureDX9( const CreateSurfaceTask& task, TexturePair* outTexturePair );
			Coherent::UI::CoherentHandle CreateSharedTextureDX11( const CreateSurfaceTask& task, TexturePair* outTexturePair );

		private:
			boost::scoped_ptr<CCoherentSystemEventListener> m_SystemEventsListener;
			boost::scoped_ptr<CCoherentInputEventListener> m_InputEventsListener;
			boost::scoped_ptr<CCoherentPlayerEventListener> m_PlayerEventListener;

			std::vector<boost::shared_ptr<CCoherentViewListener> > m_ViewListeners;

			Coherent::UI::UISystem* m_pUISystem;

			std::vector<CreateSurfaceTask> m_PendingCreateSurfaceTasks;
			CryCriticalSection m_PendingCreateSurfaceTasksLock;

			boost::scoped_ptr<CFullscreenTriangleDrawer> m_FullscreenDrawer;

			// Shared texture specific member variable
			typedef std::map<Coherent::UI::CoherentHandle, TexturePair> HandleToSurfaceMap;
			HandleToSurfaceMap m_Surfaces;
			CryCriticalSection m_SurfacesCollectionLock;
	};

}

extern CoherentUIPlugin::CCoherentUISystem* gCoherentUISystem; //!< Global internal Coherent UI System Pointer
