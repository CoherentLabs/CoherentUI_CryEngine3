/*************************************************************************
  Crytek Source File.
  Copyright (C), Crytek Studios, 2001-2004.
 -------------------------------------------------------------------------
  $Id$
  $DateTime$
  
 -------------------------------------------------------------------------
  History:
  - 3:8:2004   11:26 : Created by Marcio Martins
  - 17:8:2005        : Modified - NickH: Factory registration moved to GameFactory.cpp

*************************************************************************/
#include "StdAfx.h"
#include "Game.h"
#include "GameCVars.h"
#include "GameActions.h"

#include "GameRules.h"
#include "BulletTime.h"
#include "Audio/GameAudio.h"
#include "WeaponSystem.h"

#include <ICryPak.h>
#include <CryPath.h>
#include <IActionMapManager.h>
#include <IViewSystem.h>
#include <ILevelSystem.h>
#include <IItemSystem.h>
#include <IVehicleSystem.h>
#include <IMovieSystem.h>
#include <IPlayerProfiles.h>
#include <IScaleformGFx.h>
#include <IPlatformOS.h>

#include "ScriptBind_Actor.h"
#include "ScriptBind_Item.h"
#include "ScriptBind_Weapon.h"
#include "ScriptBind_GameRules.h"
#include "ScriptBind_Game.h"
#include "Boids/ScriptBind_Boids.h"

#include "Network/Squad/SquadManager.h"
#include "Network/Lobby/GameBrowser.h"
#include "Network/Lobby/GameUserPackets.h"
#include "Network/Lobby/ScriptBind_MatchMaking.h"

#include "Camera/CameraManager.h"
#include "GameFactory.h"

#include "ItemSharedParams.h"
#include "WeaponSharedParams.h"

#include "Nodes/G2FlowBaseNode.h"

#include "ServerSynchedStorage.h"
#include "ClientSynchedStorage.h"

#include "SPAnalyst.h"

#include "ISaveGame.h"
#include "ILoadGame.h"
#include "CryPath.h"
#include "GameStateRecorder.h"
#include <IPathfinder.h>

#include "Editor/GameRealtimeRemoteUpdate.h"
#include "Graphics/ColorGradientManager.h"
#include "HUD/BitmapUi.h"
#include "HUD/UIManager.h"
// #include "HUD/UIWarnings.h"

#include "Stereo3D/StereoFramework.h"
#include "IMaterialEffects.h"





#include "Player.h"

#include "GameMechanismManager/GameMechanismManager.h"
#include "Testing/FeatureTester.h"
#include "ICheckPointSystem.h"

#include "CodeCheckpointDebugMgr.h"

#include "ScriptBind_HitDeathReactions.h"
#include "HitDeathReactionsSystem.h"


#include <IPluginCoherentUI.h> // CoherentUI
#include <IPluginManager.h>









#define GAME_DEBUG_MEM  // debug memory usage
#undef  GAME_DEBUG_MEM

#define SDK_GUID "{CDCB9B7A-7390-45AA-BF2F-3A7C7933DCF3}"

//FIXME: really horrible. Remove ASAP
int OnImpulse( const EventPhys *pEvent ) 
{ 
	//return 1;
	return 0;
}

#if !defined(FINAL_RELEASE)
#include "Editor/GameRealtimeRemoteUpdate.h"
#endif 
#include "Network/Lobby/CryLobbySessionHandler.h"
#include "ICryLobbyUI.h"

#define GAME_DEBUG_MEM  // debug memory usage
#undef  GAME_DEBUG_MEM

// Needed for the Game02 specific flow node
CG2AutoRegFlowNodeBase *CG2AutoRegFlowNodeBase::m_pFirst=0;
CG2AutoRegFlowNodeBase *CG2AutoRegFlowNodeBase::m_pLast=0;

CGame *g_pGame = 0;
SCVars *g_pGameCVars = 0;
CGameActions *g_pGameActions = 0;

CGame::CGame()
: m_pFramework(0),
	m_pConsole(0),
	m_pWeaponSystem(0),
	m_pScriptBindActor(0),
	m_pScriptBindGame(0),
	m_pPlayerProfileManager(0),
	m_pBulletTime(0),
	m_pGameAudio(0),
	m_pServerSynchedStorage(0),
	m_pClientSynchedStorage(0),
	m_pServerGameTokenSynch(0),
	m_pClientGameTokenSynch(0),
	m_pLobbySessionHandler(0),
	m_clientActorId(-1),
	m_pSPAnalyst(0),
	m_colorGradientManager(0),
	m_pBitmapUi(0),
  m_pRayCaster(0),
	m_pScriptBindHitDeathReactions(0),
	m_pHitDeathReactionsSystem(NULL),
	m_pIntersectionTester(NULL),
	m_cachedUserRegion(-1),
	m_hostMigrationState(eHMS_NotMigrating),
	m_hostMigrationTimeStateChanged(0.f),
	m_randomGenerator(gEnv->bNoRandomSeed?0:(uint32)gEnv->pTimer->GetAsyncTime().GetValue())
{
	m_pCVars = new SCVars();
	g_pGameCVars = m_pCVars;
	m_pGameActions = new CGameActions();
	g_pGameActions = m_pGameActions;
	g_pGame = this;
	m_bReload = false;
	m_inDevMode = false;
	m_pCameraManager = new CCameraManager();

	m_pGameMechanismManager = new CGameMechanismManager();

	m_pRemoteUpdateListener=&CGameRealtimeRemoteUpdateListener::GetGameRealtimeRemoteUpdateListener();

#if !defined(FINAL_RELEASE)
	m_pRemoteUpdateListener=&CGameRealtimeRemoteUpdateListener::GetGameRealtimeRemoteUpdateListener();
#endif

	m_pDefaultAM = 0;
	m_pMultiplayerAM = 0;

	GetISystem()->SetIGame( this );
}

CGame::~CGame()
{
  m_pFramework->EndGameContext();
  m_pFramework->UnregisterListener(this);
	gEnv->pSystem->GetISystemEventDispatcher()->RemoveListener(this);
  ReleaseScriptBinds();
	SAFE_DELETE(m_pBulletTime);
	SAFE_DELETE(m_pGameAudio);
	//SAFE_DELETE(m_pCameraManager);
	SAFE_DELETE(m_pSPAnalyst);
	m_pWeaponSystem->Release();
	SAFE_DELETE(m_pItemStrings);
	SAFE_DELETE(m_pItemSharedParamsList);
	SAFE_DELETE(m_pWeaponSharedParamsList);
	SAFE_DELETE(m_pGameMechanismManager);
	SAFE_DELETE(m_pCVars);
	ClearGameSessionHandler(); // make sure this is cleared before the gamePointer is gone
	g_pGame = 0;
	g_pGameCVars = 0;
	g_pGameActions = 0;
	SAFE_DELETE(m_colorGradientManager);
	SAFE_DELETE(m_pBitmapUi);
	SAFE_DELETE(m_pRayCaster);
	SAFE_DELETE(m_pGameActions);
	SAFE_DELETE(m_pHitDeathReactionsSystem);
	SAFE_DELETE(m_pIntersectionTester);
	gEnv->pGame = 0;
}

