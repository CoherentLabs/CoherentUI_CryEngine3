#include <d3d9.h>

#include "../hooktools.h"

#pragma once

// Create function pointer types
#undef INTERFACE
#define INTERFACE IDirect3DDevice9
#undef PURE
#define PURE
#undef THIS
#define THIS                    INTERFACE FAR* This
#undef STDMETHOD
#define STDMETHOD(method)       DECLARE_FUNCTION_PTR(method)
#undef STDMETHOD_
#define STDMETHOD_(type,method) DECLARE_FUNCTION_PTR_(type,method)

// 1:1 copy from d3d9.h:
STDMETHOD( QueryInterface )DECLARE_PARAMS( THIS, REFIID riid, void** ppvObj ) PURE;
STDMETHOD_( ULONG, AddRef )DECLARE_PARAMS( THIS ) PURE;
STDMETHOD_( ULONG, Release )DECLARE_PARAMS( THIS ) PURE;
STDMETHOD( TestCooperativeLevel )DECLARE_PARAMS( THIS ) PURE;
STDMETHOD_( UINT, GetAvailableTextureMem )DECLARE_PARAMS( THIS ) PURE;
STDMETHOD( EvictManagedResources )DECLARE_PARAMS( THIS ) PURE;
STDMETHOD( GetDirect3D )DECLARE_PARAMS( THIS, IDirect3D9** ppD3D9 ) PURE;
STDMETHOD( GetDeviceCaps )DECLARE_PARAMS( THIS, D3DCAPS9* pCaps ) PURE;
STDMETHOD( GetDisplayMode )DECLARE_PARAMS( THIS, UINT iSwapChain, D3DDISPLAYMODE* pMode ) PURE;
STDMETHOD( GetCreationParameters )DECLARE_PARAMS( THIS, D3DDEVICE_CREATION_PARAMETERS* pParameters ) PURE;
STDMETHOD( SetCursorProperties )DECLARE_PARAMS( THIS, UINT XHotSpot, UINT YHotSpot, IDirect3DSurface9* pCursorBitmap ) PURE;
STDMETHOD_( void, SetCursorPosition )DECLARE_PARAMS( THIS, int X, int Y, DWORD Flags ) PURE;
STDMETHOD_( BOOL, ShowCursor )DECLARE_PARAMS( THIS, BOOL bShow ) PURE;
STDMETHOD( CreateAdditionalSwapChain )DECLARE_PARAMS( THIS, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DSwapChain9** pSwapChain ) PURE;
STDMETHOD( GetSwapChain )DECLARE_PARAMS( THIS, UINT iSwapChain, IDirect3DSwapChain9** pSwapChain ) PURE;
STDMETHOD_( UINT, GetNumberOfSwapChains )DECLARE_PARAMS( THIS ) PURE;
STDMETHOD( Reset )DECLARE_PARAMS( THIS, D3DPRESENT_PARAMETERS* pPresentationParameters ) PURE;
STDMETHOD( Present )DECLARE_PARAMS( THIS, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion ) PURE;
STDMETHOD( GetBackBuffer )DECLARE_PARAMS( THIS, UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9** ppBackBuffer ) PURE;
STDMETHOD( GetRasterStatus )DECLARE_PARAMS( THIS, UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus ) PURE;
STDMETHOD( SetDialogBoxMode )DECLARE_PARAMS( THIS, BOOL bEnableDialogs ) PURE;
STDMETHOD_( void, SetGammaRamp )DECLARE_PARAMS( THIS, UINT iSwapChain, DWORD Flags, CONST D3DGAMMARAMP* pRamp ) PURE;
STDMETHOD_( void, GetGammaRamp )DECLARE_PARAMS( THIS, UINT iSwapChain, D3DGAMMARAMP* pRamp ) PURE;
STDMETHOD( CreateTexture )DECLARE_PARAMS( THIS, UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle ) PURE;
STDMETHOD( CreateVolumeTexture )DECLARE_PARAMS( THIS, UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture9** ppVolumeTexture, HANDLE* pSharedHandle ) PURE;
STDMETHOD( CreateCubeTexture )DECLARE_PARAMS( THIS, UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DCubeTexture9** ppCubeTexture, HANDLE* pSharedHandle ) PURE;
STDMETHOD( CreateVertexBuffer )DECLARE_PARAMS( THIS, UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9** ppVertexBuffer, HANDLE* pSharedHandle ) PURE;
STDMETHOD( CreateIndexBuffer )DECLARE_PARAMS( THIS, UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer9** ppIndexBuffer, HANDLE* pSharedHandle ) PURE;
STDMETHOD( CreateRenderTarget )DECLARE_PARAMS( THIS, UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle ) PURE;
STDMETHOD( CreateDepthStencilSurface )DECLARE_PARAMS( THIS, UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle ) PURE;
STDMETHOD( UpdateSurface )DECLARE_PARAMS( THIS, IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestinationSurface, CONST POINT* pDestPoint ) PURE;
STDMETHOD( UpdateTexture )DECLARE_PARAMS( THIS, IDirect3DBaseTexture9* pSourceTexture, IDirect3DBaseTexture9* pDestinationTexture ) PURE;
STDMETHOD( GetRenderTargetData )DECLARE_PARAMS( THIS, IDirect3DSurface9* pRenderTarget, IDirect3DSurface9* pDestSurface ) PURE;
STDMETHOD( GetFrontBufferData )DECLARE_PARAMS( THIS, UINT iSwapChain, IDirect3DSurface9* pDestSurface ) PURE;
STDMETHOD( StretchRect )DECLARE_PARAMS( THIS, IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestSurface, CONST RECT* pDestRect, D3DTEXTUREFILTERTYPE Filter ) PURE;
STDMETHOD( ColorFill )DECLARE_PARAMS( THIS, IDirect3DSurface9* pSurface, CONST RECT* pRect, D3DCOLOR color ) PURE;
STDMETHOD( CreateOffscreenPlainSurface )DECLARE_PARAMS( THIS, UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle ) PURE;
STDMETHOD( SetRenderTarget )DECLARE_PARAMS( THIS, DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget ) PURE;
STDMETHOD( GetRenderTarget )DECLARE_PARAMS( THIS, DWORD RenderTargetIndex, IDirect3DSurface9** ppRenderTarget ) PURE;
STDMETHOD( SetDepthStencilSurface )DECLARE_PARAMS( THIS, IDirect3DSurface9* pNewZStencil ) PURE;
STDMETHOD( GetDepthStencilSurface )DECLARE_PARAMS( THIS, IDirect3DSurface9** ppZStencilSurface ) PURE;
STDMETHOD( BeginScene )DECLARE_PARAMS( THIS ) PURE;
STDMETHOD( EndScene )DECLARE_PARAMS( THIS ) PURE;
STDMETHOD( Clear )DECLARE_PARAMS( THIS, DWORD Count, CONST D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil ) PURE;
STDMETHOD( SetTransform )DECLARE_PARAMS( THIS, D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix ) PURE;
STDMETHOD( GetTransform )DECLARE_PARAMS( THIS, D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix ) PURE;
STDMETHOD( MultiplyTransform )DECLARE_PARAMS( THIS, D3DTRANSFORMSTATETYPE, CONST D3DMATRIX* ) PURE;
STDMETHOD( SetViewport )DECLARE_PARAMS( THIS, CONST D3DVIEWPORT9* pViewport ) PURE;
STDMETHOD( GetViewport )DECLARE_PARAMS( THIS, D3DVIEWPORT9* pViewport ) PURE;
STDMETHOD( SetMaterial )DECLARE_PARAMS( THIS, CONST D3DMATERIAL9* pMaterial ) PURE;
STDMETHOD( GetMaterial )DECLARE_PARAMS( THIS, D3DMATERIAL9* pMaterial ) PURE;
STDMETHOD( SetLight )DECLARE_PARAMS( THIS, DWORD Index, CONST D3DLIGHT9* ) PURE;
STDMETHOD( GetLight )DECLARE_PARAMS( THIS, DWORD Index, D3DLIGHT9* ) PURE;
STDMETHOD( LightEnable )DECLARE_PARAMS( THIS, DWORD Index, BOOL Enable ) PURE;
STDMETHOD( GetLightEnable )DECLARE_PARAMS( THIS, DWORD Index, BOOL* pEnable ) PURE;
STDMETHOD( SetClipPlane )DECLARE_PARAMS( THIS, DWORD Index, CONST float* pPlane ) PURE;
STDMETHOD( GetClipPlane )DECLARE_PARAMS( THIS, DWORD Index, float* pPlane ) PURE;
STDMETHOD( SetRenderState )DECLARE_PARAMS( THIS, D3DRENDERSTATETYPE State, DWORD Value ) PURE;
STDMETHOD( GetRenderState )DECLARE_PARAMS( THIS, D3DRENDERSTATETYPE State, DWORD* pValue ) PURE;
STDMETHOD( CreateStateBlock )DECLARE_PARAMS( THIS, D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9** ppSB ) PURE;
STDMETHOD( BeginStateBlock )DECLARE_PARAMS( THIS ) PURE;
STDMETHOD( EndStateBlock )DECLARE_PARAMS( THIS, IDirect3DStateBlock9** ppSB ) PURE;
STDMETHOD( SetClipStatus )DECLARE_PARAMS( THIS, CONST D3DCLIPSTATUS9* pClipStatus ) PURE;
STDMETHOD( GetClipStatus )DECLARE_PARAMS( THIS, D3DCLIPSTATUS9* pClipStatus ) PURE;
STDMETHOD( GetTexture )DECLARE_PARAMS( THIS, DWORD Stage, IDirect3DBaseTexture9** ppTexture ) PURE;
STDMETHOD( SetTexture )DECLARE_PARAMS( THIS, DWORD Stage, IDirect3DBaseTexture9* pTexture ) PURE;
STDMETHOD( GetTextureStageState )DECLARE_PARAMS( THIS, DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue ) PURE;
STDMETHOD( SetTextureStageState )DECLARE_PARAMS( THIS, DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value ) PURE;
STDMETHOD( GetSamplerState )DECLARE_PARAMS( THIS, DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD* pValue ) PURE;
STDMETHOD( SetSamplerState )DECLARE_PARAMS( THIS, DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value ) PURE;
STDMETHOD( ValidateDevice )DECLARE_PARAMS( THIS, DWORD* pNumPasses ) PURE;
STDMETHOD( SetPaletteEntries )DECLARE_PARAMS( THIS, UINT PaletteNumber, CONST PALETTEENTRY* pEntries ) PURE;
STDMETHOD( GetPaletteEntries )DECLARE_PARAMS( THIS, UINT PaletteNumber, PALETTEENTRY* pEntries ) PURE;
STDMETHOD( SetCurrentTexturePalette )DECLARE_PARAMS( THIS, UINT PaletteNumber ) PURE;
STDMETHOD( GetCurrentTexturePalette )DECLARE_PARAMS( THIS, UINT* PaletteNumber ) PURE;
STDMETHOD( SetScissorRect )DECLARE_PARAMS( THIS, CONST RECT* pRect ) PURE;
STDMETHOD( GetScissorRect )DECLARE_PARAMS( THIS, RECT* pRect ) PURE;
STDMETHOD( SetSoftwareVertexProcessing )DECLARE_PARAMS( THIS, BOOL bSoftware ) PURE;
STDMETHOD_( BOOL, GetSoftwareVertexProcessing )DECLARE_PARAMS( THIS ) PURE;
STDMETHOD( SetNPatchMode )DECLARE_PARAMS( THIS, float nSegments ) PURE;
STDMETHOD_( float, GetNPatchMode )DECLARE_PARAMS( THIS ) PURE;
STDMETHOD( DrawPrimitive )DECLARE_PARAMS( THIS, D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount ) PURE;
STDMETHOD( DrawIndexedPrimitive )DECLARE_PARAMS( THIS, D3DPRIMITIVETYPE, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount ) PURE;
STDMETHOD( DrawPrimitiveUP )DECLARE_PARAMS( THIS, D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride ) PURE;
STDMETHOD( DrawIndexedPrimitiveUP )DECLARE_PARAMS( THIS, D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, CONST void* pIndexData, D3DFORMAT IndexDataFormat, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride ) PURE;
STDMETHOD( ProcessVertices )DECLARE_PARAMS( THIS, UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer9* pDestBuffer, IDirect3DVertexDeclaration9* pVertexDecl, DWORD Flags ) PURE;
STDMETHOD( CreateVertexDeclaration )DECLARE_PARAMS( THIS, CONST D3DVERTEXELEMENT9* pVertexElements, IDirect3DVertexDeclaration9** ppDecl ) PURE;
STDMETHOD( SetVertexDeclaration )DECLARE_PARAMS( THIS, IDirect3DVertexDeclaration9* pDecl ) PURE;
STDMETHOD( GetVertexDeclaration )DECLARE_PARAMS( THIS, IDirect3DVertexDeclaration9** ppDecl ) PURE;
STDMETHOD( SetFVF )DECLARE_PARAMS( THIS, DWORD FVF ) PURE;
STDMETHOD( GetFVF )DECLARE_PARAMS( THIS, DWORD* pFVF ) PURE;
STDMETHOD( CreateVertexShader )DECLARE_PARAMS( THIS, CONST DWORD* pFunction, IDirect3DVertexShader9** ppShader ) PURE;
STDMETHOD( SetVertexShader )DECLARE_PARAMS( THIS, IDirect3DVertexShader9* pShader ) PURE;
STDMETHOD( GetVertexShader )DECLARE_PARAMS( THIS, IDirect3DVertexShader9** ppShader ) PURE;
STDMETHOD( SetVertexShaderConstantF )DECLARE_PARAMS( THIS, UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount ) PURE;
STDMETHOD( GetVertexShaderConstantF )DECLARE_PARAMS( THIS, UINT StartRegister, float* pConstantData, UINT Vector4fCount ) PURE;
STDMETHOD( SetVertexShaderConstantI )DECLARE_PARAMS( THIS, UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount ) PURE;
STDMETHOD( GetVertexShaderConstantI )DECLARE_PARAMS( THIS, UINT StartRegister, int* pConstantData, UINT Vector4iCount ) PURE;
STDMETHOD( SetVertexShaderConstantB )DECLARE_PARAMS( THIS, UINT StartRegister, CONST BOOL* pConstantData, UINT  BoolCount ) PURE;
STDMETHOD( GetVertexShaderConstantB )DECLARE_PARAMS( THIS, UINT StartRegister, BOOL* pConstantData, UINT BoolCount ) PURE;
STDMETHOD( SetStreamSource )DECLARE_PARAMS( THIS, UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride ) PURE;
STDMETHOD( GetStreamSource )DECLARE_PARAMS( THIS, UINT StreamNumber, IDirect3DVertexBuffer9** ppStreamData, UINT* pOffsetInBytes, UINT* pStride ) PURE;
STDMETHOD( SetStreamSourceFreq )DECLARE_PARAMS( THIS, UINT StreamNumber, UINT Setting ) PURE;
STDMETHOD( GetStreamSourceFreq )DECLARE_PARAMS( THIS, UINT StreamNumber, UINT* pSetting ) PURE;
STDMETHOD( SetIndices )DECLARE_PARAMS( THIS, IDirect3DIndexBuffer9* pIndexData ) PURE;
STDMETHOD( GetIndices )DECLARE_PARAMS( THIS, IDirect3DIndexBuffer9** ppIndexData ) PURE;
STDMETHOD( CreatePixelShader )DECLARE_PARAMS( THIS, CONST DWORD* pFunction, IDirect3DPixelShader9** ppShader ) PURE;
STDMETHOD( SetPixelShader )DECLARE_PARAMS( THIS, IDirect3DPixelShader9* pShader ) PURE;
STDMETHOD( GetPixelShader )DECLARE_PARAMS( THIS, IDirect3DPixelShader9** ppShader ) PURE;
STDMETHOD( SetPixelShaderConstantF )DECLARE_PARAMS( THIS, UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount ) PURE;
STDMETHOD( GetPixelShaderConstantF )DECLARE_PARAMS( THIS, UINT StartRegister, float* pConstantData, UINT Vector4fCount ) PURE;
STDMETHOD( SetPixelShaderConstantI )DECLARE_PARAMS( THIS, UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount ) PURE;
STDMETHOD( GetPixelShaderConstantI )DECLARE_PARAMS( THIS, UINT StartRegister, int* pConstantData, UINT Vector4iCount ) PURE;
STDMETHOD( SetPixelShaderConstantB )DECLARE_PARAMS( THIS, UINT StartRegister, CONST BOOL* pConstantData, UINT  BoolCount ) PURE;
STDMETHOD( GetPixelShaderConstantB )DECLARE_PARAMS( THIS, UINT StartRegister, BOOL* pConstantData, UINT BoolCount ) PURE;
STDMETHOD( DrawRectPatch )DECLARE_PARAMS( THIS, UINT Handle, CONST float* pNumSegs, CONST D3DRECTPATCH_INFO* pRectPatchInfo ) PURE;
STDMETHOD( DrawTriPatch )DECLARE_PARAMS( THIS, UINT Handle, CONST float* pNumSegs, CONST D3DTRIPATCH_INFO* pTriPatchInfo ) PURE;
STDMETHOD( DeletePatch )DECLARE_PARAMS( THIS, UINT Handle ) PURE;
STDMETHOD( CreateQuery )DECLARE_PARAMS( THIS, D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery ) PURE;

