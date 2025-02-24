//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Grigori's personal shotgun (npc_monk)
//
//=============================================================================//
#include "cbase.h"
#include "npcevent.h"
#include "weapon_hl2mpbasehlmpcombatweapon.h"
#include "gamerules.h"		// For g_pGameRules
#include "in_buttons.h"
#include "vstdlib/random.h"
#include "gamestats.h"

#ifdef CLIENT_DLL
	#include "c_hl2mp_player.h"
#else
	#include "basecombatcharacter.h"
	#include "ai_basenpc.h"
	#include "hl2mp_player.h"
	#include "soundent.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#ifdef CLIENT_DLL
	#define CWeaponAnnabelle C_WeaponAnnabelle
#endif

extern ConVar sk_auto_reload_time;

#ifdef MAPBASE
extern acttable_t *GetShotgunActtable();
extern int GetShotgunActtableCount();
#endif

class CWeaponAnnabelle : public CBaseHL2MPCombatWeapon
{
public:
	DECLARE_CLASS( CWeaponAnnabelle, CBaseHL2MPCombatWeapon );

	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

private:
	CNetworkVar( bool,	m_bNeedPump );		// When emptied completely
	CNetworkVar( bool,	m_bDelayedFire1 );	// Fire primary when finished reloading
	CNetworkVar( bool,	m_bDelayedFire2 );	// Fire secondary when finished reloading

public:

#ifdef GAME_DLL
	int CapabilitiesGet( void ) { return bits_CAP_WEAPON_RANGE_ATTACK1; }
#endif // GAME_DLL

	virtual const Vector& GetBulletSpread( void )
	{
		static Vector cone = vec3_origin;

#ifdef GAME_DLL
#ifdef MAPBASE
		if ( GetOwner() && GetOwner()->OverridingWeaponProficiency() )
		{
			// If the owner's weapon proficiency is being overridden, return a more realistic spread
			static Vector cone2 = VECTOR_CONE_6DEGREES;
			return cone2;
		}
#endif
#endif // GAME_DLL

		return cone;
	}

	virtual int	GetMinBurst() { return 1; }
	virtual int	GetMaxBurst() { return 3; }

	void ItemHolsterFrame( void );
	bool StartReload( void );
	bool Reload( void );
	void FillClip( void );
	void FinishReload( void );
	void CheckHolsterReload( void );
	void Pump( void );
	void DryFire( void );
	virtual float GetFireRate( void ) { return 1.5; };
	virtual float GetMinRestTime() { return 1.0; }
	virtual float GetMaxRestTime() { return 1.5; }

#ifdef GAME_DLL
	void Operator_HandleAnimEvent( animevent_t *pEvent, CBaseCombatCharacter *pOperator );
#endif // GAME_DLL

#ifdef MAPBASE
	virtual acttable_t		*GetBackupActivityList() { return GetShotgunActtable(); }
	virtual int				GetBackupActivityListCount() { return GetShotgunActtableCount(); }
#endif

	DECLARE_ACTTABLE();

	CWeaponAnnabelle(void);

private:

	CWeaponAnnabelle( const CWeaponAnnabelle & );
};

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponAnnabelle, DT_WeaponAnnabelle )

BEGIN_NETWORK_TABLE( CWeaponAnnabelle, DT_WeaponAnnabelle )
#ifdef CLIENT_DLL
	RecvPropBool( RECVINFO( m_bNeedPump ) ),
	RecvPropBool( RECVINFO( m_bDelayedFire1 ) ),
	RecvPropBool( RECVINFO( m_bDelayedFire2 ) ),
#else
	SendPropBool( SENDINFO( m_bNeedPump ) ),
	SendPropBool( SENDINFO( m_bDelayedFire1 ) ),
	SendPropBool( SENDINFO( m_bDelayedFire2 ) ),
#endif
END_NETWORK_TABLE()

#ifdef CLIENT_DLL
BEGIN_PREDICTION_DATA( CWeaponAnnabelle )
	DEFINE_PRED_FIELD( m_bNeedPump, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_bDelayedFire1, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_bDelayedFire2, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE ),
END_PREDICTION_DATA()
#endif

LINK_ENTITY_TO_CLASS( weapon_annabelle, CWeaponAnnabelle );
PRECACHE_WEAPON_REGISTER( weapon_annabelle );