bool CGame::Init(IGameFramework *pFramework)
{
  LOADING_TIME_PROFILE_SECTION(GetISystem());

#ifdef GAME_DEBUG_MEM
	DumpMemInfo("CGame::Init start");
#endif

	m_pFramework = pFramework;
	assert(m_pFramework);

	m_pConsole = gEnv->pConsole;

	RegisterConsoleVars();
	RegisterConsoleCommands();
	RegisterGameObjectEvents();

	// Initialize static item strings
	m_pItemStrings = new SItemStrings();

	m_pItemSharedParamsList = new CItemSharedParamsList();
	m_pWeaponSharedParamsList = new CWeaponSharedParamsList();

	LoadActionMaps( ACTIONMAP_DEFAULT_PROFILE );

	InitScriptBinds();

	//load user levelnames for ingame text and savegames
	XmlNodeRef lnames = GetISystem()->LoadXmlFromFile(PathUtil::GetGameFolder() + "/Scripts/GameRules/LevelNames.xml");
	if(lnames)
	{
		int num = lnames->getNumAttributes();
		const char *nameA, *nameB;
		for(int n = 0; n < num; ++n)
		{
			lnames->getAttributeByIndex(n, &nameA, &nameB);
			m_mapNames[string(nameA)] = string(nameB);
		}
	}

  // Register all the games factory classes e.g. maps "Player" to CPlayer
  InitGameFactory(m_pFramework);

	m_colorGradientManager = new Graphics::CColorGradientManager();

	m_pSPAnalyst = new CSPAnalyst();
 
	gEnv->pConsole->CreateKeyBind("f12", "r_getscreenshot 2");

	// set game GUID
	m_pFramework->SetGameGUID(SDK_GUID);
	gEnv->pSystem->GetPlatformOS()->UserDoSignIn(0); // sign in the default user

	gEnv->pSystem->GetISystemEventDispatcher()->RegisterListener(this);
	m_pHitDeathReactionsSystem = new CHitDeathReactionsSystem;
	CRY_ASSERT(m_pHitDeathReactionsSystem);

	// TEMP
	// Load the action map beforehand (see above)
	// afterwards load the user's profile whose action maps get merged with default's action map
	m_pPlayerProfileManager = m_pFramework->GetIPlayerProfileManager();

	bool bIsFirstTime = false;
	const bool bResetProfile = gEnv->pSystem->GetICmdLine()->FindArg(eCLAT_Pre,"ResetProfile") != 0;
	if (m_pPlayerProfileManager)
	{
		const char* userName = gEnv->pSystem->GetLoggedInUserName();

		bool ok = m_pPlayerProfileManager->LoginUser(userName, bIsFirstTime);
		if (ok)
		{
			m_pPlayerProfileManager->SetExclusiveControllerDeviceIndex(0);

			// activate the always present profile "default"
			int profileCount = m_pPlayerProfileManager->GetProfileCount(userName);
			if (profileCount > 0)
			{
				bool handled = false;
				if(gEnv->IsDedicated())
				{
					for(int i = 0; i < profileCount; ++i )
					{
						IPlayerProfileManager::SProfileDescription profDesc;
						ok = m_pPlayerProfileManager->GetProfileInfo(userName, i, profDesc);
						if(ok)
						{
							const IPlayerProfile *preview = m_pPlayerProfileManager->PreviewProfile(userName, profDesc.name);
							int iActive = 0;
							if(preview)
							{
								preview->GetAttribute("Activated",iActive);
							}
							if(iActive>0)
							{
								m_pPlayerProfileManager->ActivateProfile(userName,profDesc.name);
								CryLogAlways("[GameProfiles]: Successfully activated profile '%s' for user '%s'", profDesc.name, userName);
								m_pFramework->GetILevelSystem()->LoadRotation();
								handled = true;
								break;
							}
						}
					}
					m_pPlayerProfileManager->PreviewProfile(userName,NULL);
				}

				if(!handled)
				{
					IPlayerProfileManager::SProfileDescription desc;
					ok = m_pPlayerProfileManager->GetProfileInfo(userName, 0, desc);
					if (ok)
					{
						IPlayerProfile* pProfile = m_pPlayerProfileManager->ActivateProfile(userName, desc.name);

						if (pProfile == 0)
						{
							GameWarning("[GameProfiles]: Cannot activate profile '%s' for user '%s'. Trying to re-create.", desc.name, userName);
							IPlayerProfileManager::EProfileOperationResult profileResult;
							m_pPlayerProfileManager->CreateProfile(userName, desc.name, true, profileResult); // override if present!
							pProfile = m_pPlayerProfileManager->ActivateProfile(userName, desc.name);
							if (pProfile == 0)
								GameWarning("[GameProfiles]: Cannot activate profile '%s' for user '%s'.", desc.name, userName);
							else
								GameWarning("[GameProfiles]: Successfully re-created profile '%s' for user '%s'.", desc.name, userName);
						}

						if (pProfile)
						{
							if (bResetProfile)
							{
								bIsFirstTime = true;
								pProfile->Reset();
								gEnv->pCryPak->RemoveFile("%USER%/game.cfg");
								CryLogAlways("[GameProfiles]: Successfully reset and activated profile '%s' for user '%s'", desc.name, userName);
							}
							CryLogAlways("[GameProfiles]: Successfully activated profile '%s' for user '%s'", desc.name, userName);
							m_pFramework->GetILevelSystem()->LoadRotation();
						}
					}
					else
					{
						GameWarning("[GameProfiles]: Cannot get profile info for user '%s'", userName);
					}
				}
			}
			else
			{
				GameWarning("[GameProfiles]: User 'dude' has no profiles");
			}
		}
		else
			GameWarning("[GameProfiles]: Cannot login user '%s'", userName);
	}
	else
		GameWarning("[GameProfiles]: PlayerProfileManager not available. Running without.");

	if (!m_pServerSynchedStorage)
		m_pServerSynchedStorage = new CServerSynchedStorage(GetIGameFramework());

	if (!m_pBulletTime)
	{
		m_pBulletTime = new CBulletTime();
	}

	if (!m_pGameAudio)
	{
		m_pGameAudio = new CGameAudio();
	}

  m_pFramework->RegisterListener(this,"Game", eFLPriority_Game);

	if ( gEnv->pScaleformGFx && gEnv->pScaleformGFx->IsScaleformSupported() )
	{
		CUIManager::Init();
	}
	else
	{
		if ( m_pBitmapUi == NULL )
		{
			m_pBitmapUi = new CBitmapUi();
		}
	}

	// Initialize Coherent UI
	PluginManager::IPluginBase* pCoherentUIPlugin = gPluginManager->GetPluginByName("CoherentUI");
	m_pCoherentUIPlugin = static_cast<CoherentUIPlugin::IPluginCoherentUI*>(pCoherentUIPlugin ? pCoherentUIPlugin->GetConcreteInterface() : NULL);
	if (m_pCoherentUIPlugin)
	{
		m_pCoherentUIPlugin->InitializeSystem();
	}

	//Even if there's no online multiplayer, we're still required to initialize the lobby as it hosts the PS3 trophies.
	//As info from the trophies must be considered when enumerating disk space on PS3, we can't finish the PlatformOS_PS3 init process
	//without the lobby having been created.

	if (gEnv->pNetwork && !gEnv->IsEditor())
	{
		gEnv->pNetwork->SetMultithreadingMode(INetwork::NETWORK_MT_PRIORITY_NORMAL);
		gEnv->pNetwork->GetLobby()->SetUserPacketEnd(eGUPD_End);

		ECryLobbyServiceFeatures	features = ECryLobbyServiceFeatures( eCLSO_Friends | eCLSO_Base | eCLSO_Stats | eCLSO_Reward | eCLSO_LobbyUI | eCLSO_Matchmaking );
		ECryLobbyError						error = eCLE_Success;
		ECryLobbyService					lobbyService = eCLS_Online;

#if !defined(_RELEASE)
		bool bLAN = !(g_pGameCVars && g_pGameCVars->g_useOnlineLobbyService);
		if (bLAN)
		{
			error = gEnv->pNetwork->GetLobby()->Initialise(eCLS_LAN, features, CGameBrowser::ConfigurationCallback, CGameBrowser::InitialiseCallback, this);
			if (error == eCLE_Success)
				lobbyService = eCLS_LAN;
			else
				CRY_ASSERT_MESSAGE( error == eCLE_Success, "Failed to initialize LAN lobby service" );
		}
#endif
		
		error = gEnv->pNetwork->GetLobby()->Initialise(eCLS_Online, features, CGameBrowser::ConfigurationCallback, CGameBrowser::InitialiseCallback, this);
		
		if( error != eCLE_Success )
		{
			CryWarning( VALIDATOR_MODULE_GAME, VALIDATOR_ERROR, "Failed to initialise CryLobby in Game. Return value is 0x%X", error );
			AddGameWarning("LobbyStartFailed", NULL);
		}

		gEnv->pNetwork->GetLobby()->SetLobbyService(lobbyService);

		m_pSquadManager = new CSquadManager();		// MUST be done before game browser is constructed

		m_pGameBrowser = new CGameBrowser();

		//PS3 TRC R154: Initialise the gamebrowser here to initialise the various components required for connection to PSN and to allow telemetry
		//The PSN trophy system needs to be initialised as close to boot as possible to allow for total game data size checking
		CGameBrowser::InitLobbyServiceType();

		m_pGameLobbyManager = new CGameLobbyManager();

		ICVar* pMaxPlayers = gEnv->pConsole->GetCVar("sv_maxplayers");
		if(pMaxPlayers)
		{
			pMaxPlayers->SetOnChangeCallback(VerifyMaxPlayers);
			pMaxPlayers->Set(MAX_PLAYER_LIMIT);
		}


		ICryLobby *pLobby = gEnv->pNetwork->GetLobby();
		ICryLobbyService *pLobbyService = pLobby ? pLobby->GetLobbyService(eCLS_Online) : NULL;
		ICryLobbyUI *pLobbyUI = pLobbyService ? pLobbyService->GetLobbyUI() : NULL;

		//register callbacks
		if(pLobby)
		{
			pLobby->RegisterEventInterest(eCLSE_InviteAccepted, CGame::InviteAcceptedCallback, this);
			pLobby->RegisterEventInterest(eCLSE_OnlineState, CGame::OnlineStateCallback, this);
		}

		// we need this to setup the xmb buttons, todo: call this on language change to have correct XMB buttons
		if(pLobbyUI)
		{
			pLobbyUI->PostLocalizationChecks();
		}
	}

#if ENABLE_FEATURE_TESTER
	new CFeatureTester();
#endif





	m_pRayCaster = new GlobalRayCaster;
	m_pRayCaster->SetQuota(6);

	m_pIntersectionTester = new GlobalIntersectionTester;
	m_pIntersectionTester->SetQuota(6);

	if (m_pServerSynchedStorage == NULL)
		m_pServerSynchedStorage = new CServerSynchedStorage(GetIGameFramework());

	if (m_pServerGameTokenSynch == NULL)
		m_pServerGameTokenSynch = new CServerGameTokenSynch(m_pFramework->GetIGameTokenSystem());
	
#ifdef GAME_DEBUG_MEM
	DumpMemInfo("CGame::Init end");
#endif

	m_pWeaponSystem = new CWeaponSystem(this, GetISystem());

	const char* itemFolder = "scripts/entities/items/xml";
	m_pFramework->GetIItemSystem()->Scan(itemFolder);
	m_pWeaponSystem->Scan(itemFolder);

	//Set correct SessionHandler
	m_pLobbySessionHandler = new CCryLobbySessionHandler();

	return true;
}

bool CGame::CompleteInit()
{
#ifdef GAME_DEBUG_MEM
	DumpMemInfo("CGame::CompleteInit");
#endif

	return true;
}

