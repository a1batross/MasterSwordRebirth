/***
*
*	Copyright (c) 1999, 2000 Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
/*

Class Hierachy

CBaseEntity
	CBaseDelay
		CBaseToggle
			CBaseItem
			CBaseMonster
				CBaseCycler
				CBasePlayer
				CBaseGroup
*/
#ifndef CBASE_H
#define CBASE_H

#define MAX_PATH_SIZE 10 // max number of nodes available for a path.

// These are caps bits to indicate what an object's capabilities (currently used for save/restore and level transitions)
#define FCAP_CUSTOMSAVE 0x00000001
#define FCAP_ACROSS_TRANSITION 0x00000002 // should transfer between transitions
#define FCAP_MUST_SPAWN 0x00000004		  // Spawn after restore
#define FCAP_DONT_SAVE 0x80000000		  // Don't save this
#define FCAP_IMPULSE_USE 0x00000008		  // can be used by the player
#define FCAP_CONTINUOUS_USE 0x00000010	  // can be used by the player
#define FCAP_ONOFF_USE 0x00000020		  // can be used by the player
#define FCAP_DIRECTIONAL_USE 0x00000040	  // Player sends +/- 1 when using (currently only tracktrains)
#define FCAP_MASTER 0x00000080			  // Can be used to "master" other entities (like multisource)

// UNDONE: This will ignore transition volumes (trigger_transition), but not the PVS!!!
#define FCAP_FORCE_TRANSITION 0x00000080 // ALWAYS goes across transitions

#include "archtypes.h"     // DAL

//Master Sword - no save/restore in client dll
#define MAXPLAYERS 32
#ifdef VALVE_DLL
#include "saverestore.h"
#else
class CSave
{
};
class CRestore
{
};
#endif
//-------------
#include "schedule.h"

#ifndef MONSTEREVENT_H
#include "monsterevent.h"
#endif

// C functions for external declarations that call the appropriate C++ methods

#ifdef EXPORT
#undef EXPORT
#endif

#ifndef CBASE_DLLEXPORT
#ifdef _WIN32
#define CBASE_DLLEXPORT _declspec( dllexport )
#else
#define CBASE_DLLEXPORT __attribute__ ((visibility("default")))
#endif
#endif

#define EXPORT CBASE_DLLEXPORT

extern "C" CBASE_DLLEXPORT int GetEntityAPI(DLL_FUNCTIONS *pFunctionTable, int interfaceVersion);
extern "C" CBASE_DLLEXPORT int GetEntityAPI2(DLL_FUNCTIONS *pFunctionTable, int *interfaceVersion);
extern "C" CBASE_DLLEXPORT int GetNewDLLFunctions(NEW_DLL_FUNCTIONS *pFunctionTable, int *interfaceVersion);

extern int DispatchSpawn(edict_t *pent);
extern void DispatchKeyValue(edict_t *pentKeyvalue, KeyValueData *pkvd);
extern void DispatchTouch(edict_t *pentTouched, edict_t *pentOther);
extern void DispatchUse(edict_t *pentUsed, edict_t *pentOther);
extern void DispatchThink(edict_t *pent);
extern void DispatchBlocked(edict_t *pentBlocked, edict_t *pentOther);
extern void DispatchSave(edict_t *pent, SAVERESTOREDATA *pSaveData);
extern int DispatchRestore(edict_t *pent, SAVERESTOREDATA *pSaveData, int globalEntity);
extern void DispatchObjectCollsionBox(edict_t *pent);
extern void SaveWriteFields(SAVERESTOREDATA *pSaveData, const char *pname, void *pBaseData, TYPEDESCRIPTION *pFields, int fieldCount);
extern void SaveReadFields(SAVERESTOREDATA *pSaveData, const char *pname, void *pBaseData, TYPEDESCRIPTION *pFields, int fieldCount);
extern void SaveGlobalState(SAVERESTOREDATA *pSaveData);
extern void RestoreGlobalState(SAVERESTOREDATA *pSaveData);
extern void ResetGlobalState(void);

typedef enum
{
	USE_OFF = 0,
	USE_ON = 1,
	USE_SET = 2,
	USE_TOGGLE = 3
} USE_TYPE;

extern void FireTargets(const char *targetName, CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);

typedef void (CBaseEntity::*BASEPTR)(void);
typedef void (CBaseEntity::*ENTITYFUNCPTR)(CBaseEntity *pOther);
typedef void (CBaseEntity::*USEPTR)(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);

// Master Sword - New CLASS defs:

#define CLASS_NONE 0
#define CLASS_MACHINE 1
#define CLASS_PLAYER 2
#define CLASS_NPC 3
#define CLASS_MONSTER 4 //CLASS_HUMAN_MILITARY

// OLD DEFS!
// For CLASSIFY
//#define	CLASS_NONE				0
//#define CLASS_MACHINE			1
//#define CLASS_PLAYER			2
#define CLASS_HUMAN_PASSIVE 5
#define CLASS_HUMAN_MILITARY 6
#define CLASS_ALIEN_MILITARY 7
#define CLASS_ALIEN_PASSIVE 8
#define CLASS_ALIEN_MONSTER 9
#define CLASS_ALIEN_PREY 10
#define CLASS_ALIEN_PREDATOR 11
#define CLASS_INSECT 12
#define CLASS_PLAYER_ALLY 13
#define CLASS_PLAYER_BIOWEAPON 14 // hornets and snarks.launched by players
#define CLASS_ALIEN_BIOWEAPON 15  // hornets and snarks.launched by the alien menace
#define CLASS_BARNACLE 99		  // special because no one pays attention to it, and it eats a wide cross-section of creatures.
#define CHAR_LEVEL_CAP 45		  // MiB JAN2010_15 Global Level Cap

