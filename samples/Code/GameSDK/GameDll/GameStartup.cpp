/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2004.
-------------------------------------------------------------------------
$Id$
$DateTime$

-------------------------------------------------------------------------
History:
- 2:8:2004   15:20 : Created by Márcio Martins

*************************************************************************/

#include "StdAfx.h"
#include "GameStartup.h"
#include "Game.h"
#include "GameCVars.h"

#include <StringUtils.h>
#include <CryFixedString.h>
#include <CryLibrary.h>
#include <platform_impl.h>
#include <INetworkService.h>

#include <LoadSeq.h>

#include "IHardwareMouse.h"
#include "ICryPak.h"
#include <ILocalizationManager.h>
#include "IBasicEventListener.h"
#include "Editor/GameRealtimeRemoteUpdate.h"
#include "Utility/StringUtils.h"

#include "Testing/AutoTester.h"
#include "ModInfoManager.h"
#include <IJobManager.h>
#include <CryProfileMarker.h>

#include <IPluginManager_impl.h>

#if ENABLE_AUTO_TESTER 
static CAutoTester s_autoTesterSingleton;
#endif
 
#if defined(ENABLE_STATS_AGENT)
#include "StatsAgent.h"
#endif

#ifdef __LINK_GCOV__
extern "C" void __gcov_flush(void);
#define GCOV_FLUSH __gcov_flush()
namespace
{
	static void gcovFlushUpdate()
	{
		static unsigned sCounter = 0;
		static const sInterval = 1000;

		if (++sCounter == sInterval)
		{
			__gcov_flush();
			sCounter = 0;
		}
	}
}
#define GCOV_FLUSH_UPDATE gcovFlushUpdate()
#else
#define GCOV_FLUSH ((void)0)
#define GCOV_FLUSH_UPDATE ((void)0)
#endif

#if defined(PS3) && !defined(_LIB) 
#if defined PS3_PRX_CryAction
extern "C" IGameFramework *CreateGameFramework();
#endif
namespace
{
	static IGameFramework *PS3CreateGameFramework()
	{
#if defined PS3_PRX_CryAction
		return CreateGameFramework();
#else
		return gPS3Env->pInitFnTable->pInitAction();
#endif
	}
}
#define CreateGameFramework PS3CreateGameFramework
#elif defined(_LIB) || defined(LINUX) || defined(PS3)
extern "C" IGameFramework *CreateGameFramework();
#endif

#ifndef XENON
#define DLL_INITFUNC_CREATEGAME "CreateGameFramework"
#else
#define DLL_INITFUNC_CREATEGAME (LPCSTR)1
#endif

#ifdef WIN32
bool g_StickyKeysStatusSaved = false;
STICKYKEYS g_StartupStickyKeys = {sizeof(STICKYKEYS), 0};
TOGGLEKEYS g_StartupToggleKeys = {sizeof(TOGGLEKEYS), 0};
FILTERKEYS g_StartupFilterKeys = {sizeof(FILTERKEYS), 0};

#endif

#if defined(CVARS_WHITELIST)
CCVarsWhiteList g_CVarsWhiteList;

bool IsCommandLiteral(const char* pLiteral, const char* pCommand)
{ 
	// Compare the command with the literal, for the length of the literal
	while ((*pLiteral) && (tolower(*pLiteral++) == tolower(*pCommand++)));

	// If they're the same, ensure the whole command was tested
	return (!*(pLiteral) && (!(*pCommand) || isspace(*pCommand)));
}

