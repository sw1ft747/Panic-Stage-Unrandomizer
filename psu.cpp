// C++
// Panic Stage Unrandomizer

#include "eiface.h"
#include "icvar.h"
#include "tier1/iconvar.h"
#include "tier1/convar.h"

#include "patcher.h"
#include "patch_data.h"
#include "signature_scanner.h"

using namespace PatchData;

class CPanicStageUnrandomizer : public IServerPluginCallbacks
{
public:
	CPanicStageUnrandomizer();

	// IServerPluginCallbacks methods
	virtual bool			Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory);
	virtual void			Unload(void);
	virtual void			Pause(void);
	virtual void			UnPause(void);
	virtual const char		*GetPluginDescription(void);
	virtual void			LevelInit(char const *pMapName);
	virtual void			ServerActivate(edict_t *pEdictList, int edictCount, int clientMax);
	virtual void			GameFrame(bool simulating);
	virtual void			LevelShutdown(void);
	virtual void			ClientActive(edict_t *pEntity);
	virtual void			ClientDisconnect(edict_t *pEntity);
	virtual void			ClientPutInServer(edict_t *pEntity, char const *playername);
	virtual void			SetCommandClient(int index);
	virtual void			ClientSettingsChanged(edict_t *pEdict);
	virtual PLUGIN_RESULT	ClientConnect(bool *bAllowConnect, edict_t *pEntity, const char *pszName, const char *pszAddress, char *reject, int maxrejectlen);
	virtual PLUGIN_RESULT	ClientCommand(edict_t *pEntity, const CCommand &args);
	virtual PLUGIN_RESULT	NetworkIDValidated(const char *pszUserName, const char *pszNetworkID);
	virtual void			OnQueryCvarValueFinished(QueryCvarCookie_t iCookie, edict_t *pPlayerEntity, EQueryCvarValueStatus eStatus, const char *pCvarName, const char *pCvarValue);

	// Version 3 of the interface
	virtual void			OnEdictAllocated(edict_t *edict);
	virtual void			OnEdictFreed(const edict_t *edict);

private:
	ConVar *m_PanicWavePauseMinConVar;
	ConVar *m_PanicWavePauseMaxConVar;

	CPatcher *m_StartPanicEventPatch;
	CPatcher *m_SpawnMegaMobPatch;
};

CPanicStageUnrandomizer g_PanicStageUnrandomizer;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CPanicStageUnrandomizer, IServerPluginCallbacks, INTERFACEVERSION_ISERVERPLUGINCALLBACKS, g_PanicStageUnrandomizer);

//---------------------------------------------------------------------------------------------------
// Constructor
//---------------------------------------------------------------------------------------------------
CPanicStageUnrandomizer::CPanicStageUnrandomizer() :
	m_PanicWavePauseMinConVar(nullptr),
	m_PanicWavePauseMaxConVar(nullptr),
	m_StartPanicEventPatch(nullptr),
	m_SpawnMegaMobPatch(nullptr)
{

}

