#pragma once

#include <Coherent/UI/ViewListener.h>

namespace Coherent
{
	namespace UI
	{
		class View;
		class SurfaceResponse;
	}
}

struct ID3D11ShaderResourceView;

namespace CoherentUIPlugin
{
	class CCoherentGeometry;

	class CCoherentViewListener : public Coherent::UI::ViewListener
	{
		public:
			CCoherentViewListener( void* pDirectXTexture = NULL ); // IDirect3D9Texture* or ID3D11Texture2D*
			virtual ~CCoherentViewListener();

			void SetTexture( void* pTexture, int cryTexID );
			void* GetTexture() const;
			void ReleaseTexture();

			bool IsReadyForBindings() const;
			Coherent::UI::View* GetView() const;

			virtual void OnViewCreated( Coherent::UI::View* pView ) COHERENT_OVERRIDE;
			virtual void OnReadyForBindings( int frameId, const wchar_t* path, bool isMainFrame ) COHERENT_OVERRIDE;
			virtual void OnDraw( Coherent::UI::CoherentHandle handle, bool sharedMem, int width, int height ) COHERENT_OVERRIDE;
			virtual void CreateSurface( bool sharedMemory, unsigned width, unsigned height, Coherent::UI::SurfaceResponse* pResponse ) COHERENT_OVERRIDE;
			virtual void DestroySurface( Coherent::UI::CoherentHandle surface, bool ) COHERENT_OVERRIDE;

			virtual bool RaycastGeometry(const Vec3& origin, const Vec3& dir, float& outDist, int& outViewX, int& outViewY);

			void SetCollisionMesh(const char* objFileName);
			bool HasCollisionMesh() const;
			void SetEngineObjectAndMaterialNames(const char* engineObjectName, const char* materialToOverrideName);
			const char* GetEngineObjectName() const;
			const char* GetOverriddenMaterialName() const;

		private:
			void DrawFrameDX9SharedMemory( Coherent::UI::CoherentHandle data, int width, int height );
			void DrawFrameDX9SharedTexture( Coherent::UI::CoherentHandle data, int width, int height );
			void DrawFrameDX11SharedMemory( Coherent::UI::CoherentHandle data, int width, int height );
			void DrawFrameDX11SharedTexture( Coherent::UI::CoherentHandle data, int width, int height );

			void CreateShaderResourceViewForDX11Texture();

		protected:
			void* m_pTexture; // IDirect3D9Texture* or ID3D11Texture2D*
			ID3D11ShaderResourceView* m_pTextureSRV; // Shader resource view for DX11 texture
			int m_CryTextureID; // Used for removal from the CryEngine renderer when the listener is destroyed

			bool m_ReadyForBindings;
			Coherent::UI::View* m_pView;

			boost::scoped_ptr<CCoherentGeometry> m_Geometry;
			string m_EngineObjectName;
			string m_MaterialToOverrideName;
	};

}