#define WHITELIST(_stringliteral) if (IsCommandLiteral(_stringliteral, pCommandMod)) { return true; }
bool CCVarsWhiteList::IsWhiteListed(const string& command, bool silent)
{
	const char * pCommandMod = command.c_str();
	if(pCommandMod[0] == '+')
	{
		pCommandMod++;
	}
	
	WHITELIST("sys_game_folder");
	
	WHITELIST("map");
	WHITELIST("i_mouse_smooth");
	WHITELIST("i_mouse_accel");
	WHITELIST("i_mouse_accel_max");
	WHITELIST("cl_sensitivity");
	WHITELIST("pl_movement.power_sprint_targetFov");
	WHITELIST("cl_fov");
	WHITELIST("hud_canvas_width_adjustment");
	WHITELIST("r_DrawNearFoV");
	WHITELIST("g_skipIntro");
	WHITELIST("hud_psychoPsycho");
	WHITELIST("hud_hide");
	WHITELIST("disconnect");

	WHITELIST("hud_bobHud");
  WHITELIST("e_CoverageBufferReproj");
  WHITELIST("e_LodRatio");
  WHITELIST("e_ViewDistRatio");
  WHITELIST("e_ViewDistRatioVegetation");
  WHITELIST("e_ViewDistRatioDetail");
  WHITELIST("e_MergedMeshesInstanceDist");
  WHITELIST("e_MergedMeshesViewDistRatio");
  WHITELIST("e_ParticlesObjectCollisions");
  WHITELIST("e_ParticlesMotionBlur");
  WHITELIST("e_ParticlesForceSoftParticles");
  WHITELIST("e_Tessellation");
  WHITELIST("e_TessellationMaxDistance");
  WHITELIST("r_TessellationTriangleSize");
  WHITELIST("r_SilhouettePOM");
	WHITELIST("e_GI");
	WHITELIST("e_GICache");
  WHITELIST("e_GIIterations");
	WHITELIST("e_ShadowsPoolSize");
	WHITELIST("e_ShadowsMaxTexRes");
	WHITELIST("r_FogShadows");
	WHITELIST("r_FogShadowsWater");
	WHITELIST("e_ParticlesShadows");
	WHITELIST("e_ShadowsTessellateCascades");
  WHITELIST("e_ShadowsResScale");
  WHITELIST("e_GsmCache");
  WHITELIST("r_WaterTessellationHW");
  WHITELIST("r_DepthOfField");
	WHITELIST("r_MotionBlur");
	WHITELIST("r_MotionBlurShutterSpeed");
	WHITELIST("g_radialBlur");
	WHITELIST("cl_zoomToggle");
	WHITELIST("r_TexMinAnisotropy");
	WHITELIST("r_TexMaxAnisotropy");
  WHITELIST("r_TexturesStreamPoolSize");
	WHITELIST("cl_crouchToggle");
	WHITELIST("r_ColorGrading");
	WHITELIST("r_SSAO");
	WHITELIST("r_SSDO");
	WHITELIST("r_SSReflections");
	WHITELIST("r_VSync");
	WHITELIST("r_DisplayInfo");
  WHITELIST("r_ChromaticAberration");
	WHITELIST("r_HDRChromaShift");
	WHITELIST("r_HDRGrainAmount");
	WHITELIST("r_HDRBloomRatio");
	WHITELIST("r_HDRBrightLevel");
  WHITELIST("r_Sharpening");
	WHITELIST("s_MusicVolume");
	WHITELIST("s_SFXVolume");
	WHITELIST("s_DialogVolume");
	WHITELIST("r_Gamma");
	WHITELIST("r_GetScreenShot");
	WHITELIST("r_FullscreenWindow");
	WHITELIST("r_MultiGPU");
	WHITELIST("r_overrideDXGIOutput");
	WHITELIST("r_overrideDXGIAdapter");
	WHITELIST("r_FullscreenPreemption");
  WHITELIST("r_buffer_sli_workaround");
	WHITELIST("r_DeferredShadingAmbientSClear");
	WHITELIST("g_useHitSoundFeedback");
	WHITELIST("sys_MaxFps");
	WHITELIST("sys_languages");
	WHITELIST("g_language");

	WHITELIST("sys_spec_ObjectDetail");
	WHITELIST("sys_spec_Shading");
	WHITELIST("sys_spec_VolumetricEffects");
	WHITELIST("sys_spec_Shadows");
	WHITELIST("sys_spec_Texture");
	WHITELIST("sys_spec_Physics");
	WHITELIST("sys_spec_PostProcessing");
	WHITELIST("sys_spec_Particles");
	WHITELIST("sys_spec_Sound");
	WHITELIST("sys_spec_Water");
	WHITELIST("sys_spec_GameEffects");
	WHITELIST("sys_spec_Light");

	WHITELIST("g_dedi_email");
	WHITELIST("g_dedi_password");

	WHITELIST("root");
	WHITELIST("logfile");
	WHITELIST("ResetProfile");
	WHITELIST("nodlc");

	WHITELIST("rcon_connect");
	WHITELIST("rcon_disconnect");
	WHITELIST("rcon_command");

	WHITELIST("quit");
	WHITELIST("votekick");
	WHITELIST("vote");

	WHITELIST("net_blaze_voip_enable");

#if defined(DEDICATED_SERVER)
	WHITELIST("ban");
	WHITELIST("ban_remove");
	WHITELIST("ban_status");
	WHITELIST("ban_timeout");
	WHITELIST("kick"); 
	WHITELIST("startPlaylist");
	WHITELIST("status");
	WHITELIST("gl_map");
	WHITELIST("gl_gamerules");
	WHITELIST("sv_gamerules");
	WHITELIST("sv_password");
	WHITELIST("maxplayers");
	WHITELIST("sv_servername");

	WHITELIST("sv_bind");
	WHITELIST("g_scoreLimit");
	WHITELIST("g_timelimit");
	WHITELIST("g_minplayerlimit");
	WHITELIST("g_autoReviveTime");
	WHITELIST("g_numLives");
	WHITELIST("g_maxHealthMultiplier");
	WHITELIST("g_mpRegenerationRate");
	WHITELIST("g_friendlyfireratio");
	WHITELIST("g_mpHeadshotsOnly");
	WHITELIST("g_mpNoVTOL");
	WHITELIST("g_mpNoEnvironmentalWeapons");
	WHITELIST("g_allowCustomLoadouts");
	WHITELIST("g_allowFatalityBonus");
	WHITELIST("g_modevarivar_proHud");
	WHITELIST("g_modevarivar_disableKillCam");
	WHITELIST("g_modevarivar_disableSpectatorCam");
	WHITELIST("g_multiplayerDefault");
	WHITELIST("g_allowExplosives");
	WHITELIST("g_forceWeapon");
	WHITELIST("g_allowWeaponCustomisation");
	WHITELIST("g_infiniteCloak");
	WHITELIST("g_infiniteAmmo");
	WHITELIST("g_forceHeavyWeapon");
	WHITELIST("g_forceLoadoutPackage");


	WHITELIST("g_autoAssignTeams");
	WHITELIST("gl_initialTime");
	WHITELIST("gl_time");
	WHITELIST("g_gameRules_startTimerLength");
	WHITELIST("sv_maxPlayers");
	WHITELIST("g_switchTeamAllowed");
	WHITELIST("g_switchTeamRequiredPlayerDifference");
	WHITELIST("g_switchTeamUnbalancedWarningDifference");
	WHITELIST("g_switchTeamUnbalancedWarningTimer");

	WHITELIST("http_startserver");
	WHITELIST("http_stopserver");
	WHITELIST("http_password");

	WHITELIST("rcon_startserver");
	WHITELIST("rcon_stopserver");
	WHITELIST("rcon_password");

	WHITELIST("gl_StartGame");
	WHITELIST("g_messageOfTheDay");
	WHITELIST("g_serverImageUrl");

	WHITELIST("log_Verbosity");
	WHITELIST("log_WriteToFile");
	WHITELIST("log_WriteToFileVerbosity");
	WHITELIST("log_IncludeTime");
	WHITELIST("log_tick");
	WHITELIST("net_log");

	WHITELIST("g_pinglimit");
	WHITELIST("g_pingLimitTimer");

	WHITELIST("g_tk_punish");
	WHITELIST("g_tk_punish_limit");
	WHITELIST("g_idleKickTime");

  WHITELIST("net_reserved_slot_system");
	WHITELIST("net_add_reserved_slot");
	WHITELIST("net_remove_reserved_slot");
	WHITELIST("net_list_reserved_slot");
	
	WHITELIST("sv_votingCooldown");
	WHITELIST("sv_votingRatio");
	WHITELIST("sv_votingTimeout");
	WHITELIST("sv_votingEnable");
	WHITELIST("sv_votingBanTime");

	WHITELIST("g_dataRefreshFrequency");
	WHITELIST("g_quitOnNewDataFound");
	WHITELIST("g_quitNumRoundsWarning");
	WHITELIST("g_allowedDataPatchFailCount");
	WHITELIST("g_shutdownMessageRepeatTime");
	WHITELIST("g_shutdownMessage");
	WHITELIST("g_patchPakDediServerMustPatch");

	WHITELIST("g_server_region");

	WHITELIST("net_log_dirtysock");
#endif

	WHITELIST("sys_user_folder");
	WHITELIST("sys_screensaver_allowed");
	WHITELIST("sys_UncachedStreamReads");

	if (!silent)
	{
		string temp = command.Left(command.find(' '));
		if (temp.empty())
		{
			temp = command;
		}

#if defined(DEDICATED_SERVER)
		CryLogAlways("[Warning] Unknown command: %s", temp.c_str());
#else
		CryLog("[Warning] Unknown command: %s", temp.c_str());
#endif
	}

	return false;
}
#endif // defined(CVARS_WHITELIST)