#undef INTERFACE
#define INTERFACE IDirect3DDevice9Ex

STDMETHOD( QueryInterface )DECLARE_PARAMS( THIS, REFIID riid, void** ppvObj ) PURE;
STDMETHOD_( ULONG, AddRef )DECLARE_PARAMS( THIS ) PURE;
STDMETHOD_( ULONG, Release )DECLARE_PARAMS( THIS ) PURE;
STDMETHOD( TestCooperativeLevel )DECLARE_PARAMS( THIS ) PURE;
STDMETHOD_( UINT, GetAvailableTextureMem )DECLARE_PARAMS( THIS ) PURE;
STDMETHOD( EvictManagedResources )DECLARE_PARAMS( THIS ) PURE;
STDMETHOD( GetDirect3D )DECLARE_PARAMS( THIS, IDirect3D9** ppD3D9 ) PURE;
STDMETHOD( GetDeviceCaps )DECLARE_PARAMS( THIS, D3DCAPS9* pCaps ) PURE;
STDMETHOD( GetDisplayMode )DECLARE_PARAMS( THIS, UINT iSwapChain, D3DDISPLAYMODE* pMode ) PURE;
STDMETHOD( GetCreationParameters )DECLARE_PARAMS( THIS, D3DDEVICE_CREATION_PARAMETERS* pParameters ) PURE;
STDMETHOD( SetCursorProperties )DECLARE_PARAMS( THIS, UINT XHotSpot, UINT YHotSpot, IDirect3DSurface9* pCursorBitmap ) PURE;
STDMETHOD_( void, SetCursorPosition )DECLARE_PARAMS( THIS, int X, int Y, DWORD Flags ) PURE;
STDMETHOD_( BOOL, ShowCursor )DECLARE_PARAMS( THIS, BOOL bShow ) PURE;
STDMETHOD( CreateAdditionalSwapChain )DECLARE_PARAMS( THIS, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DSwapChain9** pSwapChain ) PURE;
STDMETHOD( GetSwapChain )DECLARE_PARAMS( THIS, UINT iSwapChain, IDirect3DSwapChain9** pSwapChain ) PURE;
STDMETHOD_( UINT, GetNumberOfSwapChains )DECLARE_PARAMS( THIS ) PURE;
STDMETHOD( Reset )DECLARE_PARAMS( THIS, D3DPRESENT_PARAMETERS* pPresentationParameters ) PURE;
STDMETHOD( Present )DECLARE_PARAMS( THIS, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion ) PURE;
STDMETHOD( GetBackBuffer )DECLARE_PARAMS( THIS, UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9** ppBackBuffer ) PURE;
STDMETHOD( GetRasterStatus )DECLARE_PARAMS( THIS, UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus ) PURE;
STDMETHOD( SetDialogBoxMode )DECLARE_PARAMS( THIS, BOOL bEnableDialogs ) PURE;
STDMETHOD_( void, SetGammaRamp )DECLARE_PARAMS( THIS, UINT iSwapChain, DWORD Flags, CONST D3DGAMMARAMP* pRamp ) PURE;
STDMETHOD_( void, GetGammaRamp )DECLARE_PARAMS( THIS, UINT iSwapChain, D3DGAMMARAMP* pRamp ) PURE;
STDMETHOD( CreateTexture )DECLARE_PARAMS( THIS, UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle ) PURE;
STDMETHOD( CreateVolumeTexture )DECLARE_PARAMS( THIS, UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture9** ppVolumeTexture, HANDLE* pSharedHandle ) PURE;
STDMETHOD( CreateCubeTexture )DECLARE_PARAMS( THIS, UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DCubeTexture9** ppCubeTexture, HANDLE* pSharedHandle ) PURE;
STDMETHOD( CreateVertexBuffer )DECLARE_PARAMS( THIS, UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9** ppVertexBuffer, HANDLE* pSharedHandle ) PURE;
STDMETHOD( CreateIndexBuffer )DECLARE_PARAMS( THIS, UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer9** ppIndexBuffer, HANDLE* pSharedHandle ) PURE;
STDMETHOD( CreateRenderTarget )DECLARE_PARAMS( THIS, UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle ) PURE;
STDMETHOD( CreateDepthStencilSurface )DECLARE_PARAMS( THIS, UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle ) PURE;
STDMETHOD( UpdateSurface )DECLARE_PARAMS( THIS, IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestinationSurface, CONST POINT* pDestPoint ) PURE;
STDMETHOD( UpdateTexture )DECLARE_PARAMS( THIS, IDirect3DBaseTexture9* pSourceTexture, IDirect3DBaseTexture9* pDestinationTexture ) PURE;
STDMETHOD( GetRenderTargetData )DECLARE_PARAMS( THIS, IDirect3DSurface9* pRenderTarget, IDirect3DSurface9* pDestSurface ) PURE;
STDMETHOD( GetFrontBufferData )DECLARE_PARAMS( THIS, UINT iSwapChain, IDirect3DSurface9* pDestSurface ) PURE;
STDMETHOD( StretchRect )DECLARE_PARAMS( THIS, IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestSurface, CONST RECT* pDestRect, D3DTEXTUREFILTERTYPE Filter ) PURE;
STDMETHOD( ColorFill )DECLARE_PARAMS( THIS, IDirect3DSurface9* pSurface, CONST RECT* pRect, D3DCOLOR color ) PURE;
STDMETHOD( CreateOffscreenPlainSurface )DECLARE_PARAMS( THIS, UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle ) PURE;
STDMETHOD( SetRenderTarget )DECLARE_PARAMS( THIS, DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget ) PURE;
STDMETHOD( GetRenderTarget )DECLARE_PARAMS( THIS, DWORD RenderTargetIndex, IDirect3DSurface9** ppRenderTarget ) PURE;
STDMETHOD( SetDepthStencilSurface )DECLARE_PARAMS( THIS, IDirect3DSurface9* pNewZStencil ) PURE;
STDMETHOD( GetDepthStencilSurface )DECLARE_PARAMS( THIS, IDirect3DSurface9** ppZStencilSurface ) PURE;
STDMETHOD( BeginScene )DECLARE_PARAMS( THIS ) PURE;
STDMETHOD( EndScene )DECLARE_PARAMS( THIS ) PURE;
STDMETHOD( Clear )DECLARE_PARAMS( THIS, DWORD Count, CONST D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil ) PURE;
STDMETHOD( SetTransform )DECLARE_PARAMS( THIS, D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix ) PURE;
STDMETHOD( GetTransform )DECLARE_PARAMS( THIS, D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix ) PURE;
STDMETHOD( MultiplyTransform )DECLARE_PARAMS( THIS, D3DTRANSFORMSTATETYPE, CONST D3DMATRIX* ) PURE;
STDMETHOD( SetViewport )DECLARE_PARAMS( THIS, CONST D3DVIEWPORT9* pViewport ) PURE;
STDMETHOD( GetViewport )DECLARE_PARAMS( THIS, D3DVIEWPORT9* pViewport ) PURE;
STDMETHOD( SetMaterial )DECLARE_PARAMS( THIS, CONST D3DMATERIAL9* pMaterial ) PURE;
STDMETHOD( GetMaterial )DECLARE_PARAMS( THIS, D3DMATERIAL9* pMaterial ) PURE;
STDMETHOD( SetLight )DECLARE_PARAMS( THIS, DWORD Index, CONST D3DLIGHT9* ) PURE;
STDMETHOD( GetLight )DECLARE_PARAMS( THIS, DWORD Index, D3DLIGHT9* ) PURE;
STDMETHOD( LightEnable )DECLARE_PARAMS( THIS, DWORD Index, BOOL Enable ) PURE;
STDMETHOD( GetLightEnable )DECLARE_PARAMS( THIS, DWORD Index, BOOL* pEnable ) PURE;
STDMETHOD( SetClipPlane )DECLARE_PARAMS( THIS, DWORD Index, CONST float* pPlane ) PURE;
STDMETHOD( GetClipPlane )DECLARE_PARAMS( THIS, DWORD Index, float* pPlane ) PURE;
STDMETHOD( SetRenderState )DECLARE_PARAMS( THIS, D3DRENDERSTATETYPE State, DWORD Value ) PURE;
STDMETHOD( GetRenderState )DECLARE_PARAMS( THIS, D3DRENDERSTATETYPE State, DWORD* pValue ) PURE;
STDMETHOD( CreateStateBlock )DECLARE_PARAMS( THIS, D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9** ppSB ) PURE;
STDMETHOD( BeginStateBlock )DECLARE_PARAMS( THIS ) PURE;
STDMETHOD( EndStateBlock )DECLARE_PARAMS( THIS, IDirect3DStateBlock9** ppSB ) PURE;
STDMETHOD( SetClipStatus )DECLARE_PARAMS( THIS, CONST D3DCLIPSTATUS9* pClipStatus ) PURE;
STDMETHOD( GetClipStatus )DECLARE_PARAMS( THIS, D3DCLIPSTATUS9* pClipStatus ) PURE;
STDMETHOD( GetTexture )DECLARE_PARAMS( THIS, DWORD Stage, IDirect3DBaseTexture9** ppTexture ) PURE;
STDMETHOD( SetTexture )DECLARE_PARAMS( THIS, DWORD Stage, IDirect3DBaseTexture9* pTexture ) PURE;
STDMETHOD( GetTextureStageState )DECLARE_PARAMS( THIS, DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue ) PURE;
STDMETHOD( SetTextureStageState )DECLARE_PARAMS( THIS, DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value ) PURE;
STDMETHOD( GetSamplerState )DECLARE_PARAMS( THIS, DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD* pValue ) PURE;
STDMETHOD( SetSamplerState )DECLARE_PARAMS( THIS, DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value ) PURE;
STDMETHOD( ValidateDevice )DECLARE_PARAMS( THIS, DWORD* pNumPasses ) PURE;
STDMETHOD( SetPaletteEntries )DECLARE_PARAMS( THIS, UINT PaletteNumber, CONST PALETTEENTRY* pEntries ) PURE;
STDMETHOD( GetPaletteEntries )DECLARE_PARAMS( THIS, UINT PaletteNumber, PALETTEENTRY* pEntries ) PURE;
STDMETHOD( SetCurrentTexturePalette )DECLARE_PARAMS( THIS, UINT PaletteNumber ) PURE;
STDMETHOD( GetCurrentTexturePalette )DECLARE_PARAMS( THIS, UINT* PaletteNumber ) PURE;
STDMETHOD( SetScissorRect )DECLARE_PARAMS( THIS, CONST RECT* pRect ) PURE;
STDMETHOD( GetScissorRect )DECLARE_PARAMS( THIS, RECT* pRect ) PURE;
STDMETHOD( SetSoftwareVertexProcessing )DECLARE_PARAMS( THIS, BOOL bSoftware ) PURE;
STDMETHOD_( BOOL, GetSoftwareVertexProcessing )DECLARE_PARAMS( THIS ) PURE;
STDMETHOD( SetNPatchMode )DECLARE_PARAMS( THIS, float nSegments ) PURE;
STDMETHOD_( float, GetNPatchMode )DECLARE_PARAMS( THIS ) PURE;
STDMETHOD( DrawPrimitive )DECLARE_PARAMS( THIS, D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount ) PURE;
STDMETHOD( DrawIndexedPrimitive )DECLARE_PARAMS( THIS, D3DPRIMITIVETYPE, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount ) PURE;
STDMETHOD( DrawPrimitiveUP )DECLARE_PARAMS( THIS, D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride ) PURE;
STDMETHOD( DrawIndexedPrimitiveUP )DECLARE_PARAMS( THIS, D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, CONST void* pIndexData, D3DFORMAT IndexDataFormat, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride ) PURE;
STDMETHOD( ProcessVertices )DECLARE_PARAMS( THIS, UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer9* pDestBuffer, IDirect3DVertexDeclaration9* pVertexDecl, DWORD Flags ) PURE;
STDMETHOD( CreateVertexDeclaration )DECLARE_PARAMS( THIS, CONST D3DVERTEXELEMENT9* pVertexElements, IDirect3DVertexDeclaration9** ppDecl ) PURE;
STDMETHOD( SetVertexDeclaration )DECLARE_PARAMS( THIS, IDirect3DVertexDeclaration9* pDecl ) PURE;
STDMETHOD( GetVertexDeclaration )DECLARE_PARAMS( THIS, IDirect3DVertexDeclaration9** ppDecl ) PURE;
STDMETHOD( SetFVF )DECLARE_PARAMS( THIS, DWORD FVF ) PURE;
STDMETHOD( GetFVF )DECLARE_PARAMS( THIS, DWORD* pFVF ) PURE;
STDMETHOD( CreateVertexShader )DECLARE_PARAMS( THIS, CONST DWORD* pFunction, IDirect3DVertexShader9** ppShader ) PURE;
STDMETHOD( SetVertexShader )DECLARE_PARAMS( THIS, IDirect3DVertexShader9* pShader ) PURE;
STDMETHOD( GetVertexShader )DECLARE_PARAMS( THIS, IDirect3DVertexShader9** ppShader ) PURE;
STDMETHOD( SetVertexShaderConstantF )DECLARE_PARAMS( THIS, UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount ) PURE;
STDMETHOD( GetVertexShaderConstantF )DECLARE_PARAMS( THIS, UINT StartRegister, float* pConstantData, UINT Vector4fCount ) PURE;
STDMETHOD( SetVertexShaderConstantI )DECLARE_PARAMS( THIS, UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount ) PURE;
STDMETHOD( GetVertexShaderConstantI )DECLARE_PARAMS( THIS, UINT StartRegister, int* pConstantData, UINT Vector4iCount ) PURE;
STDMETHOD( SetVertexShaderConstantB )DECLARE_PARAMS( THIS, UINT StartRegister, CONST BOOL* pConstantData, UINT  BoolCount ) PURE;
STDMETHOD( GetVertexShaderConstantB )DECLARE_PARAMS( THIS, UINT StartRegister, BOOL* pConstantData, UINT BoolCount ) PURE;
STDMETHOD( SetStreamSource )DECLARE_PARAMS( THIS, UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride ) PURE;
STDMETHOD( GetStreamSource )DECLARE_PARAMS( THIS, UINT StreamNumber, IDirect3DVertexBuffer9** ppStreamData, UINT* pOffsetInBytes, UINT* pStride ) PURE;
STDMETHOD( SetStreamSourceFreq )DECLARE_PARAMS( THIS, UINT StreamNumber, UINT Setting ) PURE;
STDMETHOD( GetStreamSourceFreq )DECLARE_PARAMS( THIS, UINT StreamNumber, UINT* pSetting ) PURE;
STDMETHOD( SetIndices )DECLARE_PARAMS( THIS, IDirect3DIndexBuffer9* pIndexData ) PURE;
STDMETHOD( GetIndices )DECLARE_PARAMS( THIS, IDirect3DIndexBuffer9** ppIndexData ) PURE;
STDMETHOD( CreatePixelShader )DECLARE_PARAMS( THIS, CONST DWORD* pFunction, IDirect3DPixelShader9** ppShader ) PURE;
STDMETHOD( SetPixelShader )DECLARE_PARAMS( THIS, IDirect3DPixelShader9* pShader ) PURE;
STDMETHOD( GetPixelShader )DECLARE_PARAMS( THIS, IDirect3DPixelShader9** ppShader ) PURE;
STDMETHOD( SetPixelShaderConstantF )DECLARE_PARAMS( THIS, UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount ) PURE;
STDMETHOD( GetPixelShaderConstantF )DECLARE_PARAMS( THIS, UINT StartRegister, float* pConstantData, UINT Vector4fCount ) PURE;
STDMETHOD( SetPixelShaderConstantI )DECLARE_PARAMS( THIS, UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount ) PURE;
STDMETHOD( GetPixelShaderConstantI )DECLARE_PARAMS( THIS, UINT StartRegister, int* pConstantData, UINT Vector4iCount ) PURE;
STDMETHOD( SetPixelShaderConstantB )DECLARE_PARAMS( THIS, UINT StartRegister, CONST BOOL* pConstantData, UINT  BoolCount ) PURE;
STDMETHOD( GetPixelShaderConstantB )DECLARE_PARAMS( THIS, UINT StartRegister, BOOL* pConstantData, UINT BoolCount ) PURE;
STDMETHOD( DrawRectPatch )DECLARE_PARAMS( THIS, UINT Handle, CONST float* pNumSegs, CONST D3DRECTPATCH_INFO* pRectPatchInfo ) PURE;
STDMETHOD( DrawTriPatch )DECLARE_PARAMS( THIS, UINT Handle, CONST float* pNumSegs, CONST D3DTRIPATCH_INFO* pTriPatchInfo ) PURE;
STDMETHOD( DeletePatch )DECLARE_PARAMS( THIS, UINT Handle ) PURE;
STDMETHOD( CreateQuery )DECLARE_PARAMS( THIS, D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery ) PURE;
STDMETHOD( SetConvolutionMonoKernel )DECLARE_PARAMS( THIS, UINT width, UINT height, float* rows, float* columns ) PURE;
STDMETHOD( ComposeRects )DECLARE_PARAMS( THIS, IDirect3DSurface9* pSrc, IDirect3DSurface9* pDst, IDirect3DVertexBuffer9* pSrcRectDescs, UINT NumRects, IDirect3DVertexBuffer9* pDstRectDescs, D3DCOMPOSERECTSOP Operation, int Xoffset, int Yoffset ) PURE;
STDMETHOD( PresentEx )DECLARE_PARAMS( THIS, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion, DWORD dwFlags ) PURE;
STDMETHOD( GetGPUThreadPriority )DECLARE_PARAMS( THIS, INT* pPriority ) PURE;
STDMETHOD( SetGPUThreadPriority )DECLARE_PARAMS( THIS, INT Priority ) PURE;
STDMETHOD( WaitForVBlank )DECLARE_PARAMS( THIS, UINT iSwapChain ) PURE;
STDMETHOD( CheckResourceResidency )DECLARE_PARAMS( THIS, IDirect3DResource9** pResourceArray, UINT32 NumResources ) PURE;
STDMETHOD( SetMaximumFrameLatency )DECLARE_PARAMS( THIS, UINT MaxLatency ) PURE;
STDMETHOD( GetMaximumFrameLatency )DECLARE_PARAMS( THIS, UINT* pMaxLatency ) PURE;
STDMETHOD( CheckDeviceState )DECLARE_PARAMS( THIS, HWND hDestinationWindow ) PURE;
STDMETHOD( CreateRenderTargetEx )DECLARE_PARAMS( THIS, UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle, DWORD Usage ) PURE;
STDMETHOD( CreateOffscreenPlainSurfaceEx )DECLARE_PARAMS( THIS, UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle, DWORD Usage ) PURE;
STDMETHOD( CreateDepthStencilSurfaceEx )DECLARE_PARAMS( THIS, UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle, DWORD Usage ) PURE;
STDMETHOD( ResetEx )DECLARE_PARAMS( THIS, D3DPRESENT_PARAMETERS* pPresentationParameters, D3DDISPLAYMODEEX* pFullscreenDisplayMode ) PURE;
STDMETHOD( GetDisplayModeEx )DECLARE_PARAMS( THIS, UINT iSwapChain, D3DDISPLAYMODEEX* pMode, D3DDISPLAYROTATION* pRotation ) PURE;