void CGame::RegisterGameFlowNodes()
{
	// Initialize Game02 flow nodes
	if (IFlowSystem *pFlow = m_pFramework->GetIFlowSystem())
	{
		CG2AutoRegFlowNodeBase *pFactory = CG2AutoRegFlowNodeBase::m_pFirst;

		while (pFactory)
		{
			pFlow->RegisterType( pFactory->m_sClassName,pFactory );
			pFactory = pFactory->m_pNext;
		}
	}
}

void CGame::ResetServerGameTokenSynch()
{
	if (m_pServerGameTokenSynch)
	{
		m_pServerGameTokenSynch->~CServerGameTokenSynch();
		new(m_pServerGameTokenSynch) CServerGameTokenSynch(m_pFramework->GetIGameTokenSystem());
	}
	else
	{
		m_pServerGameTokenSynch = new CServerGameTokenSynch(m_pFramework->GetIGameTokenSystem());
	}
}

// Small test for the IPathfinder.h interfaces
/*
extern INavPath *g_testPath;
extern IPathFollower *g_pathFollower;
extern Vec3 g_pos;
extern Vec3 g_vel;
*/

int CGame::Update(bool haveFocus, unsigned int updateFlags)
{
	bool bRun = m_pFramework->PreUpdate( true, updateFlags );

	float frameTime = gEnv->pTimer->GetFrameTime();

	m_colorGradientManager->UpdateForThisFrame(frameTime);

	if(m_pRayCaster)
	{
		m_pRayCaster->Update(frameTime);
	}

	if (m_pIntersectionTester)
	{
			FRAME_PROFILER("GlobalIntersectionTester", gEnv->pSystem, PROFILE_AI);

			m_pIntersectionTester->SetQuota(6);
			m_pIntersectionTester->Update(frameTime);
	}

	if (m_pFramework->IsGamePaused() == false)
	{
		m_pWeaponSystem->Update(frameTime);

		m_pBulletTime->Update();

		m_pGameAudio->Update();

		m_pRemoteUpdateListener->Update();
	}
	else
	{
		if (m_hostMigrationState == eHMS_WaitingForPlayers)
		{
			if (gEnv->bServer)
			{
				if (GetRemainingHostMigrationTimeoutTime() <= 0.f)
				{
					CryLog("CGame: HostMigration timeout reached");
					SetHostMigrationState(eHMS_Resuming);
				}
			}
		}
		else if (m_hostMigrationState == eHMS_Resuming)
		{
			const float curTime = gEnv->pTimer->GetAsyncCurTime();
			const float timePassed = curTime - m_hostMigrationTimeStateChanged;
			const float timeRemaining = g_pGameCVars->g_hostMigrationResumeTime - timePassed;
			if (timeRemaining > 0.f)
			{
				// todo: UI
			}
			else
			{
				SetHostMigrationState(eHMS_NotMigrating);
			}
		}
	}

	UpdateInviteAcceptedState();
	m_pGameMechanismManager->Update(frameTime);








	// Small test for the IPathfinder.h interfaces
	/* 
	if (g_testPath && !g_testPath->Empty())
	{
		float dt=gEnv->pTimer->GetFrameTime();

		PathFollowResult result;
		g_pathFollower->Update(result, g_pos, g_vel, dt);

		if (!result.reachedEnd)
		{
			g_pos+=result.velocityOut*dt;
			g_vel=result.velocityOut;
		}
		else
			g_vel.zero();

		g_pathFollower->Draw(gEnv->pRenderer, Vec3(0.0f, 0.0f, 0.5f));

		if (result.reachedEnd)
			g_testPath->Clear("");
	}
	*/

#if !defined(FINAL_RELEASE)
	m_pRemoteUpdateListener->Update();
#endif 

	m_pFramework->PostUpdate( true, updateFlags );

	if(m_inDevMode != gEnv->pSystem->IsDevMode())
	{
		m_inDevMode = gEnv->pSystem->IsDevMode();
		m_pFramework->GetIActionMapManager()->EnableActionMap("debug", m_inDevMode);
	}
	
  Stereo3D::Update(frameTime);

	CheckReloadLevel();
	
	return bRun ? 1 : 0;
}

void CGame::ConfigureGameChannel(bool isServer, IProtocolBuilder *pBuilder)
{
	if (isServer)
	{
		m_pServerSynchedStorage->DefineProtocol(pBuilder);
		m_pServerGameTokenSynch->DefineProtocol(pBuilder);
	}
	else
	{
		m_pClientSynchedStorage = new CClientSynchedStorage(GetIGameFramework());
		m_pClientSynchedStorage->DefineProtocol(pBuilder);

		m_pClientGameTokenSynch = new CClientGameTokenSynch(m_pFramework->GetIGameTokenSystem());
		m_pClientGameTokenSynch->DefineProtocol(pBuilder);
	}
}

void CGame::EditorResetGame(bool bStart)
{
	CRY_ASSERT(gEnv->IsEditor());

	if(bStart)
	{
		IActionMapManager* pAM = m_pFramework->GetIActionMapManager();
		if (pAM)
		{
			pAM->EnableActionMap(0, true); // enable all action maps
			pAM->EnableFilter(0, false); // disable all filters
		}		
	}	
}

void CGame::PlayerIdSet(EntityId playerId)
{
	m_clientActorId = playerId;	
}

string CGame::InitMapReloading()
{
	string levelFileName = GetIGameFramework()->GetLevelName();
	levelFileName = PathUtil::GetFileName(levelFileName);
	if(const char* visibleName = GetMappedLevelName(levelFileName))
		levelFileName = visibleName;

	levelFileName.append("_cryengine.cryenginejmsf");
	if (m_pPlayerProfileManager)
	{
		const char* userName = GetISystem()->GetLoggedInUserName();
		IPlayerProfile* pProfile = m_pPlayerProfileManager->GetCurrentProfile(userName);
		if (pProfile)
		{
			const char* sharedSaveGameFolder = m_pPlayerProfileManager->GetSharedSaveGameFolder();
			if (sharedSaveGameFolder && *sharedSaveGameFolder)
			{
				string prefix = pProfile->GetName();
				prefix+="_";
				levelFileName = prefix + levelFileName;
			}
			ISaveGameEnumeratorPtr pSGE = pProfile->CreateSaveGameEnumerator();
			ISaveGameEnumerator::SGameDescription desc;	
			const int nSaveGames = pSGE->GetCount();
			for (int i=0; i<nSaveGames; ++i)
			{
				if (pSGE->GetDescription(i, desc))
				{
					if(!stricmp(desc.name,levelFileName.c_str()))
					{
						m_bReload = true;
						return levelFileName;
					}
				}
			}
		}
	}
#ifndef WIN32
	m_bReload = true; //using map command
#else
	m_bReload = false;
	levelFileName.clear();
#endif
	return levelFileName;
}

void CGame::Shutdown()
{
	if (m_pPlayerProfileManager)
	{
		m_pPlayerProfileManager->LogoutUser(m_pPlayerProfileManager->GetCurrentUser());
	}

	SAFE_DELETE(m_pServerSynchedStorage);
	SAFE_DELETE(m_pServerGameTokenSynch);

	if ( gEnv->pScaleformGFx && gEnv->pScaleformGFx->IsScaleformSupported() )
	{
		CUIManager::Destroy();
	}

	this->~CGame();
}

const char *CGame::GetLongName()
{
	return GAME_LONGNAME;
}

const char *CGame::GetName()
{
	return GAME_NAME;
}

void CGame::OnPostUpdate(float fDeltaTime)
{
	//update camera system
	//m_pCameraManager->Update();
}

void CGame::OnSaveGame(ISaveGame* pSaveGame)
{
	ScopedSwitchToGlobalHeap useGlobalHeap;

	IActor*		pActor = GetIGameFramework()->GetClientActor();
	CPlayer*	pPlayer = static_cast<CPlayer*>(pActor);
	GetGameRules()->PlayerPosForRespawn(pPlayer, true);

	//save difficulty
	pSaveGame->AddMetadata("sp_difficulty", g_pGameCVars->g_difficultyLevel);

	//write file to profile
	if(m_pPlayerProfileManager)
	{
		const char* saveGameFolder = m_pPlayerProfileManager->GetSharedSaveGameFolder();
		const bool bSaveGameFolderShared = saveGameFolder && *saveGameFolder;
		const char *user = m_pPlayerProfileManager->GetCurrentUser();
		if(IPlayerProfile *pProfile = m_pPlayerProfileManager->GetCurrentProfile(user))
		{
			string filename(pSaveGame->GetFileName());
			CryFixedStringT<128> profilename(pProfile->GetName());
			profilename+='_';
			filename = filename.substr(filename.rfind('/')+1);
			// strip profileName_ prefix
			if (bSaveGameFolderShared)
			{
				if(strnicmp(filename.c_str(), profilename.c_str(), profilename.length()) == 0)
					filename = filename.substr(profilename.length());
			}
			pProfile->SetAttribute("Singleplayer.LastSavedGame", filename);
		}
	}

	pSaveGame->AddMetadata("v_altitudeLimit", g_pGameCVars->pAltitudeLimitCVar->GetString());
}

