#pragma once

#include <Coherent/UI/UISystem.h>

namespace CoherentUIPlugin
{
    class CCoherentUISystem;

    class CCoherentSystemEventListener : public Coherent::UI::EventListener
    {
        public:
            CCoherentSystemEventListener( CCoherentUISystem* pUISystem );

            virtual void SystemReady() COHERENT_OVERRIDE;

            bool IsReady() const
            {
                return m_IsReady;
            }

        private:
            bool m_IsReady;
            CCoherentUISystem* m_pUISystem;
    };

}