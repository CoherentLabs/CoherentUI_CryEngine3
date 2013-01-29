#include <d3d11.h>
#include <D3D11SDKLayers.h>

#include "../hooktools.h"

#pragma once

#undef THIS_
#define THIS_                   INTERFACE * This,
#undef THIS
#define THIS                    INTERFACE * This
#undef STDMETHOD
#define STDMETHOD(method)       DECLARE_FUNCTION_PTR(method)
#undef STDMETHOD_
#define STDMETHOD_(type,method) DECLARE_FUNCTION_PTR_(type,method)

// Create function pointer types
#undef INTERFACE
#define INTERFACE ID3D11Device

// from d3d11.h vtable:
STDMETHOD( QueryInterface )DECLARE_PARAMS(
    THIS,
    /* [in] */ REFIID riid,
    /* [annotation][iid_is][out] */
    __RPC__deref_out  void** ppvObject );

STDMETHOD_( ULONG, AddRef )DECLARE_PARAMS(
    THIS );

STDMETHOD_( ULONG, Release )DECLARE_PARAMS(
    THIS );

STDMETHOD( CreateBuffer )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  const D3D11_BUFFER_DESC* pDesc,
    /* [annotation] */
    __in_opt  const D3D11_SUBRESOURCE_DATA* pInitialData,
    /* [annotation] */
    __out_opt  ID3D11Buffer** ppBuffer );

STDMETHOD( CreateTexture1D )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  const D3D11_TEXTURE1D_DESC* pDesc,
    /* [annotation] */
    __in_xcount_opt( pDesc->MipLevels * pDesc->ArraySize )  const D3D11_SUBRESOURCE_DATA* pInitialData,
    /* [annotation] */
    __out_opt  ID3D11Texture1D** ppTexture1D );

STDMETHOD( CreateTexture2D )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  const D3D11_TEXTURE2D_DESC* pDesc,
    /* [annotation] */
    __in_xcount_opt( pDesc->MipLevels * pDesc->ArraySize )  const D3D11_SUBRESOURCE_DATA* pInitialData,
    /* [annotation] */
    __out_opt  ID3D11Texture2D** ppTexture2D );

STDMETHOD( CreateTexture3D )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  const D3D11_TEXTURE3D_DESC* pDesc,
    /* [annotation] */
    __in_xcount_opt( pDesc->MipLevels )  const D3D11_SUBRESOURCE_DATA* pInitialData,
    /* [annotation] */
    __out_opt  ID3D11Texture3D** ppTexture3D );

STDMETHOD( CreateShaderResourceView )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  ID3D11Resource* pResource,
    /* [annotation] */
    __in_opt  const D3D11_SHADER_RESOURCE_VIEW_DESC* pDesc,
    /* [annotation] */
    __out_opt  ID3D11ShaderResourceView** ppSRView );

STDMETHOD( CreateUnorderedAccessView )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  ID3D11Resource* pResource,
    /* [annotation] */
    __in_opt  const D3D11_UNORDERED_ACCESS_VIEW_DESC* pDesc,
    /* [annotation] */
    __out_opt  ID3D11UnorderedAccessView** ppUAView );

STDMETHOD( CreateRenderTargetView )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  ID3D11Resource* pResource,
    /* [annotation] */
    __in_opt  const D3D11_RENDER_TARGET_VIEW_DESC* pDesc,
    /* [annotation] */
    __out_opt  ID3D11RenderTargetView** ppRTView );

STDMETHOD( CreateDepthStencilView )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  ID3D11Resource* pResource,
    /* [annotation] */
    __in_opt  const D3D11_DEPTH_STENCIL_VIEW_DESC* pDesc,
    /* [annotation] */
    __out_opt  ID3D11DepthStencilView** ppDepthStencilView );

STDMETHOD( CreateInputLayout )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_ecount( NumElements )  const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs,
    /* [annotation] */
    __in_range( 0, D3D11_IA_VERTEX_INPUT_STRUCTURE_ELEMENT_COUNT )  UINT NumElements,
    /* [annotation] */
    __in  const void* pShaderBytecodeWithInputSignature,
    /* [annotation] */
    __in  SIZE_T BytecodeLength,
    /* [annotation] */
    __out_opt  ID3D11InputLayout** ppInputLayout );

STDMETHOD( CreateVertexShader )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  const void* pShaderBytecode,
    /* [annotation] */
    __in  SIZE_T BytecodeLength,
    /* [annotation] */
    __in_opt  ID3D11ClassLinkage* pClassLinkage,
    /* [annotation] */
    __out_opt  ID3D11VertexShader** ppVertexShader );

STDMETHOD( CreateGeometryShader )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  const void* pShaderBytecode,
    /* [annotation] */
    __in  SIZE_T BytecodeLength,
    /* [annotation] */
    __in_opt  ID3D11ClassLinkage* pClassLinkage,
    /* [annotation] */
    __out_opt  ID3D11GeometryShader** ppGeometryShader );

STDMETHOD( CreateGeometryShaderWithStreamOutput )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  const void* pShaderBytecode,
    /* [annotation] */
    __in  SIZE_T BytecodeLength,
    /* [annotation] */
    __in_ecount_opt( NumEntries )  const D3D11_SO_DECLARATION_ENTRY* pSODeclaration,
    /* [annotation] */
    __in_range( 0, D3D11_SO_STREAM_COUNT* D3D11_SO_OUTPUT_COMPONENT_COUNT )  UINT NumEntries,
    /* [annotation] */
    __in_ecount_opt( NumStrides )  const UINT* pBufferStrides,
    /* [annotation] */
    __in_range( 0, D3D11_SO_BUFFER_SLOT_COUNT )  UINT NumStrides,
    /* [annotation] */
    __in  UINT RasterizedStream,
    /* [annotation] */
    __in_opt  ID3D11ClassLinkage* pClassLinkage,
    /* [annotation] */
    __out_opt  ID3D11GeometryShader** ppGeometryShader );

STDMETHOD( CreatePixelShader )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  const void* pShaderBytecode,
    /* [annotation] */
    __in  SIZE_T BytecodeLength,
    /* [annotation] */
    __in_opt  ID3D11ClassLinkage* pClassLinkage,
    /* [annotation] */
    __out_opt  ID3D11PixelShader** ppPixelShader );

STDMETHOD( CreateHullShader )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  const void* pShaderBytecode,
    /* [annotation] */
    __in  SIZE_T BytecodeLength,
    /* [annotation] */
    __in_opt  ID3D11ClassLinkage* pClassLinkage,
    /* [annotation] */
    __out_opt  ID3D11HullShader** ppHullShader );

