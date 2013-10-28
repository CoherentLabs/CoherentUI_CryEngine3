#include "StdAfx.h"
#include "CoherentPlayerEventListener.h"

#include "CoherentViewListener.h"
#include <Coherent/UI/View.h>
#include <IActorSystem.h>

namespace CoherentUIPlugin
{

    CCoherentPlayerEventListener::CCoherentPlayerEventListener()
    {
    }

    void CCoherentPlayerEventListener::AddViewListener( CCoherentViewListener* pViewListener )
    {
        m_ViewEventListeners.push_back( pViewListener );
    }

    void CCoherentPlayerEventListener::RemoveViewListener( CCoherentViewListener* pViewListener )
    {
        std::vector<CCoherentViewListener*>::iterator it = std::find( m_ViewEventListeners.begin(),
                m_ViewEventListeners.end(), pViewListener );
        m_ViewEventListeners.erase( it );
    }

    void CCoherentPlayerEventListener::OnHealthChanged( IActor* pActor, float fHealth )
    {
        for ( size_t i = 0, count = m_ViewEventListeners.size(); i < count; ++i )
        {
            CCoherentViewListener* pViewListener = m_ViewEventListeners[i];

            if ( pViewListener->GetView() != nullptr )
            {
                pViewListener->GetView()->TriggerEvent( "OnPlayerHealthChanged", fHealth / pActor->GetMaxHealth() * 100.0f );
            }
        }
    }

}