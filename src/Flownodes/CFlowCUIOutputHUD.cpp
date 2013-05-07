#include <StdAfx.h>
#include <Nodes/G2FlowBaseNode.h>
#include <CPluginCoherentUI.h>
#include "CoherentViewListener.h"
#include "CoherentUISystem.h"
#include "ViewConfig.h"

namespace CoherentUIPlugin
{
    class CFlowCUIOutputHUD : public CFlowBaseNode<eNCT_Instanced>
    {
        private:
            CCoherentViewListener* m_pViewListener;

            enum EInputPorts
            {
                EIP_ACTIVATE = 0,
                EIP_PATH,
            };

        public:
            CFlowCUIOutputHUD( SActivationInfo* pActInfo )
            {
                m_pViewListener = NULL;
            }

            virtual ~CFlowCUIOutputHUD()
            {
                if ( m_pViewListener )
                {
                    gCoherentUISystem->DeleteHUDView();
                }
            }

            virtual IFlowNodePtr Clone( SActivationInfo* pActInfo )
            {
                return new CFlowCUIOutputHUD( pActInfo );
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
                    InputPortConfig<string>( "Path",                  "",        _HELP( "path to html files" ) ),
                    InputPortConfig_Null(),
                };

                config.pInputPorts = inputs;
                config.pOutputPorts = NULL;//outputs;
                config.sDescription = _HELP( PLUGIN_CONSOLE_PREFIX "CoherentUI HUD" );

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
                        if ( !m_pViewListener )
                        {
                            std::string sPath = GetPortString( pActInfo, EIP_PATH );
                            std::wstring sPathW( sPath.length(), L' ' );
                            sPathW.assign( sPath.begin(), sPath.end() );

                            m_pViewListener = gCoherentUISystem->CreateHUDView( sPathW );
                        }

                        break;

                    case eFE_Update:
                        break;
                }
            }
    };
}

REGISTER_FLOW_NODE_EX( "CoherentUI_Plugin:OutputHUD", CoherentUIPlugin::CFlowCUIOutputHUD, CFlowCUIOutputHUD );