class CBaseEntity;
class CBaseMonster;
class CBasePlayerItem;
class CSquadMonster;

#define SF_NORESPAWN (1 << 30) // !!!set this bit on guns and stuff that should never respawn.

//
// EHANDLE. Safe way to point to CBaseEntities who may die between frames
//
class EHANDLE
{
private:
	edict_t *m_pent;
	int m_serialnumber;

public:
	edict_t *Get(void);
	edict_t *Set(edict_t *pent);

	operator int();

	operator CBaseEntity *();

	CBaseEntity *operator=(CBaseEntity *pEntity);
	CBaseEntity *operator->();
};

enum givetype_e
{
	GIVE_HP,
	GIVE_MP,
	GIVE_GOLD
};

struct entityinfo_t
{ //For keeping track of an entity across frames
	entityinfo_t() {}
	entityinfo_t(CBaseEntity *pEntity);
	entityinfo_t &operator=(CBaseEntity *pEntity);
	operator bool();
	CBaseEntity *Entity();
	int entindex;
	CBaseEntity *pvPrivData;
};

enum enttype_e
{
	ENT_LASTSEEN,
	ENT_LASTHEARD,		//Last sound I heard
	ENT_LASTSPOKE,		//Last person I heard speaking to me
	ENT_LASTOFFERITEM,	//Last person that offered me an item
	ENT_LASTGAVEITEM,	//Last person I accepted an offer from
	ENT_LASTSTOLE,		//Last person that stole from me
	ENT_LASTUSED,		//Last person that used me
	ENT_LASTSTRUCK,		//Last person that hit me
	ENT_LASTSTRUCKBYME, //Last person I hit
	ENT_LASTPROJECTILE, //Last projectile I created
	ENT_LASTCREATED,	//Last entity created (createnpc/createitem)
	ENT_ME,				//Me
	ENT_OWNER,			//My owner (items)
	ENT_CREATIONOWNER,	//The entity that created me
	ENT_TARGET,			//My current target (npcs)
	ENT_EXPOWNER,		//My exp owner (monsters - gives exp from my kills to him)
	ENT_LOCALPLAYER,	//Local player - client only
	ENT_CURRENTPLAYER,	//The player that's running a command
	ENT_TYPE_TOTAL
};
extern char *g_EntTypeByName[ENT_TYPE_TOTAL];
int EntityNameToType(const char *pszName);

//
// Base Entity.  All entity types derive from this
//
#ifdef VALVE_DLL
#define STUB_CLIENT ;
#else
#define STUB_CLIENT \
	{               \
	}
#endif

//MIB FEB2008a
//- Thothie - I really wish you wouldn't call it "scriptfile_t" *sigh*
struct scriptfile_t
{
private:
	int curline;

public:
	msstring fileName;
	bool endoffile;
	bool nofile;

	//Variables for FN Files
	int handleIdx;
	bool readyForRead; //For FN Files
	//End

	msstringlist Lines;

	void JumpToLine(int line) { curline = line; }

	scriptfile_t &operator=(const msstring_ref a);

	void Open(msstring_ref a);
	void Reset();
	void AddLine(msstring line, int lineNum, bool overwrite);

	msstring ScriptFile_ReadLine();
	msstring ScriptFile_ReadLine(int line);
	void ScriptFile_WriteLine(msstring line);
	void ScriptFile_WriteLine(msstring line, int lineNum, bool overwrite = false);
	//Only used if you're "inserting" a line
	//Choice between inserting or overwriting
};

typedef msstringlist msscriptarray;
typedef std::map<msstring, msscriptarray> msscriptarrayhash;
typedef msstringstringhash msscripthash;
typedef std::map<msstring, msscripthash> msscripthashhash;
typedef std::set<msstring> msscriptset;
typedef std::map<msstring, msscriptset> msscriptsethash;

//MiB JAN2010_27
struct scriptarray_t
{
	msstring Name;
	msstringlist Vals;
};

//Thothie JAN2013_02 - scriptflags array
struct scriptflags_t
{
	msstringlist names;
	msstringlist types;
	msstringlist values;
	msstringlist expiretimes;
	msstringlist exp_messages;
};

//Thothie MAR2012_26 attempting to avoid duplicate precaches
struct modelprecachelist_t
{
	msstring PrecacheName;
};

msscriptarray * GetScriptedArrayFromHashMap(msscriptarrayhash & vArrayHashMap, msstring & vsName, bool bAllowCreate = false, bool * pbExisted = NULL);
msscripthash * GetScriptedHashMapFromHashMap(msscripthashhash & vHashMapHashMap, msstring & vsName, bool bAllowCreate = false, bool * pbExisted = NULL);
msscriptset * GetScriptedSetFromHashMap(msscriptsethash & vSetHashMap, msstring & vsName, bool bAllowCreate = false, bool * pbExisted = NULL);

class CBaseEntity
{
public:
	typedef CBaseEntity BaseClass;
	
	/**
	*	Virtual destructor method
	*	Used to clean up per-entity resources when the entity gets removed
	*	Note: don't use EHANDLEs or attempt to access other entities here
	*/
	virtual void OnDestroy() {}
	
	//MasterSword
	//MiB DEC2007a
	string_i ScriptFName;
	mslist<scriptfile_t> filesOpen;		  //MiB FEB2008a - file i/o