STDMETHOD( CreateDomainShader )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  const void* pShaderBytecode,
    /* [annotation] */
    __in  SIZE_T BytecodeLength,
    /* [annotation] */
    __in_opt  ID3D11ClassLinkage* pClassLinkage,
    /* [annotation] */
    __out_opt  ID3D11DomainShader** ppDomainShader );

STDMETHOD( CreateComputeShader )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  const void* pShaderBytecode,
    /* [annotation] */
    __in  SIZE_T BytecodeLength,
    /* [annotation] */
    __in_opt  ID3D11ClassLinkage* pClassLinkage,
    /* [annotation] */
    __out_opt  ID3D11ComputeShader** ppComputeShader );

STDMETHOD( CreateClassLinkage )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __out  ID3D11ClassLinkage** ppLinkage );

STDMETHOD( CreateBlendState )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  const D3D11_BLEND_DESC* pBlendStateDesc,
    /* [annotation] */
    __out_opt  ID3D11BlendState** ppBlendState );

STDMETHOD( CreateDepthStencilState )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  const D3D11_DEPTH_STENCIL_DESC* pDepthStencilDesc,
    /* [annotation] */
    __out_opt  ID3D11DepthStencilState** ppDepthStencilState );

STDMETHOD( CreateRasterizerState )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  const D3D11_RASTERIZER_DESC* pRasterizerDesc,
    /* [annotation] */
    __out_opt  ID3D11RasterizerState** ppRasterizerState );

STDMETHOD( CreateSamplerState )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  const D3D11_SAMPLER_DESC* pSamplerDesc,
    /* [annotation] */
    __out_opt  ID3D11SamplerState** ppSamplerState );

STDMETHOD( CreateQuery )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  const D3D11_QUERY_DESC* pQueryDesc,
    /* [annotation] */
    __out_opt  ID3D11Query** ppQuery );

STDMETHOD( CreatePredicate )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  const D3D11_QUERY_DESC* pPredicateDesc,
    /* [annotation] */
    __out_opt  ID3D11Predicate** ppPredicate );

STDMETHOD( CreateCounter )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  const D3D11_COUNTER_DESC* pCounterDesc,
    /* [annotation] */
    __out_opt  ID3D11Counter** ppCounter );

STDMETHOD( CreateDeferredContext )DECLARE_PARAMS(
    THIS,
    UINT ContextFlags,
    /* [annotation] */
    __out_opt  ID3D11DeviceContext** ppDeferredContext );

STDMETHOD( OpenSharedResource )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  HANDLE hResource,
    /* [annotation] */
    __in  REFIID ReturnedInterface,
    /* [annotation] */
    __out_opt  void** ppResource );

STDMETHOD( CheckFormatSupport )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  DXGI_FORMAT Format,
    /* [annotation] */
    __out  UINT* pFormatSupport );

STDMETHOD( CheckMultisampleQualityLevels )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  DXGI_FORMAT Format,
    /* [annotation] */
    __in  UINT SampleCount,
    /* [annotation] */
    __out  UINT* pNumQualityLevels );

STDMETHOD_( void, CheckCounterInfo )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __out  D3D11_COUNTER_INFO* pCounterInfo );

STDMETHOD( CheckCounter )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  const D3D11_COUNTER_DESC* pDesc,
    /* [annotation] */
    __out  D3D11_COUNTER_TYPE* pType,
    /* [annotation] */
    __out  UINT* pActiveCounters,
    /* [annotation] */
    __out_ecount_opt( *pNameLength )  LPSTR szName,
    /* [annotation] */
    __inout_opt  UINT* pNameLength,
    /* [annotation] */
    __out_ecount_opt( *pUnitsLength )  LPSTR szUnits,
    /* [annotation] */
    __inout_opt  UINT* pUnitsLength,
    /* [annotation] */
    __out_ecount_opt( *pDescriptionLength )  LPSTR szDescription,
    /* [annotation] */
    __inout_opt  UINT* pDescriptionLength );

STDMETHOD( CheckFeatureSupport )DECLARE_PARAMS(
    THIS,
    D3D11_FEATURE Feature,
    /* [annotation] */
    __out_bcount( FeatureSupportDataSize )  void* pFeatureSupportData,
    UINT FeatureSupportDataSize );

STDMETHOD( GetPrivateData )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  REFGUID guid,
    /* [annotation] */
    __inout  UINT* pDataSize,
    /* [annotation] */
    __out_bcount_opt( *pDataSize )  void* pData );

STDMETHOD( SetPrivateData )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  REFGUID guid,
    /* [annotation] */
    __in  UINT DataSize,
    /* [annotation] */
    __in_bcount_opt( DataSize )  const void* pData );

STDMETHOD( SetPrivateDataInterface )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  REFGUID guid,
    /* [annotation] */
    __in_opt  const IUnknown* pData );

STDMETHOD_( D3D_FEATURE_LEVEL, GetFeatureLevel )DECLARE_PARAMS(
    THIS );

STDMETHOD_( UINT, GetCreationFlags )DECLARE_PARAMS(
    THIS );

STDMETHOD( GetDeviceRemovedReason )DECLARE_PARAMS(
    THIS );

STDMETHOD_( void, GetImmediateContext )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __out  ID3D11DeviceContext** ppImmediateContext );

STDMETHOD( SetExceptionMode )DECLARE_PARAMS(
    THIS,
    UINT RaiseFlags );

STDMETHOD_( UINT, GetExceptionMode )DECLARE_PARAMS(
    THIS );


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
    STDMETHOD( CreateBuffer )
    STDMETHOD( CreateTexture1D )
    STDMETHOD( CreateTexture2D )
    STDMETHOD( CreateTexture3D )
    STDMETHOD( CreateShaderResourceView )
    STDMETHOD( CreateUnorderedAccessView )
    STDMETHOD( CreateRenderTargetView )
    STDMETHOD( CreateDepthStencilView )
    STDMETHOD( CreateInputLayout )
    STDMETHOD( CreateVertexShader )
    STDMETHOD( CreateGeometryShader )
    STDMETHOD( CreateGeometryShaderWithStreamOutput )
    STDMETHOD( CreatePixelShader )
    STDMETHOD( CreateHullShader )
    STDMETHOD( CreateDomainShader )
    STDMETHOD( CreateComputeShader )
    STDMETHOD( CreateClassLinkage )
    STDMETHOD( CreateBlendState )
    STDMETHOD( CreateDepthStencilState )
    STDMETHOD( CreateRasterizerState )
    STDMETHOD( CreateSamplerState )
    STDMETHOD( CreateQuery )
    STDMETHOD( CreatePredicate )
    STDMETHOD( CreateCounter )
    STDMETHOD( CreateDeferredContext )
    STDMETHOD( OpenSharedResource )
    STDMETHOD( CheckFormatSupport )
    STDMETHOD( CheckMultisampleQualityLevels )
    STDMETHOD_( void, CheckCounterInfo )
    STDMETHOD( CheckCounter )
    STDMETHOD( CheckFeatureSupport )
    STDMETHOD( GetPrivateData )
    STDMETHOD( SetPrivateData )
    STDMETHOD( SetPrivateDataInterface )
    STDMETHOD_( D3D_FEATURE_LEVEL, GetFeatureLevel )
    STDMETHOD_( UINT, GetCreationFlags )
    STDMETHOD( GetDeviceRemovedReason )
    STDMETHOD_( void, GetImmediateContext )
    STDMETHOD( SetExceptionMode )
    STDMETHOD_( UINT, GetExceptionMode )
};

