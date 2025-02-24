//========= Mapbase - https://github.com/mapbase-source/source-sdk-2013 =================
//
// Purpose: VScript-driven custom weapon class.
//
// $NoKeywords: $
//=============================================================================

#ifndef WEAPON_CUSTOM_SCRIPTED_H
#define WEAPON_CUSTOM_SCRIPTED_H
#ifdef _WIN32
#pragma once
#endif

// The base class of the scripted weapon is game-specific.
#if defined ( HL2MP )
	#include "weapon_hl2mpbasehlmpcombatweapon.h"
	#define SCRIPTED_WEAPON_DERIVED_FROM CBaseHL2MPCombatWeapon
#elif defined( HL2_DLL ) || defined( HL2_CLIENT_DLL )
	#include "basehlcombatweapon_shared.h"
	#define SCRIPTED_WEAPON_DERIVED_FROM CBaseHLCombatWeapon
#else
	#include "basecombatweapon_shared.h"
	#define SCRIPTED_WEAPON_DERIVED_FROM CBaseCombatWeapon
#endif // HL2_DLL || HL2_CLIENT_DLL || HL2MP

#ifdef CLIENT_DLL
#include "vscript_client.h"
#endif

#ifdef CLIENT_DLL
	#define CWeaponCustomScripted C_WeaponCustomScripted
#endif

#define DECLARE_CACHED_HOOK(name) static ScriptHook_t g_Hook_##name; \
	HSCRIPT m_Func_##name;

class CWeaponCustomScripted : public SCRIPTED_WEAPON_DERIVED_FROM
{
public:
	DECLARE_CLASS( CWeaponCustomScripted, SCRIPTED_WEAPON_DERIVED_FROM );
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CWeaponCustomScripted();

	bool	RunWeaponHook( ScriptHook_t &hook, HSCRIPT &cached, ScriptVariant_t *retVal = NULL, ScriptVariant_t *pArgs = NULL );

	bool	KeyValue( const char *szKeyName, const char *szValue );
	bool	GetKeyValue( const char *szKeyName, char *szValue, int iMaxLen );

	// Base script has a function for this
	//void	Precache( void );

	virtual void	Spawn( void );

	virtual bool	IsPredicted( void ) const { return m_iszClientScripts[0] != '\0'; }

	const char*		GetWeaponScriptName() { return m_iszWeaponScriptName[0] != '\0' ? m_iszWeaponScriptName : BaseClass::GetWeaponScriptName(); }

	// Weapon selection
	virtual bool	HasAnyAmmo( void );						// Returns true is weapon has ammo
	virtual bool	HasPrimaryAmmo( void );					// Returns true is weapon has ammo
	virtual bool	HasSecondaryAmmo( void );				// Returns true is weapon has ammo

	virtual bool	CanHolster( void );						// returns true if the weapon can be holstered
	virtual bool	CanDeploy( void );						// return true if the weapon's allowed to deploy
	virtual bool	Deploy( void );							// returns true is deploy was successful
	virtual bool	Holster( CBaseCombatWeapon *pSwitchingTo = NULL );

	// Weapon behaviour
	virtual void	ItemPreFrame( void );					// called each frame by the player PreThink
	virtual void	ItemPostFrame( void );					// called each frame by the player PostThink
	virtual void	ItemBusyFrame( void );					// called each frame by the player PostThink, if the player's not ready to attack yet
	virtual void	ItemHolsterFrame( void );				// called each frame by the player PreThink, if the weapon is holstered
	virtual void	WeaponIdle( void );						// called when no buttons pressed
	virtual void	HandleFireOnEmpty();					// Called when they have the attack button down

	// Reloading
	virtual void	CheckReload( void );
	virtual void	FinishReload( void );
	virtual void	AbortReload( void );
	virtual bool	Reload( void );
	virtual void	Reload_NPC( bool bPlaySound = true );

	// Weapon firing
	virtual void	PrimaryAttack( void );				// do "+ATTACK"
	virtual void	SecondaryAttack( void );			// do "+ATTACK2"

	// Firing animations
	virtual Activity	GetPrimaryAttackActivity( void );
	virtual Activity	GetSecondaryAttackActivity( void );
	virtual Activity	GetDrawActivity( void );
	virtual float		GetDefaultAnimSpeed( void );

	// Bullet launch information
	virtual const Vector&	GetBulletSpread( void );
	virtual Vector			GetBulletSpread( WeaponProficiency_t proficiency );
	virtual float			GetFireRate( void );
	virtual int				GetMinBurst();
	virtual int				GetMaxBurst();
	virtual float			GetMinRestTime();
	virtual float			GetMaxRestTime();