void CGame::OnLoadGame(ILoadGame* pLoadGame)
{
	int difficulty = g_pGameCVars->g_difficultyLevel;
	pLoadGame->GetMetadata("sp_difficulty", difficulty);
	if(difficulty != g_pGameCVars->g_difficultyLevel)
	{
		IPlayerProfile *pProfile = m_pPlayerProfileManager->GetCurrentProfile(m_pPlayerProfileManager->GetCurrentUser());
		if(pProfile)
		{
			pProfile->SetAttribute("Singleplayer.LastSelectedDifficulty", difficulty);
			pProfile->SetAttribute("Option.g_difficultyLevel", difficulty);
			IPlayerProfileManager::EProfileOperationResult result;
			const int reason = 0;
			m_pPlayerProfileManager->SaveProfile(m_pPlayerProfileManager->GetCurrentUser(), result, reason);
		}		
	}

	// altitude limit
	const char* v_altitudeLimit =	pLoadGame->GetMetadata("v_altitudeLimit");
	if (v_altitudeLimit && *v_altitudeLimit)
		g_pGameCVars->pAltitudeLimitCVar->ForceSet(v_altitudeLimit);
	else
	{
		CryFixedStringT<128> buf;
		buf.FormatFast("%g", g_pGameCVars->v_altitudeLimitDefault());
		g_pGameCVars->pAltitudeLimitCVar->ForceSet(buf.c_str());
	}
}

void CGame::OnActionEvent(const SActionEvent& event)
{ 
	switch(event.m_event)
  {
  case  eAE_channelDestroyed:
    GameChannelDestroyed(event.m_value == 1);
    break;
	case eAE_serverIp:
		if(gEnv->bServer && GetServerSynchedStorage())
		{
			GetServerSynchedStorage()->SetGlobalValue(GLOBAL_SERVER_IP_KEY,string(event.m_description));
			GetServerSynchedStorage()->SetGlobalValue(GLOBAL_SERVER_PUBLIC_PORT_KEY,event.m_value);
		}
		break;
	case eAE_serverName:
		if(gEnv->bServer && GetServerSynchedStorage())
			GetServerSynchedStorage()->SetGlobalValue(GLOBAL_SERVER_NAME_KEY,string(event.m_description));
		break;
	case eAE_unloadLevel:
			m_clientActorId = 0;
		break;
	case eAE_mapCmdIssued:
		if (gEnv->bMultiplayer)
		{
			if (CGameLobby *pGameLobby = g_pGame->GetGameLobby())
			{
				pGameLobby->OnMapCommandIssued();
			}
		}
		break;
	case eAE_inGame:
		if (m_pCoherentUIPlugin)
		{
			IPlayerEventListener* pPlayerEventListner = m_pCoherentUIPlugin->GetPlayerEventListener();
			CPlayer* pPlayer = static_cast<CPlayer*>(gEnv->pGameFramework->GetClientActor());
			CRY_ASSERT(pPlayer != NULL);
			pPlayer->RegisterPlayerEventListener(pPlayerEventListner);
		}
		break;
  }
}


void CGame::GameChannelDestroyed(bool isServer)
{
  if (!isServer)
  {
	SAFE_DELETE(m_pClientSynchedStorage);
	SAFE_DELETE(m_pClientGameTokenSynch);

	if (!gEnv->pSystem->IsSerializingFile())
	{
		CryFixedStringT<128> buf;
		buf.FormatFast("%g", g_pGameCVars->v_altitudeLimitDefault());
		g_pGameCVars->pAltitudeLimitCVar->ForceSet(buf.c_str());
	}
  }
}


void CGame::BlockingProcess(BlockingConditionFunction f)
{
  INetwork* pNetwork = gEnv->pNetwork;

  if (!pNetwork)
	  return;

  bool ok = false;

  ITimer * pTimer = gEnv->pTimer;
  CTimeValue startTime = pTimer->GetAsyncTime();

  while (!ok)
  {
    pNetwork->SyncWithGame(eNGS_FrameStart);
    pNetwork->SyncWithGame(eNGS_FrameEnd);
    gEnv->pTimer->UpdateOnFrameStart();
    ok = (*f)();
  }
}

uint32 CGame::AddGameWarning(const char* stringId, const char* paramMessage, IGameWarningsListener* pListener)
{
	if(CUIManager::GetInstance() && !gEnv->IsDedicated())
	{
		return CUIManager::GetInstance()->GetWarningManager()->AddGameWarning(stringId, paramMessage, pListener);
	}
	else
	{
		CryLogAlways("GameWarning trying to display: %s", stringId);
		return 0;
	}
}

void CGame::RemoveGameWarning(const char* stringId)
{
	if(CUIManager::GetInstance() && !gEnv->IsDedicated())
	{
		CUIManager::GetInstance()->GetWarningManager()->RemoveGameWarning(stringId);
	}
}







const static uint8 drmKeyData[16] = {0};
const static char* drmFiles = NULL;


const uint8* CGame::GetDRMKey()
{
	return drmKeyData;
}

const char* CGame::GetDRMFileList()
{
	return drmFiles;
}

CGameRules *CGame::GetGameRules() const
{
	return static_cast<CGameRules *>(m_pFramework->GetIGameRulesSystem()->GetCurrentGameRules());
}

CBulletTime *CGame::GetBulletTime() const
{
	return m_pBulletTime;
}

void CGame::LoadActionMaps(const char* filename)
{
	CRY_ASSERT_MESSAGE((filename || *filename != 0), "filename is empty!");
	if(g_pGame->GetIGameFramework()->IsGameStarted())
	{
		CryLogAlways("Can't change configuration while game is running (yet)");
		return;
	}

	IActionMapManager *pActionMapMan = m_pFramework->GetIActionMapManager();
	pActionMapMan->AddInputDeviceMapping(eAID_KeyboardMouse, "keyboard");
	pActionMapMan->AddInputDeviceMapping(eAID_XboxPad, "xboxpad");
	pActionMapMan->AddInputDeviceMapping(eAID_PS3Pad, "ps3pad");
	pActionMapMan->AddInputDeviceMapping(eAID_WiiPad, "wiipad");

	// make sure that they are also added to the GameActions.actions file!
	XmlNodeRef rootNode = m_pFramework->GetISystem()->LoadXmlFromFile(filename);
	if(rootNode)
	{
		pActionMapMan->Clear();
		pActionMapMan->LoadFromXML(rootNode);
		m_pDefaultAM = pActionMapMan->GetActionMap("default");
		m_pDebugAM = pActionMapMan->GetActionMap("debug");
		m_pMultiplayerAM = pActionMapMan->GetActionMap("multiplayer");

		// enable defaults
		pActionMapMan->EnableActionMap("default",true);

		// enable debug
		pActionMapMan->EnableActionMap("debug",gEnv->pSystem->IsDevMode());

		// enable player action map
		pActionMapMan->EnableActionMap("player",true);
	}
	else
	{
		CryLogAlways("[game] error: Could not open configuration file %s", filename);
		CryLogAlways("[game] error: this will probably cause an infinite loop later while loading a map");
	}

	m_pGameActions->Init();
}

void CGame::InitScriptBinds()
{
	m_pScriptBindActor = new CScriptBind_Actor(m_pFramework->GetISystem());
	m_pScriptBindItem = new CScriptBind_Item(m_pFramework->GetISystem(), m_pFramework);
	m_pScriptBindWeapon = new CScriptBind_Weapon(m_pFramework->GetISystem(), m_pFramework);
	m_pScriptBindGameRules = new CScriptBind_GameRules(m_pFramework->GetISystem(), m_pFramework);
	m_pScriptBindGame = new CScriptBind_Game(m_pFramework->GetISystem(), m_pFramework);
	m_pScriptBindsBoids = new CScriptBind_Boids(GetISystem());
	m_pScriptBindHitDeathReactions = new CScriptBind_HitDeathReactions(m_pFramework->GetISystem(), m_pFramework);
	m_pScriptBindMatchMaking = new CScriptBind_MatchMaking(m_pFramework->GetISystem());
}

void CGame::ReleaseScriptBinds()
{
	SAFE_DELETE(m_pScriptBindActor);
	SAFE_DELETE(m_pScriptBindItem);
	SAFE_DELETE(m_pScriptBindWeapon);
	SAFE_DELETE(m_pScriptBindGameRules);
	SAFE_DELETE(m_pScriptBindGame);
	SAFE_DELETE(m_pScriptBindsBoids);
	SAFE_DELETE(m_pScriptBindHitDeathReactions);
	SAFE_DELETE(m_pScriptBindMatchMaking);
}

void CGame::CheckReloadLevel()
{
	if(!m_bReload)
		return;

	if(gEnv->IsEditor() || gEnv->bMultiplayer)
	{
		if(m_bReload)
			m_bReload = false;
		return;
	}

#ifdef WIN32
	// Restart interrupts cutscenes
	gEnv->pMovieSystem->StopAllCutScenes();

	GetISystem()->SerializingFile(1);

	//load levelstart
	ILevelSystem* pLevelSystem = m_pFramework->GetILevelSystem();
	ILevel*			pLevel = pLevelSystem->GetCurrentLevel();
	ILevelInfo* pLevelInfo = pLevelSystem->GetLevelInfo(m_pFramework->GetLevelName());
	//**********
	EntityId playerID = GetIGameFramework()->GetClientActorId();
	pLevelSystem->OnLoadingStart(pLevelInfo);
	PlayerIdSet(playerID);
	string levelstart(GetIGameFramework()->GetLevelName());
	if(const char* visibleName = GetMappedLevelName(levelstart))
		levelstart = visibleName;

	levelstart.append("_cryengine.cryenginejmsf");
	GetIGameFramework()->LoadGame(levelstart.c_str(), true, true);
	//**********
	pLevelSystem->OnLoadingComplete(pLevel);
	m_bReload = false;	//if m_bReload is true - load at levelstart

	//if paused - start game
	m_pFramework->PauseGame(false, true);

	GetISystem()->SerializingFile(0);
#else
	string command("map ");
	command.append(m_pFramework->GetLevelName());
	gEnv->pConsole->ExecuteString(command);
#endif
}