	msscriptarrayhash scriptedArrays; //MiB JAN2010_27 - Arrays, new type.
	msscripthashhash mScriptedHashes; // MiB SEP2019_23 - Scripted Hashes
	msscriptsethash mScriptedSets; // MiB SEP2019_23 - Scripted sets

	scriptflags_t m_scriptflags;		  //Thothie JAN2013_02 - Scripted Flags
	virtual bool IsMSItem() { return false; }
	virtual int MSProperties() { return 0; }														//Useful for identification
	virtual void *MSQuery(int iRequest) { return NULL; }											//Useful for obtaining specific information
	virtual msstring_ref GetProp(CBaseEntity *pTarget, msstring &FullParams, msstringlist &Params); //Gets a property of the specified entity. Can be myself, or another entity for determining relationships
	virtual float Give(givetype_e Type, float Amt) { return 0.0f; }									//Give/Take Health, MP, Gold, etc
	virtual void Deactivate() {}																	//Called when the server shuts down
	//CounterEffect: Fists hit a player with armor.  Armor class calls
	//PlayerHands->CounterEffect() to have the hands hurt their owner
	virtual void CounterEffect(CBaseEntity *pInflictor, int iEffect, void *pExtraData = NULL);
	//TakeDamageEffect: TakeDamage Calls this so that special effects
	//from certain damages can resolve
	virtual void TakeDamageEffect(CBaseEntity *pInflicter, CBaseEntity *pAttacker, float flDamage, int bitsDamageType){};
	//virtual void	StruckSound( CBaseEntity *pInflicter, CBaseEntity *pAttacker, float flDamage, TraceResult *ptr, int bitsDamageType );
	virtual const char *DisplayName() { return m_DisplayName.c_str(); }
	virtual void DelayedRemove();
	virtual float Volume() { return m_Volume; }
	virtual float Weight() { return m_Weight; }
	virtual class IScripted *GetScripted() { return NULL; }
	virtual void SetFollow(CBaseEntity *pTarget, int Flags = 0) STUB_CLIENT //Special follow type.  Follows and rotates with the entity while offset by a certain amount
		inline float BaseThinkTime()
	{
		return m_Brush ? pev->ltime : gpGlobals->time;
	}
	virtual bool CloseEnough(Vector Point, float Radius) { return GetBodyDist(Point, Radius) <= 0; }
	virtual float GetBodyDist(Vector Point, float Radius) { return (Center() - Point).Length() - Radius; }

	entityinfo_t m_EntityList[ENT_TYPE_TOTAL];
	virtual void StoreEntity(CBaseEntity *pEntity, enttype_e EntType);
	virtual CBaseEntity *RetrieveEntity(enttype_e EntType);
	virtual CBaseEntity *RetrieveEntity(const char *pszName);
	Vector DetermineOrigin(msstring &vsOrigin);

	msscriptarray * GetScriptedArray(msstring & vsName, bool bAllowCreate = false, bool * pbExisted = NULL)
	{
			return GetScriptedArrayFromHashMap(scriptedArrays, vsName, bAllowCreate, pbExisted);
	}

	msscripthash * GetScriptedHashMap(msstring & vsName, bool bAllowCreate = false, bool * pbExisted = NULL)
	{
			return GetScriptedHashMapFromHashMap(mScriptedHashes, vsName, bAllowCreate, pbExisted);
	}

	msscriptset * GetScriptedSet(msstring & vsName, bool bAllowCreate = false, bool * pbExisted = NULL)
	{
			return GetScriptedSetFromHashMap(mScriptedSets, vsName, bAllowCreate, pbExisted);
	}

	//Properties for both NPCs and weapons
	float SndVolume, m_Volume, m_Weight;
	msstring m_DisplayName, DisplayPrefix, DisplayDesc,
		m_NetName, m_Target, m_TargetName, m_ModelName;
	entityinfo_t m_AttachToEnt;

	int MSMoveType;		 //Special move types
	Vector OldBounds[2]; //For use with G_SolidifyEnts()
	bool m_Brush;
//----------------
#ifdef VALVE_DLL
	virtual bool FInViewCone(CBaseEntity* pEntity, float m_flFieldOfView) STUB_CLIENT
		virtual bool FInViewCone(Vector& Origin, float m_flFieldOfView) STUB_CLIENT
		virtual bool CanDamage(CBaseEntity* pOther); //Can I damage this entity?
	virtual void SUB_FadeOut(float FadeDuration);
	virtual void Think_FadeOut(void);
	virtual float TraceAttack(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, Vector vecDir, TraceResult* ptr, int bitsDamageType, int AccuracyRoll = 0);

	float m_FadeDuration;
	float m_TimeFadeStart;
#endif

	// Constructor.  Set engine to use C/C++ callback functions
	// pointers to engine data
	entvars_t *pev; // Don't need to save/restore this pointer, the engine resets it

	// path corners
	CBaseEntity *m_pGoalEnt; // path corner we are heading towards
	CBaseEntity *m_pLink;	 // used for temporary link-list operations.

