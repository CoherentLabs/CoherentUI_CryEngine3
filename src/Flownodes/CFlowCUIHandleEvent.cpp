#include <StdAfx.h>
#include <Nodes/G2FlowBaseNode.h>
#include <CPluginCoherentUI.h>
#include <Coherent/UI/View.h>
#include <Coherent/UI/Binding/EventHandler.h>
#include <Coherent/UI/Binding/String.h>
#include "CoherentViewListener.h"
#include "CoherentUISystem.h"

namespace CoherentUIPlugin
{
    struct Event
    {
        std::string strArg;
        bool boolArg;
    };

    class CFlowCUIHandleEvent : public CFlowBaseNode<eNCT_Instanced>
    {
        private:

            enum EInputPorts
            {
                EIP_ACTIVATE = 0,
                EIP_VIEWID,
                EIP_EVENT,
            };

            enum EOutputPorts
            {
                EIP_ARG1 = 0,
                EIP_ARG2
            };

            int m_iViewId;
            bool m_bHandlerRegistered;
            bool m_bReceivedEvent;
            Event m_event; 

        public:
            CFlowCUIHandleEvent( SActivationInfo* pActInfo )
            {
                m_iViewId = 0;
                m_bHandlerRegistered = false;
                m_bReceivedEvent = false;
            }

            virtual ~CFlowCUIHandleEvent()
            {
            }

            virtual IFlowNodePtr Clone( SActivationInfo* pActInfo )
            {
                return new CFlowCUIHandleEvent( pActInfo );
            }

            virtual void GetMemoryUsage( ICrySizer* s ) const
            {
                s->Add( *this );
            }

            void Serialize( SActivationInfo* pActInfo, TSerialize ser )
            {
            }

            virtual void GetConfiguration( SFlowNodeConfig& config )
            {
                static const SInputPortConfig inputs[] =
                {
                    InputPortConfig_Void( "Activate",                            _HELP( "Activate View" ) ),
                    InputPortConfig<int>( "ViewID",                   0,         _HELP( "View ID" ) ),
                    InputPortConfig<string>( "Event",                "",          _HELP( "Event Name" ) ),
                    InputPortConfig_AnyType( NULL ),
                };

                static const SOutputPortConfig outputs[] =
                {
                    OutputPortConfig<string>( "Arg1",                             _HELP( "String argument (optional)" ),                        "sArg" ),
                    OutputPortConfig<string>( "Arg2",                             _HELP( "Boolean argument (optional)" ),                        "bArg" ),
                    OutputPortConfig_AnyType( NULL ),
                };

                config.pInputPorts = inputs;
                config.pOutputPorts = outputs;
                config.sDescription = _HELP( PLUGIN_CONSOLE_PREFIX "CoherentUI event handler" );

                //config.nFlags |= EFLN_TARGET_ENTITY;
                config.SetCategory( EFLN_APPROVED );
            }

            virtual void ProcessEvent( EFlowEvent evt, SActivationInfo* pActInfo )
            {
                switch ( evt )
                {
                    case eFE_Suspend:
                        pActInfo->pGraph->SetRegularlyUpdated( pActInfo->myID, false );
                        break;

                    case eFE_Resume:
                        pActInfo->pGraph->SetRegularlyUpdated( pActInfo->myID, true );
                        break;

                    case eFE_Initialize:
                        break;

                    case eFE_Activate:
                        {
                            if ( IsPortActive( pActInfo, EIP_ACTIVATE ) ) {
                                m_iViewId = GetPortInt( pActInfo, EIP_VIEWID );
                                m_bHandlerRegistered = false;
                                pActInfo->pGraph->SetRegularlyUpdated( pActInfo->myID, true );
                            }
                        }
                        break;

                    case eFE_Update:
                        if ( !m_bHandlerRegistered )
                        {
                            CCoherentViewListener* pViewListener = gCoherentUISystem->GetViewListener( m_iViewId );
                            if ( pViewListener && pViewListener->IsReadyForBindings() )
                            {
                                Coherent::UI::View* pView = pViewListener->GetView();
                                if ( pView )
                                {
                                    std::string sEvent = GetPortString( pActInfo, EIP_EVENT );
                                    pView->RegisterForEvent( sEvent.c_str(), Coherent::UI::MakeHandler(this, &CFlowCUIHandleEvent::HandleEvent));
                                    m_bHandlerRegistered = true;
                                }
                            }
                        }
                        if ( m_bReceivedEvent )
                        {
                            m_bReceivedEvent = false;
                            string strArg = m_event.strArg.c_str();
                            ActivateOutput( pActInfo, EIP_ARG1, strArg );
                            ActivateOutput( pActInfo, EIP_ARG2, m_event.boolArg );
                        }
                        break;
                }
            }

            void HandleEvent( Event& e )
            {
                m_event = e;
                m_bReceivedEvent = true;
            }
    };
}

// Expose the Options structure to JavaScript
void CoherentBind(Coherent::UI::Binder* binder, CoherentUIPlugin::Event* e)
{
    // Specify the exposed name of the type
    if(auto type = binder->RegisterType("Event", e))
    {
        // expose normal data properties
        type.Property("strArg", &CoherentUIPlugin::Event::strArg);
        type.Property("boolArg", &CoherentUIPlugin::Event::boolArg);
    }
}

REGISTER_FLOW_NODE_EX( "CoherentUI_Plugin:HandleEvent", CoherentUIPlugin::CFlowCUIHandleEvent, CFlowCUIHandleEvent );