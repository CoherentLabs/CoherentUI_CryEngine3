/* D3D_Plugin - for licensing and copyright see license.txt */

#include <Windows.h>
#include "pp_tools.h"
#include <concrt.h>

#pragma once

#define getVT(ptr) (IsBadWritePtr(ptr, sizeof(INT_PTR)) ? NULL : *(void***)ptr)
#define changeVT(n, target) changeVTEx(getVT(This), n, target)

#define MF_PTR(x) (*(void**)((void*)(&x)))
#define MF_DATA(x) ((void*)x)

// variant that optionally doesn't remember original function (e.g. if vtables are patched by libraries)
#define _hookVT(ptr, INTERFACE, METHOD, REMEMBER) \
    if(getVT(ptr)[PPCAT(PPCAT(PPCAT(vt, INTERFACE),_), METHOD)] != PPCAT(PPCAT(PPCAT(fh, INTERFACE),_), METHOD)) \
    { \
        if(REMEMBER) \
        { \
            MF_PTR(PPCAT(PPCAT(PPCAT(fo, INTERFACE),_), METHOD)) = getVT(ptr)[PPCAT(PPCAT(PPCAT(vt, INTERFACE),_), METHOD)];\
        } \
        changeVTEx(getVT(ptr), PPCAT(PPCAT(PPCAT(vt, INTERFACE),_), METHOD), PPCAT(PPCAT(PPCAT(fh, INTERFACE),_), METHOD));\
    }

// variant that is used to initially remember original functions (after that well only call those initially remembered function)
#define hookVT(ptr, INTERFACE, METHOD) \
    {\
        Concurrency::critical_section::scoped_lock lock( PPCAT(PPCAT(PPCAT(cs, INTERFACE),_), METHOD) ); \
        _hookVT(ptr, INTERFACE, METHOD, true); \
    }

#define unhookVT(ptr, INTERFACE, METHOD, REMEMBER) \
    {\
        Concurrency::critical_section::scoped_lock lock( PPCAT(PPCAT(PPCAT(cs, INTERFACE),_), METHOD) ); \
        if(REMEMBER) \
        { \
            if(getVT(ptr)[PPCAT(PPCAT(PPCAT(vt, INTERFACE),_), METHOD)] != PPCAT(PPCAT(PPCAT(fh, INTERFACE),_), METHOD)) \
            { \
                MF_PTR(PPCAT(PPCAT(PPCAT(fo, INTERFACE),_), METHOD)) = getVT(ptr)[PPCAT(PPCAT(PPCAT(vt, INTERFACE),_), METHOD)];\
            } \
        } \
        changeVTEx(getVT(ptr), PPCAT(PPCAT(PPCAT(vt, INTERFACE),_), METHOD), PPCAT(PPCAT(PPCAT(fo, INTERFACE),_), METHOD));\
    }

// variant that will rehook the vtable if it has been patched by someone (but won't update remembered function)
#define rehookVT(ptr, INTERFACE, METHOD) \
    {\
        Concurrency::critical_section::scoped_lock lock( PPCAT(PPCAT(PPCAT(cs, INTERFACE),_), METHOD) ); \
        if(getVT(ptr)[PPCAT(PPCAT(PPCAT(vt, INTERFACE),_), METHOD)] != PPCAT(PPCAT(PPCAT(fh, INTERFACE),_), METHOD)) \
        { \
            _hookVT(ptr, INTERFACE, METHOD, false);\
        } \
    }

#define GEN_HOOK_(TYPE, ...) \
    Concurrency::critical_section PPCAT(PPCAT(PPCAT(cs, INTERFACE),_), METHOD);\
    PPCAT(PPCAT(PPCAT(f, INTERFACE), _), METHOD) PPCAT(PPCAT(PPCAT(fo, INTERFACE),_), METHOD) = NULL;\
    TYPE STDMETHODCALLTYPE PPCAT(PPCAT(PPCAT(fh, INTERFACE),_), METHOD)(INTERFACE FAR* This, __VA_ARGS__)

#define GEN_HOOK(...) \
    GEN_HOOK_(HRESULT, __VA_ARGS__)

#define UNHOOK \
    unhookVT(This, INTERFACE, METHOD)

#define REHOOK \
    rehookVT(This, INTERFACE, METHOD)

