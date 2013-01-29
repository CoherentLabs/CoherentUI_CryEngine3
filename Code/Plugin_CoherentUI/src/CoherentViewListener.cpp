#include "StdAfx.h"
#include "CoherentViewListener.h"
#include "CoherentUISystem.h"
#include "CoherentGeometry.h"
#include "CPluginCoherentUI.h"

#include <Coherent/UI/View.h>

#include <d3d9.h>
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>

namespace CoherentUIPlugin
{

    CCoherentViewListener::CCoherentViewListener( void* pTexture )
        : m_pTexture( pTexture )
        , m_ReadyForBindings( false )
        , m_pView( NULL )
        , m_CryTextureID( 0 )
    {
    }

    CCoherentViewListener::~CCoherentViewListener()
    {
        ReleaseTexture();

        if ( m_pView )
        {
            m_pView->Destroy();
        }
    }

    bool CCoherentViewListener::IsReadyForBindings() const
    {
        return m_ReadyForBindings;
    }

    Coherent::UI::View* CCoherentViewListener::GetView() const
    {
        return m_pView;
    }

    void CCoherentViewListener::OnViewCreated( Coherent::UI::View* pView )
    {
        m_pView = pView;
        m_pView->SetFocus();
    }

    void CCoherentViewListener::OnReadyForBindings( int frameId, const wchar_t* path, bool isMainFrame )
    {
        m_ReadyForBindings = true;
    }

    void CCoherentViewListener::DrawFrameDX9SharedMemory( Coherent::UI::CoherentHandle handle, int width, int height )
    {
        D3DLOCKED_RECT rect;
        // Assume that the whole texture should be mapped
        IDirect3DTexture9* pD3DTex = static_cast<IDirect3DTexture9*>( m_pTexture );
        HRESULT hr = pD3DTex->LockRect( 0, &rect, nullptr, D3DLOCK_DISCARD );

        if ( FAILED( hr ) )
        {
            return;
        }

        const size_t size = width * height * 4;
        void* pMapped = ::MapViewOfFile( handle, FILE_MAP_READ, 0, 0, size );
        char* pSrc = static_cast<char*>( pMapped );
        if (pSrc == NULL)
        {
            pD3DTex->UnlockRect( 0 );
            return;
        }

        char* pDest = static_cast<char*>( rect.pBits );
        int bytesPerRow = width * 4; // 32 BPP

        for ( int row = 0; row < height; ++row )
        {
            ::memcpy( pDest, pSrc, bytesPerRow );
            pSrc += bytesPerRow;
            pDest += rect.Pitch;
        }

        ::UnmapViewOfFile( pMapped );
        pD3DTex->UnlockRect( 0 );
    }

    void CCoherentViewListener::DrawFrameDX9SharedTexture( Coherent::UI::CoherentHandle handle, int width, int height )
    {
        if ( gCoherentUISystem == NULL )
        {
            return;
        }

        void* pNativeTexture = gCoherentUISystem->GetNativeTextureFromSharedHandle( handle );

        if ( pNativeTexture == NULL )
        {
            return;
        }

        IDirect3DTexture9* pD3DSrcTex = static_cast<IDirect3DTexture9*>( pNativeTexture );
        IDirect3DSurface9* pSurfaceSource;
        pD3DSrcTex->GetSurfaceLevel( 0, &pSurfaceSource );
        IDirect3DSurface9* pSurfaceDest;

        IDirect3DTexture9* pD3DDestTex = static_cast<IDirect3DTexture9*>( m_pTexture );
        pD3DDestTex->GetSurfaceLevel( 0, &pSurfaceDest );
        static_cast<IDirect3DDevice9*>( gD3DDevice )->StretchRect( pSurfaceSource, nullptr, pSurfaceDest, nullptr, D3DTEXF_NONE );

        pSurfaceSource->Release();
        pSurfaceDest->Release();
    }

    void CCoherentViewListener::DrawFrameDX11SharedMemory( Coherent::UI::CoherentHandle handle, int width, int height )
    {
        ID3D11Texture2D* pTexture = static_cast<ID3D11Texture2D*>( m_pTexture );
        ID3D11Device* pDevice = static_cast<ID3D11Device*>( gD3DDevice );
        ID3D11DeviceContext* pContext = NULL;
        pDevice->GetImmediateContext( &pContext );

        D3D11_MAPPED_SUBRESOURCE mapped;
        HRESULT hr = pContext->Map( pTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped );

        if ( FAILED( hr ) )
        {
            return;
        }

        const size_t size = width * height * 4;
        void* pMapped = ::MapViewOfFile( handle, FILE_MAP_READ, 0, 0, size );
        char* pSrc = static_cast<char*>( pMapped );

        if ( pSrc == NULL )
        {
            pContext->Unmap( pTexture, 0 );
            return;
        }

        char* pDest = static_cast<char*>( mapped.pData );
        int bytesPerRow = width * 4; // 32 BPP

        for ( int row = 0; row < height; ++row )
        {
            ::memcpy( pDest, pSrc, bytesPerRow );
            pSrc += bytesPerRow;
            pDest += mapped.RowPitch;
        }

        ::UnmapViewOfFile( pMapped );
        pContext->Unmap( pTexture, 0 );
    }

