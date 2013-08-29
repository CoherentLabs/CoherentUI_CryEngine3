#pragma once

#include <IInput.h>

namespace Coherent
{
    namespace UI
    {
        struct KeyEventData;
        struct MouseEventData;
    }
}

namespace CoherentUIPlugin
{

    class CCoherentViewListener;

    class CCoherentInputEventListener : public IInputEventListener
    {
        public:
            CCoherentInputEventListener();

            int GetMouseX() const
            {
                return m_MouseX;
            }

            int GetMouseY() const
            {
                return m_MouseY;
            }

            bool IsPlayerInputEnabled() const
            {
                return m_PlayerInputEnabled;
            }

            bool ShouldDrawCoherentUI() const
            {
                return m_DrawCoherentUI;
            }

            bool ShouldDrawCursor() const
            {
                return m_DrawCursor;
            }

            virtual int GetPriority() const COHERENT_OVERRIDE;

            virtual bool OnInputEvent( const SInputEvent& event ) COHERENT_OVERRIDE;

            virtual bool OnInputEventUI( const SInputEvent& event ) COHERENT_OVERRIDE;

            virtual void SetPlayerInput( bool enabled );

        private:
            bool ToKeyEventData( const SInputEvent& event, Coherent::UI::KeyEventData& keyData );

            bool ToMouseEvent( const SInputEvent& event, Coherent::UI::MouseEventData& mouseData );

            bool TraceMouse( int& outX, int& outY, CCoherentViewListener*& pViewListener );

        private:
            CCoherentViewListener* m_pLastFocusedViewListener; // The last View listener that the user moused over
            int m_MouseX;
            int m_MouseY;
            bool m_PlayerInputEnabled;
            bool m_DrawCoherentUI;
            bool m_DrawCursor;
    };

}