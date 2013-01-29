#include "StdAfx.h"
#include "CoherentInputEventListener.h"
#include "CoherentViewListener.h"
#include "CoherentUISystem.h"

#include <Coherent/UI/InputEvents.h>

#include <Coherent/UI/View.h>

#include <IActorSystem.h>
#include <IHardwareMouse.h>
#include <IWorldQuery.h>

namespace CoherentUIPlugin
{

    //--------------------------------------------------------------------------------------
    // Helper class for tracking down repeatedly pressed keys
    //--------------------------------------------------------------------------------------

    class RepeatFilter
    {
        public:
            static const int MAX_KEYS = 256; // Support only the ASCII set

            static void Init()
            {
                memset( s_LastDownTime, 0, sizeof( s_LastDownTime ) );
                memset( s_IsDown, 0, sizeof( s_IsDown ) );

                /*
                SPI_GETKEYBOARDDELAY
                Retrieves the keyboard repeat-delay setting, which is a value in the range from 0
                (approximately 250 ms delay) through 3 (approximately 1 second delay). The actual delay
                associated with each value may vary depending on the hardware. The pvParam parameter
                must point to an integer variable that receives the setting.
                */
                DWORD keyboardDelay;
                ::SystemParametersInfo( SPI_GETKEYBOARDDELAY, 0, &keyboardDelay, 0 );

                s_RepeatDelay = 0.25 + 0.75 * ( keyboardDelay / 3.0 );

                /*
                SPI_GETKEYBOARDSPEED
                Retrieves the keyboard repeat-speed setting, which is a value in the range from 0
                (approximately 2.5 repetitions per second) through 31 (approximately 30 repetitions per
                second). The actual repeat rates are hardware-dependent and may vary from a linear scale
                by as much as 20%. The pvParam parameter must point to a DWORD variable that receives the setting.
                */
                DWORD keyboardSpeed;
                ::SystemParametersInfo( SPI_GETKEYBOARDSPEED, 0, &keyboardSpeed, 0 );

                double repsPerSec = 2.5 + 27.5 * ( keyboardSpeed / 31.0 );
                s_RepeatSpeed = 1.0 / repsPerSec;
            }

            /// Returns true if the key should be forwarded to Coherent UI.
            /// Call only for WM_KEYDOWN events, not WM_CHAR.
            /// Note that this is a very simplistic implementation
            static bool FilterKey( int key, double timestamp )
            {
                // TODO: implement properly, this code will just ignore multiple keydown events
                // before a keyup is received and will not repeat the key as windows would
                if ( s_IsDown[key] && ( timestamp - s_LastDownTime[key] < s_RepeatDelay ) )
                {
                    return false;
                }

                else
                {
                    s_LastDownTime[key] = timestamp;
                    s_IsDown[key] = true;
                    return true;
                }
            }

            static void ReleaseKey( int key )
            {
                s_IsDown[key] = false;
            }
        private:
            RepeatFilter();

            static double s_LastDownTime[MAX_KEYS];
            static bool s_IsDown[MAX_KEYS];
            static double s_RepeatDelay; // When should the repetition start after first holding a key (seconds)
            static double s_RepeatSpeed; // Repeat speed when actually holding down a key (seconds)
    };

    double RepeatFilter::s_LastDownTime[MAX_KEYS];
    bool   RepeatFilter::s_IsDown[MAX_KEYS];
    double RepeatFilter::s_RepeatDelay;
    double RepeatFilter::s_RepeatSpeed;

    //--------------------------------------------------------------------------------------

    CCoherentInputEventListener::CCoherentInputEventListener()
		: m_pLastFocusedViewListener(NULL)
        , m_MouseX( 0 )
        , m_MouseY( 0 )
        , m_PlayerInputEnabled( true )
        , m_DrawCoherentUI( true )
        , m_DrawCursor( false )
    {
        RepeatFilter::Init();
    }

    int CCoherentInputEventListener::GetPriority() const
    {
        return 0;
    }