static void RestoreStickyKeys();

static void AllowAccessibilityShortcutKeys(bool bAllowKeys)
{
// disabling sticky keys for now since they are not being restored, atexit is not being called because the game process is killed by the system
//#if defined(WIN32)



































}
static void RestoreStickyKeys()
{
	AllowAccessibilityShortcutKeys(true);
}

#define EYEADAPTIONBASEDEFAULT		0.25f					// only needed for Crysis

#if defined(WIN32) || defined (WIN64)
void debugLogCallStack()
{
	// Print call stack for each find.
	const char *funcs[32];
	int nCount = 32;

	CryLogAlways( "    ----- CallStack () -----");
	gEnv->pSystem->debug_GetCallStack( funcs, nCount);
	for (int i = 1; i < nCount; i++) // start from 1 to skip this function.
	{
		CryLogAlways( "    %02d) %s",i,funcs[i] );
	}
}
#endif

void GameStartupErrorObserver::OnAssert(const char* condition, const char* message, const char* fileName, unsigned int fileLineNumber)
{
	if(!g_pGameCVars)
		return;

	if (g_pGameCVars->cl_logAsserts != 0)
		CryLogAlways("---ASSERT: condition:%s; message:%s; fileName:%s; line %d", condition, message, fileName, fileLineNumber);

#if defined(WIN32) || defined (WIN64)
	if (g_pGameCVars->cl_logAsserts > 1)
	{
		debugLogCallStack();
		CryLogAlways("----------------------------------------");
	}
#endif
}

void GameStartupErrorObserver::OnFatalError(const char* message)
{
	CryLogAlways("---FATAL ERROR: message:%s", message);

#if defined(WIN32) || defined (WIN64)
	gEnv->pSystem->debug_LogCallStack();
	CryLogAlways("----------------------------------------");
#endif
}

//////////////////////////////////////////////////////////////////////////

CGameStartup* CGameStartup::Create(IBasicEventListener& basicEventListener)
{
	static char buff[sizeof(CGameStartup)];
	return new (buff) CGameStartup(basicEventListener);
}

CGameStartup::CGameStartup(IBasicEventListener& basicEventListener)
	:	m_basicEventListener(basicEventListener), 
		m_pMod(NULL),
		m_modRef(&m_pMod),
		m_initWindow(false), 
		m_quit(false),
		m_reqModUnload(false),
		m_modDll(0), 
		m_frameworkDll(NULL),
		m_pFramework(NULL),
		m_fullScreenCVarSetup(false),
		m_nVOIPWasActive(-1)
{
	CGameStartupStatic::g_pGameStartup = this;
}

CGameStartup::~CGameStartup()
{
	if (m_pMod)
	{
		m_pMod->Shutdown();
		m_pMod = 0;
	}

	if (m_modDll)
	{
		CryFreeLibrary(m_modDll);
		m_modDll = 0;
	}

	CGameStartupStatic::g_pGameStartup = NULL;

	ShutdownFramework();
}

#if defined(_RELEASE) && defined(XENON) && defined(_LIB)
extern void EngineStartProfiler(const char * name);
#else
#define EngineStartProfiler(x)
#endif

#ifdef PS3
extern void InitTerminationCheck(const char * sDescription);
#else
#define InitTerminationCheck(x)
#endif

static inline void InlineInitializationProcessing(const char *sDescription)
{
	EngineStartProfiler( sDescription );
	InitTerminationCheck( sDescription );
}

