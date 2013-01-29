#include "StdAfx.h"
#include "CoherentPlayerEventListener.h"

#include "CoherentViewListener.h"
#include <Coherent/UI/View.h>

namespace CoherentUIPlugin
{

	CCoherentPlayerEventListener::CCoherentPlayerEventListener( CCoherentViewListener* pViewListener )
		: m_pViewEventListener( pViewListener )
	{
	}

	void CCoherentPlayerEventListener::OnHealthChange( IActor* pActor, float fHealth )
	{
		if (m_pViewEventListener && m_pViewEventListener->GetView())
		{
			m_pViewEventListener->GetView()->TriggerEvent( "OnPlayerHealthChanged", fHealth );
		}
	}

}