//-----------------------------------------------------------------------------
// Maps base activities to weapons-specific ones so our characters do the right things.
//-----------------------------------------------------------------------------
acttable_t	CWeaponAnnabelle::m_acttable[] = 
{
#if defined( EXPANDED_HL2_WEAPON_ACTIVITIES ) && AR2_ACTIVITY_FIX == 1
	{ ACT_IDLE,						ACT_IDLE_AR2,						false },
	{ ACT_IDLE_ANGRY,				ACT_IDLE_ANGRY_AR2,					true },
	{ ACT_RANGE_AIM_LOW,			ACT_RANGE_AIM_AR2_LOW,				false },
	{ ACT_RANGE_ATTACK1,			ACT_RANGE_ATTACK_ANNABELLE,			true },
	{ ACT_RANGE_ATTACK1_LOW,		ACT_RANGE_ATTACK_ANNABELLE_LOW,		true },
	{ ACT_RELOAD,					ACT_RELOAD_ANNABELLE,				true },
	{ ACT_WALK,						ACT_WALK_AR2,						true },
	{ ACT_WALK_AIM,					ACT_WALK_AIM_AR2,					true },
	{ ACT_WALK_CROUCH,				ACT_WALK_CROUCH_RIFLE,				false },
	{ ACT_WALK_CROUCH_AIM,			ACT_WALK_CROUCH_AIM_RIFLE,			false },
	{ ACT_RUN,						ACT_RUN_AR2,						true },
	{ ACT_RUN_AIM,					ACT_RUN_AIM_AR2,					true },
	{ ACT_RUN_CROUCH,				ACT_RUN_CROUCH_RIFLE,				false },
	{ ACT_RUN_CROUCH_AIM,			ACT_RUN_CROUCH_AIM_RIFLE,			false },
	{ ACT_GESTURE_RANGE_ATTACK1,	ACT_GESTURE_RANGE_ATTACK_ANNABELLE,	true },
	{ ACT_RELOAD_LOW,				ACT_RELOAD_ANNABELLE_LOW,			false },
	{ ACT_GESTURE_RELOAD,			ACT_GESTURE_RELOAD_ANNABELLE,		false },

	{ ACT_ARM,						ACT_ARM_RIFLE,					true },
	{ ACT_DISARM,					ACT_DISARM_RIFLE,				true },
#else
#ifdef MAPBASE
	{ ACT_IDLE,						ACT_IDLE_SMG1,						false },
#endif
	{ ACT_IDLE_ANGRY,				ACT_IDLE_ANGRY_SMG1,				true },
	{ ACT_RANGE_ATTACK1,			ACT_RANGE_ATTACK_SHOTGUN,			true },
	{ ACT_RELOAD,					ACT_RELOAD_SMG1,					true },
	{ ACT_WALK,						ACT_WALK_RIFLE,						true },
	{ ACT_WALK_AIM,					ACT_WALK_AIM_RIFLE,					true },
	{ ACT_WALK_CROUCH,				ACT_WALK_CROUCH_RIFLE,				false },
	{ ACT_WALK_CROUCH_AIM,			ACT_WALK_CROUCH_AIM_RIFLE,			false },
	{ ACT_RUN,						ACT_RUN_RIFLE,						true },
	{ ACT_RUN_AIM,					ACT_RUN_AIM_RIFLE,					true },
	{ ACT_RUN_CROUCH,				ACT_RUN_CROUCH_RIFLE,				false },
	{ ACT_RUN_CROUCH_AIM,			ACT_RUN_CROUCH_AIM_RIFLE,			false },
	{ ACT_GESTURE_RANGE_ATTACK1,	ACT_GESTURE_RANGE_ATTACK_SHOTGUN,	true },
	{ ACT_RELOAD_LOW,				ACT_RELOAD_SMG1_LOW,				false },
	{ ACT_GESTURE_RELOAD,			ACT_GESTURE_RELOAD_SMG1,			false },
#endif

	// HL2:DM activities (for third-person animations in SP)
	{ ACT_MP_STAND_IDLE,				ACT_HL2MP_IDLE_SHOTGUN,					false },
	{ ACT_MP_CROUCH_IDLE,				ACT_HL2MP_IDLE_CROUCH_SHOTGUN,			false },

	{ ACT_MP_RUN,						ACT_HL2MP_RUN_SHOTGUN,					false },
#if EXPANDED_HL2DM_ACTIVITIES
	{ ACT_MP_WALK,						ACT_HL2MP_WALK_SHOTGUN,						false },
#endif
	{ ACT_MP_CROUCHWALK,				ACT_HL2MP_WALK_CROUCH_SHOTGUN,				false },

	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK_SHOTGUN,		false },