	// initialization functions
	virtual void Spawn(void) {}
	virtual void Precache(void) { return; }
	virtual void KeyValue(KeyValueData *pkvd) { pkvd->fHandled = FALSE; }
	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);
	virtual int ObjectCaps(void) { return FCAP_ACROSS_TRANSITION; }
	virtual void Activate(void) {}

	// Setup the object->object collision box (pev->mins / pev->maxs is the object->world collision box)
	virtual void SetObjectCollisionBox(void);

	// Classify - returns the type of group (i.e, "houndeye", or "human military" so that monsters with different classnames
	// still realize that they are teammates. (overridden for monsters that form groups)
	virtual int Classify(void) { return CLASS_NONE; };
	virtual void DeathNotice(entvars_t *pevChild) {} // monster maker children use this to tell the monster maker that they have died.

	static TYPEDESCRIPTION m_SaveData[];

	virtual int TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType);
	virtual int TakeHealth(float flHealth, int bitsDamageType);
	virtual void Killed(entvars_t *pevAttacker, int iGib);
	virtual int BloodColor(void) { return DONT_BLEED; }
	virtual void TraceBleed(float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);
	virtual BOOL IsTriggered(CBaseEntity *pActivator) { return TRUE; }
	virtual CBaseMonster *MyMonsterPointer(void) { return NULL; }
	virtual CSquadMonster *MySquadMonsterPointer(void) { return NULL; }
	virtual bool IsMSMonster(void) { return false; }
	virtual int GetToggleState(void) { return TS_AT_TOP; }
	virtual void AddPoints(int score, BOOL bAllowNegativeScore) {}
	virtual void AddPointsToTeam(int score, BOOL bAllowNegativeScore) {}
	virtual int GiveAmmo(int iAmount, char *szName, int iMax) { return -1; };
	virtual float GetDelay(void) { return 0; }
	virtual int IsMoving(void) { return pev->velocity != g_vecZero; }
	virtual void OverrideReset(void) {}
	virtual int DamageDecal(int bitsDamageType);
	// This is ONLY used by the node graph to test movement through a door
	virtual void SetToggleState(int state) {}
	virtual void StartSneaking(void) {}
	virtual void StopSneaking(void) {}
	virtual BOOL OnControls(entvars_t *pev) { return FALSE; }
	virtual BOOL IsSneaking(void) { return FALSE; }
	virtual BOOL IsAlive(void) { return (pev->deadflag == DEAD_NO) && pev->health > 0; }
	virtual BOOL IsBSPModel(void) { return pev->solid == SOLID_BSP || pev->movetype == MOVETYPE_PUSHSTEP; }
	virtual BOOL ReflectGauss(void) { return (IsBSPModel() && !pev->takedamage); }
	virtual BOOL HasTarget(string_t targetname) { return FStrEq(STRING(targetname), STRING(pev->targetname)); }
	virtual BOOL IsInWorld(void);
	virtual BOOL IsPlayer(void) { return FALSE; }
	virtual BOOL IsNetClient(void) { return FALSE; }
	virtual const char *TeamID(void) { return ""; }

	//	virtual void	SetActivator( CBaseEntity *pActivator ) {}
	virtual CBaseEntity *GetNextTarget(void);

	// fundamental callbacks
	void (CBaseEntity ::*m_pfnThink)(void);
	void (CBaseEntity ::*m_pfnTouch)(CBaseEntity *pOther);
	void (CBaseEntity ::*m_pfnUse)(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	void (CBaseEntity ::*m_pfnBlocked)(CBaseEntity *pOther);

	virtual void Think(void)
	{
		if (m_pfnThink)
			(this->*m_pfnThink)();
	}
	virtual void Touch(CBaseEntity *pOther)
	{
		if (m_pfnTouch)
			(this->*m_pfnTouch)(pOther);
	};
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
	{
		if (m_pfnUse)
			(this->*m_pfnUse)(pActivator, pCaller, useType, value);
	}
	virtual void Blocked(CBaseEntity *pOther)
	{
		if (m_pfnBlocked)
			(this->*m_pfnBlocked)(pOther);
	};

	// allow engine to allocate instance data
	void *operator new(size_t stAllocateBlock, entvars_t *pev)
	{
		return (void *)ALLOC_PRIVATE(ENT(pev), stAllocateBlock);
	};

		// don't use this.
#if _MSC_VER >= 1200 // only build this code if MSVC++ 6.0 or higher
	void operator delete(void *pMem, entvars_t *pev)
	{
		pev->flags |= FL_KILLME;
	};
#endif

	void UpdateOnRemove(void);

	// common member functions
	// Master Sword: this is virtual now
	virtual void EXPORT SUB_Remove(void);
	// ---------------------------------
	void EXPORT SUB_DoNothing(void);
	void EXPORT SUB_StartFadeOut(void);
	void EXPORT SUB_FadeOut(void);
	void EXPORT SUB_CallUseToggle(void) { this->Use(this, this, USE_TOGGLE, 0); }
	int ShouldToggle(USE_TYPE useType, BOOL currentState);
	void FireBullets(ULONG cShots, Vector vecSrc, Vector vecDirShooting, Vector vecSpread, float flDistance, int iBulletType, int iTracerFreq = 4, int iDamage = 0, entvars_t *pevAttacker = NULL);

	virtual CBaseEntity *Respawn(void) { return NULL; }

//Master Sword
#ifdef VALVE_DLL
	void SUB_UseTargets(CBaseEntity *pActivator, USE_TYPE useType, float value);
#endif

	// Do the bounding boxes of these two intersect?
	int Intersects(CBaseEntity *pOther);
	void MakeDormant(void);
	int IsDormant(void);
	BOOL IsLockedByMaster(void) { return FALSE; }

	static CBaseEntity *Instance(edict_t *pent)
	{
		if (!pent)
			pent = ENT(0);
		CBaseEntity *pEnt = (CBaseEntity *)GET_PRIVATE(pent);
		return pEnt;
	}

	static CBaseEntity *Instance(entvars_t *pev) { return Instance(ENT(pev)); }
	static CBaseEntity *Instance(int eoffset) { return Instance(ENT(eoffset)); }

	CBaseMonster *GetMonsterPointer(entvars_t *pevMonster)
	{
		CBaseEntity *pEntity = Instance(pevMonster);
		if (pEntity)
			return pEntity->MyMonsterPointer();
		return NULL;
	}
	CBaseMonster *GetMonsterPointer(edict_t *pentMonster)
	{
		CBaseEntity *pEntity = Instance(pentMonster);
		if (pEntity)
			return pEntity->MyMonsterPointer();
		return NULL;
	}
	virtual CBaseEntity *GetSelf()
	{
		return this;
	}

	// Ugly code to lookup all functions to make sure they are exported when set.
#ifdef _DEBUG
	void FunctionCheck(void *pFunction, char *name){
#ifdef _WIN32
//		if (pFunction && !NAME_FOR_FUNCTION((unsigned long)(pFunction)) )
//			ALERT( at_error, "No EXPORT: %s:%s (%08lx)\n", STRING(pev->classname), name, (unsigned long)pFunction );
#endif // _WIN32
	}

	BASEPTR ThinkSet(BASEPTR func, char *name)
	{
		m_pfnThink = func;
		FunctionCheck((void *)*((int *)((char *)this + (offsetof(CBaseEntity, m_pfnThink)))), name);
		return func;
	}
	ENTITYFUNCPTR TouchSet(ENTITYFUNCPTR func, char *name)
	{
		m_pfnTouch = func;
		FunctionCheck((void *)*((int *)((char *)this + (offsetof(CBaseEntity, m_pfnTouch)))), name);
		return func;
	}
	USEPTR UseSet(USEPTR func, char *name)
	{
		m_pfnUse = func;
		FunctionCheck((void *)*((int *)((char *)this + (offsetof(CBaseEntity, m_pfnUse)))), name);
		return func;
	}
	ENTITYFUNCPTR BlockedSet(ENTITYFUNCPTR func, char *name)
	{
		m_pfnBlocked = func;
		FunctionCheck((void *)*((int *)((char *)this + (offsetof(CBaseEntity, m_pfnBlocked)))), name);
		return func;
	}

#endif

	// virtual functions used by a few classes

	// used by monsters that are created by the MonsterMaker
	virtual void UpdateOwner(void) { return; };

	//
	static CBaseEntity *Create(char *szName, const Vector &vecOrigin, const Vector &vecAngles, edict_t *pentOwner = NULL);

	virtual BOOL FBecomeProne(void) { return FALSE; };
	edict_t *edict()
	{ //Master Sword... Client has no edict_t !
#ifdef VALVE_DLL
		return ENT(pev);
#else
		return pev ? (edict_t *)pev : NULL;
#endif
	};
	EOFFSET eoffset() { return OFFSET(pev); };
	int entindex() { return ENTINDEX(edict()); };

	virtual Vector Center() { return (pev->absmax + pev->absmin) * 0.5; }; // center point of entity
	virtual Vector EyePosition() { return pev->origin + pev->view_ofs; };  // position of eyes
	virtual Vector EarPosition() { return pev->origin + pev->view_ofs; };  // position of ears
	virtual Vector BodyTarget(const Vector &posSrc) { return Center(); };  // position to shoot at

	virtual int Illumination() { return GETENTITYILLUM(ENT(pev)); };

	virtual BOOL FMVisible(CBaseEntity *pEntity);
	virtual BOOL FMVisible(const Vector &vecOrigin);

	virtual BOOL FVisible(CBaseEntity *pEntity);
	virtual BOOL FVisible(const Vector &vecOrigin);
	
#ifdef VALVE_DLL
	bool NOSEND;
	bool FORCESEND;
#endif
};