IGameRef CGameStartup::Init(SSystemInitParams &startupParams)
{
	MEMSTAT_CONTEXT(EMemStatContextTypes::MSC_Other, 0, "Game startup initialisation");

	LOADING("game_startup");

#if defined(CVARS_WHITELIST)
	startupParams.pCVarsWhitelist = &g_CVarsWhiteList;
#endif // defined(CVARS_WHITELIST)
	startupParams.pGameStartup = this;

	if (!InitFramework(startupParams))
	{
		return 0;
	}

	InlineInitializationProcessing("CGameStartup::Init");

  LOADING_TIME_PROFILE_SECTION(m_pFramework->GetISystem());

	ISystem* pSystem = m_pFramework->GetISystem();
	startupParams.pSystem = pSystem;

	const ICmdLineArg* pSvBind = gEnv->pSystem->GetICmdLine()->FindArg(eCLAT_Pre, "sv_bind"); 
	IConsole* pConsole = pSystem->GetIConsole();
	if ((pSvBind != NULL) && (pConsole != NULL))
	{
		string command = pSvBind->GetName() + string(" ") + pSvBind->GetValue();
		pConsole->ExecuteString(command.c_str(), true, false);
	}

#if defined(ENABLE_STATS_AGENT)
	const ICmdLineArg *pPipeArg = pSystem->GetICmdLine()->FindArg(eCLAT_Pre,"lt_pipename");
	CStatsAgent::CreatePipe( pPipeArg );
#endif

	PluginManager::InitPluginManager(startupParams);
	PluginManager::InitPluginsBeforeFramework();

	REGISTER_COMMAND("g_loadMod", CGameStartupStatic::RequestLoadMod,VF_NULL,"");
	REGISTER_COMMAND("g_unloadMod", CGameStartupStatic::RequestUnloadMod, VF_NULL, "");

	// load the appropriate game/mod
#if !defined(_RELEASE)
	const ICmdLineArg *pModArg = pSystem->GetICmdLine()->FindArg(eCLAT_Pre,"MOD");
#else
	const ICmdLineArg *pModArg = NULL;
#endif // !defined(_RELEASE)

	InlineInitializationProcessing("CGameStartup::Init LoadLocalizationData");

	IGameRef pOut;
	if (pModArg && (*pModArg->GetValue() != 0) && (pSystem->IsMODValid(pModArg->GetValue())))
	{
		const char* pModName = pModArg->GetValue();
		assert(pModName);

		pOut = Reset(pModName);
	}
	else
	{
		pOut = Reset(GAME_NAME);
	}

	if (!m_pFramework->CompleteInit())
	{
		pOut->Shutdown();
		return 0;
	}

	InlineInitializationProcessing("CGameStartup::Init FrameworkCompleteInit");

	LOADING_DONE;

	// should be after init game (should be executed even if there is no game)
	if(startupParams.bExecuteCommandLine)
		pSystem->ExecuteCommandLine();

	pSystem->GetISystemEventDispatcher()->RegisterListener(this);

	// Creates and starts the realtime update system listener.
	if (pSystem->IsDevMode())
	{
		CGameRealtimeRemoteUpdateListener::GetGameRealtimeRemoteUpdateListener().Enable(true);
	}


	GCOV_FLUSH;

	if (gEnv && GetISystem())
	{
		GetISystem()->RegisterErrorObserver(&m_errorObsever);
	}
	else
	{
		CryLogAlways("failed to find ISystem to register error observer");
		assert(0);
	}

	
	InlineInitializationProcessing("CGameStartup::Init End");

#if defined(CRY_UNIT_TESTING)
	// Register All unit tests of this module.
#if defined(_LIB)
	if(gEnv->pSystem)
	{
		CryUnitTest::Test *pTest = CryUnitTest::Test::m_pFirst; 
		for (; pTest != 0; pTest = pTest->m_pNext)
		{
			CryUnitTest::IUnitTestManager *pTestManager = gEnv->pSystem->GetITestSystem()->GetIUnitTestManager();
			if (pTestManager)
			{
				pTest->m_unitTestInfo.module = "StaticBinary";
				pTestManager->CreateTest( pTest->m_unitTestInfo );
			}
		}
	}
#endif

	// run unit tests
	CryUnitTest::IUnitTestManager *pTestManager = gEnv->pSystem->GetITestSystem()->GetIUnitTestManager();
	if (pTestManager)
	{
		const ICmdLineArg* pSkipUnitTest = gEnv->pSystem->GetICmdLine()->FindArg(eCLAT_Pre, "skip_unit_tests"); 
		if(pSkipUnitTest == NULL)
		{
			const ICmdLineArg* pUseUnitTestExcelReporter = gEnv->pSystem->GetICmdLine()->FindArg(eCLAT_Pre, "use_unit_test_excel_reporter"); 
			if(pUseUnitTestExcelReporter)
			{
				gEnv->pSystem->GetITestSystem()->GetIUnitTestManager()->RunAllTests(CryUnitTest::ExcelReporter);
			}
			else // default is the minimal reporter
			{
				gEnv->pSystem->GetITestSystem()->GetIUnitTestManager()->RunAllTests(CryUnitTest::MinimalReporter);
			}
		}
	}
#endif // CRY_UNIT_TESTING

	PluginManager::InitPluginsLast();

	GetISystem()->GetISystemEventDispatcher()->OnSystemEvent(ESYSTEM_EVENT_RANDOM_SEED, (UINT_PTR)gEnv->pTimer->GetAsyncTime().GetMicroSecondsAsInt64(), 0);
	return pOut;
}

IGameRef CGameStartup::Reset(const char *pModName)
{
	if (m_pMod)
	{
		m_pMod->Shutdown();

		if (m_modDll)
		{
			CryFreeLibrary(m_modDll);
			m_modDll = 0;
		}
	}

	m_modDll = 0;
	string modPath;
	
	if (stricmp(pModName, GAME_NAME) != 0)
	{
		modPath.append("Mods\\");
		modPath.append(pModName);
		modPath.append("\\");

		string filename;
		filename.append("..\\");
		filename.append(modPath);
		
#ifdef WIN64
		filename.append("Bin64\\");
#else
		filename.append("Bin32\\");
#endif
		
		filename.append(pModName);
		filename.append(".dll");

#if !defined(_LIB) && !defined(LINUX) && !defined(PS3)
		m_modDll = CryLoadLibrary(filename.c_str());
#endif
	}

	if (!m_modDll)
	{
		ModuleInitISystem(m_pFramework->GetISystem(),"CryGame");
		static char pGameBuffer[sizeof(CGame)];
		m_pMod = new ((void*)pGameBuffer) CGame();
	}
	else
	{
		IGame::TEntryFunction CreateGame = (IGame::TEntryFunction)CryGetProcAddress(m_modDll, "CreateGame");
		if (!CreateGame)
			return 0;

		m_pMod = CreateGame(m_pFramework);
	}

	if (m_pMod && m_pMod->Init(m_pFramework))
	{
		return m_modRef;
	}

	return 0;
}