void CGame::RegisterGameObjectEvents()
{
	IGameObjectSystem* pGOS = m_pFramework->GetIGameObjectSystem();

	pGOS->RegisterEvent(eCGE_PostFreeze, "PostFreeze");
	pGOS->RegisterEvent(eCGE_PostShatter,"PostShatter");
	pGOS->RegisterEvent(eCGE_OnShoot,"OnShoot");
	pGOS->RegisterEvent(eCGE_Recoil,"Recoil");
	pGOS->RegisterEvent(eCGE_BeginReloadLoop,"BeginReloadLoop");
	pGOS->RegisterEvent(eCGE_EndReloadLoop,"EndReloadLoop");
	pGOS->RegisterEvent(eCGE_ActorRevive,"ActorRevive");
	pGOS->RegisterEvent(eCGE_VehicleDestroyed,"VehicleDestroyed");
	pGOS->RegisterEvent(eCGE_TurnRagdoll,"TurnRagdoll");
	pGOS->RegisterEvent(eCGE_EnableFallAndPlay,"EnableFallAndPlay");
	pGOS->RegisterEvent(eCGE_DisableFallAndPlay,"DisableFallAndPlay");
	pGOS->RegisterEvent(eCGE_VehicleTransitionEnter,"VehicleTransitionEnter");
	pGOS->RegisterEvent(eCGE_VehicleTransitionExit,"VehicleTransitionExit");
	pGOS->RegisterEvent(eCGE_TextArea,"TextArea");
	pGOS->RegisterEvent(eCGE_InitiateAutoDestruction,"InitiateAutoDestruction");
	pGOS->RegisterEvent(eCGE_Event_Collapsing,"Event_Collapsing");
	pGOS->RegisterEvent(eCGE_Event_Collapsed,"Event_Collapsed");
	pGOS->RegisterEvent(eCGE_MultiplayerChatMessage,"MultiplayerChatMessage");
	pGOS->RegisterEvent(eCGE_ResetMovementController,"ResetMovementController");
	pGOS->RegisterEvent(eCGE_AnimateHands,"AnimateHands");
	pGOS->RegisterEvent(eCGE_Ragdoll,"Ragdoll");
	pGOS->RegisterEvent(eCGE_EnablePhysicalCollider,"EnablePhysicalCollider");
	pGOS->RegisterEvent(eCGE_DisablePhysicalCollider,"DisablePhysicalCollider");
	pGOS->RegisterEvent(eCGE_RebindAnimGraphInputs,"RebindAnimGraphInputs");
	pGOS->RegisterEvent(eCGE_OpenParachute, "OpenParachute");
	pGOS->RegisterEvent(eCGE_ReactionEnd, "ReactionEnd");
}

void CGame::GetMemoryStatistics(ICrySizer * s) const
{
	s->Add(*this);
	m_pWeaponSystem->GetMemoryUsage(s);

	s->Add(*m_pScriptBindActor);
	s->Add(*m_pScriptBindItem);
	s->Add(*m_pScriptBindWeapon);
	s->Add(*m_pScriptBindGameRules);
	s->Add(*m_pScriptBindGame);
	s->Add(*m_pScriptBindsBoids);
	s->Add(*m_pScriptBindHitDeathReactions);
	s->Add(*m_pScriptBindMatchMaking);
	s->Add(*m_pGameActions);


	m_pItemSharedParamsList->GetMemoryUsage(s);
	m_pWeaponSharedParamsList->GetMemoryUsage(s);

	if (m_pPlayerProfileManager)
	  m_pPlayerProfileManager->GetMemoryUsage(s);

	if (m_pServerSynchedStorage)
		m_pServerSynchedStorage->GetMemoryUsage(s);

	if (m_pClientSynchedStorage)
		m_pClientSynchedStorage->GetMemoryUsage(s);

	if (m_pServerGameTokenSynch)
		m_pServerGameTokenSynch->GetMemoryUsage(s);

	if (m_pClientGameTokenSynch)
		m_pClientGameTokenSynch->GetMemoryUsage(s);

	if (m_pHitDeathReactionsSystem)
			m_pHitDeathReactionsSystem->GetMemoryUsage(s);
}

void CGame::OnClearPlayerIds()
{
}

void CGame::DumpMemInfo(const char* format, ...)
{
	CryModuleMemoryInfo memInfo;
	CryGetMemoryInfoForModule(&memInfo);

	va_list args;
	va_start(args,format);
	gEnv->pLog->LogV( ILog::eAlways,format,args );
	va_end(args);

	gEnv->pLog->LogWithType( ILog::eAlways, "Alloc=%I64d kb  String=%I64d kb  STL-alloc=%I64d kb  STL-wasted=%I64d kb", (memInfo.allocated - memInfo.freed) >> 10 , memInfo.CryString_allocated >> 10, memInfo.STL_allocated >> 10 , memInfo.STL_wasted >> 10);
	// gEnv->pLog->LogV( ILog::eAlways, "%s alloc=%llu kb  instring=%llu kb  stl-alloc=%llu kb  stl-wasted=%llu kb", text, memInfo.allocated >> 10 , memInfo.CryString_allocated >> 10, memInfo.STL_allocated >> 10 , memInfo.STL_wasted >> 10);
}


const string& CGame::GetLastSaveGame(string &levelName)
{
	if (m_pPlayerProfileManager)
	{
		const char* userName = GetISystem()->GetLoggedInUserName();
		IPlayerProfile* pProfile = m_pPlayerProfileManager->GetCurrentProfile(userName);
		if (pProfile)
		{
			ISaveGameEnumeratorPtr pSGE = pProfile->CreateSaveGameEnumerator();
			ISaveGameEnumerator::SGameDescription desc;	
			time_t curLatestTime = (time_t) 0;
			const char* lastSaveGame = "";
			const int nSaveGames = pSGE->GetCount();
			for (int i=0; i<nSaveGames; ++i)
			{
				if (pSGE->GetDescription(i, desc))
				{
					if (desc.metaData.saveTime > curLatestTime)
					{
						lastSaveGame = desc.name;
						curLatestTime = desc.metaData.saveTime;
						levelName = desc.metaData.levelName;
					}
				}
			}
			m_lastSaveGame = lastSaveGame;
		}
	}

	return m_lastSaveGame;
}

bool RespawnIfDead(CActor *pActor)
{
	if(pActor && pActor->IsDead())
	{
		pActor->StandUp();

		pActor->Revive();

		pActor->SetHealth(pActor->GetMaxHealth());

		pActor->HolsterItem(true);

		pActor->HolsterItem(false);

		if( IEntity *pEntity = pActor->GetEntity() )
		{
			pEntity->GetAI()->Event(AIEVENT_ENABLE, NULL);
		}

		return true;
	}

	return false;
}


bool CGame::LoadLastSave()
{
	if (gEnv->bMultiplayer)
		return false;

	bool bLoadSave = true;
	if (gEnv->IsEditor())
	{
		ICVar* pAllowSaveLoadInEditor = gEnv->pConsole->GetCVar("g_allowSaveLoadInEditor");
		if (pAllowSaveLoadInEditor)
		{
			bLoadSave = (pAllowSaveLoadInEditor->GetIVal() != 0);
		}
		else
		{
			bLoadSave = false;
		}

		if (!bLoadSave) // Wont go through normal path which reloads hud, reload here
		{
			g_pGame->PostSerialize();
		}
	}

	bool bSuccess = true;

	if (bLoadSave)
	{
		if(g_pGameCVars->g_enableSlimCheckpoints)
			bSuccess = GetIGameFramework()->GetICheckpointSystem()->LoadLastCheckpoint();
		else
		{
			const string& file = g_pGame->GetLastSaveGame();

			if(file.length())
			{
				if(!g_pGame->GetIGameFramework()->LoadGame(file.c_str(), true))
					bSuccess = g_pGame->GetIGameFramework()->LoadGame(file.c_str(), false);
			}
		}
	}
	else
	{
		CActor* pPlayer = static_cast<CActor*>(GetIGameFramework()->GetClientActor());
		bSuccess = RespawnIfDead( pPlayer );
	}

	return bSuccess;
}


void CGame::PostSerialize()
{

}


ILINE void expandSeconds(int secs, int& days, int& hours, int& minutes, int& seconds)
{
	days  = secs / 86400;
	secs -= days * 86400;
	hours = secs / 3600;
	secs -= hours * 3600;
	minutes = secs / 60;
	seconds = secs - minutes * 60;
	hours += days*24;
	days = 0;
}

void secondsToString(int secs, string& outString)
{
	int d,h,m,s;
	expandSeconds(secs, d, h, m, s);
	if (h > 0)
		outString.Format("%02dh_%02dm_%02ds", h, m, s);
	else
		outString.Format("%02dm_%02ds", m, s);
}

