#include <StdAfx.h>
#include <Nodes/G2FlowBaseNode.h>
#include <CPluginCoherentUI.h>
#include <Coherent/UI/View.h>
#include "CoherentViewListener.h"
#include "CoherentUISystem.h"

namespace CoherentUIPlugin
{
    class CFlowCUITriggerEventFloat : public CFlowBaseNode<eNCT_Instanced>
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
            CFlowCUITriggerEventFloat( SActivationInfo* pActInfo )
            {
            }

            virtual ~CFlowCUITriggerEventFloat()
            {
            }

            virtual IFlowNodePtr Clone( SActivationInfo* pActInfo )
            {
                return new CFlowCUITriggerEventFloat( pActInfo );
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
                    InputPortConfig<float>( "Arg1",                _HELP( "Argument 1 (optional: float)" ) ),
                    InputPortConfig<float>( "Arg2",                _HELP( "Argument 2 (optional: float)" ) ),
                    InputPortConfig<float>( "Arg3",                _HELP( "Argument 3 (optional: float)" ) ),
                    InputPortConfig<float>( "Arg4",                _HELP( "Argument 4 (optional: float)" ) ),
                    InputPortConfig_AnyType( NULL ),
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
                            if ( IsPortActive( pActInfo, EIP_ACTIVATE ) ) {
                                int viewId = GetPortInt( pActInfo, EIP_VIEWID );
                                CCoherentViewListener* pViewListener = gCoherentUISystem->GetViewListener( viewId );
                                if ( pViewListener && pViewListener->IsReadyForBindings() )
                                {
                                    Coherent::UI::View* pView = pViewListener->GetView();
                                    if ( pView )
                                    {
                                        std::string sEvent = GetPortString( pActInfo, EIP_EVENT );
                                        float fArg1 = GetPortFloat( pActInfo, EIP_ARG1 );
                                        float fArg2 = GetPortFloat( pActInfo, EIP_ARG2 );
                                        float fArg3 = GetPortFloat( pActInfo, EIP_ARG3 );
                                        float fArg4 = GetPortFloat( pActInfo, EIP_ARG4 );

                                        pView->TriggerEvent(sEvent.c_str(), 
                                            GetPortFloat( pActInfo, EIP_ARG1 ),
                                            GetPortFloat( pActInfo, EIP_ARG2 ),
                                            GetPortFloat( pActInfo, EIP_ARG3 ),
                                            GetPortFloat( pActInfo, EIP_ARG4 ) 
                                        );
                                    }
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

REGISTER_FLOW_NODE_EX( "CoherentUI_Plugin:TriggerEventFloat", CoherentUIPlugin::CFlowCUITriggerEventFloat, CFlowCUITriggerEventFloat );