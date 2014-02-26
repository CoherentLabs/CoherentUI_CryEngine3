#pragma once

struct IActor;

#include <IPlayerEventListener.h>

namespace CoherentUIPlugin
{

    class CCoherentViewListener;

    class CCoherentPlayerEventListener : public IPlayerEventListener
    {
        public:
            CCoherentPlayerEventListener();
            void AddViewListener( CCoherentViewListener* pViewListener );
            void RemoveViewListener( CCoherentViewListener* pViewListener );

            virtual void OnHealthChanged( IActor* pActor, float fHealth ) COHERENT_OVERRIDE;

        private:
            std::vector<CCoherentViewListener*> m_ViewEventListeners;
    };

}