    bool CCoherentInputEventListener::TraceMouse( int& outX, int& outY, CCoherentViewListener*& pViewListener )
    {
        if (gCoherentUISystem == nullptr)
        {
            return false;
        }

        CCamera& camera = gEnv->pSystem->GetViewCamera();
        int vpWidth = gEnv->pRenderer->GetWidth();
        int vpHeight = gEnv->pRenderer->GetHeight();
        float proj22 = 1.0f / cry_tanf( camera.GetFov() / 2.0f );
        float proj11 = proj22 / camera.GetProjRatio();
        float viewX = ( ( ( 2.0f * ( float )GetMouseX() ) / vpWidth ) - 1.0f ) / proj11;
        float viewY = ( ( ( -2.0f * ( float )GetMouseY() ) / vpHeight ) + 1.0f ) / proj22;
        Matrix34 invView = camera.GetMatrix();
        Vec3 dir = invView.TransformVector( Vec3( viewX, 1.0f, viewY ) ); // Z is up

        Vec3 origin = camera.GetPosition();

		return gCoherentUISystem->RaycastClosestViewListenersGeometry(origin, dir, outX, outY, pViewListener);
    }

    bool CCoherentInputEventListener::OnInputEvent( const SInputEvent& event )
    {
        // Process special keys that toggle a function
        if ( event.deviceId == eDI_Keyboard && ( event.state & eIS_Released ) )
        {
            switch ( event.keyId )
            {
                case eKI_NP_0:
                    m_PlayerInputEnabled = !m_PlayerInputEnabled;
                    gEnv->pGameFramework->GetIActionMapManager()->EnableActionMap( "player", m_PlayerInputEnabled );
                    return false;

                    // NP_1 starts raycasting so don't use it
                case eKI_NP_2:
                    m_DrawCoherentUI = !m_DrawCoherentUI;
                    return false;

                case eKI_NP_3:
                    m_DrawCursor = !m_DrawCursor;
                    ::ShowCursor( m_DrawCursor );

                    if ( gEnv && gEnv->pSystem && gEnv->pSystem->GetIHardwareMouse() )
                    {
                        if ( m_DrawCursor )
                        {
                            gEnv->pSystem->GetIHardwareMouse()->IncrementCounter();
                        }

                        else
                        {
                            gEnv->pSystem->GetIHardwareMouse()->DecrementCounter();
                        }
                    }

                    return false;

                case eKI_NP_4:
                    {
                        EntityId id = gEnv->pGameFramework->GetClientActor()->GetGameObject()->GetWorldQuery()->GetLookAtEntityId();
                        IEntity* pEntityInFront = gEnv->pEntitySystem->GetEntity( id );

                        if ( pEntityInFront )
                        {
                            OutputDebugString( pEntityInFront->GetName() );
                        }
                    }

                    return false;

                case eKI_NP_5:
                    {
                        CCamera& cam = gEnv->pSystem->GetViewCamera();
                        int vpWidth = gEnv->pRenderer->GetWidth();
                        int vpHeight = gEnv->pRenderer->GetHeight();
                        float proj22 = 1.0f / cry_tanf( cam.GetFov() / 2.0f );
                        float proj11 = proj22 / cam.GetProjRatio();
                        float viewX = ( ( ( 2.0f * ( float )GetMouseX() ) / vpWidth ) - 1.0f ) / proj11;
                        float viewY = ( ( ( -2.0f * ( float )GetMouseY() ) / vpHeight ) + 1.0f ) / proj22;
                        Matrix34 invView = cam.GetMatrix();
                        Vec3 dir = invView.TransformVector( Vec3( viewX, 1.0f, viewY ) ); // Z is up
                        dir.Normalize();
                        dir *= 1000.0f;

                        Vec3 origin = cam.GetPosition();

                        ray_hit hit;
                        IPhysicalEntity* pSkipEnt = nullptr;
                        int hitCount = gEnv->pPhysicalWorld->RayWorldIntersection( origin, dir,
                                       ent_sleeping_rigid | ent_rigid,
                                       rwi_stop_at_pierceable | rwi_colltype_any, &hit, 1, pSkipEnt );

                        if ( hitCount > 0 )
                        {
                            IEntity* pEntity = gEnv->pEntitySystem->GetEntityFromPhysics( hit.pCollider );
                            OutputDebugString( pEntity->GetName() );
                        }
                    }

                    return false;

                case eKI_NP_6:
                    {
                        int dummyx, dummyy;
						CCoherentViewListener* pViewListener;
                        TraceMouse( dummyx, dummyy, pViewListener );
                    }

                    return false;
            }
        }


        // Send input to Coherent UI only when the player input is disabled
        if ( m_PlayerInputEnabled )
        {
            return false;
        }

        if ( event.deviceId == eDI_Keyboard )
        {
			if ( !m_pLastFocusedViewListener || !m_pLastFocusedViewListener->GetView() )
			{
				return false;
			}

            Coherent::UI::KeyEventData keyEvent;

            if ( ToKeyEventData( event, keyEvent ) )
            {
                // Enter needs to be sent as KeyDown to work
                if ( keyEvent.KeyCode == 13 )
                {
                    keyEvent.Type = Coherent::UI::KeyEventData::KeyDown;
                    m_pLastFocusedViewListener->GetView()->KeyEvent( keyEvent );
                    keyEvent.Type = Coherent::UI::KeyEventData::Char;
                }

                m_pLastFocusedViewListener->GetView()->KeyEvent( keyEvent );
            }
        }
        else if ( event.deviceId == eDI_Mouse )
        {
            Coherent::UI::MouseEventData mouseEvent;
            int mouseX, mouseY;

            if ( ToMouseEvent( event, mouseEvent ) && TraceMouse( mouseX, mouseY, m_pLastFocusedViewListener ) )
            {
                mouseEvent.X = mouseX;
                mouseEvent.Y = mouseY;
				if ( m_pLastFocusedViewListener->GetView() )
				{
					m_pLastFocusedViewListener->GetView()->MouseEvent( mouseEvent );
				}
            }
        }

        return false;
    }