#undef THIS_
#define THIS_                   INTERFACE * This,
#undef THIS
#define THIS                    INTERFACE * This
#undef STDMETHOD
#define STDMETHOD(method)       DECLARE_FUNCTION_PTR(method)
#undef STDMETHOD_
#define STDMETHOD_(type,method) DECLARE_FUNCTION_PTR_(type,method)

// Create function pointer types
#undef INTERFACE
#define INTERFACE ID3D11DeviceContext

// from d3d11.h vtable:
STDMETHOD( QueryInterface )DECLARE_PARAMS(
    THIS,
    /* [in] */ REFIID riid,
    /* [annotation][iid_is][out] */
    __RPC__deref_out  void** ppvObject );

STDMETHOD( AddRef )DECLARE_PARAMS(
    THIS );

STDMETHOD_( ULONG, Release )DECLARE_PARAMS(
    THIS );

STDMETHOD_( void, GetDevice )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __out  ID3D11Device** ppDevice );

STDMETHOD( GetPrivateData )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  REFGUID guid,
    /* [annotation] */
    __inout  UINT* pDataSize,
    /* [annotation] */
    __out_bcount_opt( *pDataSize )  void* pData );

STDMETHOD( SetPrivateData )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  REFGUID guid,
    /* [annotation] */
    __in  UINT DataSize,
    /* [annotation] */
    __in_bcount_opt( DataSize )  const void* pData );

STDMETHOD( SetPrivateDataInterface )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  REFGUID guid,
    /* [annotation] */
    __in_opt  const IUnknown* pData );

STDMETHOD_( void, VSSetConstantBuffers )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
    /* [annotation] */
    __in_ecount( NumBuffers )  ID3D11Buffer* const* ppConstantBuffers );

STDMETHOD_( void, PSSetShaderResources )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
    /* [annotation] */
    __in_ecount( NumViews )  ID3D11ShaderResourceView* const* ppShaderResourceViews );

STDMETHOD_( void, PSSetShader )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_opt  ID3D11PixelShader* pPixelShader,
    /* [annotation] */
    __in_ecount_opt( NumClassInstances )  ID3D11ClassInstance* const* ppClassInstances,
    UINT NumClassInstances );

STDMETHOD_( void, PSSetSamplers )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
    /* [annotation] */
    __in_ecount( NumSamplers )  ID3D11SamplerState* const* ppSamplers );

STDMETHOD_( void, VSSetShader )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_opt  ID3D11VertexShader* pVertexShader,
    /* [annotation] */
    __in_ecount_opt( NumClassInstances )  ID3D11ClassInstance* const* ppClassInstances,
    UINT NumClassInstances );

STDMETHOD_( void, DrawIndexed )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  UINT IndexCount,
    /* [annotation] */
    __in  UINT StartIndexLocation,
    /* [annotation] */
    __in  INT BaseVertexLocation );

STDMETHOD_( void, Draw )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  UINT VertexCount,
    /* [annotation] */
    __in  UINT StartVertexLocation );

STDMETHOD( Map )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  ID3D11Resource* pResource,
    /* [annotation] */
    __in  UINT Subresource,
    /* [annotation] */
    __in  D3D11_MAP MapType,
    /* [annotation] */
    __in  UINT MapFlags,
    /* [annotation] */
    __out  D3D11_MAPPED_SUBRESOURCE* pMappedResource );

STDMETHOD_( void, Unmap )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  ID3D11Resource* pResource,
    /* [annotation] */
    __in  UINT Subresource );

STDMETHOD_( void, PSSetConstantBuffers )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
    /* [annotation] */
    __in_ecount( NumBuffers )  ID3D11Buffer* const* ppConstantBuffers );

STDMETHOD_( void, IASetInputLayout )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_opt  ID3D11InputLayout* pInputLayout );

STDMETHOD_( void, IASetVertexBuffers )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_range( 0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */
    __in_range( 0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumBuffers,
    /* [annotation] */
    __in_ecount( NumBuffers )  ID3D11Buffer* const* ppVertexBuffers,
    /* [annotation] */
    __in_ecount( NumBuffers )  const UINT* pStrides,
    /* [annotation] */
    __in_ecount( NumBuffers )  const UINT* pOffsets );

STDMETHOD_( void, IASetIndexBuffer )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_opt  ID3D11Buffer* pIndexBuffer,
    /* [annotation] */
    __in  DXGI_FORMAT Format,
    /* [annotation] */
    __in  UINT Offset );

STDMETHOD_( void, DrawIndexedInstanced )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  UINT IndexCountPerInstance,
    /* [annotation] */
    __in  UINT InstanceCount,
    /* [annotation] */
    __in  UINT StartIndexLocation,
    /* [annotation] */
    __in  INT BaseVertexLocation,
    /* [annotation] */
    __in  UINT StartInstanceLocation );

STDMETHOD_( void, DrawInstanced )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  UINT VertexCountPerInstance,
    /* [annotation] */
    __in  UINT InstanceCount,
    /* [annotation] */
    __in  UINT StartVertexLocation,
    /* [annotation] */
    __in  UINT StartInstanceLocation );

STDMETHOD_( void, GSSetConstantBuffers )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
    /* [annotation] */
    __in_ecount( NumBuffers )  ID3D11Buffer* const* ppConstantBuffers );

STDMETHOD_( void, GSSetShader )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_opt  ID3D11GeometryShader* pShader,
    /* [annotation] */
    __in_ecount_opt( NumClassInstances )  ID3D11ClassInstance* const* ppClassInstances,
    UINT NumClassInstances );