void CGameStartup::Shutdown()
{
#ifdef WIN32
	AllowAccessibilityShortcutKeys(true);
#endif

#if defined(ENABLE_STATS_AGENT)
	CStatsAgent::ClosePipe();
#endif

	/*delete this;*/
	this->~CGameStartup();
}

int CGameStartup::Update(bool haveFocus, unsigned int updateFlags)
{
	CRYPROFILE_SCOPE_PROFILE_MARKER("MT: MainLoop");

#if defined(JOBMANAGER_SUPPORT_PROFILING)
	gEnv->GetJobManager()->SetFrameStartTime(gEnv->pTimer->GetAsyncTime());
#endif
	gEnv->pSystem->ResetWatchdogTimer();
	int returnCode = 0;

	if (gEnv && gEnv->pSystem && gEnv->pConsole)
	{
#ifdef WIN32
		if(gEnv && gEnv->pRenderer && gEnv->pRenderer->GetHWND())
		{
			bool focus = (::GetFocus() == gEnv->pRenderer->GetHWND());
			static bool focused = focus;
			if (focus != focused)
			{
				if(gEnv->pSystem->GetISystemEventDispatcher())
				{
					gEnv->pSystem->GetISystemEventDispatcher()->OnSystemEvent(ESYSTEM_EVENT_CHANGE_FOCUS, focus, 0);
				}
				focused = focus;
			}
		}
#endif
	}

	// update the game
	if (m_pMod)
	{
		returnCode = m_pMod->Update(haveFocus, updateFlags);
	}

#if defined(ENABLE_STATS_AGENT)
	CStatsAgent::Update();
#endif

#if defined(JOBMANAGER_SUPPORT_FRAMEPROFILER)

	// Update Backend profilers
	uint32 timeSample = JobManager::IWorkerBackEndProfiler::GetTimeSample();

	const JobManager::IBackend * const __restrict pBackends[] = 
	{
		gEnv->GetJobManager()->GetBackEnd(JobManager::eBET_Thread),
		gEnv->GetJobManager()->GetBackEnd(JobManager::eBET_Blocking),
#if defined(PS3)
		gEnv->GetJobManager()->GetBackEnd(JobManager::eBET_SPU),
#endif
	};

	for(int i=0; i<sizeof(pBackends)/sizeof(pBackends[0]); ++i)
	{
		if(pBackends[i])
		{
			JobManager::IWorkerBackEndProfiler* pWorkerProfiler = pBackends[i]->GetBackEndWorkerProfiler();
			pWorkerProfiler->Update(timeSample);
		}
	}
#endif

	// ghetto fullscreen detection, because renderer does not provide any kind of listener
	if (!m_fullScreenCVarSetup && gEnv && gEnv->pSystem && gEnv->pConsole)
	{
		ICVar *pVar = gEnv->pConsole->GetCVar("r_Fullscreen");
		if (pVar)
		{
			pVar->SetOnChangeCallback(FullScreenCVarChanged);
			m_fullScreenCVarSetup = true;
		}
	}
#if ENABLE_AUTO_TESTER 
	s_autoTesterSingleton.Update();
#endif
	GCOV_FLUSH_UPDATE;

	// do an additional IsQuittingCheck, for the Case CSystem::Update was not called
#if defined(PS3) 
	if(gEnv->pSystem->IsQuitting())
		gEnv->pSystem->Quit();
#endif

	return returnCode;
}

void CGameStartup::FullScreenCVarChanged( ICVar *pVar )
{
	if(gEnv->pSystem->GetISystemEventDispatcher())
	{
		gEnv->pSystem->GetISystemEventDispatcher()->OnSystemEvent(ESYSTEM_EVENT_TOGGLE_FULLSCREEN, pVar->GetIVal(), 0);
	}
}

bool CGameStartup::GetRestartLevel(char** levelName)
{
	if(GetISystem()->IsRelaunch())
		*levelName = (char*)(gEnv->pGame->GetIGameFramework()->GetLevelName());
	return GetISystem()->IsRelaunch();
}

bool CGameStartup::GetRestartMod(char* pModName, int nameLenMax)
{
	if (m_reqModUnload)
	{
		if (nameLenMax > 0)
			pModName[0] = 0;
		return true;
	}

	if (m_reqModName.empty())
		return false;

	cry_strncpy(pModName, m_reqModName.c_str(), nameLenMax);
	return true;
}

const char* CGameStartup::GetPatch() const
{
	// michiel - GS
	return NULL;	
}

#if defined(RELEASE_SERVER_SECURITY)

#define TMP_CONFIG_LINE_BUFFER			(2048)
#define FILE_CHECK_BUFFER_SIZE			(32768)

#include "IZLibCompressor.h"