    bool CCoherentInputEventListener::OnInputEventUI( const SInputEvent& event )
    {
        return false;
    }

    bool CCoherentInputEventListener::ToKeyEventData( const SInputEvent& event, Coherent::UI::KeyEventData& keyEvent )
    {
        keyEvent.IsAutoRepeat = false;

        if ( ( event.state & eIS_Pressed ) != 0 )
        {
            keyEvent.Type = Coherent::UI::KeyEventData::Char;
        }

        else if ( ( event.state & eIS_Down ) != 0 )
        {
            keyEvent.Type = Coherent::UI::KeyEventData::KeyDown;
        }

        else if ( ( event.state & eIS_Released ) != 0 )
        {
            keyEvent.Type = Coherent::UI::KeyEventData::KeyUp;
        }

        else
        {
            keyEvent.Type = Coherent::UI::KeyEventData::Unknown;
        }

        keyEvent.Modifiers = Coherent::UI::EventModifiersState();
        keyEvent.Modifiers.IsShiftDown = ( ( event.modifiers & eMM_Shift ) != 0 );
        keyEvent.Modifiers.IsCtrlDown = ( ( event.modifiers & eMM_Ctrl ) != 0 );
        keyEvent.Modifiers.IsAltDown = ( ( event.modifiers & eMM_Alt ) != 0 );
        keyEvent.Modifiers.IsNumLockOn = ( ( event.modifiers & eMM_NumLock ) != 0 );
        keyEvent.Modifiers.IsCapsOn = ( ( event.modifiers & eMM_CapsLock ) != 0 );

        const int Characters[] =
        {
            VK_ESCAPE, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', VK_BACK,
            VK_TAB, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', VK_RETURN,
            VK_CONTROL/*left control*/, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', VK_SHIFT/*Left shift*/, '\\',
            'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', VK_SHIFT/*right shift*/, VK_MULTIPLY, VK_MENU/*Left alt*/, VK_SPACE, VK_CAPITAL,
            VK_F1, VK_F2, VK_F3, VK_F4, VK_F5, VK_F6, VK_F7, VK_F8, VK_F9, VK_F10, VK_NUMLOCK, VK_SCROLL,
            VK_NUMPAD7, VK_NUMPAD8, VK_NUMPAD9, VK_SUBTRACT, VK_NUMPAD4, VK_NUMPAD5, VK_NUMPAD6, VK_ADD,
            VK_NUMPAD1, VK_NUMPAD2, VK_NUMPAD3, VK_NUMPAD0, VK_F11, VK_F12, VK_F13, VK_F14, VK_F15,
            ':', '_', VK_RETURN /*NUMPAD VK_RETURN*/, VK_CONTROL/*right control*/, VK_DECIMAL, VK_DIVIDE, VK_PRINT, VK_MENU/*right alt*/,
            VK_PAUSE, VK_HOME, VK_UP, VK_PRIOR, VK_LEFT, VK_RIGHT, VK_END, VK_DOWN, VK_NEXT, VK_INSERT, VK_DELETE,
            VK_LWIN, VK_RWIN, VK_APPS, VK_OEM_102
        };

        const int CapitalCharacters[] =
        {
            VK_ESCAPE, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', VK_BACK,
            VK_TAB, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', VK_RETURN,
            VK_CONTROL, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', VK_SHIFT, '|',
            'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', VK_SHIFT, VK_MULTIPLY, VK_MENU, VK_SPACE, VK_CAPITAL,
            VK_F1, VK_F2, VK_F3, VK_F4, VK_F5, VK_F6, VK_F7, VK_F8, VK_F9, VK_F10, VK_NUMLOCK, VK_SCROLL,
            VK_NUMPAD7, VK_NUMPAD8, VK_NUMPAD9, VK_SUBTRACT, VK_NUMPAD4, VK_NUMPAD5, VK_NUMPAD6, VK_ADD,
            VK_NUMPAD1, VK_NUMPAD2, VK_NUMPAD3, VK_NUMPAD0, VK_F11, VK_F12, VK_F13, VK_F14, VK_F15,
            ':', '_', VK_RETURN /*NUMPAD VK_RETURN*/, VK_CONTROL, VK_DECIMAL, VK_DIVIDE, VK_PRINT, VK_MENU,
            VK_PAUSE, VK_HOME, VK_UP, VK_PRIOR, VK_LEFT, VK_RIGHT, VK_END, VK_DOWN, VK_NEXT, VK_INSERT, VK_DELETE,
            VK_LWIN, VK_RWIN, VK_APPS, VK_OEM_102
        };

        const bool IsPrintable[] =
        {
            false, true, true, true, true, true, true, true, true, true, true, true, true, false,
            true, true, true, true, true, true, true, true, true, true, true, true, true, true,
            false, true, true, true, true, true, true, true, true, true, true, true, true, false, true,
            true, true, true, true, true, true, true, true, true, true, false, true, false, true, false,
            false, false, false, false, false, false, false, false, false, false, false, false,
            false, false, false, true, false, false, false, true,
            false, false, false, false, false, false, false, false, false,
            true, true, true, false, true, true, false, false,
            false, false, false, false, false, false, false, false, false, false, false,
            false, false, false, false
        };

        int character = Characters[event.keyId];
        bool shouldBeCapitalized = ( keyEvent.Modifiers.IsCapsOn || ( !keyEvent.Modifiers.IsCapsOn && keyEvent.Modifiers.IsShiftDown ) );

        if ( !IsPrintable[event.keyId] )
        {
            switch ( event.state )
            {
                case eIS_Pressed:
                    return false; // Produce WM_CHAR messages only for printable symbols

                case eIS_Down:
                    if ( !RepeatFilter::FilterKey( character, ::GetTickCount() / 1000.0 ) )
                    {
                        return false;
                    }

                    break;

                case eIS_Released:
                    RepeatFilter::ReleaseKey( character );
                    break;
            }
        }


        // WM_KEYDOWN / WM_KEYUP produce capital letters
        // NOTE: Only latin characters are supported
        bool capitalizeBecauseItsNotWMChar = ( ( event.state == eIS_Down || event.state == eIS_Released ) && ( character >= 'a' && character <= 'z' ) );

        const int* pCharArray = ( shouldBeCapitalized || capitalizeBecauseItsNotWMChar ) ? CapitalCharacters : Characters;
        int keyCode = pCharArray[event.keyId];

        keyEvent.IsNumPad = ( event.keyId >= eKI_NP_7 && event.keyId <= eKI_NP_0 )
                            || event.keyId == eKI_NumLock
                            || event.keyId == eKI_NP_Enter
                            || event.keyId == eKI_NP_Period
                            || event.keyId == eKI_NP_Divide;

        keyEvent.KeyCode = keyCode;

        return true;
    }

