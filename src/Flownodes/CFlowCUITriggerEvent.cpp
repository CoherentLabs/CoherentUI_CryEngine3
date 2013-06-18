#include <StdAfx.h>
#include <Nodes/G2FlowBaseNode.h>
#include <CPluginCoherentUI.h>
#include <Coherent/UI/View.h>
#include "CoherentUISystem.h"

namespace CoherentUIPlugin
{
    class CCFlowCUITriggerEvent : public CFlowBaseNode<eNCT_Instanced>
    {
        private:

            enum EInputPorts
            {
                EIP_ACTIVATE = 0,
                EIP_VIEWID,
                EIP_EVENT,
                EIP_ARG1,
            };

        public:
            CCFlowCUITriggerEvent( SActivationInfo* pActInfo )
            {
            }

            virtual ~CCFlowCUITriggerEvent()
            {
            }

            virtual IFlowNodePtr Clone( SActivationInfo* pActInfo )
            {
                return new CCFlowCUITriggerEvent( pActInfo );
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
                    InputPortConfig_Void( "Activate",                            _HELP( "activate view" ) ),
                    InputPortConfig<int>( "ViewID",                   0,         _HELP( "view id" ) ),
                    InputPortConfig<string>( "Event",                "",          _HELP( "event name" ) ),
                    InputPortConfig<bool>( "Arg1",                _HELP( "argument 1 (optional: boolean)" ) ),
                    InputPortConfig_Null(),
                };

                config.pInputPorts = inputs;
                config.pOutputPorts = NULL;//output
                config.sDescription = _HELP( PLUGIN_CONSOLE_PREFIX "CoherentUI event trigger (1 argument)" );

                //config.nFlags |= EFLN_TARGET_ENTITY;
                config.SetCategory( EFLN_APPROVED );
            }

            virtual void ProcessEvent( EFlowEvent evt, SActivationInfo* pActInfo )
            {
                switch ( evt )
                {
                    case eFE_Suspend:
                        break;

                    case eFE_Resume:
                        break;

                    case eFE_Initialize:
                        break;

                    case eFE_Activate: 
                        {
                            int viewId = GetPortInt( pActInfo, EIP_VIEWID );
                            Coherent::UI::View* pView = gCoherentUISystem->GetView( viewId );
                            if ( pView )
                            {
                                std::string sEvent = GetPortString( pActInfo, EIP_EVENT );

                                if ( IsPortActive( pActInfo, EIP_ARG1 ) ) {
                                    pView->TriggerEvent(sEvent.c_str(), 
                                        GetPortBool( pActInfo, EIP_ARG1 ) 
                                    );
                                }

                                else {
                                    pView->TriggerEvent(sEvent.c_str());
                                }
                            }
                        }
                        break;

                    case eFE_Update:
                        break;
                }
            }
    };
}

REGISTER_FLOW_NODE_EX( "CoherentUI_Plugin:TriggerEvent", CoherentUIPlugin::CCFlowCUITriggerEvent, CCFlowCUITriggerEvent );