class CScriptedEnt : public CBaseEntity, public IScripted
{
public:
	class IScripted *GetScripted() { return this; }
	void Spawn();
	void Think();
	void Blocked(CBaseEntity *pOther);
	void Touch(CBaseEntity *pOther);
	void Deactivate(); //Called when the server shuts down

	void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	void KeyValue(KeyValueData *pkvd);

	bool m_HandleTouch, m_HandleThink, m_HandleBlocked;
};

// Ugly technique to override base member functions
// Normally it's illegal to cast a pointer to a member function of a derived class to a pointer to a
// member function of a base class.  static_cast is a sleezy way around that problem.

#ifdef _DEBUG

#define SetThink(a) ThinkSet(static_cast<void (CBaseEntity::*)(void)>(a), #a)
#define SetTouch(a) TouchSet(static_cast<void (CBaseEntity::*)(CBaseEntity *)>(a), #a)
#define SetUse(a) UseSet(static_cast<void (CBaseEntity::*)(CBaseEntity * pActivator, CBaseEntity * pCaller, USE_TYPE useType, float value)>(a), #a)
#define SetBlocked(a) BlockedSet(static_cast<void (CBaseEntity::*)(CBaseEntity *)>(a), #a)

#else

#define SetThink(a) m_pfnThink = static_cast<void (CBaseEntity::*)(void)>(a)
#define SetTouch(a) m_pfnTouch = static_cast<void (CBaseEntity::*)(CBaseEntity *)>(a)
#define SetUse(a) m_pfnUse = static_cast<void (CBaseEntity::*)(CBaseEntity * pActivator, CBaseEntity * pCaller, USE_TYPE useType, float value)>(a)
#define SetBlocked(a) m_pfnBlocked = static_cast<void (CBaseEntity::*)(CBaseEntity *)>(a)

#endif

class CPointEntity : public CBaseEntity
{
public:
	void Spawn(void);
	virtual int ObjectCaps(void) { return CBaseEntity ::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }

private:
};