bool ValidateFile(string path,string md5)
{
	bool bOk;
	// Try to open file on disk and hash it.
	FILE *file = fopen( path.c_str(),"rb" );
	if (file)
	{
		fseek( file,0,SEEK_END );
    unsigned int nFileSize = ftell(file);
		fseek( file,0,SEEK_SET );

		unsigned char *pBuf = (unsigned char*)malloc( FILE_CHECK_BUFFER_SIZE );
		if (!pBuf)
		{
			fclose(file);
			return false;
		}
																
		IZLibCompressor			*pZLib=GetISystem()->GetIZLibCompressor();

		char digest[16];
		SMD5Context context;
		string digestAsString;
		
		pZLib->MD5Init(&context);

		while (nFileSize)
		{
			unsigned int fetchLength=min(nFileSize,(unsigned int)FILE_CHECK_BUFFER_SIZE);

			if (fread( pBuf,fetchLength,1,file ) != 1)
			{
				free( pBuf );
				fclose(file);
				return false;
			}
			
			pZLib->MD5Update(&context,(const char*)pBuf,fetchLength);

			nFileSize-=fetchLength;
		}

		pZLib->MD5Final(&context,digest);

		digestAsString="";
		for (int a=0;a<16;a++)
		{
			string hex;
			hex.Format("%02x",(unsigned char)digest[a]);
			digestAsString.append(hex);
		}

		bOk = (digestAsString.compare(md5) == 0 );

		free( pBuf );
		fclose(file);

		return bOk;
	}

	return false;
}

bool PerformDedicatedInstallationSanityCheck()
{
	char tmpLineBuffer[TMP_CONFIG_LINE_BUFFER];
	string checksumPath = PathUtil::GetGameFolder() + "/Scripts/DedicatedConfigs/" + "check.txt";
	bool bOk=true;
	
	FILE *checksumFile = fopen(checksumPath.c_str(),"r");

	if (!checksumFile)
	{
		CryLogAlways("Failed to open validation configuration - Please check your install!");
		return false;
	}
	else
	{
		while (fgets(tmpLineBuffer,TMP_CONFIG_LINE_BUFFER-1,checksumFile))
		{
			string temp = tmpLineBuffer;
				
			string::size_type posEq = temp.find( "*", 0 );
			
			if (string::npos!=posEq)
			{
					string md5( temp, 0, posEq );
					string path( temp,posEq+1,temp.length());

					path.TrimRight(" \r\n");
					md5.TrimRight(" \r\n");

					if (!ValidateFile(path,md5))
					{
						CryLogAlways(path + " failed validation check - Please check your install! ("+md5+")");
						bOk=false;
					}
			}
		}

		fclose(checksumFile);
	}

	return bOk;
}
#endif

int CGameStartup::Run( const char * autoStartLevelName )
{
#if	defined(RELEASE_SERVER_SECURITY)
	CryLogAlways("Performing Validation Checks");
	if (!PerformDedicatedInstallationSanityCheck())
	{
		CryFatalError("Installation appears to be corrupt. Please check the log file for a list of problems.");
	}
#endif
	gEnv->pConsole->ExecuteString( "exec autoexec.cfg" );
	if (autoStartLevelName)
	{
		//load savegame
		if(CryStringUtils::stristr(autoStartLevelName, CRY_SAVEGAME_FILE_EXT) != 0 )
		{
			CryFixedStringT<256> fileName (autoStartLevelName);
			// NOTE! two step trimming is intended!
			fileName.Trim(" ");  // first:  remove enclosing spaces (outside ")
			fileName.Trim("\""); // second: remove potential enclosing "
			gEnv->pGame->GetIGameFramework()->LoadGame(fileName.c_str());
		}
		else	//start specified level
		{
			CryFixedStringT<256> mapCmd ("map ");
			mapCmd+=autoStartLevelName;
			gEnv->pConsole->ExecuteString(mapCmd.c_str());
		}
	}

#ifdef WIN32
	if (!(gEnv && gEnv->pSystem) || (!gEnv->IsEditor() && !gEnv->IsDedicated()))
	{
		::ShowCursor(TRUE);
		if (gEnv && gEnv->pSystem && gEnv->pSystem->GetIHardwareMouse())
			gEnv->pSystem->GetIHardwareMouse()->DecrementCounter();
	}

	AllowAccessibilityShortcutKeys(false);

	for(;;)
	{
		MSG msg;

 		// Must be PeekMessageW for Scaleform IME to function correctly and to ensure WM_CHAR contains
		// Unicode widechar for languages like Russian
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			if (msg.message != WM_QUIT)
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
 				break;
			}
		}
		else
		{
			if (!Update(true, 0))
			{
				// need to clean the message loop (WM_QUIT might cause problems in the case of a restart)
				// another message loop might have WM_QUIT already so we cannot rely only on this

 				// Must be PeekMessageW for Scaleform IME to function correctly and to ensure WM_CHAR contains
				// Unicode widechar for languages like Russian
				while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				break;
			}
		}
	}
#else
	// We should use bVisibleByDefault=false then...
	if (gEnv && gEnv->pHardwareMouse)
		gEnv->pHardwareMouse->DecrementCounter();

#if !defined(DURANGO)
	for(;;)
	{
		if (!Update(true, 0))
		{
			break;
		}
	}
#endif

#endif //WIN32

	return 0;
}

static unsigned char g_rsa_public_key_data[] =
{
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

const uint8* CGameStartup::GetRSAKey(uint32 *pKeySize) const
{
	*pKeySize = sizeof(g_rsa_public_key_data);
	return g_rsa_public_key_data;
}

void CGameStartup::OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam)
{
	switch (event)
	{
	case ESYSTEM_EVENT_RANDOM_SEED:
		g_random_generator.seed(gEnv->bNoRandomSeed?0:(uint32)wparam);
		break;
	case ESYSTEM_EVENT_CHANGE_FOCUS:
		{
			#ifdef WIN32

			AllowAccessibilityShortcutKeys(wparam==0);
			
			#endif
			GetISystem()->GetISystemEventDispatcher()->OnSystemEvent(ESYSTEM_EVENT_RANDOM_SEED, (UINT_PTR)gEnv->pTimer->GetAsyncTime().GetMicroSecondsAsInt64(), 0);
		}
		break;
	case ESYSTEM_EVENT_LEVEL_LOAD_START:
		{			
			// For MP gamemodes set the correct sound parameter
			// Default to SP
			float sp_coop_mp = 0.0f;
			if ( gEnv->bMultiplayer )
			{
				sp_coop_mp = 2.0f;
			}
			gEnv->pSoundSystem->SetGlobalParameter( "sp_coop_mp", sp_coop_mp );
			CryLog("sp_coop_mp set to %f", sp_coop_mp);
		}
		break;

	case ESYSTEM_EVENT_LEVEL_POST_UNLOAD:
		STLALLOCATOR_CLEANUP;
		break;

	case ESYSTEM_EVENT_SHUTDOWN:
		m_quit = true;
		break;
	}
}