STDMETHOD_( void, IASetPrimitiveTopology )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  D3D11_PRIMITIVE_TOPOLOGY Topology );

STDMETHOD_( void, VSSetShaderResources )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
    /* [annotation] */
    __in_ecount( NumViews )  ID3D11ShaderResourceView* const* ppShaderResourceViews );

STDMETHOD_( void, VSSetSamplers )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
    /* [annotation] */
    __in_ecount( NumSamplers )  ID3D11SamplerState* const* ppSamplers );

STDMETHOD_( void, Begin )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  ID3D11Asynchronous* pAsync );

STDMETHOD_( void, End )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  ID3D11Asynchronous* pAsync );

STDMETHOD( GetData )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  ID3D11Asynchronous* pAsync,
    /* [annotation] */
    __out_bcount_opt( DataSize )  void* pData,
    /* [annotation] */
    __in  UINT DataSize,
    /* [annotation] */
    __in  UINT GetDataFlags );

STDMETHOD_( void, SetPredication )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_opt  ID3D11Predicate* pPredicate,
    /* [annotation] */
    __in  BOOL PredicateValue );

STDMETHOD_( void, GSSetShaderResources )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
    /* [annotation] */
    __in_ecount( NumViews )  ID3D11ShaderResourceView* const* ppShaderResourceViews );

STDMETHOD_( void, GSSetSamplers )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
    /* [annotation] */
    __in_ecount( NumSamplers )  ID3D11SamplerState* const* ppSamplers );

STDMETHOD_( void, OMSetRenderTargets )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_range( 0, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT )  UINT NumViews,
    /* [annotation] */
    __in_ecount_opt( NumViews )  ID3D11RenderTargetView* const* ppRenderTargetViews,
    /* [annotation] */
    __in_opt  ID3D11DepthStencilView* pDepthStencilView );

STDMETHOD_( void, OMSetRenderTargetsAndUnorderedAccessViews )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  UINT NumRTVs,
    /* [annotation] */
    __in_ecount_opt( NumRTVs )  ID3D11RenderTargetView* const* ppRenderTargetViews,
    /* [annotation] */
    __in_opt  ID3D11DepthStencilView* pDepthStencilView,
    /* [annotation] */
    __in_range( 0, D3D11_PS_CS_UAV_REGISTER_COUNT - 1 )  UINT UAVStartSlot,
    /* [annotation] */
    __in  UINT NumUAVs,
    /* [annotation] */
    __in_ecount_opt( NumUAVs )  ID3D11UnorderedAccessView* const* ppUnorderedAccessViews,
    /* [annotation] */
    __in_ecount_opt( NumUAVs )  const UINT* pUAVInitialCounts );

STDMETHOD_( void, OMSetBlendState )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_opt  ID3D11BlendState* pBlendState,
    /* [annotation] */
    __in_opt  const FLOAT BlendFactor[ 4 ],
    /* [annotation] */
    __in  UINT SampleMask );

STDMETHOD_( void, OMSetDepthStencilState )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_opt  ID3D11DepthStencilState* pDepthStencilState,
    /* [annotation] */
    __in  UINT StencilRef );

STDMETHOD_( void, SOSetTargets )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_range( 0, D3D11_SO_BUFFER_SLOT_COUNT )  UINT NumBuffers,
    /* [annotation] */
    __in_ecount_opt( NumBuffers )  ID3D11Buffer* const* ppSOTargets,
    /* [annotation] */
    __in_ecount_opt( NumBuffers )  const UINT* pOffsets );

STDMETHOD_( void, DrawAuto )DECLARE_PARAMS(
    THIS );

STDMETHOD_( void, DrawIndexedInstancedIndirect )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  ID3D11Buffer* pBufferForArgs,
    /* [annotation] */
    __in  UINT AlignedByteOffsetForArgs );

STDMETHOD_( void, DrawInstancedIndirect )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  ID3D11Buffer* pBufferForArgs,
    /* [annotation] */
    __in  UINT AlignedByteOffsetForArgs );

STDMETHOD_( void, Dispatch )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  UINT ThreadGroupCountX,
    /* [annotation] */
    __in  UINT ThreadGroupCountY,
    /* [annotation] */
    __in  UINT ThreadGroupCountZ );

STDMETHOD_( void, DispatchIndirect )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  ID3D11Buffer* pBufferForArgs,
    /* [annotation] */
    __in  UINT AlignedByteOffsetForArgs );

STDMETHOD_( void, RSSetState )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_opt  ID3D11RasterizerState* pRasterizerState );

STDMETHOD_( void, RSSetViewports )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_range( 0, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE )  UINT NumViewports,
    /* [annotation] */
    __in_ecount_opt( NumViewports )  const D3D11_VIEWPORT* pViewports );

STDMETHOD_( void, RSSetScissorRects )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_range( 0, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE )  UINT NumRects,
    /* [annotation] */
    __in_ecount_opt( NumRects )  const D3D11_RECT* pRects );

STDMETHOD_( void, CopySubresourceRegion )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  ID3D11Resource* pDstResource,
    /* [annotation] */
    __in  UINT DstSubresource,
    /* [annotation] */
    __in  UINT DstX,
    /* [annotation] */
    __in  UINT DstY,
    /* [annotation] */
    __in  UINT DstZ,
    /* [annotation] */
    __in  ID3D11Resource* pSrcResource,
    /* [annotation] */
    __in  UINT SrcSubresource,
    /* [annotation] */
    __in_opt  const D3D11_BOX* pSrcBox );

STDMETHOD_( void, CopyResource )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  ID3D11Resource* pDstResource,
    /* [annotation] */
    __in  ID3D11Resource* pSrcResource );

STDMETHOD_( void, UpdateSubresource )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  ID3D11Resource* pDstResource,
    /* [annotation] */
    __in  UINT DstSubresource,
    /* [annotation] */
    __in_opt  const D3D11_BOX* pDstBox,
    /* [annotation] */
    __in  const void* pSrcData,
    /* [annotation] */
    __in  UINT SrcRowPitch,
    /* [annotation] */
    __in  UINT SrcDepthPitch );

STDMETHOD_( void, CopyStructureCount )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  ID3D11Buffer* pDstBuffer,
    /* [annotation] */
    __in  UINT DstAlignedByteOffset,
    /* [annotation] */
    __in  ID3D11UnorderedAccessView* pSrcView );

STDMETHOD_( void, ClearRenderTargetView )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  ID3D11RenderTargetView* pRenderTargetView,
    /* [annotation] */
    __in  const FLOAT ColorRGBA[ 4 ] );

