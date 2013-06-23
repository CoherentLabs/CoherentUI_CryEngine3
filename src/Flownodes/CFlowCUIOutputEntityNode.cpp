#include <StdAfx.h>
#include <Nodes/G2FlowBaseNode.h>
#include <CPluginCoherentUI.h>
#include <Coherent/UI/View.h>
#include "CoherentViewListener.h"
#include "CoherentUISystem.h"
#include "ViewConfig.h"

namespace CoherentUIPlugin
{
    class CFlowCUIOutputEntityNode : public CFlowBaseNode<eNCT_Instanced>
    {
        private:
            CCoherentViewListener* m_pViewListener;
            IEntity* m_pEntity;

            enum EInputPorts
            {
                EIP_ACTIVATE = 0,
                EIP_URL,
                EIP_WIDTH,
                EIP_HEIGHT,
                EIP_TRANSPARENT,
                EIP_CLICKABLE,
                EIP_MESH,
                EIP_SHARED_MEMORY,
            };

            enum EOutputPorts
            {
                EOP_VIEWID = 0
            };

#define INITIALIZE_OUTPUTS(x) \
    ActivateOutput<int>(x, EOP_VIEWID, -1);

        public:
            CFlowCUIOutputEntityNode( SActivationInfo* pActInfo )
            {
                m_pViewListener = NULL;
                m_pEntity = NULL;
            }

            virtual ~CFlowCUIOutputEntityNode()
            {
                if ( m_pViewListener )
                {
                    gCoherentUISystem->DeleteView( m_pViewListener );
                }
            }

            virtual IFlowNodePtr Clone( SActivationInfo* pActInfo )
            {
                return new CFlowCUIOutputEntityNode( pActInfo );
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
                    InputPortConfig<string>( "Url",                  "",         _HELP( "initial url" ) ),
                    InputPortConfig<int>( "Width",                   0,          _HELP( "width" ) ),
                    InputPortConfig<int>( "Height",                  0,          _HELP( "height" ) ),
                    InputPortConfig<bool>( "Transparent",        false,          _HELP( "is transparent" ) ),
                    InputPortConfig<bool>( "Clickable",           true,          _HELP( "is clickable" ) ),
                    InputPortConfig<string>( "Mesh",                "",          _HELP( "collision mesh" ) ),
                    InputPortConfig<bool>( "SharedMemory",        true,          _HELP( "uses shared memory" ) ),
                    InputPortConfig_Null(),
                };

                static const SOutputPortConfig outputs[] =
                {
                    OutputPortConfig<int>( "ViewID",                             _HELP( "id for further use" ),                        "nViewID" ),
                    OutputPortConfig_Null(),
                };

                config.pInputPorts = inputs;
                config.pOutputPorts = outputs;
                config.sDescription = _HELP( PLUGIN_CONSOLE_PREFIX "CoherentUI on entity" );

                config.nFlags |= EFLN_TARGET_ENTITY;
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
                        pActInfo->pGraph->SetRegularlyUpdated( pActInfo->myID, true );
                        INITIALIZE_OUTPUTS( pActInfo );
                        break;

                    case eFE_SetEntityId:
                        m_pEntity = pActInfo->pEntity;
                        break;

                    case eFE_Activate:
                        if ( m_pEntity )
                        {
                            if ( m_pViewListener )
                            {
                                gCoherentUISystem->DeleteView( m_pViewListener );
                            }

                            std::string sUrl = GetPortString( pActInfo, EIP_URL );
                            std::wstring sUrlW( sUrl.length(), L' ' );
                            sUrlW.assign( sUrl.begin(), sUrl.end() );

                            Coherent::UI::ViewInfo info;
                            info.Width = GetPortInt( pActInfo, EIP_WIDTH );
                            info.Height = GetPortInt( pActInfo, EIP_HEIGHT );
                            info.IsTransparent = GetPortBool( pActInfo, EIP_TRANSPARENT );
                            info.UsesSharedMemory = GetPortBool( pActInfo, EIP_SHARED_MEMORY );
                            info.SupportClickThrough = GetPortBool( pActInfo, EIP_CLICKABLE );

                            ViewConfig viewConfig;
                            viewConfig.ViewInfo = info;
                            viewConfig.Url = sUrlW;
                            viewConfig.Entity = m_pEntity;
                            viewConfig.CollisionMesh = GetPortString( pActInfo, EIP_MESH );

                            m_pViewListener = gCoherentUISystem->CreateView( &viewConfig );
                        }

                        break;

                    case eFE_Update:
                        if ( m_pViewListener )
                        {
                            Coherent::UI::View* view = m_pViewListener->GetView();
                            if ( view )
                            {
                                ActivateOutput<int>( pActInfo, EOP_VIEWID, view->GetId() );
                                pActInfo->pGraph->SetRegularlyUpdated( pActInfo->myID, false );
                            }
                        }
                        break;
                }
            }
    };
}

REGISTER_FLOW_NODE_EX( "CoherentUI_Plugin:OutputEntity", CoherentUIPlugin::CFlowCUIOutputEntityNode, CFlowCUIOutputEntityNode );