IGame::TSaveGameName CGame::CreateSaveGameName()
{
	//design wants to have different, more readable names for the savegames generated
	int id = 0;

	TSaveGameName saveGameName;



	//saves a running savegame id which is displayed with the savegame name
	if(IPlayerProfileManager *m_pPlayerProfileManager = gEnv->pGame->GetIGameFramework()->GetIPlayerProfileManager())
	{
		const char *user = m_pPlayerProfileManager->GetCurrentUser();
		if(IPlayerProfile *pProfile = m_pPlayerProfileManager->GetCurrentProfile(user))
		{
			pProfile->GetAttribute("Singleplayer.SaveRunningID", id);
			pProfile->SetAttribute("Singleplayer.SaveRunningID", id+1);
		}
	}

	saveGameName = CRY_SAVEGAME_FILENAME;
	char buffer[16];
	itoa(id, buffer, 10);
	saveGameName.clear();
	if(id < 10)
		saveGameName += "0";
	saveGameName += buffer;
	saveGameName += "_";

	const char* levelName = GetIGameFramework()->GetLevelName();
	const char* mappedName = GetMappedLevelName(levelName);
	saveGameName += mappedName;
	saveGameName += "_cryengine";

	saveGameName += CRY_SAVEGAME_FILE_EXT;

	return saveGameName;

}

const char* CGame::GetMappedLevelName(const char *levelName) const
{ 
	TLevelMapMap::const_iterator iter = m_mapNames.find(CONST_TEMP_STRING(levelName));
	return (iter == m_mapNames.end()) ? levelName : iter->second.c_str();
}


IGameStateRecorder* CGame::CreateGameStateRecorder(IGameplayListener* pL)
{
	ScopedSwitchToGlobalHeap globalHeap;

	CGameStateRecorder* pGSP = new CGameStateRecorder();
	
	if(pGSP)
		pGSP->RegisterListener(pL);

	return (IGameStateRecorder*)pGSP;

}

Graphics::CColorGradientManager& CGame::GetColorGradientManager()
{
	return *m_colorGradientManager;
}

void CGame::OnSystemEvent( ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam )
{
		switch (event)
		{
		case ESYSTEM_EVENT_LEVEL_LOAD_START:
				{
						if (!m_pRayCaster)
						{
								m_pRayCaster = new GlobalRayCaster;
								m_pRayCaster->SetQuota(6);
						}
						if (!m_pIntersectionTester)
						{
								m_pIntersectionTester = new GlobalIntersectionTester;
								m_pIntersectionTester->SetQuota(6);
						}
				}
				break;
		case ESYSTEM_EVENT_LEVEL_UNLOAD:
				{
						SAFE_DELETE(m_pRayCaster);
						SAFE_DELETE(m_pIntersectionTester);

						m_pHitDeathReactionsSystem->Reset();
				}
				break;
		}
}

CGameLobby* CGame::GetGameLobby()
{
	return m_pGameLobbyManager ? m_pGameLobbyManager->GetGameLobby() : NULL;
}

bool CGame::IsGameActive() const
{
	assert(g_pGame);
	IGameFramework* pGameFramework = g_pGame->GetIGameFramework();
	assert(pGameFramework);
	return (pGameFramework->StartingGameContext() || pGameFramework->StartedGameContext()) && (IsGameSessionHostMigrating() || pGameFramework->GetClientChannel());
}

void CGame::ClearGameSessionHandler()
{
	GetIGameFramework()->SetGameSessionHandler(NULL);
	m_pLobbySessionHandler = NULL;
}

uint32 CGame::GetRandomNumber()
{
	return m_randomGenerator.Generate();
}

void CGame::VerifyMaxPlayers(ICVar * pVar)
{
	int nPlayers = pVar->GetIVal();
	if (nPlayers < 2)
		pVar->Set(2);
}

//------------------------------------------------------------------------
float CGame::GetTimeSinceHostMigrationStateChanged() const
{
	const float curTime = gEnv->pTimer->GetAsyncCurTime();
	const float timePassed = curTime - m_hostMigrationTimeStateChanged;
	return timePassed;
}

//------------------------------------------------------------------------
float CGame::GetRemainingHostMigrationTimeoutTime() const
{
	const float timePassed = GetTimeSinceHostMigrationStateChanged();
	const float timeRemaining = m_hostMigrationNetTimeoutLength - timePassed;
	return MAX(timeRemaining, 0.f);
}

//------------------------------------------------------------------------
float CGame::GetHostMigrationTimeTillResume() const
{
	float timeRemaining = 0.f;
	if (m_hostMigrationState == eHMS_WaitingForPlayers)
	{
		timeRemaining = GetRemainingHostMigrationTimeoutTime() + g_pGameCVars->g_hostMigrationResumeTime;
	}
	else if (m_hostMigrationState == eHMS_Resuming)
	{
		const float curTime = gEnv->pTimer->GetAsyncCurTime();
		const float timePassed = curTime - m_hostMigrationTimeStateChanged;
		timeRemaining = MAX(g_pGameCVars->g_hostMigrationResumeTime - timePassed, 0.f);
	}
	return timeRemaining;
}

//------------------------------------------------------------------------
void CGame::SetHostMigrationState(EHostMigrationState newState)
{
	float timeOfChange = gEnv->pTimer->GetAsyncCurTime();
	SetHostMigrationStateAndTime(newState, timeOfChange);
}

//------------------------------------------------------------------------
void CGame::SetHostMigrationStateAndTime( EHostMigrationState newState, float timeOfChange )
{
	CryLog("CGame::SetHostMigrationState() state changing to '%i' (from '%i')", int(newState), int(m_hostMigrationState));

	if ((m_hostMigrationState == eHMS_NotMigrating) && (newState != eHMS_NotMigrating))
	{
		m_pFramework->PauseGame(true, false);

		ICVar *pTimeoutCVar = gEnv->pConsole->GetCVar("net_migrate_timeout");
		m_hostMigrationNetTimeoutLength = pTimeoutCVar->GetFVal();
		pTimeoutCVar->SetOnChangeCallback(OnHostMigrationNetTimeoutChanged);
	}

	m_hostMigrationState = newState;
	m_hostMigrationTimeStateChanged = timeOfChange;

	if (newState == eHMS_WaitingForPlayers)
	{
		// todo: show UI host migration
	}
	else if (newState == eHMS_Resuming)
	{
		// todo: hide UI host migration
	}
	else if (newState == eHMS_NotMigrating)
	{
		AbortHostMigration();
	}

	// Notify the gamerules
	CGameRules *pGameRules = GetGameRules();
	pGameRules->OnHostMigrationStateChanged();
}

//------------------------------------------------------------------------
void CGame::AbortHostMigration()
{
	m_pFramework->PauseGame(false, false);
	m_hostMigrationState = eHMS_NotMigrating;
	m_hostMigrationTimeStateChanged = 0.f;
	ICVar *pTimeoutCVar = gEnv->pConsole->GetCVar("net_migrate_timeout");
	pTimeoutCVar->SetOnChangeCallback(NULL);
}

//------------------------------------------------------------------------
void CGame::OnHostMigrationNetTimeoutChanged(ICVar *pVar)
{
	g_pGame->m_hostMigrationNetTimeoutLength = pVar->GetFVal();
}


//static---------------------------------------
void CGame::InviteAcceptedCallback(UCryLobbyEventData eventData, void *arg)
{
	CryLog("[Invite] InviteAcceptedCallback");

	CGame *pGame = (CGame*)arg;
	SCryLobbyInviteAcceptedData* inviteData = eventData.pInviteAcceptedData;

	bool acceptInvite = true;

	CRY_ASSERT_MESSAGE(pGame, "No game!");

	// we should always accept the invite if we have no exclusive controller
	//if(pGame->GetExclusiveControllerDeviceIndex())
	{
		// can't possibly be in a squad if we're not multiplayer, i hope
		if(gEnv->bMultiplayer && inviteData->m_error == eCLE_Success)
		{
			ICryLobby *pLobby = gEnv->pNetwork->GetLobby();
			ICryLobbyService *pLobbyService = pLobby ? pLobby->GetLobbyService(eCLS_Online) : NULL;
			ICryMatchMaking *pMatchMaking = pLobbyService ? pLobbyService->GetMatchMaking(): NULL;

			bool alreadyInThisSquad = false;

			// the session we are trying to join is the same as the session we are in
			if( alreadyInThisSquad )
			{
				//CWarningsManager *pWarnings = pGame->GetWarnings();
				if(g_pGame->GetPlayerProfileManager()->GetExclusiveControllerDeviceIndex() == inviteData->m_user)
				{
					CryLog("[invite] we tried to accept an invite to a session we are already in");

					// the user is already in the session, tell them
					acceptInvite = false;
					//pWarnings->AddWarning("InviteFailedAlreadyInSession", pGame->GetFlashMenu());
				}
				else if(pGame->m_pSquadManager->InCharge())
				{
					CryLog("[invite] session is hosted on this system, yet someone on this system has tried to join it via invite");

					// someone else is trying to accept the session on the same system
					// as the host. only a problem on 360 which can have multiple users
					// signed in at a time
					acceptInvite = false;
					//pWarnings->AddWarning("InviteFailedIsHost", pGame->GetFlashMenu());
				}
			}
		}
	}

	if(acceptInvite)
	{
	//	CryInviteID id = inviteData->m_id;

		pGame->SetInviteData(inviteData->m_service, inviteData->m_user, inviteData->m_id, inviteData->m_error);
		pGame->SetInviteAcceptedState(eIAS_Init);

		// 360 will set/reset m_bLoggedInFromInvite correctly because the system has to cope with
		// multiple profiles being logged in at the same time, the other platforms, not so much,
		// at least not yet...























	}
}


void CGame::SetInviteAcceptedState(EInviteAcceptedState state)
{
	CryLog("[Invite] SetInviteAcceptedState %d to %d", m_inviteAcceptedState, state);

	m_inviteAcceptedState = state; 
}

