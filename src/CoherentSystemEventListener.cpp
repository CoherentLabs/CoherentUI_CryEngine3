#include "StdAfx.h"
#include "CoherentSystemEventListener.h"
#include "CoherentUISystem.h"

namespace CoherentUIPlugin
{

    CCoherentSystemEventListener::CCoherentSystemEventListener( CCoherentUISystem* pUISystem )
        : m_IsReady( false )
        , m_pUISystem( pUISystem )
    {}

    void CCoherentSystemEventListener::SystemReady()
    {
        m_IsReady = true;
        m_pUISystem->OnSystemReady();
    }

    void CCoherentSystemEventListener::OnError( const Coherent::UI::SystemError& error )
    {
        m_pUISystem->OnError( error );
    }
}