//---------------------------------------------------------------------------------------------------
// Called when the plugin is loaded by the engine
//---------------------------------------------------------------------------------------------------
bool CPanicStageUnrandomizer::Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory)
{
	void *pStartPanicEvent, *pSpawnMegaMob;

	ICvar *g_Cvars = reinterpret_cast<ICvar *>(interfaceFactory(CVAR_INTERFACE_VERSION, nullptr));

	if (g_Cvars == nullptr)
	{
		Warning("[PSU] Failed to get CVar interface\n");
		return false;
	}

	DevMsg("[PSU] Found CVar interface: %8X\n", g_Cvars);

	m_PanicWavePauseMinConVar = g_Cvars->FindVar("director_panic_wave_pause_min");
	m_PanicWavePauseMaxConVar = g_Cvars->FindVar("director_panic_wave_pause_max");

	if (m_PanicWavePauseMinConVar == nullptr)
	{
		Warning("[PSU] Failed to get ConVar 'director_panic_wave_pause_min'\n");
		return false;
	}

	if (m_PanicWavePauseMaxConVar == nullptr)
	{
		Warning("[PSU] Failed to get ConVar 'director_panic_wave_pause_max'\n");
		return false;
	}

	pStartPanicEvent = FindPattern(L"server.dll", Signatures::StartPanicEvent, Signatures::StartPanicEventMask);
	pSpawnMegaMob = FindPattern(L"server.dll", Signatures::SpawnMegaMob, Signatures::SpawnMegaMobMask);

	if (pStartPanicEvent == nullptr)
	{
		Warning("[PSU] Failed to get function 'CDirectorScriptedEventManager::StartPanicEvent'\n");
		return false;
	}
	
	if (pSpawnMegaMob == nullptr)
	{
		Warning("[PSU] Failed to get function 'ZombieManager::SpawnMegaMob'\n");
		return false;
	}

	DevMsg("[PSU] Found 'CDirectorScriptedEventManager::StartPanicEvent' function: %8X\n", pStartPanicEvent);
	DevMsg("[PSU] Found 'ZombieManager::SpawnMegaMob' function: %8X\n", pSpawnMegaMob);

	/* Patch loading onto the FPU stack of float constant 2.0 to 1.0

		Before patching:

		FLD DWORD PTR [FLOAT_CONSTANT_TWO_ADDRESS]		// push 2.0
		FLD1											// push 1.0
		CALL extern:RandomFloat							// get result between 1.0 and 2.0 inclusive 

		After patching:

		FLD1											// push 1.0
		NOP
		NOP
		NOP
		NOP
		FLD1											// push 1.0
		CALL extern:RandomFloat							// get result between 1.0 and 1.0 inclusive

	*/
	m_StartPanicEventPatch = new CPatcher(pStartPanicEvent, StartPanicEvent_PatchedBytes, 6);

	/* Set loading address for FLD opcode the same as in the minimum value (4.0 to 2.0)

		Before patching:

		FLD DWORD PTR [FLOAT_CONSTANT_FOUR_ADDRESS]		// push 4.0
		FLD DWORD PTR [FLOAT_CONSTANT_TWO_ADDRESS]		// push 2.0
		CALL extern:RandomFloat							// get result between 2.0 and 4.0 inclusive

		After patching:

		FLD DWORD PTR [FLOAT_CONSTANT_TWO_ADDRESS]		// push 2.0
		FLD DWORD PTR [FLOAT_CONSTANT_TWO_ADDRESS]		// push 2.0
		CALL extern:RandomFloat							// get result between 2.0 and 2.0 inclusive

	*/
	m_SpawnMegaMobPatch = new CPatcher((BYTE *)pSpawnMegaMob + 2, (BYTE *)pSpawnMegaMob + 19, 4);

	// Set the value of ConVar 'director_panic_wave_pause_max' the same as in ConVar 'director_panic_wave_pause_min'
	m_PanicWavePauseMinConVar->SetValue(5);
	m_PanicWavePauseMaxConVar->SetValue(5);

	m_StartPanicEventPatch->Patch();
	m_SpawnMegaMobPatch->Patch();

	Msg("[PSU] Successfully loaded\n");

	return true;
}

//---------------------------------------------------------------------------------------------------
// Called when a plugin is unloaded
//---------------------------------------------------------------------------------------------------
void CPanicStageUnrandomizer::Unload(void)
{
	m_PanicWavePauseMinConVar->SetValue(5);
	m_PanicWavePauseMaxConVar->SetValue(7);

	m_StartPanicEventPatch->Unpatch();
	m_SpawnMegaMobPatch->Unpatch();

	delete m_StartPanicEventPatch;
	delete m_SpawnMegaMobPatch;

	Msg("[PSU] Successfully unloaded\n");
}

//---------------------------------------------------------------------------------------------------
// Called when the operation of the plugin is paused
//---------------------------------------------------------------------------------------------------
void CPanicStageUnrandomizer::Pause(void)
{
	m_PanicWavePauseMinConVar->SetValue(5);
	m_PanicWavePauseMaxConVar->SetValue(7);

	m_StartPanicEventPatch->Unpatch();
	m_SpawnMegaMobPatch->Unpatch();
}

//---------------------------------------------------------------------------------------------------
// Called when a plugin is brought out of the paused state
//---------------------------------------------------------------------------------------------------
void CPanicStageUnrandomizer::UnPause(void)
{
	m_PanicWavePauseMinConVar->SetValue(5);
	m_PanicWavePauseMaxConVar->SetValue(5);

	m_StartPanicEventPatch->Patch();
	m_SpawnMegaMobPatch->Patch();
}

