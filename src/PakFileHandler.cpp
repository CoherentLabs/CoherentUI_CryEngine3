/* CoherentUI_Plugin - for licensing and copyright see license.txt */


#include "StdAfx.h"
#include "PakFileHandler.h"

#include <platform.h>
#include <ICryPak.h>

#include <CPluginCoherentUI.h>
#include <IPluginManager.h>
#include <PMUtils.hpp>

namespace CoherentUIPlugin
{
    PakFileHandler::PakFileHandler()
    {
    }

    void PakFileHandler::ReadFile( const wchar_t* sURL, Coherent::UI::ResourceResponse* pResponse )
    {
        if ( !sURL )
        {
            return;
        }

        FILE* fHandle = NULL;

        // Use only the netloc of the url for resouce name, ignoring the scheme, any queries and fragment parts
        Coherent::UI::URLComponent sPathURL;

        if ( ( !CoherentGetURLParser().Parse( sURL, nullptr, &sPathURL, nullptr, nullptr ) && sPathURL.Start ) )
        {
            gPlugin->LogWarning( "ReadFile() Couldn't parse URL:", PluginManager::UCS22UTF8( wstring( sURL ) ).c_str() );
            pResponse->SignalFailure();
            return;
        }

        // CryPak uses UTF-8
        wstring sPathW( sPathURL.Start, sPathURL.Start + sPathURL.Length );
        string sPath = PluginManager::UCS22UTF8( sPathW );

        // Open File
        if ( ( fHandle = gEnv->pCryPak->FOpen( sPath, "rb" ) ) == NULL )
        {
            gPlugin->LogWarning( "ReadFile(%s) Unable to find specified path in pak", sPath.c_str() );
            pResponse->SignalFailure();
            goto End;
        }

        size_t nSize = gEnv->pCryPak->FGetSize( fHandle );

        if ( !nSize )
        {
            gPlugin->LogWarning( "ReadFile(%s) File is empty", sPath.c_str() );
            pResponse->SignalFailure( );
            goto End;
        }

        void* pBuffer = pResponse->GetBuffer( nSize );

        if ( !pBuffer )
        {
            gPlugin->LogError( "ReadFile(%s) Unable to obtain buffer for this resource (%d bytes)", sPath.c_str(), int( nSize ) );
            pResponse->SignalFailure( );
            goto End;
        }

        if ( nSize != gEnv->pCryPak->FReadRawAll( pBuffer, nSize, fHandle ) )
        {
            gPlugin->LogError( "ReadFile(%s) Unable to read all data (%d bytes)", sPath.c_str(), int( nSize ) );
            pResponse->SignalFailure( );
            goto End;
        }

        pResponse->SignalSuccess( );

End:

        if ( fHandle )
        {
            gEnv->pCryPak->FClose( fHandle );
        }
    }

    void PakFileHandler::WriteFile( const wchar_t* sURL, Coherent::UI::ResourceData* pResource )
    {
        pResource->SignalFailure();
    }
};