void CGame::SetInviteData(ECryLobbyService service, uint32 user, CryInviteID id, ECryLobbyError error)
{

	m_inviteAcceptedData.m_service = service;
	m_inviteAcceptedData.m_user = user;
	m_inviteAcceptedData.m_id = id;
	m_inviteAcceptedData.m_error = error;
	m_inviteAcceptedData.m_bannedFromSession = false;

	if(m_pSquadManager && error == eCLE_Success)
	{
		m_pSquadManager->SetInvitePending(true);
	}
}

void CGame::InvalidateInviteData()
{
	CryLog("[Invite] InvalidateInviteData");

	SetInviteData(eCLS_Online, 0, CryInvalidInvite, eCLE_Success);

	if(m_pSquadManager)
	{
		m_pSquadManager->SetInvitePending(false);
	}

	//if(m_pWarningsManager)
	//{
	//	m_pWarningsManager->RemoveWarning("InviteSelectController");
	//	m_pWarningsManager->RemoveWarning("ConfirmInvite");
	//}

	m_bLoggedInFromInvite = false;
}

void CGame::SetInviteUserFromPreviousControllerIndex()
{
	CryLog("[Invite] SetInviteUserFromPreviousControllerIndex %d", m_previousInputControllerDeviceIndex);

	m_inviteAcceptedData.m_user = m_previousInputControllerDeviceIndex;
}

//static---------------------------------------
void CGame::OnlineStateCallback(UCryLobbyEventData eventData, void *arg)
{
	CryLog("[Game] OnlineStateCallback");

	CGame *pGame = (CGame*)arg;
	CRY_ASSERT_MESSAGE(pGame, "No game!");

	SCryLobbyOnlineStateData *pOnlineStateData  = eventData.pOnlineStateData;
	if(pOnlineStateData)
	{
		if(pOnlineStateData->m_curState == eOS_SignedOut)
		{
			// ps3/pc do not have the concept of multiple signed-in users,
			// so isCurrent user is always true, for 360 test against the user id
			bool isCurrentUser = true;



			if((pGame->m_inviteAcceptedData.m_id != CryInvalidInvite) && (isCurrentUser))
			{
				if(pOnlineStateData->m_reason != eCLE_CyclingForInvite)
				{
					CryLog("[Game] User %d signed out and was accepting an invite, invalidating the invite", pOnlineStateData->m_user);

					pGame->InvalidateInviteData();
				}
			}
		}

	}
}