STDMETHOD_( void, ClearUnorderedAccessViewUint )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  ID3D11UnorderedAccessView* pUnorderedAccessView,
    /* [annotation] */
    __in  const UINT Values[ 4 ] );

STDMETHOD_( void, ClearUnorderedAccessViewFloat )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  ID3D11UnorderedAccessView* pUnorderedAccessView,
    /* [annotation] */
    __in  const FLOAT Values[ 4 ] );

STDMETHOD_( void, ClearDepthStencilView )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  ID3D11DepthStencilView* pDepthStencilView,
    /* [annotation] */
    __in  UINT ClearFlags,
    /* [annotation] */
    __in  FLOAT Depth,
    /* [annotation] */
    __in  UINT8 Stencil );

STDMETHOD_( void, GenerateMips )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  ID3D11ShaderResourceView* pShaderResourceView );

STDMETHOD_( void, SetResourceMinLOD )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  ID3D11Resource* pResource,
    FLOAT MinLOD );

STDMETHOD_( FLOAT, GetResourceMinLOD )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  ID3D11Resource* pResource );

STDMETHOD_( void, ResolveSubresource )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  ID3D11Resource* pDstResource,
    /* [annotation] */
    __in  UINT DstSubresource,
    /* [annotation] */
    __in  ID3D11Resource* pSrcResource,
    /* [annotation] */
    __in  UINT SrcSubresource,
    /* [annotation] */
    __in  DXGI_FORMAT Format );

STDMETHOD_( void, ExecuteCommandList )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in  ID3D11CommandList* pCommandList,
    BOOL RestoreContextState );

STDMETHOD_( void, HSSetShaderResources )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
    /* [annotation] */
    __in_ecount( NumViews )  ID3D11ShaderResourceView* const* ppShaderResourceViews );

STDMETHOD_( void, HSSetShader )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_opt  ID3D11HullShader* pHullShader,
    /* [annotation] */
    __in_ecount_opt( NumClassInstances )  ID3D11ClassInstance* const* ppClassInstances,
    UINT NumClassInstances );

STDMETHOD_( void, HSSetSamplers )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
    /* [annotation] */
    __in_ecount( NumSamplers )  ID3D11SamplerState* const* ppSamplers );

STDMETHOD_( void, HSSetConstantBuffers )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
    /* [annotation] */
    __in_ecount( NumBuffers )  ID3D11Buffer* const* ppConstantBuffers );

STDMETHOD_( void, DSSetShaderResources )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
    /* [annotation] */
    __in_ecount( NumViews )  ID3D11ShaderResourceView* const* ppShaderResourceViews );

STDMETHOD_( void, DSSetShader )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_opt  ID3D11DomainShader* pDomainShader,
    /* [annotation] */
    __in_ecount_opt( NumClassInstances )  ID3D11ClassInstance* const* ppClassInstances,
    UINT NumClassInstances );

STDMETHOD_( void, DSSetSamplers )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
    /* [annotation] */
    __in_ecount( NumSamplers )  ID3D11SamplerState* const* ppSamplers );

STDMETHOD_( void, DSSetConstantBuffers )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
    /* [annotation] */
    __in_ecount( NumBuffers )  ID3D11Buffer* const* ppConstantBuffers );

STDMETHOD_( void, CSSetShaderResources )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
    /* [annotation] */
    __in_ecount( NumViews )  ID3D11ShaderResourceView* const* ppShaderResourceViews );

STDMETHOD_( void, CSSetUnorderedAccessViews )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_range( 0, D3D11_PS_CS_UAV_REGISTER_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */
    __in_range( 0, D3D11_PS_CS_UAV_REGISTER_COUNT - StartSlot )  UINT NumUAVs,
    /* [annotation] */
    __in_ecount( NumUAVs )  ID3D11UnorderedAccessView* const* ppUnorderedAccessViews,
    /* [annotation] */
    __in_ecount( NumUAVs )  const UINT* pUAVInitialCounts );

STDMETHOD_( void, CSSetShader )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_opt  ID3D11ComputeShader* pComputeShader,
    /* [annotation] */
    __in_ecount_opt( NumClassInstances )  ID3D11ClassInstance* const* ppClassInstances,
    UINT NumClassInstances );

STDMETHOD_( void, CSSetSamplers )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
    /* [annotation] */
    __in_ecount( NumSamplers )  ID3D11SamplerState* const* ppSamplers );

STDMETHOD_( void, CSSetConstantBuffers )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
    /* [annotation] */
    __in_ecount( NumBuffers )  ID3D11Buffer* const* ppConstantBuffers );

STDMETHOD_( void, VSGetConstantBuffers )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
    /* [annotation] */
    __out_ecount( NumBuffers )  ID3D11Buffer** ppConstantBuffers );

STDMETHOD_( void, PSGetShaderResources )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
    /* [annotation] */
    __out_ecount( NumViews )  ID3D11ShaderResourceView** ppShaderResourceViews );

STDMETHOD_( void, PSGetShader )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __out  ID3D11PixelShader** ppPixelShader,
    /* [annotation] */
    __out_ecount_opt( *pNumClassInstances )  ID3D11ClassInstance** ppClassInstances,
    /* [annotation] */
    __inout_opt  UINT* pNumClassInstances );

STDMETHOD_( void, PSGetSamplers )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
    /* [annotation] */
    __out_ecount( NumSamplers )  ID3D11SamplerState** ppSamplers );

STDMETHOD_( void, VSGetShader )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __out  ID3D11VertexShader** ppVertexShader,
    /* [annotation] */
    __out_ecount_opt( *pNumClassInstances )  ID3D11ClassInstance** ppClassInstances,
    /* [annotation] */
    __inout_opt  UINT* pNumClassInstances );

STDMETHOD_( void, PSGetConstantBuffers )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
    /* [annotation] */
    __out_ecount( NumBuffers )  ID3D11Buffer** ppConstantBuffers );

STDMETHOD_( void, IAGetInputLayout )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __out  ID3D11InputLayout** ppInputLayout );

STDMETHOD_( void, IAGetVertexBuffers )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_range( 0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */
    __in_range( 0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumBuffers,
    /* [annotation] */
    __out_ecount_opt( NumBuffers )  ID3D11Buffer** ppVertexBuffers,
    /* [annotation] */
    __out_ecount_opt( NumBuffers )  UINT* pStrides,
    /* [annotation] */
    __out_ecount_opt( NumBuffers )  UINT* pOffsets );

STDMETHOD_( void, IAGetIndexBuffer )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __out_opt  ID3D11Buffer** pIndexBuffer,
    /* [annotation] */
    __out_opt  DXGI_FORMAT* Format,
    /* [annotation] */
    __out_opt  UINT* Offset );

