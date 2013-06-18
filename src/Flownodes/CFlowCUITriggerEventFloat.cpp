#include <StdAfx.h>
#include <Nodes/G2FlowBaseNode.h>
#include <CPluginCoherentUI.h>
#include <Coherent/UI/View.h>
#include "CoherentUISystem.h"

namespace CoherentUIPlugin
{
    class CCFlowCUITriggerEventFloat : public CFlowBaseNode<eNCT_Instanced>
    {
        private:

            enum EInputPorts
            {
                EIP_ACTIVATE = 0,
                EIP_VIEWID,
                EIP_EVENT,
                EIP_ARG1,
                EIP_ARG2,
                EIP_ARG3,
                EIP_ARG4,
            };

        public:
            CCFlowCUITriggerEventFloat( SActivationInfo* pActInfo )
            {
            }

            virtual ~CCFlowCUITriggerEventFloat()
            {
            }

            virtual IFlowNodePtr Clone( SActivationInfo* pActInfo )
            {
                return new CCFlowCUITriggerEventFloat( pActInfo );
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
                    InputPortConfig<float>( "Arg1",                _HELP( "argument 1 (optional: float)" ) ),
                    InputPortConfig<float>( "Arg2",                _HELP( "argument 2 (optional: float)" ) ),
                    InputPortConfig<float>( "Arg3",                _HELP( "argument 3 (optional: float)" ) ),
                    InputPortConfig<float>( "Arg4",                _HELP( "argument 4 (optional: float)" ) ),
                    InputPortConfig_Null(),
                };

                config.pInputPorts = inputs;
                config.pOutputPorts = NULL;//output
                config.sDescription = _HELP( PLUGIN_CONSOLE_PREFIX "CoherentUI event trigger (4 arguments)" );

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

                                if ( IsPortActive( pActInfo, EIP_ARG4 ) ) {
                                    pView->TriggerEvent(sEvent.c_str(), 
                                        GetPortFloat( pActInfo, EIP_ARG1 ),
                                        GetPortFloat( pActInfo, EIP_ARG2 ),
                                        GetPortFloat( pActInfo, EIP_ARG3 ),
                                        GetPortFloat( pActInfo, EIP_ARG4 ) 
                                    );
                                }

                                else if ( IsPortActive( pActInfo, EIP_ARG3 ) ) {
                                    pView->TriggerEvent(sEvent.c_str(), 
                                        GetPortFloat( pActInfo, EIP_ARG1 ),
                                        GetPortFloat( pActInfo, EIP_ARG2 ),
                                        GetPortFloat( pActInfo, EIP_ARG3 ) 
                                    );
                                }

                                else if ( IsPortActive( pActInfo, EIP_ARG2 ) ) {
                                    pView->TriggerEvent(sEvent.c_str(), 
                                        GetPortFloat( pActInfo, EIP_ARG1 ),
                                        GetPortFloat( pActInfo, EIP_ARG2 ) 
                                    );
                                }

                                else if ( IsPortActive( pActInfo, EIP_ARG1 ) ) {
                                    pView->TriggerEvent(sEvent.c_str(), 
                                        GetPortFloat( pActInfo, EIP_ARG1 ) 
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

REGISTER_FLOW_NODE_EX( "CoherentUI_Plugin:TriggerEventFloat", CoherentUIPlugin::CCFlowCUITriggerEventFloat, CCFlowCUITriggerEventFloat );