    bool CCoherentInputEventListener::ToMouseEvent( const SInputEvent& event, Coherent::UI::MouseEventData& mouseEvent )
    {
        static bool s_Mouse1Down = false;
        static bool s_Mouse2Down = false;
        static bool s_Mouse3Down = false;

        if ( event.state == eIS_Down )
        {
            mouseEvent.Type = Coherent::UI::MouseEventData::MouseDown;

            if ( event.keyId == eKI_Mouse1 )
            {
                mouseEvent.Button = Coherent::UI::MouseEventData::ButtonLeft;

                if ( s_Mouse1Down )
                {
                    return false;
                }

                s_Mouse1Down = true;
            }

            else if ( event.keyId == eKI_Mouse2 )
            {
                mouseEvent.Button = Coherent::UI::MouseEventData::ButtonRight;

                if ( s_Mouse2Down )
                {
                    return false;
                }

                s_Mouse2Down = true;
            }

            else if ( event.keyId == eKI_Mouse3 )
            {
                mouseEvent.Button = Coherent::UI::MouseEventData::ButtonMiddle;

                if ( s_Mouse3Down )
                {
                    return false;
                }

                s_Mouse3Down = true;
            }

            else
            {
                return false;
            }
        }

        else if ( event.state == eIS_Released )
        {
            mouseEvent.Type = Coherent::UI::MouseEventData::MouseUp;

            if ( event.keyId == eKI_Mouse1 )
            {
                mouseEvent.Button = Coherent::UI::MouseEventData::ButtonLeft;
                s_Mouse1Down = false;
            }

            else if ( event.keyId == eKI_Mouse2 )
            {
                mouseEvent.Button = Coherent::UI::MouseEventData::ButtonRight;
                s_Mouse2Down = false;
            }

            else if ( event.keyId == eKI_Mouse3 )
            {
                mouseEvent.Button = Coherent::UI::MouseEventData::ButtonMiddle;
                s_Mouse3Down = false;
            }

            else
            {
                return false;
            }
        }

        else if ( event.state == eIS_Changed )
        {
            mouseEvent.Type = Coherent::UI::MouseEventData::MouseMove;

            if ( s_Mouse1Down )
            {
                mouseEvent.Button = Coherent::UI::MouseEventData::ButtonLeft;
            }

            else if ( s_Mouse2Down )
            {
                mouseEvent.Button = Coherent::UI::MouseEventData::ButtonRight;
            }

            else if ( s_Mouse3Down )
            {
                mouseEvent.Button = Coherent::UI::MouseEventData::ButtonMiddle;
            }

            else
            {
                mouseEvent.Button = Coherent::UI::MouseEventData::ButtonNone;
            }

            POINT cursorPos;
            ::GetCursorPos( &cursorPos );
            ::ScreenToClient( HWND( gEnv->pRenderer->GetHWND() ), &cursorPos );
            m_MouseX = cursorPos.x;
            m_MouseY = cursorPos.y;

        }

        else
        {
            return false; // Do not process eIS_Pressed because eIS_Down is enough
        }

        mouseEvent.Modifiers.IsShiftDown = ( ( event.modifiers & eMM_Shift ) != 0 );
        mouseEvent.Modifiers.IsCtrlDown = ( ( event.modifiers & eMM_Ctrl ) != 0 );
        mouseEvent.Modifiers.IsAltDown = ( ( event.modifiers & eMM_Alt ) != 0 );
        mouseEvent.Modifiers.IsNumLockOn = ( ( event.modifiers & eMM_NumLock ) != 0 );
        mouseEvent.Modifiers.IsCapsOn = ( ( event.modifiers & eMM_CapsLock ) != 0 );

        mouseEvent.MouseModifiers.IsLeftButtonDown = s_Mouse1Down;
        mouseEvent.MouseModifiers.IsRightButtonDown = s_Mouse2Down;
        mouseEvent.MouseModifiers.IsMiddleButtonDown = s_Mouse3Down;

        mouseEvent.X = m_MouseX;
        mouseEvent.Y = m_MouseY;

        //------------------------------------------------------------------------------

        if ( event.state == eIS_Changed && ( event.keyId == eKI_MouseZ || event.keyId == eKI_MouseX ) )
        {
            mouseEvent.Type = Coherent::UI::MouseEventData::MouseWheel;

            float wheelDelta = event.value / WHEEL_DELTA;

            if ( event.keyId == eKI_MouseZ )
            {
                mouseEvent.WheelY = wheelDelta;
            }

            else
            {
                mouseEvent.WheelX = wheelDelta;
            }
        }

        return true;
    }

}