#undef INTERFACE
#define INTERFACE IDirect3DDevice9

// Create vtable offsets
#undef STDMETHOD
#define STDMETHOD(method) PPCAT(PPCAT(PPCAT(vt, INTERFACE),_), method),
#undef STDMETHOD_
#define STDMETHOD_(type,method) PPCAT(PPCAT(PPCAT(vt, INTERFACE),_), method),

enum PPCAT( vt, INTERFACE )
{
    // 1:1 copy from d3d9.h: replace (THIS* -> ""
    STDMETHOD( QueryInterface )
    STDMETHOD_( ULONG, AddRef )
    STDMETHOD_( ULONG, Release )
    STDMETHOD( TestCooperativeLevel )
    STDMETHOD_( UINT, GetAvailableTextureMem )
    STDMETHOD( EvictManagedResources )
    STDMETHOD( GetDirect3D )
    STDMETHOD( GetDeviceCaps )
    STDMETHOD( GetDisplayMode )
    STDMETHOD( GetCreationParameters )
    STDMETHOD( SetCursorProperties )
    STDMETHOD_( void, SetCursorPosition )
    STDMETHOD_( BOOL, ShowCursor )
    STDMETHOD( CreateAdditionalSwapChain )
    STDMETHOD( GetSwapChain )
    STDMETHOD_( UINT, GetNumberOfSwapChains )
    STDMETHOD( Reset )
    STDMETHOD( Present )
    STDMETHOD( GetBackBuffer )
    STDMETHOD( GetRasterStatus )
    STDMETHOD( SetDialogBoxMode )
    STDMETHOD_( void, SetGammaRamp )
    STDMETHOD_( void, GetGammaRamp )
    STDMETHOD( CreateTexture )
    STDMETHOD( CreateVolumeTexture )
    STDMETHOD( CreateCubeTexture )
    STDMETHOD( CreateVertexBuffer )
    STDMETHOD( CreateIndexBuffer )
    STDMETHOD( CreateRenderTarget )
    STDMETHOD( CreateDepthStencilSurface )
    STDMETHOD( UpdateSurface )
    STDMETHOD( UpdateTexture )
    STDMETHOD( GetRenderTargetData )
    STDMETHOD( GetFrontBufferData )
    STDMETHOD( StretchRect )
    STDMETHOD( ColorFill )
    STDMETHOD( CreateOffscreenPlainSurface )
    STDMETHOD( SetRenderTarget )
    STDMETHOD( GetRenderTarget )
    STDMETHOD( SetDepthStencilSurface )
    STDMETHOD( GetDepthStencilSurface )
    STDMETHOD( BeginScene )
    STDMETHOD( EndScene )
    STDMETHOD( Clear )
    STDMETHOD( SetTransform )
    STDMETHOD( GetTransform )
    STDMETHOD( MultiplyTransform )
    STDMETHOD( SetViewport )
    STDMETHOD( GetViewport )
    STDMETHOD( SetMaterial )
    STDMETHOD( GetMaterial )
    STDMETHOD( SetLight )
    STDMETHOD( GetLight )
    STDMETHOD( LightEnable )
    STDMETHOD( GetLightEnable )
    STDMETHOD( SetClipPlane )
    STDMETHOD( GetClipPlane )
    STDMETHOD( SetRenderState )
    STDMETHOD( GetRenderState )
    STDMETHOD( CreateStateBlock )
    STDMETHOD( BeginStateBlock )
    STDMETHOD( EndStateBlock )
    STDMETHOD( SetClipStatus )
    STDMETHOD( GetClipStatus )
    STDMETHOD( GetTexture )
    STDMETHOD( SetTexture )
    STDMETHOD( GetTextureStageState )
    STDMETHOD( SetTextureStageState )
    STDMETHOD( GetSamplerState )
    STDMETHOD( SetSamplerState )
    STDMETHOD( ValidateDevice )
    STDMETHOD( SetPaletteEntries )
    STDMETHOD( GetPaletteEntries )
    STDMETHOD( SetCurrentTexturePalette )
    STDMETHOD( GetCurrentTexturePalette )
    STDMETHOD( SetScissorRect )
    STDMETHOD( GetScissorRect )
    STDMETHOD( SetSoftwareVertexProcessing )
    STDMETHOD_( BOOL, GetSoftwareVertexProcessing )
    STDMETHOD( SetNPatchMode )
    STDMETHOD_( float, GetNPatchMode )
    STDMETHOD( DrawPrimitive )
    STDMETHOD( DrawIndexedPrimitive )
    STDMETHOD( DrawPrimitiveUP )
    STDMETHOD( DrawIndexedPrimitiveUP )
    STDMETHOD( ProcessVertices )
    STDMETHOD( CreateVertexDeclaration )
    STDMETHOD( SetVertexDeclaration )
    STDMETHOD( GetVertexDeclaration )
    STDMETHOD( SetFVF )
    STDMETHOD( GetFVF )
    STDMETHOD( CreateVertexShader )
    STDMETHOD( SetVertexShader )
    STDMETHOD( GetVertexShader )
    STDMETHOD( SetVertexShaderConstantF )
    STDMETHOD( GetVertexShaderConstantF )
    STDMETHOD( SetVertexShaderConstantI )
    STDMETHOD( GetVertexShaderConstantI )
    STDMETHOD( SetVertexShaderConstantB )
    STDMETHOD( GetVertexShaderConstantB )
    STDMETHOD( SetStreamSource )
    STDMETHOD( GetStreamSource )
    STDMETHOD( SetStreamSourceFreq )
    STDMETHOD( GetStreamSourceFreq )
    STDMETHOD( SetIndices )
    STDMETHOD( GetIndices )
    STDMETHOD( CreatePixelShader )
    STDMETHOD( SetPixelShader )
    STDMETHOD( GetPixelShader )
    STDMETHOD( SetPixelShaderConstantF )
    STDMETHOD( GetPixelShaderConstantF )
    STDMETHOD( SetPixelShaderConstantI )
    STDMETHOD( GetPixelShaderConstantI )
    STDMETHOD( SetPixelShaderConstantB )
    STDMETHOD( GetPixelShaderConstantB )
    STDMETHOD( DrawRectPatch )
    STDMETHOD( DrawTriPatch )
    STDMETHOD( DeletePatch )
    STDMETHOD( CreateQuery )
};


