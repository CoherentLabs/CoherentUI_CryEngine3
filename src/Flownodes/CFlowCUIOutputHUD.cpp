#include <StdAfx.h>
#include <Nodes/G2FlowBaseNode.h>
#include <CPluginCoherentUI.h>
#include <Coherent/UI/View.h>
#include "CoherentViewListener.h"
#include "CoherentUISystem.h"
#include "ViewConfig.h"

namespace CoherentUIPlugin
{
    class CFlowCUIOutputHUD : public CFlowBaseNode<eNCT_Instanced>
    {
        private:
            CCoherentViewListener* m_pViewListener;
            std::wstring m_sPathW;
            bool m_bViewNeedsUpdate;

            enum EInputPorts
            {
                EIP_ACTIVATE = 0,
                EIP_PATH,
            };

            enum EOutputPorts
            {
                EOP_VIEWID = 0
            };

#define INITIALIZE_OUTPUTS(x) \
    ActivateOutput<int>(x, EOP_VIEWID, -1);

        public:
            CFlowCUIOutputHUD( SActivationInfo* pActInfo )
            {
                m_pViewListener = NULL;
                m_bViewNeedsUpdate = false;
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

                static const SOutputPortConfig outputs[] =
                {
                    OutputPortConfig<int>( "ViewID",                             _HELP( "id for further use" ),                        "nViewID" ),
                    OutputPortConfig_Null(),
                };

                config.pInputPorts = inputs;
                config.pOutputPorts = outputs;
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
                        INITIALIZE_OUTPUTS( pActInfo );
                        break;

                    case eFE_Activate:
                        {
                            // get the view definition
                            std::string sPath = GetPortString( pActInfo, EIP_PATH );
                            m_sPathW.assign( sPath.begin(), sPath.end() );

                            // indicate that we have to create/update the view later
                            m_bViewNeedsUpdate = true;
                            pActInfo->pGraph->SetRegularlyUpdated( pActInfo->myID, true );
                        }
                        break;

                    case eFE_Update:
                        // make sure the view is created, after the system is ready
                        if ( m_bViewNeedsUpdate &&  gCoherentUISystem->IsReady() )
                        {
                            if ( m_pViewListener )
                            {
                                // update only
                                Coherent::UI::View* view = m_pViewListener->GetView();
                                view->Load( m_sPathW.c_str() );
                            }
                            else
                            {
                                // create
                                m_pViewListener = gCoherentUISystem->CreateHUDView( m_sPathW );
                            }
                            m_bViewNeedsUpdate = false;
                        }

                        // set the view id output after the view is available
                        if ( m_pViewListener )
                        {
                            Coherent::UI::View* view = m_pViewListener->GetView();
                            if ( view )
                            {
                                ActivateOutput<int>( pActInfo, EOP_VIEWID, view->GetId() );
                                // updates are not necessary until next initialization
                                pActInfo->pGraph->SetRegularlyUpdated( pActInfo->myID, false );
                            }
                        }
                        break;
                }
            }
    };
}

REGISTER_FLOW_NODE_EX( "CoherentUI_Plugin:OutputHUD", CoherentUIPlugin::CFlowCUIOutputHUD, CFlowCUIOutputHUD );