STDMETHOD_( void, GSGetConstantBuffers )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
    /* [annotation] */
    __out_ecount( NumBuffers )  ID3D11Buffer** ppConstantBuffers );

STDMETHOD_( void, GSGetShader )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __out  ID3D11GeometryShader** ppGeometryShader,
    /* [annotation] */
    __out_ecount_opt( *pNumClassInstances )  ID3D11ClassInstance** ppClassInstances,
    /* [annotation] */
    __inout_opt  UINT* pNumClassInstances );

STDMETHOD_( void, IAGetPrimitiveTopology )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __out  D3D11_PRIMITIVE_TOPOLOGY* pTopology );

STDMETHOD_( void, VSGetShaderResources )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
    /* [annotation] */
    __out_ecount( NumViews )  ID3D11ShaderResourceView** ppShaderResourceViews );

STDMETHOD_( void, VSGetSamplers )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
    /* [annotation] */
    __out_ecount( NumSamplers )  ID3D11SamplerState** ppSamplers );

STDMETHOD_( void, GetPredication )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __out_opt  ID3D11Predicate** ppPredicate,
    /* [annotation] */
    __out_opt  BOOL* pPredicateValue );

STDMETHOD_( void, GSGetShaderResources )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
    /* [annotation] */
    __out_ecount( NumViews )  ID3D11ShaderResourceView** ppShaderResourceViews );

STDMETHOD_( void, GSGetSamplers )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
    /* [annotation] */
    __out_ecount( NumSamplers )  ID3D11SamplerState** ppSamplers );

STDMETHOD_( void, OMGetRenderTargets )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_range( 0, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT )  UINT NumViews,
    /* [annotation] */
    __out_ecount_opt( NumViews )  ID3D11RenderTargetView** ppRenderTargetViews,
    /* [annotation] */
    __out_opt  ID3D11DepthStencilView** ppDepthStencilView );

STDMETHOD_( void, OMGetRenderTargetsAndUnorderedAccessViews )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_range( 0, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT )  UINT NumRTVs,
    /* [annotation] */
    __out_ecount_opt( NumRTVs )  ID3D11RenderTargetView** ppRenderTargetViews,
    /* [annotation] */
    __out_opt  ID3D11DepthStencilView** ppDepthStencilView,
    /* [annotation] */
    __in_range( 0, D3D11_PS_CS_UAV_REGISTER_COUNT - 1 )  UINT UAVStartSlot,
    /* [annotation] */
    __in_range( 0, D3D11_PS_CS_UAV_REGISTER_COUNT - UAVStartSlot )  UINT NumUAVs,
    /* [annotation] */
    __out_ecount_opt( NumUAVs )  ID3D11UnorderedAccessView** ppUnorderedAccessViews );

STDMETHOD_( void, OMGetBlendState )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __out_opt  ID3D11BlendState** ppBlendState,
    /* [annotation] */
    __out_opt  FLOAT BlendFactor[ 4 ],
    /* [annotation] */
    __out_opt  UINT* pSampleMask );

STDMETHOD_( void, OMGetDepthStencilState )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __out_opt  ID3D11DepthStencilState** ppDepthStencilState,
    /* [annotation] */
    __out_opt  UINT* pStencilRef );

STDMETHOD_( void, SOGetTargets )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_range( 0, D3D11_SO_BUFFER_SLOT_COUNT )  UINT NumBuffers,
    /* [annotation] */
    __out_ecount( NumBuffers )  ID3D11Buffer** ppSOTargets );

STDMETHOD_( void, RSGetState )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __out  ID3D11RasterizerState** ppRasterizerState );

STDMETHOD_( void, RSGetViewports )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __inout /*_range(0, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE )*/   UINT* pNumViewports,
    /* [annotation] */
    __out_ecount_opt( *pNumViewports )  D3D11_VIEWPORT* pViewports );

STDMETHOD_( void, RSGetScissorRects )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __inout /*_range(0, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE )*/   UINT* pNumRects,
    /* [annotation] */
    __out_ecount_opt( *pNumRects )  D3D11_RECT* pRects );

STDMETHOD_( void, HSGetShaderResources )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
    /* [annotation] */
    __out_ecount( NumViews )  ID3D11ShaderResourceView** ppShaderResourceViews );

STDMETHOD_( void, HSGetShader )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __out  ID3D11HullShader** ppHullShader,
    /* [annotation] */
    __out_ecount_opt( *pNumClassInstances )  ID3D11ClassInstance** ppClassInstances,
    /* [annotation] */
    __inout_opt  UINT* pNumClassInstances );

STDMETHOD_( void, HSGetSamplers )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
    /* [annotation] */
    __out_ecount( NumSamplers )  ID3D11SamplerState** ppSamplers );

STDMETHOD_( void, HSGetConstantBuffers )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
    /* [annotation] */
    __out_ecount( NumBuffers )  ID3D11Buffer** ppConstantBuffers );

STDMETHOD_( void, DSGetShaderResources )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
    /* [annotation] */
    __out_ecount( NumViews )  ID3D11ShaderResourceView** ppShaderResourceViews );

STDMETHOD_( void, DSGetShader )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __out  ID3D11DomainShader** ppDomainShader,
    /* [annotation] */
    __out_ecount_opt( *pNumClassInstances )  ID3D11ClassInstance** ppClassInstances,
    /* [annotation] */
    __inout_opt  UINT* pNumClassInstances );

STDMETHOD_( void, DSGetSamplers )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
    /* [annotation] */
    __out_ecount( NumSamplers )  ID3D11SamplerState** ppSamplers );

STDMETHOD_( void, DSGetConstantBuffers )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
    /* [annotation] */
    __out_ecount( NumBuffers )  ID3D11Buffer** ppConstantBuffers );

STDMETHOD_( void, CSGetShaderResources )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
    /* [annotation] */
    __out_ecount( NumViews )  ID3D11ShaderResourceView** ppShaderResourceViews );

STDMETHOD_( void, CSGetUnorderedAccessViews )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_range( 0, D3D11_PS_CS_UAV_REGISTER_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */
    __in_range( 0, D3D11_PS_CS_UAV_REGISTER_COUNT - StartSlot )  UINT NumUAVs,
    /* [annotation] */
    __out_ecount( NumUAVs )  ID3D11UnorderedAccessView** ppUnorderedAccessViews );