bool CGameStartup::InitFramework(SSystemInitParams &startupParams)
{
	MEMSTAT_CONTEXT(EMemStatContextTypes::MSC_Other, 0, "Init Game Framework" );

#if !defined(_LIB) && !defined(PS3)
	m_frameworkDll = GetFrameworkDLL(startupParams.szBinariesDir);

	if (!m_frameworkDll)
	{
		// failed to open the framework dll
		CryFatalError("Failed to open the GameFramework DLL!");
		
		return false;
	}

	IGameFramework::TEntryFunction CreateGameFramework = (IGameFramework::TEntryFunction)CryGetProcAddress(m_frameworkDll, DLL_INITFUNC_CREATEGAME );

	if (!CreateGameFramework)
	{
		// the dll is not a framework dll
		CryFatalError("Specified GameFramework DLL is not valid!");

		return false;
	}
#endif //_LIB

	m_pFramework = CreateGameFramework();

	if (!m_pFramework)
	{
		CryFatalError("Failed to create the GameFramework Interface!");
		// failed to create the framework

		return false;
	}

	if (!startupParams.hWnd)
	{
		m_initWindow = true;

		if (!InitWindow(startupParams))
		{
			// failed to register window class
			CryFatalError("Failed to register CryENGINE window class!");

			return false;
		}
	}

	// initialize the engine
	if (!m_pFramework->Init(startupParams))
	{
		CryFatalError("Failed to initialize CryENGINE!");

		return false;
	}
	ModuleInitISystem(m_pFramework->GetISystem(),"CryGame");

#ifdef WIN32
	SetWindowLongPtr(reinterpret_cast<HWND>(gEnv->pRenderer->GetHWND()), GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
#endif
	return true;
}

void CGameStartup::ShutdownFramework()
{
	if (m_pFramework)
	{
		m_pFramework->Shutdown();
		m_pFramework = 0;
	}

	ShutdownWindow();
}

bool CGameStartup::InitWindow(SSystemInitParams &startupParams)
{
#ifdef WIN32
	WNDCLASS wc;

	memset(&wc, 0, sizeof(WNDCLASS));

	wc.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc   = (WNDPROC)CGameStartup::WndProcHndl;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = GetModuleHandle(0);
	// FIXME: Very bad way of getting the Icon and Cursor from the Launcher project
	wc.hIcon         = LoadIcon((HINSTANCE)startupParams.hInstance, MAKEINTRESOURCE(101));
	wc.hCursor       = LoadCursor((HINSTANCE)startupParams.hInstance, MAKEINTRESOURCE(105));
	wc.hbrBackground =(HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = GAME_WINDOW_CLASSNAME;

	if (!RegisterClass(&wc))
	{
		return false;
	}

	if (startupParams.pSystem == NULL || (!startupParams.bEditor && !gEnv->IsDedicated()))
		::ShowCursor(FALSE);

#endif // WIN32
	return true;
}

void CGameStartup::ShutdownWindow()
{
#ifdef WIN32
	if (m_initWindow)
	{
		UnregisterClass(GAME_WINDOW_CLASSNAME, GetModuleHandle(0));
	}
#endif
}

//////////////////////////////////////////////////////////////////////////
#ifdef WIN32

//LRESULT CALLBACK CGameStartup::WndProcHndl(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
PLUGIN_SDK_WINPROC_INJECTOR(LRESULT CALLBACK CGameStartup::WndProcHndl(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam))
{
	CGameStartup* self = reinterpret_cast<CGameStartup*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return self 
		? self->WndProc(hWnd, msg, wParam, lParam)
		: DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CGameStartup::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;

	IBasicEventListener::EAction action = m_quit ? IBasicEventListener::eA_Default : ProcessMessage(hWnd, msg, wParam, lParam);
	switch (action)
	{
	case IBasicEventListener::eA_None:
		result = 0;
		break;
	case IBasicEventListener::eA_Default:
		result = DefWindowProc(hWnd, msg, wParam, lParam);;
		break;
	case IBasicEventListener::eA_ActivateAndEat:
		result = MA_ACTIVATEANDEAT;
		break;
	}

	return result;
}

IBasicEventListener::EAction CGameStartup::ProcessMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	CRY_ASSERT(gEnv && gEnv->pSystem && !m_quit);

	IBasicEventListener::EAction result = IBasicEventListener::eA_Default;
	switch(msg)
	{
	case WM_CLOSE:
		result = m_basicEventListener.OnClose(hWnd);
		break;
	case WM_MOUSEACTIVATE:
		result = m_basicEventListener.OnMouseActivate(hWnd);
		break;
	case WM_ENTERSIZEMOVE:
		result = m_basicEventListener.OnEnterSizeMove(hWnd);
		break;
	case WM_EXITSIZEMOVE:
		result = m_basicEventListener.OnExitSizeMove(hWnd);
		break;
	case WM_ENTERMENULOOP:
		result = m_basicEventListener.OnEnterMenuLoop(hWnd);
		break;
	case WM_EXITMENULOOP:
		result = m_basicEventListener.OnExitMenuLoop(hWnd);
		break;
	case WM_HOTKEY:
		result = m_basicEventListener.OnHotKey(hWnd);
		break;
	case WM_SYSCHAR:	// prevent ALT + key combinations from creating 'ding' sounds
		result = m_basicEventListener.OnSycChar(hWnd);
		break;
	case WM_CHAR:
		result = m_basicEventListener.OnChar(hWnd, wParam);
		break;
	case WM_SYSKEYDOWN:	// prevent ALT-key entering menu loop
		result = m_basicEventListener.OnSysKeyDown(hWnd, wParam, lParam);
		break;
	case WM_SETCURSOR:
		result = m_basicEventListener.OnSetCursor(hWnd);
		break;
	case WM_MOUSEMOVE:
		result = m_basicEventListener.OnMouseMove(hWnd, lParam);
		break;
	case WM_LBUTTONDOWN:
		result = m_basicEventListener.OnLeftButtonDown(hWnd, lParam);
		break;
#if (_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400)
	case WM_MOUSEWHEEL:
		{
		short wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		result = m_basicEventListener.OnMouseWheel(hWnd, lParam, wheelDelta);
		break;
		}
#endif
	case WM_LBUTTONUP:
		result = m_basicEventListener.OnLeftButtonUp(hWnd, lParam);
		break;
	case WM_LBUTTONDBLCLK:
		result = m_basicEventListener.OnLeftButtonDoubleClick(hWnd, lParam);
		break;
	case WM_MOVE:
		result = m_basicEventListener.OnMove(hWnd, lParam);
		break;
	case WM_SIZE:
		result = m_basicEventListener.OnSize(hWnd, lParam);

		HandleResizeForVOIP(wParam);

		break;
	case WM_ACTIVATE:
		result = m_basicEventListener.OnActivate(hWnd, wParam);
		break;
	case WM_SETFOCUS:
		result = m_basicEventListener.OnSetFocus(hWnd);
		if (g_pGameCVars)
		{
		g_pGameCVars->g_hasWindowFocus = true;
		}
		break;
	case WM_KILLFOCUS:
		result = m_basicEventListener.OnKillFocus(hWnd);
		if (g_pGameCVars)
		{
		g_pGameCVars->g_hasWindowFocus = false;
		}
		break;
	case WM_WINDOWPOSCHANGED:
		result = m_basicEventListener.OnWindowPositionChanged(hWnd);
		break;
	case WM_STYLECHANGED:
		result = m_basicEventListener.OnWindowStyleChanged(hWnd);
		break;
	case WM_IME_SETCONTEXT:
		result = IBasicEventListener::eA_None; // In order to hide the system IME windows
		break;
	case WM_INPUTLANGCHANGE:
		result = m_basicEventListener.OnInputLanguageChanged(hWnd, wParam, lParam);
		break;
	case WM_IME_STARTCOMPOSITION:
	case WM_IME_KEYDOWN:
	case WM_IME_COMPOSITION:
	case WM_IME_ENDCOMPOSITION:
	case WM_IME_NOTIFY:
	case WM_IME_CHAR:
		result = IBasicEventListener::eA_None;
		break;
	case WM_SYSCOMMAND:
		result = m_basicEventListener.OnSysCommand(hWnd, wParam);
		break;
	}
	return result;
}


void CGameStartup::HandleResizeForVOIP(WPARAM wparam)
{
	if(gEnv->pConsole)
	{
		ICVar * pVOIPCvar = gEnv->pConsole->GetCVar("net_blaze_voip_enable");

		if(pVOIPCvar)
		{
			if(wparam == SIZE_RESTORED || wparam == SIZE_MAXIMIZED)
			{
				int currentVal = pVOIPCvar->GetIVal();
				if(m_nVOIPWasActive == -1)
				{
					m_nVOIPWasActive = currentVal;
				}
				if(m_nVOIPWasActive != currentVal)
				{
					pVOIPCvar->Set(m_nVOIPWasActive);
				}
				CryLog("[VOIP] Game maximized or restored, VOIP was set to %d, saved value %d - now restored", currentVal, m_nVOIPWasActive);
			}
			else if(wparam == SIZE_MINIMIZED)
			{
				m_nVOIPWasActive = pVOIPCvar->GetIVal();
				pVOIPCvar->Set(0);
				CryLog("[VOIP] Game minimized, VOIP was set to %d, setting to 0 while minimized", m_nVOIPWasActive);
			}
		}
	}	
}

//////////////////////////////////////////////////////////////////////////
#endif //WIN32

CGameStartup* CGameStartupStatic::g_pGameStartup = NULL;

void CGameStartupStatic::RequestLoadMod(IConsoleCmdArgs* pCmdArgs)
{
	if (pCmdArgs->GetArgCount() == 2)
	{
		if (g_pGameStartup) 
		{	
			g_pGameStartup->m_reqModName = pCmdArgs->GetArg(1);
			ISystem* pSystem = g_pGameStartup->m_pFramework->GetISystem();
			pSystem->Quit();
		}
	}
	else
	{
		CryLog("Error, correct syntax is: g_loadMod modname");
	}
}

void CGameStartupStatic::RequestUnloadMod(IConsoleCmdArgs* pCmdArgs)
{
	if (pCmdArgs->GetArgCount() == 1)
	{
		if (g_pGameStartup) 
		{	
			g_pGameStartup->m_reqModUnload = true;
			ISystem* pSystem = g_pGameStartup->m_pFramework->GetISystem();
			pSystem->Quit();
		}
	}
	else
	{
		CryLog("Error, correct syntax is: g_unloadMod");
	}
}

void CGameStartupStatic::ForceCursorUpdate()
{
#ifdef WIN32
	if(gEnv && gEnv->pRenderer && gEnv->pRenderer->GetHWND())
	{
		SendMessage(HWND(gEnv->pRenderer->GetHWND()),WM_SETCURSOR,0,0);
	}
#endif
}
//--------------------------------------------------------------------------