#undef INTERFACE
#define INTERFACE IDirect3DDevice9Ex

enum PPCAT( vt, INTERFACE )
{
    // 1:1 copy from d3d9.h: replace (THIS* -> ""
    STDMETHOD( QueryInterface )
    STDMETHOD_( ULONG, AddRef )
    STDMETHOD_( ULONG, Release )
    STDMETHOD( TestCooperativeLevel )
    STDMETHOD_( UINT, GetAvailableTextureMem )
    STDMETHOD( EvictManagedResources )
    STDMETHOD( GetDirect3D )
    STDMETHOD( GetDeviceCaps )
    STDMETHOD( GetDisplayMode )
    STDMETHOD( GetCreationParameters )
    STDMETHOD( SetCursorProperties )
    STDMETHOD_( void, SetCursorPosition )
    STDMETHOD_( BOOL, ShowCursor )
    STDMETHOD( CreateAdditionalSwapChain )
    STDMETHOD( GetSwapChain )
    STDMETHOD_( UINT, GetNumberOfSwapChains )
    STDMETHOD( Reset )
    STDMETHOD( Present )
    STDMETHOD( GetBackBuffer )
    STDMETHOD( GetRasterStatus )
    STDMETHOD( SetDialogBoxMode )
    STDMETHOD_( void, SetGammaRamp )
    STDMETHOD_( void, GetGammaRamp )
    STDMETHOD( CreateTexture )
    STDMETHOD( CreateVolumeTexture )
    STDMETHOD( CreateCubeTexture )
    STDMETHOD( CreateVertexBuffer )
    STDMETHOD( CreateIndexBuffer )
    STDMETHOD( CreateRenderTarget )
    STDMETHOD( CreateDepthStencilSurface )
    STDMETHOD( UpdateSurface )
    STDMETHOD( UpdateTexture )
    STDMETHOD( GetRenderTargetData )
    STDMETHOD( GetFrontBufferData )
    STDMETHOD( StretchRect )
    STDMETHOD( ColorFill )
    STDMETHOD( CreateOffscreenPlainSurface )
    STDMETHOD( SetRenderTarget )
    STDMETHOD( GetRenderTarget )
    STDMETHOD( SetDepthStencilSurface )
    STDMETHOD( GetDepthStencilSurface )
    STDMETHOD( BeginScene )
    STDMETHOD( EndScene )
    STDMETHOD( Clear )
    STDMETHOD( SetTransform )
    STDMETHOD( GetTransform )
    STDMETHOD( MultiplyTransform )
    STDMETHOD( SetViewport )
    STDMETHOD( GetViewport )
    STDMETHOD( SetMaterial )
    STDMETHOD( GetMaterial )
    STDMETHOD( SetLight )
    STDMETHOD( GetLight )
    STDMETHOD( LightEnable )
    STDMETHOD( GetLightEnable )
    STDMETHOD( SetClipPlane )
    STDMETHOD( GetClipPlane )
    STDMETHOD( SetRenderState )
    STDMETHOD( GetRenderState )
    STDMETHOD( CreateStateBlock )
    STDMETHOD( BeginStateBlock )
    STDMETHOD( EndStateBlock )
    STDMETHOD( SetClipStatus )
    STDMETHOD( GetClipStatus )
    STDMETHOD( GetTexture )
    STDMETHOD( SetTexture )
    STDMETHOD( GetTextureStageState )
    STDMETHOD( SetTextureStageState )
    STDMETHOD( GetSamplerState )
    STDMETHOD( SetSamplerState )
    STDMETHOD( ValidateDevice )
    STDMETHOD( SetPaletteEntries )
    STDMETHOD( GetPaletteEntries )
    STDMETHOD( SetCurrentTexturePalette )
    STDMETHOD( GetCurrentTexturePalette )
    STDMETHOD( SetScissorRect )
    STDMETHOD( GetScissorRect )
    STDMETHOD( SetSoftwareVertexProcessing )
    STDMETHOD_( BOOL, GetSoftwareVertexProcessing )
    STDMETHOD( SetNPatchMode )
    STDMETHOD_( float, GetNPatchMode )
    STDMETHOD( DrawPrimitive )
    STDMETHOD( DrawIndexedPrimitive )
    STDMETHOD( DrawPrimitiveUP )
    STDMETHOD( DrawIndexedPrimitiveUP )
    STDMETHOD( ProcessVertices )
    STDMETHOD( CreateVertexDeclaration )
    STDMETHOD( SetVertexDeclaration )
    STDMETHOD( GetVertexDeclaration )
    STDMETHOD( SetFVF )
    STDMETHOD( GetFVF )
    STDMETHOD( CreateVertexShader )
    STDMETHOD( SetVertexShader )
    STDMETHOD( GetVertexShader )
    STDMETHOD( SetVertexShaderConstantF )
    STDMETHOD( GetVertexShaderConstantF )
    STDMETHOD( SetVertexShaderConstantI )
    STDMETHOD( GetVertexShaderConstantI )
    STDMETHOD( SetVertexShaderConstantB )
    STDMETHOD( GetVertexShaderConstantB )
    STDMETHOD( SetStreamSource )
    STDMETHOD( GetStreamSource )
    STDMETHOD( SetStreamSourceFreq )
    STDMETHOD( GetStreamSourceFreq )
    STDMETHOD( SetIndices )
    STDMETHOD( GetIndices )
    STDMETHOD( CreatePixelShader )
    STDMETHOD( SetPixelShader )
    STDMETHOD( GetPixelShader )
    STDMETHOD( SetPixelShaderConstantF )
    STDMETHOD( GetPixelShaderConstantF )
    STDMETHOD( SetPixelShaderConstantI )
    STDMETHOD( GetPixelShaderConstantI )
    STDMETHOD( SetPixelShaderConstantB )
    STDMETHOD( GetPixelShaderConstantB )
    STDMETHOD( DrawRectPatch )
    STDMETHOD( DrawTriPatch )
    STDMETHOD( DeletePatch )
    STDMETHOD( CreateQuery )
    STDMETHOD( SetConvolutionMonoKernel )
    STDMETHOD( ComposeRects )
    STDMETHOD( PresentEx )
    STDMETHOD( GetGPUThreadPriority )
    STDMETHOD( SetGPUThreadPriority )
    STDMETHOD( WaitForVBlank )
    STDMETHOD( CheckResourceResidency )
    STDMETHOD( SetMaximumFrameLatency )
    STDMETHOD( GetMaximumFrameLatency )
    STDMETHOD( CheckDeviceState )
    STDMETHOD( CreateRenderTargetEx )
    STDMETHOD( CreateOffscreenPlainSurfaceEx )
    STDMETHOD( CreateDepthStencilSurfaceEx )
    STDMETHOD( ResetEx )
    STDMETHOD( GetDisplayModeEx )
};