#define CALL_ORGINAL_(RET, ...) \
    PPCAT(PPCAT(PPCAT(cs, INTERFACE),_), METHOD).lock(); \
    assert("Please mail hendrikpolczyn at gmail dot com this error message: SO1 " && (MF_PTR(PPCAT(PPCAT(PPCAT(fo, INTERFACE),_), METHOD)) != PPCAT(PPCAT(PPCAT(fh, INTERFACE),_), METHOD))); \
    RET (PPCAT((This)->*,((PPCAT(PPCAT(PPCAT(f, INTERFACE),_), METHOD))PPCAT(PPCAT(PPCAT(fo, INTERFACE),_), METHOD))))(__VA_ARGS__); \
    PPCAT(PPCAT(PPCAT(cs, INTERFACE),_), METHOD).unlock();

#define CALL_ORGINAL(...) \
    CALL_ORGINAL_(HRESULT hr =, __VA_ARGS__)

#define DECLARE_PARAMS(pthis,...) (__VA_ARGS__)

#define DECLARE_FUNCTION_PTR(method)            DECLARE_MEMBER_FUNCTION_PTR_(HRESULT,method)
#define DECLARE_FUNCTION_PTR_(rettype,method)   DECLARE_MEMBER_FUNCTION_PTR_(rettype,method)

#define DECLARE_MEMBER_FUNCTION_PTR(method)         DECLARE_MEMBER_FUNCTION_PTR_(HRESULT,method)

#define DECLARE_MEMBER_FUNCTION_PTR_(rettype,method) \
    typedef rettype (STDMETHODCALLTYPE INTERFACE::*PPCAT(PPCAT(PPCAT(f, INTERFACE),_), method))


template<typename func, typename vttype> bool changeVTEx( void** ppVt, vttype n, func pfTarget )
{
    DWORD dwOldProtections = 0;
    bool bRet = false;

    if ( ppVt && !IsBadReadPtr( ppVt, sizeof( LPCVOID ) ) && !IsBadReadPtr( &ppVt[n], sizeof( LPCVOID ) ) )
    {
        if ( VirtualProtect( &ppVt[n], sizeof( LPCVOID ), PAGE_EXECUTE_READWRITE, &dwOldProtections ) )
        {
            if ( !IsBadWritePtr( &ppVt[n], sizeof( LPCVOID ) ) )
            {
                ppVt[n] = MF_PTR( pfTarget );
                bRet = true;
            }

            // restore old access rights
            VirtualProtect( &ppVt[n], sizeof( LPCVOID ), dwOldProtections, &dwOldProtections );

            // flush instructions since vtable changed
            FlushInstructionCache ( GetCurrentProcess (), NULL, 0 );
        }
    }

    return bRet;
}

#define D3D_TARGETDX9 TEXT("dx9")
#define D3D_TARGETDX11 TEXT("dx11")

typedef bool( *ptGetVTData )( INT_PTR*, int, void* );