STDMETHOD_( void, CSGetShader )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __out  ID3D11ComputeShader** ppComputeShader,
    /* [annotation] */
    __out_ecount_opt( *pNumClassInstances )  ID3D11ClassInstance** ppClassInstances,
    /* [annotation] */
    __inout_opt  UINT* pNumClassInstances );

STDMETHOD_( void, CSGetSamplers )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
    /* [annotation] */
    __out_ecount( NumSamplers )  ID3D11SamplerState** ppSamplers );

STDMETHOD_( void, CSGetConstantBuffers )DECLARE_PARAMS(
    THIS,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */
    __in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
    /* [annotation] */
    __out_ecount( NumBuffers )  ID3D11Buffer** ppConstantBuffers );

STDMETHOD_( void, ClearState )DECLARE_PARAMS(
    THIS );

STDMETHOD_( void, Flush )DECLARE_PARAMS(
    THIS );

STDMETHOD_( D3D11_DEVICE_CONTEXT_TYPE, GetType )DECLARE_PARAMS(
    THIS );

STDMETHOD_( UINT, GetContextFlags )DECLARE_PARAMS(
    THIS );

STDMETHOD( FinishCommandList )DECLARE_PARAMS(
    THIS,
    BOOL RestoreDeferredContextState,
    /* [annotation] */
    __out_opt  ID3D11CommandList** ppCommandList );

// Create vtable offsets
#undef STDMETHOD
#define STDMETHOD(method) PPCAT(PPCAT(PPCAT(vt, INTERFACE),_), method),
#undef STDMETHOD_
#define STDMETHOD_(type,method) PPCAT(PPCAT(PPCAT(vt, INTERFACE),_), method),

enum PPCAT( vt, INTERFACE )
{
    STDMETHOD( QueryInterface )
    STDMETHOD( AddRef )
    STDMETHOD_( ULONG, Release )
    STDMETHOD_( void, GetDevice )
    STDMETHOD( GetPrivateData )
    STDMETHOD( SetPrivateData )
    STDMETHOD( SetPrivateDataInterface )
    STDMETHOD_( void, VSSetConstantBuffers )
    STDMETHOD_( void, PSSetShaderResources )
    STDMETHOD_( void, PSSetShader )
    STDMETHOD_( void, PSSetSamplers )
    STDMETHOD_( void, VSSetShader )
    STDMETHOD_( void, DrawIndexed )
    STDMETHOD_( void, Draw )
    STDMETHOD( Map )
    STDMETHOD_( void, Unmap )
    STDMETHOD_( void, PSSetConstantBuffers )
    STDMETHOD_( void, IASetInputLayout )
    STDMETHOD_( void, IASetVertexBuffers )
    STDMETHOD_( void, IASetIndexBuffer )
    STDMETHOD_( void, DrawIndexedInstanced )
    STDMETHOD_( void, DrawInstanced )
    STDMETHOD_( void, GSSetConstantBuffers )
    STDMETHOD_( void, GSSetShader )
    STDMETHOD_( void, IASetPrimitiveTopology )
    STDMETHOD_( void, VSSetShaderResources )
    STDMETHOD_( void, VSSetSamplers )
    STDMETHOD_( void, Begin )
    STDMETHOD_( void, End )
    STDMETHOD( GetData )
    STDMETHOD_( void, SetPredication )
    STDMETHOD_( void, GSSetShaderResources )
    STDMETHOD_( void, GSSetSamplers )
    STDMETHOD_( void, OMSetRenderTargets )
    STDMETHOD_( void, OMSetRenderTargetsAndUnorderedAccessViews )
    STDMETHOD_( void, OMSetBlendState )
    STDMETHOD_( void, OMSetDepthStencilState )
    STDMETHOD_( void, SOSetTargets )
    STDMETHOD_( void, DrawAuto )
    STDMETHOD_( void, DrawIndexedInstancedIndirect )
    STDMETHOD_( void, DrawInstancedIndirect )
    STDMETHOD_( void, Dispatch )
    STDMETHOD_( void, DispatchIndirect )
    STDMETHOD_( void, RSSetState )
    STDMETHOD_( void, RSSetViewports )
    STDMETHOD_( void, RSSetScissorRects )
    STDMETHOD_( void, CopySubresourceRegion )
    STDMETHOD_( void, CopyResource )
    STDMETHOD_( void, UpdateSubresource )
    STDMETHOD_( void, CopyStructureCount )
    STDMETHOD_( void, ClearRenderTargetView )
    STDMETHOD_( void, ClearUnorderedAccessViewUint )
    STDMETHOD_( void, ClearUnorderedAccessViewFloat )
    STDMETHOD_( void, ClearDepthStencilView )
    STDMETHOD_( void, GenerateMips )
    STDMETHOD_( void, SetResourceMinLOD )
    STDMETHOD_( FLOAT, GetResourceMinLOD )
    STDMETHOD_( void, ResolveSubresource )
    STDMETHOD_( void, ExecuteCommandList )
    STDMETHOD_( void, HSSetShaderResources )
    STDMETHOD_( void, HSSetShader )
    STDMETHOD_( void, HSSetSamplers )
    STDMETHOD_( void, HSSetConstantBuffers )
    STDMETHOD_( void, DSSetShaderResources )
    STDMETHOD_( void, DSSetShader )
    STDMETHOD_( void, DSSetSamplers )
    STDMETHOD_( void, DSSetConstantBuffers )
    STDMETHOD_( void, CSSetShaderResources )
    STDMETHOD_( void, CSSetUnorderedAccessViews )
    STDMETHOD_( void, CSSetShader )
    STDMETHOD_( void, CSSetSamplers )
    STDMETHOD_( void, CSSetConstantBuffers )
    STDMETHOD_( void, VSGetConstantBuffers )
    STDMETHOD_( void, PSGetShaderResources )
    STDMETHOD_( void, PSGetShader )
    STDMETHOD_( void, PSGetSamplers )
    STDMETHOD_( void, VSGetShader )
    STDMETHOD_( void, PSGetConstantBuffers )
    STDMETHOD_( void, IAGetInputLayout )
    STDMETHOD_( void, IAGetVertexBuffers )
    STDMETHOD_( void, IAGetIndexBuffer )
    STDMETHOD_( void, GSGetConstantBuffers )
    STDMETHOD_( void, GSGetShader )
    STDMETHOD_( void, IAGetPrimitiveTopology )
    STDMETHOD_( void, VSGetShaderResources )
    STDMETHOD_( void, VSGetSamplers )
    STDMETHOD_( void, GetPredication )
    STDMETHOD_( void, GSGetShaderResources )
    STDMETHOD_( void, GSGetSamplers )
    STDMETHOD_( void, OMGetRenderTargets )
    STDMETHOD_( void, OMGetRenderTargetsAndUnorderedAccessViews )
    STDMETHOD_( void, OMGetBlendState )
    STDMETHOD_( void, OMGetDepthStencilState )
    STDMETHOD_( void, SOGetTargets )
    STDMETHOD_( void, RSGetState )
    STDMETHOD_( void, RSGetViewports )
    STDMETHOD_( void, RSGetScissorRects )
    STDMETHOD_( void, HSGetShaderResources )
    STDMETHOD_( void, HSGetShader )
    STDMETHOD_( void, HSGetSamplers )
    STDMETHOD_( void, HSGetConstantBuffers )
    STDMETHOD_( void, DSGetShaderResources )
    STDMETHOD_( void, DSGetShader )
    STDMETHOD_( void, DSGetSamplers )
    STDMETHOD_( void, DSGetConstantBuffers )
    STDMETHOD_( void, CSGetShaderResources )
    STDMETHOD_( void, CSGetUnorderedAccessViews )
    STDMETHOD_( void, CSGetShader )
    STDMETHOD_( void, CSGetSamplers )
    STDMETHOD_( void, CSGetConstantBuffers )
    STDMETHOD_( void, ClearState )
    STDMETHOD_( void, Flush )
    STDMETHOD_( D3D11_DEVICE_CONTEXT_TYPE, GetType )
    STDMETHOD_( UINT, GetContextFlags )
    STDMETHOD( FinishCommandList )
};