#if EXPANDED_HL2DM_ACTIVITIES
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK2_SHOTGUN,	false },
#else
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK_SHOTGUN,		false },
#endif

	{ ACT_MP_RELOAD_STAND,				ACT_HL2MP_GESTURE_RELOAD_SHOTGUN,		false },
	{ ACT_MP_RELOAD_CROUCH,				ACT_HL2MP_GESTURE_RELOAD_SHOTGUN,		false },

	{ ACT_MP_JUMP,						ACT_HL2MP_JUMP_SHOTGUN,					false },
};

IMPLEMENT_ACTTABLE( CWeaponAnnabelle );

#ifdef GAME_DLL
//-----------------------------------------------------------------------------
// Purpose:
// Input  :
// Output :
//-----------------------------------------------------------------------------
void CWeaponAnnabelle::Operator_HandleAnimEvent( animevent_t *pEvent, CBaseCombatCharacter *pOperator )
{
	switch( pEvent->event )
	{
		case EVENT_WEAPON_SHOTGUN_FIRE:
		{
			Vector vecShootOrigin, vecShootDir;
			vecShootOrigin = pOperator->Weapon_ShootPosition();
			CAI_BaseNPC *npc = pOperator->MyNPCPointer();
			ASSERT( npc != NULL );
			WeaponSound( SINGLE_NPC );
			pOperator->DoMuzzleFlash();
			m_iClip1 = m_iClip1 - 1;

			vecShootDir = npc->GetActualShootTrajectory( vecShootOrigin );
			pOperator->FireBullets( 1, vecShootOrigin, vecShootDir, VECTOR_CONE_PRECALCULATED, MAX_TRACE_LENGTH, m_iPrimaryAmmoType, 0 );
		}
		break;

		default:
			CBaseCombatWeapon::Operator_HandleAnimEvent( pEvent, pOperator );
			break;
	}
}
#endif // GAME_DLL