//---------------------------------------------------------------------------------------------------
// The name of this plugin, returned in "plugin_print" command
//---------------------------------------------------------------------------------------------------
const char *CPanicStageUnrandomizer::GetPluginDescription(void)
{
	return "Panic Stage Unrandomizer v1.0 : Sw1ft";
}

//---------------------------------------------------------------------------------------------------
// Called on level (map) startup, it's the first function called as a server enters a new level
//---------------------------------------------------------------------------------------------------
void CPanicStageUnrandomizer::LevelInit(char const *pMapName)
{

}

//---------------------------------------------------------------------------------------------------
// Called when the server successfully enters a new level, this will happen after the LevelInit call
//---------------------------------------------------------------------------------------------------
void CPanicStageUnrandomizer::ServerActivate(edict_t *pEdictList, int edictCount, int clientMax)
{

}

//---------------------------------------------------------------------------------------------------
// Called once per server frame
//---------------------------------------------------------------------------------------------------
void CPanicStageUnrandomizer::GameFrame(bool simulating)
{

}

//---------------------------------------------------------------------------------------------------
// Called when a server is changing to a new level or is being shutdown
//---------------------------------------------------------------------------------------------------
void CPanicStageUnrandomizer::LevelShutdown(void) // can be called multiple times during a map change
{

}

//---------------------------------------------------------------------------------------------------
// Called after a client is fully spawned
//---------------------------------------------------------------------------------------------------
void CPanicStageUnrandomizer::ClientActive(edict_t *pEntity)
{

}

//---------------------------------------------------------------------------------------------------
// Called when a client disconnects from the server
//---------------------------------------------------------------------------------------------------
void CPanicStageUnrandomizer::ClientDisconnect(edict_t *pEntity)
{

}

//---------------------------------------------------------------------------------------------------
// Called when a client spawns into a server
//---------------------------------------------------------------------------------------------------
void CPanicStageUnrandomizer::ClientPutInServer(edict_t *pEntity, char const *playername)
{

}

//---------------------------------------------------------------------------------------------------
// Called by the ConVar code to track of which client is entering a ConCommand
//---------------------------------------------------------------------------------------------------
void CPanicStageUnrandomizer::SetCommandClient(int index)
{

}

//---------------------------------------------------------------------------------------------------
// Called when player specific cvars about a player change (for example the users name)
//---------------------------------------------------------------------------------------------------
void CPanicStageUnrandomizer::ClientSettingsChanged(edict_t *pEdict)
{

}

//---------------------------------------------------------------------------------------------------
// Called when a client initially connects to a server
//---------------------------------------------------------------------------------------------------
PLUGIN_RESULT CPanicStageUnrandomizer::ClientConnect(bool *bAllowConnect, edict_t *pEntity, const char *pszName, const char *pszAddress, char *reject, int maxrejectlen)
{
	return PLUGIN_CONTINUE;
}

//---------------------------------------------------------------------------------------------------
// Called when a remote client enters a command into the console
//---------------------------------------------------------------------------------------------------
PLUGIN_RESULT CPanicStageUnrandomizer::ClientCommand(edict_t *pEntity, const CCommand &args)
{
	return PLUGIN_CONTINUE;
}

//---------------------------------------------------------------------------------------------------
// Called when the server retrieves a clients network ID (i.e Steam ID)
//---------------------------------------------------------------------------------------------------
PLUGIN_RESULT CPanicStageUnrandomizer::NetworkIDValidated(const char *pszUserName, const char *pszNetworkID)
{
	return PLUGIN_CONTINUE;
}

//---------------------------------------------------------------------------------------------------
// Called when a query from IServerPluginHelpers::StartQueryCvarValue() finishes
//---------------------------------------------------------------------------------------------------
void CPanicStageUnrandomizer::OnQueryCvarValueFinished(QueryCvarCookie_t iCookie, edict_t *pPlayerEntity, EQueryCvarValueStatus eStatus, const char *pCvarName, const char *pCvarValue)
{

}

//---------------------------------------------------------------------------------------------------
// ToDo
//---------------------------------------------------------------------------------------------------
void CPanicStageUnrandomizer::OnEdictAllocated(edict_t *edict)
{

}

//---------------------------------------------------------------------------------------------------
// ToDo
//---------------------------------------------------------------------------------------------------
void CPanicStageUnrandomizer::OnEdictFreed(const edict_t *edict)
{

}