    void CCoherentViewListener::DrawFrameDX11SharedTexture( Coherent::UI::CoherentHandle handle, int width, int height )
    {
        if ( gCoherentUISystem == NULL )
        {
            return;
        }

        void* pNativeTexture = gCoherentUISystem->GetNativeTextureFromSharedHandle( handle );

        if ( pNativeTexture == NULL )
        {
            return;
        }

        ID3D11Texture2D* pSourceResource = static_cast<ID3D11Texture2D*>( pNativeTexture );
        ID3D11Texture2D* pDestResource = static_cast<ID3D11Texture2D*>( m_pTexture );

        ID3D11DeviceContext* pContext = NULL;
        ID3D11Device* pDevice = static_cast<ID3D11Device*>( gD3DDevice );
        pDevice->GetImmediateContext( &pContext );

        pContext->CopySubresourceRegion( pDestResource, 0, 0, 0, 0, pSourceResource, 0, NULL );
    }

    void CCoherentViewListener::OnDraw( Coherent::UI::CoherentHandle handle, bool useSharedMem, int width, int height )
    {
        // Note that this method is always called in the render thread since we're calling
        // Coherent::UI::UISystem::FetchSurfaces from it

        if ( m_pTexture == NULL )
        {
            return;
        }

        ERenderType rendererType = gEnv->pRenderer->GetRenderType();
        CRY_ASSERT_MESSAGE( rendererType == eRT_DX11 || rendererType == eRT_DX9, "Currently only DX9 and DX11 renderers are supported!" );

        if ( useSharedMem )
        {
            if ( rendererType == eRT_DX11 )
            {
                DrawFrameDX11SharedMemory( handle, width, height );
            }

            else if ( rendererType == eRT_DX9 )
            {
                DrawFrameDX9SharedMemory( handle, width, height );
            }
        }

        else
        {
            if ( rendererType == eRT_DX11 )
            {
                DrawFrameDX11SharedTexture( handle, width, height );
            }

            else if ( rendererType == eRT_DX9 )
            {
                DrawFrameDX9SharedTexture( handle, width, height );
            }
        }
    }

    void CCoherentViewListener::CreateSurface( bool sharedMemory, unsigned width, unsigned height, Coherent::UI::SurfaceResponse* response )
    {
        if ( sharedMemory )
        {
            Coherent::UI::CoherentHandle sharedHandle( ( int )::CreateFileMapping( INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, width * height * 4, nullptr ) );
            response->Signal( sharedHandle );
        }

        else
        {
            if ( gCoherentUISystem )
            {
                gCoherentUISystem->QueueCreateSurface( ( int )width, ( int )height, response );
            }

            else
            {
                response->Signal( Coherent::UI::CoherentHandle( 0 ) );
            }
        }
    }

    void CCoherentViewListener::DestroySurface( Coherent::UI::CoherentHandle surface, bool useSharedMemory )
    {
        if ( useSharedMemory )
        {
            ::CloseHandle( surface );
        }

        else
        {
            if ( gCoherentUISystem )
            {
                gCoherentUISystem->ReleaseSurface( surface );
            }
        }
    }

    void CCoherentViewListener::SetTexture( void* pTexture, int cryTexID )
    {
        m_CryTextureID = cryTexID;
        m_pTexture = pTexture;
    }

    void* CCoherentViewListener::GetTexture() const
    {
        return m_pTexture;
    }

    void CCoherentViewListener::ReleaseTexture()
    {
        if ( m_pTexture )
        {
            gEnv->pRenderer->RemoveTexture( m_CryTextureID );
            m_pTexture = NULL;
            m_CryTextureID = 0;
        }
    }

	bool CCoherentViewListener::RaycastGeometry(const Vec3& origin, const Vec3& dir, float& outDist, int& outViewX, int& outViewY)
	{
		if (!m_pView || !m_Geometry)
		{
			return false;
		}

		IEntity* pEntity = gEnv->pEntitySystem->FindEntityByName(m_EngineObjectName.c_str());

		if ( pEntity )
		{
			const Matrix34& invWorldMatrix = pEntity->GetWorldTM().GetInverted();
			Vec3 originModelSpace = invWorldMatrix.TransformPoint( origin );
			Vec3 dirModelSpace = invWorldMatrix.TransformVector( dir );
			dirModelSpace.Normalize();

			float t, u, v;

			if ( m_Geometry->IntersectWithRay( originModelSpace, dirModelSpace, t, u, v ) )
			{
				outDist = t;
				outViewX = ( int )( m_pView->GetWidth() * u );
				outViewY = ( int )( m_pView->GetHeight() * v );
				return true;
			}
		}

		return false;
	}

	void CCoherentViewListener::SetCollisionMesh(const char* objFileName)
	{
		m_Geometry.reset(new CCoherentGeometry());
		if (!m_Geometry->LoadFromObj(objFileName))
		{
			m_Geometry.reset();
		}
	}

	bool CCoherentViewListener::HasCollisionMesh() const
	{
		return m_Geometry;
	}

	void CCoherentViewListener::SetEngineObjectAndMaterialNames(const char* engineObjectName, const char* materialToOverrideName)
	{
		m_EngineObjectName = engineObjectName;
		m_MaterialToOverrideName = materialToOverrideName;
	}

	const char* CCoherentViewListener::GetEngineObjectName() const
	{
		return m_EngineObjectName.c_str();
	}

	const char* CCoherentViewListener::GetOverriddenMaterialName() const
	{
		return m_MaterialToOverrideName.c_str();
	}

}