typedef struct locksounds // sounds that doors and buttons make when locked/unlocked
{
	string_t sLockedSound;		// sound a door makes when it's locked
	string_t sLockedSentence;	// sentence group played when door is locked
	string_t sUnlockedSound;	// sound a door makes when it's unlocked
	string_t sUnlockedSentence; // sentence group played when door is unlocked

	int iLockedSentence;   // which sentence in sentence group to play next
	int iUnlockedSentence; // which sentence in sentence group to play next

	float flwaitSound;	  // time delay between playing consecutive 'locked/unlocked' sounds
	float flwaitSentence; // time delay between playing consecutive sentences
	BYTE bEOFLocked;	  // true if hit end of list of locked sentences
	BYTE bEOFUnlocked;	  // true if hit end of list of unlocked sentences
} locksound_t;

void PlayLockSounds(entvars_t *pev, locksound_t *pls, int flocked, int fbutton);

//
// MultiSouce
//

#define MAX_MULTI_TARGETS 16 // maximum number of targets a single multi_manager entity may be assigned.
#define MS_MAX_TARGETS 32

class CMultiSource : public CPointEntity
{
public:
	void Spawn();
	void KeyValue(KeyValueData *pkvd);
	void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	int ObjectCaps(void) { return (CPointEntity::ObjectCaps() | FCAP_MASTER); }
	BOOL IsTriggered(CBaseEntity *pActivator);
	void EXPORT Register(void);
	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);

	static TYPEDESCRIPTION m_SaveData[];

	EHANDLE m_rgEntities[MS_MAX_TARGETS];
	int m_rgTriggered[MS_MAX_TARGETS];

	int m_iTotal;
	string_t m_globalstate;
};

//
// generic Delay entity.
//
class CBaseDelay : public CScriptedEnt //Master Sword - changed
{
public:
	//Master Sword
	CBaseEntity *GetSelf();

	float m_flDelay;
	int m_iszKillTarget;

	virtual void KeyValue(KeyValueData *pkvd);
	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);

	static TYPEDESCRIPTION m_SaveData[];
	// common member functions
	//Master Sword
#ifdef VALVE_DLL
	void SUB_UseTargets(CBaseEntity *pActivator, USE_TYPE useType, float value);
#endif
	void EXPORT DelayThink(void);
};

class CBaseAnimating : public CBaseDelay
{
public:
	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);

	static TYPEDESCRIPTION m_SaveData[];

	// Basic Monster Animation functions
	float StudioFrameAdvance(float flInterval = 0.0); // accumulate animation frame time from last time called until now
	int GetSequenceFlags(void);
	int LookupActivity(int activity);
	int LookupActivityHeaviest(int activity);
	int LookupSequence(const char *label);
	void ResetSequenceInfo();
	void DispatchAnimEvents(float flFutureInterval = 0.1); // Handle events that have happend since last time called up until X seconds into the future
	virtual void HandleAnimEvent(MonsterEvent_t *pEvent) { return; };
	float SetBoneController(int iController, float flValue);
	void InitBoneControllers(void);
	float SetBlending(int iBlender, float flValue);
	void GetBonePosition(int iBone, Vector &origin, Vector &angles);
	void GetAutomovement(Vector &origin, Vector &angles, float flInterval = 0.1);
	int FindTransition(int iEndingSequence, int iGoalSequence, int *piDir);
	void GetAttachment(int iAttachment, Vector &origin, Vector &angles);
	void SetBodygroup(int iGroup, int iValue);
	int GetBodygroup(int iGroup);
	int ExtractBbox(int sequence, float *mins, float *maxs);
	void SetSequenceBox(void);

	// animation needs
	float m_flFrameRate;	  // computed FPS for current sequence
	float m_flGroundSpeed;	  // computed linear movement rate for current sequence
	float m_flLastEventCheck; // last time the event list was checked
	BOOL m_fSequenceFinished; // flag set when StudioAdvanceFrame moves across a frame boundry
	BOOL m_fSequenceLoops;	  // true if the sequence loops
};

//
// generic Toggle entity.
//
#define SF_ITEM_USE_ONLY 256 //  ITEM_USE_ONLY = BUTTON_USE_ONLY = DOOR_USE_ONLY!!!

class CBaseToggle : public CBaseAnimating
{
public:
	void KeyValue(KeyValueData *pkvd);

	TOGGLE_STATE m_toggle_state;
	float m_flActivateFinished; //like attack_finished, but for doors
	float m_flMoveDistance;		// how far a door should slide or rotate
	float m_flWait;
	float m_flLip;
	float m_flTWidth;  // for plats
	float m_flTLength; // for plats

	Vector m_vecPosition1;
	Vector m_vecPosition2;
	Vector m_vecAngle1;
	Vector m_vecAngle2;

	int m_cTriggersLeft; // trigger_counter only, # of activations remaining
	float m_flHeight;
	EHANDLE m_hActivator;
	void (CBaseToggle::*m_pfnCallWhenMoveDone)(void);
	Vector m_vecFinalDest;
	Vector m_vecFinalAngle;