void CGame::UpdateInviteAcceptedState()
{
	if(GetInviteAcceptedState() != eIAS_None && m_inviteAcceptedData.m_error == eCLE_Success && m_inviteAcceptedData.m_id == CryInvalidInvite )
	{
		CryLog("[Invite] Join invite in progress, but session id is invalid, bailing...");

		SetInviteAcceptedState(eIAS_None);
		InvalidateInviteData();	// for safety
	}

	switch(GetInviteAcceptedState())
	{
	case eIAS_None:
		{
			break;
		}

	case eIAS_Init:
		{
			bool bContinue = true;

			if (m_inviteAcceptedData.m_id != CryInvalidInvite )
			{
				CSquadManager *pSquadManager = g_pGame->GetSquadManager();
				if (pSquadManager)
				{
					if (m_inviteAcceptedData.m_id->IsFromInvite() == false)
					{
						if (pSquadManager->AllowedToJoinSession(m_inviteAcceptedData.m_id) == false)
						{
							SetInviteAcceptedState(eIAS_Error);
							m_inviteAcceptedData.m_bannedFromSession = true;
							bContinue = false;
						}
					}
					else
					{
						pSquadManager->RemoveFromBannedList(m_inviteAcceptedData.m_id);
					}
				}
			}

			if (bContinue)
			{
				SetInviteAcceptedState(eIAS_StartAcceptInvite);
			}
			break;
		}

	case eIAS_ConfirmInvite:
		{
			// this isn't used anymore, but just in case it comes back, leaving here for now
			if(true/* // todo: ui !pFlashFrontEnd->IsLoading()*/)	// don't show dialog while in the middle of loading, wait until ingame
			{
				//m_pWarningsManager->RemoveWarning("ConfirmInvite");
				//m_pWarningsManager->AddWarning("ConfirmInvite", GetFlashMenu());
				SetInviteAcceptedState(eIAS_WaitForInviteConfirmation);
			}
			break;
		}

	case eIAS_WaitForInviteConfirmation:
		{
			// warning return will push this onto the next state
			break;
		}

	case eIAS_StartAcceptInvite:
		{



			const bool bChangeUser = false;


//			if(m_pWarningsManager)
//			{
//#if defined(PS3) || defined(XENON)
//				// invites are destructive acts that have to happen
//				// (according to TCRs), we need to clear any active
//				// warnings here as they are no longer relevant
//				m_pWarningsManager->ClearCurrentWarnings(true);
//#else
//				// on pc though we accept invites on the friends pop-up.
//				if (gEnv->bMultiplayer)
//				{
//					//we dont want to close the friends popup when it's joining a squad and you're already in MP.
//					const THUDWarningId  exceptions[] = { m_pWarningsManager->GetWarningId(CQuickSquadPopUp::GetQuickSquadPopUpName()) };
//					m_pWarningsManager->ClearCurrentWarningsWithExceptions(exceptions, ARRAY_COUNT(exceptions), true);
//				}
//				else
//				{
//					//for now SP closes all warnings, possible TODO would be to reopen the friends pop-up (if open) once in MP. We don't leave it open as the SP->MP stall doesn't look good when leaving it open.
//					m_pWarningsManager->ClearCurrentWarnings(true);
//				}
//#endif
//			}

			// If we're in singleplayer or we need to change user, do existing behaviour
			if ((gEnv->bMultiplayer == false) || bChangeUser)
			{
#ifdef USE_C2_FRONTEND
				if(pFlashFrontEnd->IsLoading())
#else
				if(false)
#endif // USE_C2_FRONTEND
				{
					CryLog("[Invite] Waiting for loading to finish");

					SetInviteAcceptedState(eIAS_WaitForLoadToFinish);
				}
				else if(GetIGameFramework()->StartedGameContext())
				{
					CryLog("[Invite] Accepting invite from in-game");

					SetInviteAcceptedState(eIAS_DisconnectGame);
				}
				else if (m_pGameLobbyManager->HaveActiveLobby())
				{
					CryLog("[Invite] Accepting invite from in-lobby");

					SetInviteAcceptedState(eIAS_DisconnectLobby);
				}
				else
				{
					CryLog("[Invite] Accepting invite from the menus");

					SetInviteAcceptedState(eIAS_InitSinglePlayer);
				}
			}
			else
			{
				if(m_inviteAcceptedData.m_error == eCLE_Success)
				{
					// theres a period during mp initialisation where
					// the squad manager is not enabled yet, we need to cope with that here
					SetInviteAcceptedState(eIAS_WaitForSquadManagerEnabled);
				}
				else
				{
					SetInviteAcceptedState(eIAS_Error);
				}
			}

			break;
		}

	case eIAS_WaitForLoadToFinish:
		{
			bool isLoadingFinished = false;

#if !defined(DEDICATED_SERVER)
			if(!gEnv->bMultiplayer)
			{
#ifdef USE_C2_FRONTEND
				if(g_pGameCVars->g_flashrenderingduringloading)
				{
					CFrontEndVideo *pVideo= pFlashFrontEnd->GetVideo();
					if(pVideo && pVideo->IsPlaying())
					{
						IFlashPlayer *pVideoFlashPlayer = pVideo->GetFlashPlayer();
						if(pVideoFlashPlayer)
						{
							SFlashVarValue hasLoadingFinished(true);
							pVideoFlashPlayer->GetVariable("_root.loadingHasFinished", hasLoadingFinished);
							isLoadingFinished = hasLoadingFinished.GetBool();

							if(isLoadingFinished || pFlashFrontEnd->IsFlashLoadingFinished())
							{
								pVideo->SkipVideo();	
							}
						}
						else
						{
							isLoadingFinished = true;
						}
					}
					else
					{
						isLoadingFinished = true;
					}
				}
				else
				{
					isLoadingFinished = !pFlashFrontEnd->IsLoading();
				}
#else
				isLoadingFinished = true;
#endif // USE_C2_FRONTEND
			}
			else
#endif
			{
#ifdef USE_C2_FRONTEND
				isLoadingFinished = !pFlashFrontEnd->IsLoading();
#endif // USE_C2_FRONTEND
			}

			if(isLoadingFinished)
			{
				SetInviteAcceptedState(eIAS_DisconnectGame);	// finished loading, kill the game off
			}
			break;
		}

	case eIAS_DisconnectGame:
		{
			if(m_inviteAcceptedData.m_error == eCLE_Success)
			{
#ifdef USE_C2_FRONTEND
				pFlashFrontEnd->OnDisconnect();			// Call flash disconnect (since this stops the loading screen)
#endif // USE_C2_FRONTEND
				SetInviteAcceptedState(eIAS_WaitForSessionDelete);
			}
			else
			{
				CryLog("[invite] trying to disconnect game for invite, but invite was retrieved with error %d", m_inviteAcceptedData.m_error);

				// single player doesn't want disconnecting if the invite was retreived with error, if mp
				// and signed out, then should of already been returned to sp main menu anyways
				SetInviteAcceptedState(eIAS_Error); 
			}

			break;
		}

	case eIAS_DisconnectLobby:
		{
			m_pGameLobbyManager->LeaveGameSession(CGameLobbyManager::eLSR_AcceptingInvite);
			SetInviteAcceptedState(eIAS_WaitForSessionDelete);
			break;
		}

	case eIAS_WaitForSessionDelete:
		{
			if (!m_pGameLobbyManager->HaveActiveLobby())
			{
#ifdef USE_C2_FRONTEND
				if(pFlashFrontEnd->IsMenuActive(CFlashFrontEnd::eFlM_Menu))
#endif // USE_C2_FRONTEND
				{
					SetInviteAcceptedState(eIAS_InitSinglePlayer);
				}
			}
			break;
		}

	case eIAS_InitSinglePlayer:
		{
			if(m_inviteAcceptedData.m_error == eCLE_Success)
			{
				EInviteAcceptedState nextState = eIAS_WaitForInitSinglePlayer;

				if(gEnv->bMultiplayer)
				{
					if(HasExclusiveControllerIndex())	// demo will likely need this guard :(
					{
						CryLog("[invite] initialise single player as we are accepting an invite as a different user");

						// if we get here and we are in multiplayer, then we must be changing
						// users, so need to init back single player to sort the user profile out
#ifdef USE_C2_FRONTEND
						pFlashFrontEnd->Execute(CFlashFrontEnd::eFECMD_switch_game, !g_pGameCVars->g_multiplayerModeOnly ? "singleplayer" : "multiplayer");
#endif // USE_C2_FRONTEND
					}
				}
#if !defined(DEDICATED_SERVER)
				else if(HasExclusiveControllerIndex())
				{
					if(GetIGameFramework()->GetIPlayerProfileManager()->GetExclusiveControllerDeviceIndex() == m_inviteAcceptedData.m_user)
					{
						CryLog("[invite] accepting an invite with the current user in sp, switching to mp");

						nextState = eIAS_InitMultiplayer;
					}
					else
					{
						CryLog("[invite] in single player with a different user, heading back to splashscreen");

						// without an exclusive controler index, we should already be on the saveicon
						// or splashscreen pages, so no need to go to them
#ifdef USE_C2_FRONTEND
						pFlashFrontEnd->Execute(CFlashFrontEnd::eFECMD_gotoPageClearStack, "splashscreen");
#endif // USE_C2_FRONTEND
					}						
				}











#endif

				SetInviteAcceptedState(nextState);
			}
			else
			{
				CryLog("[invite] trying to init singleplayer from invite, but invite was retrieved with error %d", m_inviteAcceptedData.m_error);
				SetInviteAcceptedState(eIAS_Error);
			}
			break;
		}

	case eIAS_WaitForInitSinglePlayer:
		{
#ifdef USE_C2_FRONTEND
			if(m_postLocalisationBootChecksDone) // it does not end well on PS3  if we do not wait for this
			{
				if(pFlashFrontEnd->IsMenuActive(CFlashFrontEnd::eFlM_Menu))
				{
					// the video check is in place so the legal videos can play
					CFrontEndVideo *pVideo= pFlashFrontEnd->GetVideo();
					if(!pVideo || !pVideo->IsPlaying() || pVideo->IsBackground())
					{
						EFlashFrontEndScreens currentScreen = pFlashFrontEnd->GetCurrentMenuScreen();
						if((currentScreen == eFFES_tosscreen && pFlashFrontEnd->CanSkipTOS()) || currentScreen == eFFES_saveicon)
						{
							pFlashFrontEnd->Execute(CFlashFrontEnd::eFECMD_goto, "splashscreen");
							SetInviteAcceptedState(eIAS_WaitForSplashScreen);
						}
						else if(currentScreen == eFFES_splashscreen)
						{
							SetInviteAcceptedState(eIAS_WaitForSplashScreen);
						}
					}
				}
			}
#endif // USE_C2_FRONTEND
				SetInviteAcceptedState(eIAS_WaitForSplashScreen);
			
			break;
		}

	case eIAS_WaitForSplashScreen:
		{
			if(true/*gamedatainstalled*/)
			{
				if(true/*pFlashFrontEnd->IsMenuActive(CFlashFrontEnd::eFlM_Menu) && pFlashFrontEnd->GetCurrentMenuScreen() == eFFES_splashscreen*/)
				{
					if(m_inviteAcceptedData.m_error == eCLE_Success)
					{










						SetInviteAcceptedState(eIAS_WaitForValidUser);
					}
					else
					{
						CryLog("[invite] trying to init singleplayer from invit, but invite was retrieved with error %d", m_inviteAcceptedData.m_error);

						SetInviteAcceptedState(eIAS_Error);
					}
				}
			}
			break;
		}

	case eIAS_WaitForValidUser:
		{
			if(m_inviteAcceptedData.m_user != INVALID_CONTROLLER_INDEX)
			{
				SetPreviousExclusiveControllerDeviceIndex(m_inviteAcceptedData.m_user);	// set the controller
				//pFlashFrontEnd->Execute(CFlashFrontEnd::eFECMD_goto, "profile_login"); // login profile, which will also set the exclusive controller for us
				SetInviteAcceptedState(eIAS_WaitForInitProfile);	// wait
			}
			break;
		}

	case eIAS_WaitForInitProfile:
		{

			if(m_bLoggedInFromInvite)	// not convinced this is needed anymore, we now just wait for main and don't progress until file writing is done
			{
				// wait until we reach the single player main menu
#ifdef USE_C2_FRONTEND
				if(pFlashFrontEnd->IsMenuActive(CFlashFrontEnd::eFlM_Menu) && pFlashFrontEnd->GetCurrentMenuScreen() == eFFES_main)
#endif // USE_C2_FRONTEND
				{
					SetInviteAcceptedState(eIAS_InitMultiplayer);
				}
			}
			break;
		}

	case eIAS_InitMultiplayer:
		{
			SetInviteAcceptedState(eIAS_WaitForInitMultiplayer);
			CGameLobby::SetLobbyService(eCLS_Online);
			break;
		}

	case eIAS_WaitForInitMultiplayer:
		{
#ifdef USE_C2_FRONTEND
			// need to wait for the multiplayer menu screen to actually load
			if(pFlashFrontEnd->IsMenuActive(CFlashFrontEnd::eFlM_Menu) && pFlashFrontEnd->GetCurrentMenuScreen() == eFFES_main)
#endif // USE_C2_FRONTEND
			{
				if(gEnv->bMultiplayer) // :(
				{
					SetInviteAcceptedState(eIAS_InitOnline);
					m_pSquadManager->SetInvitePending(true);
				}
			}
			break;
		}

	case eIAS_InitOnline:
		{
			SetInviteAcceptedState(eIAS_WaitForInitOnline);
			break;
		}

	case eIAS_WaitForInitOnline:
		{
			// MP Loader sets accept invite now	
			break;
		}

	case eIAS_WaitForSquadManagerEnabled:
		{
			CRY_ASSERT(gEnv->bMultiplayer);

			if(m_pSquadManager->IsEnabled())
			{
				// if we're loading, then need to wait, it does not end well otherwise
#ifdef USE_C2_FRONTEND
				if(!pFlashFrontEnd->IsFlashRenderingDuringLoad())
#endif // USE_C2_FRONTEND
				{	
#ifdef USE_C2_FRONTEND
					// there are certain menu screens where accepting an invite is bad, mploader and console mycrysis screens need to wait before they can go forwards
					bool isValidMPScreen = pFlashFrontEnd->GetCurrentMenuScreen() != eFFES_main && pFlashFrontEnd->GetCurrentMenuScreen() != eFFES_mycrysis_signup && pFlashFrontEnd->GetCurrentMenuScreen() != eFFES_mycrysis_tosscreen_console;

					if(!pFlashFrontEnd->IsMenuActive(CFlashFrontEnd::eFlM_Menu) || isValidMPScreen)
#endif // USE_C2_FRONTEND
					{
						SetInviteAcceptedState(eIAS_Accept);
					}
				}
			}
			break;
		}

	case eIAS_Accept:
		{
			m_pSquadManager->InviteAccepted(m_inviteAcceptedData.m_service, m_inviteAcceptedData.m_user, m_inviteAcceptedData.m_id);
			SetInviteAcceptedState(eIAS_None);
			InvalidateInviteData();
			break;
		}

	case eIAS_Error:
		{
			//CFrontEndVideo *pVideo= pFlashFrontEnd->GetVideo();
			//if((!pVideo || !pVideo->IsPlaying() || pVideo->IsBackground()) && (!pFlashFrontEnd->IsLoading()))
			//{
			//	m_pWarningsManager->RemoveWarning("InviteNotSignedIn");
			//	m_pWarningsManager->RemoveWarning("InviteInvalidRequest");

			//	if (m_inviteAcceptedData.m_bannedFromSession)
			//	{
			//		m_pWarningsManager->AddWarning("BannedFromSquad", GetFlashMenu());
			//	}
			//	else
			//	{
			//		switch(m_inviteAcceptedData.m_error)
			//		{
			//		case eCLE_UserNotSignedIn:	// user accepted the invite while in a signed out state
			//			m_pWarningsManager->AddWarning("InviteNotSignedIn", GetFlashMenu());
			//			break;

			//		case eCLE_InvalidInviteFriendData:
			//		case eCLE_InvalidJoinFriendData:
			//			m_pWarningsManager->AddWarning("InviteInvalidRequest", GetFlashMenu());
			//			break;

			//		default:
			//			CryLog("[invite] unexpected error %d passed into invite data", m_inviteAcceptedData.m_error);
			//			break;
			//		}
			//	}

				InvalidateInviteData();
			//}
			break;
		}

	default:
		{
			CryLog("[Invite] unknown invite accepted state");
			break;
		}
	}
}

#include UNIQUE_VIRTUAL_WRAPPER(IGame)

