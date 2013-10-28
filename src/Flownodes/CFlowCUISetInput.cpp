#include <StdAfx.h>
#include <Nodes/G2FlowBaseNode.h>
#include <CPluginCoherentUI.h>
#include "CoherentUISystem.h"

namespace CoherentUIPlugin
{
    class CFlowCUISetInput : public CFlowBaseNode<eNCT_Instanced>
    {
        private:

            enum EInputPorts
            {
                EIP_ACTIVATE = 0,
                EIP_ENABLED,
            };

        public:
            CFlowCUISetInput( SActivationInfo* pActInfo )
            {
            }

            virtual ~CFlowCUISetInput()
            {
            }

            virtual IFlowNodePtr Clone( SActivationInfo* pActInfo )
            {
                return new CFlowCUISetInput( pActInfo );
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
                    InputPortConfig_Void( "Activate",                            _HELP( "activate input" ) ),
                    InputPortConfig<bool>( "Enabled",                _HELP( "dis-/enable" ) ),
                    InputPortConfig_AnyType( NULL ),
                };

                config.pInputPorts = inputs;
                config.pOutputPorts = NULL;//output
                config.sDescription = _HELP( PLUGIN_CONSOLE_PREFIX "CoherentUI enable input" );

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
                                if ( gCoherentUISystem->IsReady() )
                                {
                                    bool enabled = GetPortBool( pActInfo, EIP_ENABLED );
                                    gCoherentUISystem->SetCUIInput( enabled );
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

REGISTER_FLOW_NODE_EX( "CoherentUI_Plugin:SetInput", CoherentUIPlugin::CFlowCUISetInput, CFlowCUISetInput );