#undef THIS_
#define THIS_                   INTERFACE * This,
#undef THIS
#define THIS                    INTERFACE * This
#undef STDMETHOD
#define STDMETHOD(method)       DECLARE_FUNCTION_PTR(method)
#undef STDMETHOD_
#define STDMETHOD_(type,method) DECLARE_FUNCTION_PTR_(type,method)

// Create function pointer types
#undef INTERFACE
#define INTERFACE IDXGISwapChain

STDMETHOD( QueryInterface )DECLARE_PARAMS(
    THIS,
    /* [in] */ REFIID riid,
    /* [annotation][iid_is][out] */
    __RPC__deref_out  void** ppvObject );

STDMETHOD_( ULONG, AddRef )DECLARE_PARAMS(
    THIS );

STDMETHOD_( ULONG, Release )DECLARE_PARAMS(
    THIS );

STDMETHOD( SetPrivateData )DECLARE_PARAMS(
    THIS,
    /* [in] */ REFGUID Name,
    /* [in] */ UINT DataSize,
    /* [in] */ const void* pData );

STDMETHOD( SetPrivateDataInterface )DECLARE_PARAMS(
    THIS,
    /* [in] */ REFGUID Name,
    /* [in] */ const IUnknown* pUnknown );

STDMETHOD( GetPrivateData )DECLARE_PARAMS(
    THIS,
    /* [in] */ REFGUID Name,
    /* [out][in] */ UINT* pDataSize,
    /* [out] */ void* pData );

STDMETHOD( GetParent )DECLARE_PARAMS(
    THIS,
    /* [in] */ REFIID riid,
    /* [retval][out] */ void** ppParent );

STDMETHOD( GetDevice )DECLARE_PARAMS(
    THIS,
    /* [in] */ REFIID riid,
    /* [retval][out] */ void** ppDevice );

STDMETHOD( Present )DECLARE_PARAMS(
    THIS,
    /* [in] */ UINT SyncInterval,
    /* [in] */ UINT Flags );

STDMETHOD( GetBuffer )DECLARE_PARAMS(
    THIS,
    /* [in] */ UINT Buffer,
    /* [in] */ REFIID riid,
    /* [out][in] */ void** ppSurface );

STDMETHOD( SetFullscreenState )DECLARE_PARAMS(
    THIS,
    /* [in] */ BOOL Fullscreen,
    /* [in] */ IDXGIOutput* pTarget );

STDMETHOD( GetFullscreenState )DECLARE_PARAMS(
    THIS,
    /* [out] */ BOOL* pFullscreen,
    /* [out] */ IDXGIOutput** ppTarget );

STDMETHOD( GetDesc )DECLARE_PARAMS(
    THIS,
    /* [out] */ DXGI_SWAP_CHAIN_DESC* pDesc );

STDMETHOD( ResizeBuffers )DECLARE_PARAMS(
    THIS,
    /* [in] */ UINT BufferCount,
    /* [in] */ UINT Width,
    /* [in] */ UINT Height,
    /* [in] */ DXGI_FORMAT NewFormat,
    /* [in] */ UINT SwapChainFlags );

STDMETHOD( ResizeTarget )DECLARE_PARAMS(
    THIS,
    /* [in] */ const DXGI_MODE_DESC* pNewTargetParameters );

STDMETHOD( GetContainingOutput )DECLARE_PARAMS(
    THIS,
    IDXGIOutput** ppOutput );

STDMETHOD( GetFrameStatistics )DECLARE_PARAMS(
    THIS,
    /* [out] */ DXGI_FRAME_STATISTICS* pStats );

STDMETHOD( GetLastPresentCount )DECLARE_PARAMS(
    THIS,
    /* [out] */ UINT* pLastPresentCount );

// Create vtable offsets
#undef STDMETHOD
#define STDMETHOD(method) PPCAT(PPCAT(PPCAT(vt, INTERFACE),_), method),
#undef STDMETHOD_
#define STDMETHOD_(type,method) PPCAT(PPCAT(PPCAT(vt, INTERFACE),_), method),

enum PPCAT( vt, INTERFACE )
{
    STDMETHOD( QueryInterface )
    STDMETHOD_( ULONG, AddRef )
    STDMETHOD_( ULONG, Release )
    STDMETHOD( SetPrivateData )
    STDMETHOD( SetPrivateDataInterface )
    STDMETHOD( GetPrivateData )
    STDMETHOD( GetParent )
    STDMETHOD( GetDevice )
    STDMETHOD( Present )
    STDMETHOD( GetBuffer )
    STDMETHOD( SetFullscreenState )
    STDMETHOD( GetFullscreenState )
    STDMETHOD( GetDesc )
    STDMETHOD( ResizeBuffers )
    STDMETHOD( ResizeTarget )
    STDMETHOD( GetContainingOutput )
    STDMETHOD( GetFrameStatistics )
    STDMETHOD( GetLastPresentCount )
};