//-----------------------------------------------------------------------------
// Purpose: Override so only reload one shell at a time
// Input  :
// Output :
//-----------------------------------------------------------------------------
bool CWeaponAnnabelle::StartReload( void )
{
	CBaseCombatCharacter *pOwner  = GetOwner();
	
	if ( pOwner == NULL )
		return false;

	if (pOwner->GetAmmoCount(m_iPrimaryAmmoType) <= 0)
		return false;

	if ( m_iClip1 >= GetMaxClip1() )
		return false;

	// If shotgun totally emptied then a pump animation is needed
	if ( m_iClip1 <= 0 )
	{
		m_bNeedPump = true;
	}

	int j = MIN(1, pOwner->GetAmmoCount(m_iPrimaryAmmoType));

	if (j <= 0)
		return false;

	SendWeaponAnim( ACT_SHOTGUN_RELOAD_START );

	// Make shotgun shell visible
	SetBodygroup(1,0);

	pOwner->m_flNextAttack = gpGlobals->curtime;
	m_flNextPrimaryAttack = gpGlobals->curtime + SequenceDuration();

#ifdef MAPBASE
	if ( pOwner->IsPlayer() )
	{
		//Tony; BUG BUG BUG!!! shotgun does one shell at a time!!! -- player model only has a single reload!!! so I'm just going to dispatch the singular for now.
		ToHL2MPPlayer( pOwner )->DoAnimationEvent( PLAYERANIMEVENT_RELOAD );
	}
#endif

	m_bInReload = true;
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Override so only reload one shell at a time
// Input  :
// Output :
//-----------------------------------------------------------------------------
bool CWeaponAnnabelle::Reload( void )
{
	// Check that StartReload was called first
	if (!m_bInReload)
	{
		Warning("ERROR: Shotgun Reload called incorrectly!\n");
	}

	CBaseCombatCharacter *pOwner  = GetOwner();
	
	if ( pOwner == NULL )
		return false;

	if (pOwner->GetAmmoCount(m_iPrimaryAmmoType) <= 0)
		return false;

	if (m_iClip1 >= GetMaxClip1())
		return false;

	int j = MIN(1, pOwner->GetAmmoCount(m_iPrimaryAmmoType));

	if (j <= 0)
		return false;

	FillClip();
	// Play reload on different channel as otherwise steals channel away from fire sound
	WeaponSound(RELOAD);
	SendWeaponAnim( ACT_VM_RELOAD );

	pOwner->m_flNextAttack = gpGlobals->curtime;
	m_flNextPrimaryAttack = gpGlobals->curtime + SequenceDuration();

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Play finish reload anim and fill clip
// Input  :
// Output :
//-----------------------------------------------------------------------------
void CWeaponAnnabelle::FinishReload( void )
{
	// Make shotgun shell invisible
	SetBodygroup(1,1);

	CBaseCombatCharacter *pOwner  = GetOwner();
	
	if ( pOwner == NULL )
		return;

	m_bInReload = false;

	// Finish reload animation
	SendWeaponAnim( ACT_SHOTGUN_RELOAD_FINISH );

	pOwner->m_flNextAttack = gpGlobals->curtime;
	m_flNextPrimaryAttack = gpGlobals->curtime + SequenceDuration();
}

//-----------------------------------------------------------------------------
// Purpose: Play finish reload anim and fill clip
// Input  :
// Output :
//-----------------------------------------------------------------------------
void CWeaponAnnabelle::FillClip( void )
{
	CBaseCombatCharacter *pOwner  = GetOwner();
	
	if ( pOwner == NULL )
		return;

	// Add them to the clip
	if ( pOwner->GetAmmoCount( m_iPrimaryAmmoType ) > 0 )
	{
		if ( Clip1() < GetMaxClip1() )
		{
			m_iClip1++;
			pOwner->RemoveAmmo( 1, m_iPrimaryAmmoType );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Play weapon pump anim
// Input  :
// Output :
//-----------------------------------------------------------------------------
void CWeaponAnnabelle::Pump( void )
{
	CBaseCombatCharacter *pOwner  = GetOwner();

	if ( pOwner == NULL )
		return;
	
	m_bNeedPump = false;
	
	WeaponSound( SPECIAL1 );

	// Finish reload animation
	SendWeaponAnim( ACT_SHOTGUN_PUMP );

	pOwner->m_flNextAttack	= gpGlobals->curtime + SequenceDuration();
	m_flNextPrimaryAttack	= gpGlobals->curtime + SequenceDuration();
}

//-----------------------------------------------------------------------------
// Purpose: 
//
//
//-----------------------------------------------------------------------------
void CWeaponAnnabelle::DryFire( void )
{
	WeaponSound(EMPTY);
	SendWeaponAnim( ACT_VM_DRYFIRE );
	
	m_flNextPrimaryAttack = gpGlobals->curtime + SequenceDuration();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponAnnabelle::ItemHolsterFrame( void )
{
	// Must be player held
	if ( GetOwner() && GetOwner()->IsPlayer() == false )
		return;

	// We can't be active
	if ( GetOwner()->GetActiveWeapon() == this )
		return;

	// If it's been longer than three seconds, reload
	if ( ( gpGlobals->curtime - m_flHolsterTime ) > sk_auto_reload_time.GetFloat() )
	{
		// Reset the timer
		m_flHolsterTime = gpGlobals->curtime;
	
		if ( GetOwner() == NULL )
			return;

		if ( m_iClip1 == GetMaxClip1() )
			return;

		// Just load the clip with no animations
		int ammoFill = MIN( (GetMaxClip1() - m_iClip1), GetOwner()->GetAmmoCount( GetPrimaryAmmoType() ) );
		
		GetOwner()->RemoveAmmo( ammoFill, GetPrimaryAmmoType() );
		m_iClip1 += ammoFill;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CWeaponAnnabelle::CWeaponAnnabelle( void )
{
	m_bReloadsSingly = true;

	m_bNeedPump		= false;
	m_bDelayedFire1 = false;
	m_bDelayedFire2 = false;

	m_fMinRange1		= 0.0;
	m_fMaxRange1		= 500;
	m_fMinRange2		= 0.0;
	m_fMaxRange2		= 200;
}