	int m_bitsDamageInflict; // DMG_ damage type that the door or tigger does

	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);

	static TYPEDESCRIPTION m_SaveData[];

	virtual int GetToggleState(void) { return m_toggle_state; }
	virtual float GetDelay(void) { return m_flWait; }

	// common member functions
	void LinearMove(Vector vecDest, float flSpeed);
	void EXPORT LinearMoveDone(void);
	void AngularMove(Vector vecDestAngle, float flSpeed);
	void EXPORT AngularMoveDone(void);
	BOOL IsLockedByMaster(void);

	static float AxisValue(int flags, const Vector &angles);
	static void AxisDir(entvars_t *pev);
	static float AxisDelta(int flags, const Vector &angle1, const Vector &angle2);

	string_t m_sMaster; // If this button has a master switch, this is the targetname.
						// A master switch must be of the multisource type. If all
						// of the switches in the multisource have been triggered, then
						// the button will be allowed to operate. Otherwise, it will be
						// deactivated.
};
#define SetMoveDone(a) m_pfnCallWhenMoveDone = static_cast<void (CBaseToggle::*)(void)>(a)

// people gib if their health is <= this at the time of death
#define GIB_HEALTH_VALUE -30

#define ROUTE_SIZE 8	  // how many waypoints a monster can store at one time
#define MAX_OLD_ENEMIES 4 // how many old enemies to remember

#define bits_CAP_DUCK (1 << 0)		 // crouch
#define bits_CAP_JUMP (1 << 1)		 // jump/leap
#define bits_CAP_STRAFE (1 << 2)	 // strafe ( walk/run sideways)
#define bits_CAP_SQUAD (1 << 3)		 // can form squads
#define bits_CAP_SWIM (1 << 4)		 // proficiently navigate in water
#define bits_CAP_CLIMB (1 << 5)		 // climb ladders/ropes
#define bits_CAP_USE (1 << 6)		 // open doors/push buttons/pull levers
#define bits_CAP_HEAR (1 << 7)		 // can hear forced sounds
#define bits_CAP_AUTO_DOORS (1 << 8) // can trigger auto doors
#define bits_CAP_OPEN_DOORS (1 << 9) // can open manual doors
#define bits_CAP_TURN_HEAD (1 << 10) // can turn head, always bone controller 0

#define bits_CAP_RANGE_ATTACK1 (1 << 11) // can do a range attack 1
#define bits_CAP_RANGE_ATTACK2 (1 << 12) // can do a range attack 2
#define bits_CAP_MELEE_ATTACK1 (1 << 13) // can do a melee attack 1
#define bits_CAP_MELEE_ATTACK2 (1 << 14) // can do a melee attack 2

#define bits_CAP_FLY (1 << 15) // can fly, move all around

#define bits_CAP_DOORS_GROUP (bits_CAP_USE | bits_CAP_AUTO_DOORS | bits_CAP_OPEN_DOORS)

// used by suit voice to indicate damage sustained and repaired type to player

// instant damage

#define DMG_GENERIC 0			 // generic damage was done
#define DMG_CRUSH (1 << 0)		 // crushed by falling or moving object
#define DMG_BULLET (1 << 1)		 // shot
#define DMG_SLASH (1 << 2)		 // cut, clawed, stabbed
#define DMG_BURN (1 << 3)		 // heat burned
#define DMG_FREEZE (1 << 4)		 // frozen
#define DMG_FALL (1 << 5)		 // fell too far
#define DMG_BLAST (1 << 6)		 // explosive blast damage
#define DMG_CLUB (1 << 7)		 // crowbar, punch, headbutt
#define DMG_SHOCK (1 << 8)		 // electric shock
#define DMG_SONIC (1 << 9)		 // sound pulse shockwave
#define DMG_ENERGYBEAM (1 << 10) // laser or other high energy beam
#define DMG_NEVERGIB (1 << 12)	 // with this bit OR'd in, no damage type will be able to gib victims upon death
#define DMG_ALWAYSGIB (1 << 13)	 // with this bit OR'd in, any damage type can be made to gib victims upon death.
#define DMG_DROWN (1 << 14)		 // Drowning
// time-based damage
#define DMG_TIMEBASED (~(0x3fff)) // mask for time-based damage

#define DMG_PARALYZE (1 << 15)	   // slows affected creature down
#define DMG_NERVEGAS (1 << 16)	   // nerve toxins, very bad
#define DMG_POISON (1 << 17)	   // blood poisioning
#define DMG_RADIATION (1 << 18)	   // radiation exposure
#define DMG_DROWNRECOVER (1 << 19) // drowning recovery
#define DMG_ACID (1 << 20)		   // toxic chemicals or acid burns
#define DMG_SLOWBURN (1 << 21)	   // in an oven
#define DMG_SLOWFREEZE (1 << 22)   // in a subzero freezer
#define DMG_MORTAR (1 << 23)	   // Hit by air raid (done to distinguish grenade from mortar)
//Master Sword --
#define DMG_NOKILL (1 << 24)	  // (Dogg) This damage cannot kill you (fists reflective damage...)
#define DMG_SIMPLEBBOX (1 << 25)  // (Dogg) Use a simple BBox for tracelines
#define DMG_DIRECT (1 << 26)	  // (Dogg) Ignore the traceline and just do damage to the target
#define DMG_REFLECTIVE (1 << 27)  // (Dogg) This damage can hurt you
#define DMG_AOE (1 << 28)		  // (Dogg) Area of Effect Damage
#define DMG_NONE (1 << 29)		  // (Dogg) Find a target, but don't damage it
#define DMG_NOSKIN (1 << 30)	  // (Dogg) This damage won't skin creatures (arrows, fireball, etc.)
#define DMG_MARTIALARTS (1 << 31) // (Dogg) Martial arts attack (punch, kick, jumpkick...)
//---------------
// these are the damage types that are allowed to gib corpses
#define DMG_GIB_CORPSE (DMG_CRUSH | DMG_FALL | DMG_BLAST | DMG_SONIC | DMG_CLUB)

