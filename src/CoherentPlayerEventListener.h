#pragma once

#include <Player.h>

namespace CoherentUIPlugin
{

    class CCoherentViewListener;

    class CCoherentPlayerEventListener : public IPlayerEventListener
    {
        public:
            CCoherentPlayerEventListener( CCoherentViewListener* pViewListener );
            virtual void OnHealthChange( IActor* pActor, float fHealth ) COHERENT_OVERRIDE;

        private:
            CCoherentViewListener* m_pViewEventListener;
    };

}