	virtual void	AddViewKick( void );

#ifndef CLIENT_DLL
	virtual bool	WeaponLOSCondition( const Vector &ownerPos, const Vector &targetPos, bool bSetConditions );
	virtual int		WeaponRangeAttack1Condition( float flDot, float flDist );
	virtual int		WeaponRangeAttack2Condition( float flDot, float flDist );
	virtual int		WeaponMeleeAttack1Condition( float flDot, float flDist );
	virtual int		WeaponMeleeAttack2Condition( float flDot, float flDist );
#endif

	ALLOW_SCRIPT_ACCESS();
private:

	// Weapon selection
	DECLARE_CACHED_HOOK( HasAnyAmmo );
	DECLARE_CACHED_HOOK( HasPrimaryAmmo );
	DECLARE_CACHED_HOOK( HasSecondaryAmmo );

	DECLARE_CACHED_HOOK( CanHolster );
	DECLARE_CACHED_HOOK( CanDeploy );
	DECLARE_CACHED_HOOK( Deploy );
	DECLARE_CACHED_HOOK( Holster );

	// Weapon behaviour
	DECLARE_CACHED_HOOK( ItemPreFrame );
	DECLARE_CACHED_HOOK( ItemPostFrame );
	DECLARE_CACHED_HOOK( ItemBusyFrame );
	DECLARE_CACHED_HOOK( ItemHolsterFrame );
	DECLARE_CACHED_HOOK( WeaponIdle );
	DECLARE_CACHED_HOOK( HandleFireOnEmpty );

	// Reloading
	DECLARE_CACHED_HOOK( CheckReload );
	DECLARE_CACHED_HOOK( FinishReload );
	DECLARE_CACHED_HOOK( AbortReload );
	DECLARE_CACHED_HOOK( Reload );
	DECLARE_CACHED_HOOK( Reload_NPC );

	// Weapon firing
	DECLARE_CACHED_HOOK( PrimaryAttack );
	DECLARE_CACHED_HOOK( SecondaryAttack );

	// Firing animations
	DECLARE_CACHED_HOOK( GetPrimaryAttackActivity );
	DECLARE_CACHED_HOOK( GetSecondaryAttackActivity );
	DECLARE_CACHED_HOOK( GetDrawActivity );
	DECLARE_CACHED_HOOK( GetDefaultAnimSpeed );

	// Bullet launch information
	DECLARE_CACHED_HOOK( GetBulletSpread );
	DECLARE_CACHED_HOOK( GetBulletSpreadForProficiency );
	DECLARE_CACHED_HOOK( GetFireRate );
	DECLARE_CACHED_HOOK( GetMinBurst );
	DECLARE_CACHED_HOOK( GetMaxBurst );
	DECLARE_CACHED_HOOK( GetMinRestTime );
	DECLARE_CACHED_HOOK( GetMaxRestTime );

	DECLARE_CACHED_HOOK( AddViewKick );

#ifndef CLIENT_DLL
	DECLARE_CACHED_HOOK( WeaponLOSCondition );
	DECLARE_CACHED_HOOK( WeaponRangeAttack1Condition );
	DECLARE_CACHED_HOOK( WeaponRangeAttack2Condition );
	DECLARE_CACHED_HOOK( WeaponMeleeAttack1Condition );
	DECLARE_CACHED_HOOK( WeaponMeleeAttack2Condition );
#endif

	DECLARE_CACHED_HOOK( ActivityList );
	//DECLARE_CACHED_HOOK( ActivityListCount );

private:

	CNetworkString( m_iszClientScripts, 256 );
	CNetworkString( m_iszWeaponScriptName, 256 );

protected:
	
	DECLARE_ACTTABLE();
	DECLARE_DATADESC();
	DECLARE_ENT_SCRIPTDESC();
};

/*
class CWeaponCustomScripted1 : public CWeaponCustomScripted
{
	DECLARE_PREDICTABLE();
};
class CWeaponCustomScripted2 : public CWeaponCustomScripted
{
	DECLARE_PREDICTABLE();
};
class CWeaponCustomScripted3 : public CWeaponCustomScripted
{
	DECLARE_PREDICTABLE();
};
class CWeaponCustomScripted4 : public CWeaponCustomScripted
{
	DECLARE_PREDICTABLE();
};
*/

#endif