template<typename baseiface_type>
bool CheckForInterface( void* pBase, INT_PTR* pData, int nDatalen, IID guidInterface, int nVTEntries )
{
    void* tmp = pBase;

    if ( !IsBadReadPtr( tmp, sizeof( INT_PTR ) ) )
    {
        // Ptr OK
        tmp = *( void** )tmp;

        if ( !IsBadReadPtr( tmp, sizeof( INT_PTR ) ) )
        {
            // Device Vtable Ptr OK
            tmp = getVT( tmp );

            if ( !IsBadReadPtr( tmp, sizeof( INT_PTR )*nVTEntries ) )
            {
                // Device Vtable Ptr OK
                if ( 0 == memcmp( pData, tmp, nDatalen ) )
                {
                    // IUnknown as expected
                    baseiface_type* unk = *( baseiface_type** )pBase;
                    void*       p_device = NULL;

                    if ( SUCCEEDED( unk->QueryInterface( guidInterface, &p_device ) ) )
                    {
                        if ( p_device )
                        {
                            // device found
                            unk->Release();
                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}

template<typename baseiface_type>
void* ScanRange( INT_PTR start, size_t len, INT_PTR* pData, int nDatalen, IID guidInterface, int nVTEntries )
{
    INT_PTR search = start;
    INT_PTR end = search + len;

    for ( ; search < end; ++search )
    {
        // Search IUnknown vtable
        if ( CheckForInterface<baseiface_type>( ( void* )search, pData, nDatalen, guidInterface, nVTEntries ) )
        {
            return ( void* )search;
        }
    }

    return NULL;
}

#define D3D_REGISTRY TEXT("SOFTWARE\\CE3Plugins\\Direct3DDevice")
#define D3D_TEXTURE TEXT("t")
#define D3D_OFFSET TEXT("offset")
#define D3D_DATA TEXT("data")
#define D3D_TARGETX86 TEXT("x86")
#define D3D_TARGETX64 TEXT("x64")
#define SEP TEXT("_")

/**
* @brief Function to find pointers to IUnknown based interfaces in a memory range
*/
template<typename iface_type, typename baseiface_type>
iface_type* FindInterface(
    INT_PTR nModuleOffset, INT_PTR nRelativeBase, int nFunctioncount, int nRange,
    LPCTSTR sSubKeyData, INT_PTR* pVTData, DWORD nVTDataSize,
    LPCTSTR sSubKeyOffset, INT_PTR pPointerOffset, DWORD nPointerOffsetSize, ptGetVTData pfGetVTData, void* pParam = NULL )
{
    HKEY    hKey;
    RegCreateKeyEx(
        HKEY_CURRENT_USER,
        D3D_REGISTRY,
        0,
        NULL,
        0,
        KEY_ALL_ACCESS,
        NULL,
        &hKey,
        NULL );

    RegQueryValueEx( hKey, sSubKeyData, 0, NULL, ( LPBYTE )pVTData, &nVTDataSize );
    RegQueryValueEx( hKey, sSubKeyOffset, 0, NULL, ( LPBYTE )&pPointerOffset, &nPointerOffsetSize );

    // Check Defaults or Rescan
    void* pInterfaceClass = ( void* )( nRelativeBase + pPointerOffset );

    // Calculate Offsets of IUnknown Interface VTable
    pVTData[0] += nModuleOffset;
    pVTData[1] += nModuleOffset;
    pVTData[2] += nModuleOffset;

    IID guidInterface = __uuidof( iface_type );

    if ( !CheckForInterface<baseiface_type>( pInterfaceClass, pVTData, nVTDataSize, guidInterface, nFunctioncount ) )
    {
        // Scan for new Offset
        if ( pInterfaceClass = ScanRange<baseiface_type>( nRelativeBase, nRange, pVTData, nVTDataSize, guidInterface, nFunctioncount ) )
        {
            goto Save_Offset;
        }

        // Get current IUnknown Interface VTable
        if ( !( ( *pfGetVTData )( pVTData, nVTDataSize, pParam ) ) )
        {
            goto Exit_Get;    // Could not get device data
        }

        // Try again with old offset
        pInterfaceClass = ( void* )( nRelativeBase + pPointerOffset );

        if ( !CheckForInterface<baseiface_type>( pInterfaceClass, pVTData, nVTDataSize, guidInterface, nFunctioncount ) )
        {
            // Scan for new Offset
            if ( pInterfaceClass = ScanRange<baseiface_type>( nRelativeBase, nRange, pVTData, nVTDataSize, guidInterface, nFunctioncount ) )
            {
                goto Save_Data;
            }
        }

        else
        {
            goto Save_Data;
        }
    }

    goto Exit_Get; // Data is OK/totally invalid no save required

Save_Data: // IUnknown Interface VTable Offsets
    pVTData[0] -= nModuleOffset;
    pVTData[1] -= nModuleOffset;
    pVTData[2] -= nModuleOffset;
    RegSetValueEx( hKey, sSubKeyData, 0, REG_BINARY, ( LPBYTE )pVTData, nVTDataSize );

Save_Offset: // Interface  Offset
    pPointerOffset = ( ( INT_PTR )pInterfaceClass ) - nRelativeBase;
    RegSetValueEx( hKey, sSubKeyOffset, 0, REG_BINARY, ( LPBYTE )&pPointerOffset, nPointerOffsetSize );

Exit_Get:
    RegCloseKey( hKey );

    // Interface Pointer OK?
    if ( !IsBadReadPtr( pInterfaceClass, sizeof( INT_PTR ) ) )
    {
        return *( iface_type** )pInterfaceClass;
    }

    return NULL;
}

enum HookTextureMode
{
    HTM_NONE = 0,
    HTM_INJECT,
    HTM_CREATE,
};