// these are the damage types that have client hud art
#define DMG_SHOWNHUD (DMG_POISON | DMG_ACID | DMG_FREEZE | DMG_SLOWFREEZE | DMG_DROWN | DMG_BURN | DMG_SLOWBURN | DMG_NERVEGAS | DMG_RADIATION | DMG_SHOCK)

// NOTE: tweak these values based on gameplay feedback:

#define PARALYZE_DURATION 2 // number of 2 second intervals to take damage
#define PARALYZE_DAMAGE 1.0 // damage to take each 2 second interval

#define NERVEGAS_DURATION 2
#define NERVEGAS_DAMAGE 5.0

#define POISON_DURATION 5
#define POISON_DAMAGE 2.0

#define RADIATION_DURATION 2
#define RADIATION_DAMAGE 1.0

#define ACID_DURATION 2
#define ACID_DAMAGE 5.0

#define SLOWBURN_DURATION 2
#define SLOWBURN_DAMAGE 1.0

#define SLOWFREEZE_DURATION 2
#define SLOWFREEZE_DAMAGE 1.0

#define itbd_Paralyze 0
#define itbd_NerveGas 1
#define itbd_Poison 2
#define itbd_Radiation 3
#define itbd_DrownRecover 4
#define itbd_Acid 5
#define itbd_SlowBurn 6
#define itbd_SlowFreeze 7
#define CDMG_TIMEBASED 8

// when calling KILLED(), a value that governs gib behavior is expected to be
// one of these three values
#define GIB_NORMAL 0 // gib if entity was overkilled
#define GIB_NEVER 1	 // never gib, no matter how much death damage is done ( freezing, etc )
#define GIB_ALWAYS 2 // always gib ( Houndeye Shock, Barnacle Bite )

class CBaseMonster;
class CCineMonster;
class CSound;

#include "basemonster.h"

//Master Sword --------------
inline int FNullEnt(CBaseEntity *ent) { return (ent == NULL) || FNullEnt(ent->edict()); }

//---------------------------

char *ButtonSound(int sound); // get string of button sound number

//
// Generic Button
//
class CBaseButton : public CBaseToggle
{
public:
	void Spawn(void);
	virtual void Precache(void);
	void RotSpawn(void);
	virtual void KeyValue(KeyValueData *pkvd);

	void ButtonActivate();
	void SparkSoundCache(void);

	void EXPORT ButtonShot(void);
	void EXPORT ButtonTouch(CBaseEntity *pOther);
	void EXPORT ButtonSpark(void);
	void EXPORT TriggerAndWait(void);
	void EXPORT ButtonReturn(void);
	void EXPORT ButtonBackHome(void);
	void EXPORT ButtonUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	virtual int TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType);
	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);

	enum BUTTON_CODE
	{
		BUTTON_NOTHING,
		BUTTON_ACTIVATE,
		BUTTON_RETURN
	};
	BUTTON_CODE ButtonResponseToTouch(void);

	static TYPEDESCRIPTION m_SaveData[];
	// Buttons that don't take damage can be IMPULSE used
	virtual int ObjectCaps(void) { return (CBaseToggle::ObjectCaps() & ~FCAP_ACROSS_TRANSITION) | (pev->takedamage ? 0 : FCAP_IMPULSE_USE); }

	BOOL m_fStayPushed; // button stays pushed in until touched again?
	BOOL m_fRotating;	// a rotating button?  default is a sliding button.

	string_t m_strChangeTarget; // if this field is not null, this is an index into the engine string array.
								// when this button is touched, it's target entity's TARGET field will be set
								// to the button's ChangeTarget. This allows you to make a func_train switch paths, etc.

	locksound_t m_ls; // door lock sounds

	BYTE m_bLockedSound; // ordinals from entity selection
	BYTE m_bLockedSentence;
	BYTE m_bUnlockedSound;
	BYTE m_bUnlockedSentence;
	int m_sounds;
};

//
// Weapons
//

#define BAD_WEAPON 0x00007FFF

//
// Converts a entvars_t * to a class pointer
// It will allocate the class and entity if necessary
//
template <class T>
T *GetClassPtr(T *a)
{
	entvars_t *pev = (entvars_t *)a;

	// allocate entity if necessary
	if (pev == NULL)
		pev = VARS(CREATE_ENTITY());

	// get the private data
	a = (T *)GET_PRIVATE(ENT(pev));

	if (a == NULL)
	{
		// allocate private data
		a = new (pev) T;
		a->pev = pev;
	}
	return a;
}

/*
bit_PUSHBRUSH_DATA | bit_TOGGLE_DATA
bit_MONSTER_DATA
bit_DELAY_DATA
bit_TOGGLE_DATA | bit_DELAY_DATA | bit_MONSTER_DATA
bit_PLAYER_DATA | bit_MONSTER_DATA
bit_MONSTER_DATA | CYCLER_DATA
bit_LIGHT_DATA
path_corner_data
bit_MONSTER_DATA | wildcard_data
bit_MONSTER_DATA | bit_GROUP_DATA
boid_flock_data
boid_data
CYCLER_DATA
bit_ITEM_DATA
bit_ITEM_DATA | func_hud_data
bit_TOGGLE_DATA | bit_ITEM_DATA
EOFFSET
env_sound_data
env_sound_data
push_trigger_data
*/

#define TRACER_FREQ 4 // Tracers fire every 4 bullets

typedef struct _SelAmmo
{
	BYTE Ammo1Type;
	BYTE Ammo1;
	BYTE Ammo2Type;
	BYTE Ammo2;
} SelAmmo;

#endif