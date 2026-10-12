#pragma once

#include "space_restrictor.h"
#include "../xrEngine/feel_touch.h"
#include "script_export_space.h"

class CActor;
class CLAItem;
class CParticlesObject;
class CZoneEffector;

#define SMALL_OBJECT_RADIUS 0.6f

//Ð¸Ð½Ñ„Ð¾Ñ€Ð¼Ð°Ñ†Ð¸Ñ Ð¾ Ð¾Ð±ÑŠÐµÐºÑ‚Ðµ, Ð½Ð°Ñ…Ð¾Ð´ÑÑ‰ÐµÐ¼ÑÑ Ð² Ð·Ð¾Ð½Ðµ
struct SZoneObjectInfo
{
	SZoneObjectInfo(): object(nullptr), zone_ignore(false), dw_time_in_zone(0), f_time_affected(Device.fTimeGlobal),
	                   small_object(false), nonalive_object(false)
	{
	}

	CGameObject* object;
	bool small_object;
	bool nonalive_object;
	//Ð¸Ð³Ð½Ð¾Ñ€Ð¸Ñ€Ð¾Ð²Ð°Ð½Ð¸Ðµ Ð¾Ð±ÑŠÐµÐºÑ‚Ð° Ð² Ð·Ð¾Ð½Ðµ
	bool zone_ignore;
	//Ð¿Ñ€Ð¸ÑÐ¾ÐµÐ´Ð¸Ð½ÐµÐ½Ð½Ñ‹Ðµ Ð¿Ð°Ñ€Ñ‚Ð¸ÐºÐ»Ñ‹
	xr_vector<CParticlesObject*> particles_vector;
	//Ð²Ñ€ÐµÐ¼Ñ Ð¿Ñ€Ð¸Ð±Ñ‹Ð²Ð°Ð½Ð¸Ñ Ð² Ð·Ð¾Ð½Ðµ
	u32 dw_time_in_zone;
	float f_time_affected;

	bool operator ==(const CGameObject* O) const { return object == O; }
};


class CCustomZone : public CSpaceRestrictor,
                    public Feel::Touch
{
private:
	typedef CSpaceRestrictor inherited;

public:
	CZoneEffector* m_actor_effector;

public:

	CCustomZone();
	virtual ~CCustomZone();

	virtual BOOL net_Spawn(CSE_Abstract* DC);
	virtual void net_Import(NET_Packet& P);
	virtual void net_Export(NET_Packet& P);
	virtual void Load(LPCSTR section);
	virtual void net_Destroy();

	virtual void save(NET_Packet& output_packet);
	virtual void load(IReader& input_packet);

	virtual void UpdateCL();
	virtual void UpdateWorkload(u32 dt);
	virtual void shedule_Update(u32 dt);
	virtual void enter_Zone(SZoneObjectInfo& io);
	virtual void exit_Zone(SZoneObjectInfo& io);
	virtual void feel_touch_new(CObject* O);
	virtual void feel_touch_delete(CObject* O);
	virtual bool feel_touch_contact(CObject* O);
	virtual bool feel_touch_on_contact(CObject* O);

	float effective_radius(float nearest_shape_radius);
	virtual void net_Relcase(CObject* O);
	virtual void OnEvent(NET_Packet& P, u16 type);

	float GetMaxPower() { return m_fMaxPower; }
	void SetMaxPower(float p) { m_fMaxPower = p; }

	float GetEffectiveRadius() { return m_fEffectiveRadius; }
	void SetEffectiveRadius(float p) { m_fEffectiveRadius = p; }

	//Ð²Ñ‹Ñ‡Ð¸ÑÐ»ÐµÐ½Ð¸Ðµ ÑÐ¸Ð»Ñ‹ Ñ…Ð¸Ñ‚Ð° Ð² Ð·Ð°Ð²Ð¸ÑÐ¸Ð¼Ð¾ÑÑ‚Ð¸ Ð¾Ñ‚ Ñ€Ð°ÑÑÑ‚Ð¾ÑÐ½Ð¸Ñ Ð´Ð¾ Ñ†ÐµÐ½Ñ‚Ñ€Ð° Ð·Ð¾Ð½Ñ‹
	//Ð¾Ñ‚Ð½Ð¾ÑÐ¸Ñ‚ÐµÐ»ÑŒÐ½Ñ‹Ð¹ Ñ€Ð°Ð·Ð¼ÐµÑ€ ÑÐ¸Ð»Ñ‹ (Ð¾Ñ‚ 0 Ð´Ð¾ 1)
	float RelativePower(float dist, float nearest_shape_radius);
	//Ð°Ð±ÑÐ¾Ð»ÑŽÑ‚Ð½Ñ‹Ð¹ Ñ€Ð°Ð·Ð¼ÐµÑ€
	float Power(float dist, float nearest_shape_radius);

	virtual CCustomZone* cast_custom_zone() { return this; }

	//Ñ€Ð°Ð·Ð»Ð¸Ñ‡Ð½Ñ‹Ðµ ÑÐ¾ÑÑ‚Ð¾ÑÐ½Ð¸Ñ Ð² ÐºÐ¾Ñ‚Ð¾Ñ€Ñ‹Ñ… Ð¼Ð¾Ð¶ÐµÑ‚ Ð½Ð°Ñ…Ð¾Ð´Ð¸Ñ‚ÑŒÑÑ Ð·Ð¾Ð½Ð°
	typedef enum
	{
		eZoneStateIdle = 0,
		//ÑÐ¾ÑÑ‚Ð¾ÑÐ½Ð¸Ðµ Ð·Ð¾Ð½Ñ‹, ÐºÐ¾Ð³Ð´Ð° Ð²Ð½ÑƒÑ‚Ñ€Ð¸ Ð½ÐµÐµ Ð½ÐµÑ‚ Ð°ÐºÑ‚Ð¸Ð²Ð½Ñ‹Ñ… Ð¾Ð±ÑŠÐµÐºÑ‚Ð¾Ð²
		eZoneStateAwaking,
		//Ð¿Ñ€Ð¾Ð±ÑƒÐ¶Ð´ÐµÐ½Ð¸Ðµ Ð·Ð¾Ð½Ñ‹ (Ð¾Ð±ÑŠÐµÐºÑ‚ Ð¿Ð¾Ð¿Ð°Ð» Ð² Ð·Ð¾Ð½Ñƒ)
		eZoneStateBlowout,
		//Ð²Ñ‹Ð±Ñ€Ð¾Ñ
		eZoneStateAccumulate,
		//Ð½Ð°ÐºÐ°Ð¿Ð»Ð¸Ð²Ð°Ð½Ð¸Ðµ ÑÐ½ÐµÑ€Ð³Ð¸Ð¸, Ð¿Ð¾ÑÐ»Ðµ Ð²Ñ‹Ð±Ñ€Ð¾ÑÐ°
		eZoneStateDisabled,
		eZoneStateMax
	} EZoneState;

	IC ALife::EHitType GetHitType() { return m_eHitTypeBlowout; }

protected:
	enum EZoneFlags
	{
		eIgnoreNonAlive =(1 << 0),
		eIgnoreSmall =(1 << 1),
		eIgnoreArtefact =(1 << 2),
		eZoneIsActive =(1 << 3),
		eBlowoutWind =(1 << 4),
		eBlowoutLight =(1 << 5),
		eIdleLight =(1 << 6),
		eBlowoutWindActive =(1 << 7),
		eUseOnOffTime =(1 << 8),
		eIdleLightVolumetric =(1 << 9),
		eIdleLightShadow =(1 << 10),
		eAlwaysFastmode =(1 << 11),
		eFastMode =(1 << 12),
		eIdleObjectParticlesDontStop=(1 << 13),
		eAffectPickDOF =(1 << 14),
		eIdleLightR1 =(1 << 15),
		eBoltEntranceParticles =(1 << 16),
		eUseSecondaryHit =(1 << 17),
		eBlowoutDisableIdle =(1 << 18),
	};

	u32 m_owner_id;
	u32 m_ttl;
	Flags32 m_zone_flags;

	//Ð¼Ð°ÐºÑÐ¸Ð¼Ð°Ð»ÑŒÐ½Ð°Ñ ÑÐ¸Ð»Ð° Ð·Ð°Ñ€ÑÐ´Ð° Ð·Ð¾Ð½Ñ‹
	float m_fMaxPower;
	//ÑÐ¸Ð»Ð° Ð¿Ð¾ÑÑ‚Ð¾ÑÐ½Ð½Ð¾Ð³Ð¾ Ð½ÐµÐ±Ð¾Ð»ÑŒÑˆÐ¾Ð³Ð¾ Ð´ÐµÐ¼ÐµÐ´Ð¶Ð° (Ð´Ð»Ñ Ð¾Ð³Ð½ÐµÐ½Ð½Ñ‹Ñ… Ð¸ ÑÐ´Ð¾Ð²Ð¸Ñ‚Ñ‹Ñ… Ð¼Ð¸Ð½)
	float m_fSecondaryHitPower;

	//Ð»Ð¸Ð½ÐµÐ¹Ð½Ñ‹Ð¹ ÐºÐ¾ÑÑ„Ñ„Ð¸Ñ†Ð¸ÐµÐ½Ñ‚ Ð·Ð°Ñ‚ÑƒÑ…Ð°Ð½Ð¸Ñ Ð² Ð·Ð°Ð²Ð¸ÑÐ¸Ð¼Ð¾ÑÑ‚Ð¸ Ð¾Ñ‚ Ñ€Ð°ÑÑÑ‚Ð¾ÑÐ½Ð¸Ñ
	float m_fAttenuation;
	//Ð¿Ñ€Ð¾Ñ†ÐµÐ½Ñ‚ ÑƒÐ´Ð°Ñ€Ð° Ð·Ð¾Ð½Ñ‹, ÐºÐ¾Ñ‚Ð¾Ñ€Ñ‹Ð¹ Ð¿Ð¾Ð¹Ð´ÐµÑ‚ Ð½Ð° Ñ„Ð¸Ð·Ð¸Ñ‡ÐµÑÐºÐ¸Ð¹ Ð¸Ð¼Ð¿ÑƒÐ»ÑŒÑ	
	float m_fHitImpulseScale;
	//Ñ€Ð°Ð·Ð¼ÐµÑ€ Ñ€Ð°Ð´Ð¸ÑƒÑÐ° Ð² Ð¿Ñ€Ð¾Ñ†ÐµÐ½Ñ‚Ð°Ñ… Ð¾Ñ‚ Ð¾Ñ€Ð¸Ð³Ð¸Ð½Ð°Ð»ÑŒÐ½Ð¾Ð³Ð¾, 
	//Ð³Ð´Ðµ Ð´ÐµÐ¹ÑÑ‚Ð²ÑƒÐµÑ‚ Ð·Ð¾Ð½Ð°
	float m_fEffectiveRadius;

	//Ñ‚Ð¸Ð¿ Ð½Ð°Ð½Ð¾ÑÐ¸Ð¼Ð¾Ð³Ð¾ Ñ…Ð¸Ñ‚Ð°
	ALife::EHitType m_eHitTypeBlowout;
	EZoneState m_eZoneState;

	//Ñ‚ÐµÐºÑƒÑ‰ÐµÐµ Ð²Ñ€ÐµÐ¼Ñ Ð¿Ñ€ÐµÐ±Ñ‹Ð²Ð°Ð½Ð¸Ñ Ð·Ð¾Ð½Ñ‹ Ð² Ð¾Ð¿Ñ€ÐµÐ´ÐµÐ»ÐµÐ½Ð½Ð¾Ð¼ ÑÐ¾ÑÑ‚Ð¾ÑÐ½Ð¸Ð¸ 
	int m_iStateTime;
	int m_iPreviousStateTime;

	u32 m_TimeToDisable;
	u32 m_TimeToEnable;
	u32 m_TimeShift;
	u32 m_StartTime;

	float volumetric_distance;
	float volumetric_intensity;
	float volumetric_quality;

	// Interactive grass Settings
	float m_fBlowoutTimeLeft;

	s8 m_BendGrass_idle_anim;
	float m_BendGrass_idle_radius;
	float m_BendGrass_idle_speed;
	float m_BendGrass_idle_str;

	s8 m_BendGrass_whenactive_anim;
	float m_BendGrass_whenactive_speed;
	float m_BendGrass_whenactive_str;

	bool m_BendGrass_Blowout;
	s32 m_BendGrass_Blowout_time;
	float m_BendGrass_Blowout_speed;
	float m_BendGrass_Blowout_radius;

	u8 grassbender_id;
	u32 grassbender_frame;

	void GrassZoneUpdate();

	//Ð¼Ð°ÑÑÐ¸Ð² Ñ Ð²Ñ€ÐµÐ¼ÐµÐ½Ð°Ð¼Ð¸, ÑÐºÐ¾Ð»ÑŒÐºÐ¾ ÐºÐ°Ð¶Ð´Ð¾Ðµ ÑÐ¾ÑÑ‚Ð¾ÑÐ½Ð¸Ðµ Ð´Ð¾Ð»Ð¶Ð½Ð¾ 
	//Ð´Ð»Ð¸Ñ‚ÑŒÑÑ (ÐµÑÐ»Ð¸ 0, Ñ‚Ð¾ Ð¼Ð³Ð½Ð¾Ð²ÐµÐ½Ð½Ð¾ -1 - Ð±ÐµÑÐºÐ¾Ð½ÐµÑ‡Ð½Ð¾ÑÑ‚ÑŒ, 
	//-2 - Ð²Ð¾Ð¾Ð±Ñ‰Ðµ Ð½Ðµ Ð´Ð¾Ð»Ð¶Ð½Ð¾ Ð²Ñ‹Ð·Ñ‹Ð²Ð°Ñ‚ÑŒÑÑ)
	typedef svector<int, eZoneStateMax> StateTimeSVec;
	StateTimeSVec m_StateTime;

	virtual void SwitchZoneState(EZoneState new_state);
	virtual void OnStateSwitch(EZoneState new_state);
	virtual void CheckForAwaking();
	//Ð¾Ð±Ñ€Ð°Ð±Ð¾Ñ‚ÐºÐ° Ð·Ð¾Ð½Ñ‹ Ð² Ñ€Ð°Ð·Ð»Ð¸Ñ‡Ð½Ñ‹Ñ… ÑÐ¾ÑÑ‚Ð¾ÑÐ½Ð¸ÑÑ…
	virtual bool IdleState();
	virtual bool AwakingState();
	virtual bool BlowoutState();
	virtual bool AccumulateState();

	virtual void UpdateSecondaryHit()
	{
	};

	virtual bool Enable();
	virtual bool Disable();
	void UpdateOnOffState();
	virtual void GoEnabledState();
	virtual void GoDisabledState();
public:
	bool IsEnabled() { return m_eZoneState != eZoneStateDisabled; };
	void ZoneEnable();
	void ZoneDisable();
	void ChangeIdleParticles(LPCSTR name, bool bIdleLight);
	void MoveScript(Fvector pos);
	EZoneState ZoneState() { return m_eZoneState; }
protected:


	//Ð²Ð¾Ð·Ð´ÐµÐ¹ÑÑ‚Ð²Ð¸Ðµ Ð·Ð¾Ð½Ð¾Ð¹ Ð½Ð° Ð¾Ð±ÑŠÐµÐºÑ‚
	virtual void Affect(SZoneObjectInfo* O)
	{
	}

	//Ð²Ð¾Ð·Ð´ÐµÐ¹ÑÑ‚Ð²Ð¾Ð²Ð°Ñ‚ÑŒ Ð½Ð° Ð²ÑÐµ Ð¾Ð±ÑŠÐµÐºÑ‚Ñ‹ Ð² Ð·Ð¾Ð½Ðµ
	void AffectObjects();

	u32 m_dwAffectFrameNum;

	//Ð¿Ð°Ñ€Ð°Ð¼ÐµÑ‚Ñ€Ñ‹ Ð´Ð»Ñ Ð²Ñ‹Ð±Ñ€Ð¾ÑÐ°, Ñ ÐºÐ°ÐºÐ¾Ð¹ Ð·Ð°Ð´ÐµÑ€Ð¶ÐºÐ¾Ð¹ 
	//Ð²ÐºÐ»ÑŽÑ‡Ð°Ñ‚ÑŒ ÑÑ„Ñ„ÐµÐºÑ‚Ñ‹ Ð¸ Ð»Ð¾Ð³Ð¸ÐºÑƒ
	u32 m_dwBlowoutParticlesTime;
	u32 m_dwBlowoutLightTime;
	u32 m_dwBlowoutSoundTime;
	u32 m_dwBlowoutExplosionTime;
	void UpdateBlowout();

	//Ð²ÐµÑ‚ÐµÑ€
	u32 m_dwBlowoutWindTimeStart;
	u32 m_dwBlowoutWindTimePeak;
	u32 m_dwBlowoutWindTimeEnd;
	//ÑÐ¸Ð»Ð° Ð²ÐµÑ‚Ñ€Ð° (ÑƒÐ²ÐµÐ»Ð¸Ñ‡ÐµÐ½Ð¸Ðµ Ñ‚ÐµÐºÑƒÑ‰ÐµÐ³Ð¾) (0,1) ÐºÐ¾Ð³Ð´Ð° Ð² Ð°Ð½Ð¾Ð¼Ð°Ð»Ð¸ÑŽ Ð¿Ð¾Ð¿Ð°Ð´Ð°ÐµÑ‚ Ð°ÐºÑ‚ÐµÑ€
	float m_fBlowoutWindPowerMax;
	float m_fStoreWindPower;

	void StartWind();
	void StopWind();
	void UpdateWind();


	//Ð²Ñ€ÐµÐ¼Ñ, Ñ‡ÐµÑ€ÐµÐ· ÐºÐ¾Ñ‚Ð¾Ñ€Ð¾Ðµ, Ð·Ð¾Ð½Ð° Ð¿ÐµÑ€ÐµÑÑ‚Ð°ÐµÑ‚ Ñ€ÐµÐ°Ð³Ð¸Ñ€Ð¾Ð²Ð°Ñ‚ÑŒ 
	//Ð½Ð° Ð¾Ð±ÑŠÐµÐºÑ‚ Ð¼ÐµÑ€Ñ‚Ð²Ñ‹Ð¹ Ð¾Ð±ÑŠÐµÐºÑ‚ (-1 ÐµÑÐ»Ð¸ Ð½Ðµ ÑƒÐºÐ°Ð·Ð°Ð½Ð¾)
	int m_iDisableHitTime;
	//Ñ‚Ð¾Ð¶Ðµ ÑÐ°Ð¼Ð¾Ðµ Ð½Ð¾ Ð´Ð»Ñ Ð¼Ð°Ð»ÐµÐ½ÑŒÐºÐ¸Ñ… Ð¾Ð±ÑŠÐµÐºÑ‚Ð¾Ð²
	int m_iDisableHitTimeSmall;
	int m_iDisableIdleTime;

	////////////////////////////////
	// Ð¸Ð¼ÐµÐ½Ð° Ð¿Ð°Ñ€Ñ‚Ð¸ÐºÐ»Ð¾Ð² Ð·Ð¾Ð½Ñ‹
	//Ð¾Ð±Ñ‹Ñ‡Ð½Ð¾Ðµ ÑÐ¾ÑÑ‚Ð¾ÑÐ½Ð¸Ðµ Ð·Ð¾Ð½Ñ‹
	shared_str m_sIdleParticles;
	//Ð²Ñ‹Ð±Ñ€Ð¾Ñ Ð·Ð¾Ð½Ñ‹
	shared_str m_sBlowoutParticles;
	BOOL m_bBlowoutOnce;
	shared_str m_sAccumParticles;
	shared_str m_sAwakingParticles;


	//Ð¿Ð¾ÑÐ²Ð»ÐµÐ½Ð¸Ðµ Ð±Ð¾Ð»ÑŒÑˆÐ¾Ð³Ð¾ Ð¸ Ð¼Ð°Ð»ÑŒÐ½ÐµÐºÐ¾Ð³Ð¾ Ð¾Ð±ÑŠÐµÐºÑ‚Ð° Ð² Ð·Ð¾Ð½Ðµ
	shared_str m_sEntranceParticlesSmall;
	shared_str m_sEntranceParticlesBig;
	//Ð¿Ð¾Ñ€Ð°Ð¶ÐµÐ½Ð¸Ðµ Ð±Ð¾Ð»ÑŒÑˆÐ¾Ð³Ð¾ Ð¸ Ð¼Ð°Ð»ÑŒÐ½ÐµÐºÐ¾Ð³Ð¾ Ð¾Ð±ÑŠÐµÐºÑ‚Ð° Ð² Ð·Ð¾Ð½Ðµ
	shared_str m_sHitParticlesSmall;
	shared_str m_sHitParticlesBig;
	//Ð½Ð°Ñ…Ð¾Ð¶Ð´ÐµÐ½Ð¸Ðµ Ð±Ð¾Ð»ÑŒÑˆÐ¾Ð³Ð¾ Ð¸ Ð¼Ð°Ð»ÑŒÐ½ÐµÐºÐ¾Ð³Ð¾ Ð¾Ð±ÑŠÐµÐºÑ‚Ð° Ð² Ð·Ð¾Ð½Ðµ
	shared_str m_sIdleObjectParticlesSmall;
	shared_str m_sIdleObjectParticlesBig;
	shared_str m_sBoltEntranceParticles;

	ref_sound m_idle_sound;
	ref_sound m_awaking_sound;
	ref_sound m_accum_sound;
	ref_sound m_blowout_sound;
	ref_sound m_hit_sound;
	ref_sound m_entrance_sound;

	//Ð¾Ð±ÑŠÐµÐºÑ‚ Ð¿Ð°Ñ€Ñ‚Ð¸ÐºÐ»Ð¾Ð² Ð¾Ð±Ñ‹Ñ‡Ð½Ð¾Ð³Ð¾ ÑÐ¾ÑÑ‚Ð¾ÑÐ½Ð¸Ñ Ð·Ð¾Ð½Ñ‹
	CParticlesObject* m_pIdleParticles;

	//////////////////////////////
	//Ð¿Ð¾Ð´ÑÐ²ÐµÑ‚ÐºÐ° Ð°Ð½Ð¾Ð¼Ð°Ð»Ð¸Ð¸

	//Ð¿Ð¾Ð´ÑÐ²ÐµÑ‚ÐºÐ° idle ÑÐ¾ÑÑ‚Ð¾ÑÐ½Ð¸Ñ
	ref_light m_pIdleLight;
	Fcolor m_IdleLightColor;
	float m_fIdleLightRange;
	float m_fIdleLightHeight;
	CLAItem* m_pIdleLAnim;
	CLAItem* m_pBlowLAnim;

	void StartIdleLight();
	void StopIdleLight();
	void UpdateIdleLight();


	//Ð¿Ð¾Ð´ÑÐ²ÐµÑ‚ÐºÐ° Ð²Ñ‹Ð±Ñ€Ð¾ÑÐ°
	ref_light m_pLight;
	float m_fLightRange;
	Fcolor m_LightColor;
	float m_fLightTime;
	float m_fLightTimeLeft;
	float m_fLightHeight;

	void StartBlowoutLight();
	void StopBlowoutLight();
	void UpdateBlowoutLight();

	//ÑÐ¿Ð¸ÑÐ¾Ðº Ð¿Ð°Ñ€Ñ‚Ð¸ÐºÐ»Ð¾Ð² Ð´Ð»Ñ Ð¾Ð±ÑŠÐµÑ‚Ð¾Ð² Ð²Ð½ÑƒÑ‚Ñ€Ð¸ Ð·Ð¾Ð½Ñ‹
	DEFINE_VECTOR(SZoneObjectInfo, OBJECT_INFO_VEC, OBJECT_INFO_VEC_IT);
	OBJECT_INFO_VEC m_ObjectInfoMap;

	void CreateHit(u16 id_to,
	               u16 id_from,
	               const Fvector& hit_dir,
	               float hit_power,
	               s16 bone_id,
	               const Fvector& pos_in_bone,
	               float hit_impulse,
	               ALife::EHitType hit_type);


	virtual void Hit(SHit* pHDS);


	//Ð´Ð»Ñ Ð²Ð¸Ð·ÑƒÐ°Ð»Ð¸Ð·Ð°Ñ†Ð¸Ð¸ Ð·Ð¾Ð½Ñ‹
	virtual void PlayIdleParticles(bool bIdleLight = true);
	virtual void StopIdleParticles(bool bIdleLight = true);
	void PlayAccumParticles();
	void PlayAwakingParticles();
	void PlayBlowoutParticles();
	void PlayEntranceParticles(CGameObject* pObject);
	void PlayBulletParticles(Fvector& pos);
	void PlayBoltEntranceParticles();

	void PlayHitParticles(CGameObject* pObject);

	void PlayObjectIdleParticles(CGameObject* pObject);
	void StopObjectIdleParticles(CGameObject* pObject);

	virtual bool IsVisibleForZones() { return false; }

	//Ð¾Ð±Ð½Ð¾Ð²Ð»ÐµÐ½Ð¸Ðµ, ÐµÑÐ»Ð¸ Ð·Ð¾Ð½Ð° Ð¿ÐµÑ€ÐµÐ´Ð²Ð¸Ð³Ð°ÐµÑ‚ÑÑ
	virtual void OnMove();
	Fvector m_vPrevPos;
	u32 m_dwLastTimeMoved;

	//Ñ€Ð°ÑÑÑ‚Ð¾ÑÐ½Ð¸Ðµ Ð¾Ñ‚ Ð·Ð¾Ð½Ñ‹ Ð´Ð¾ Ñ‚ÐµÐºÑƒÑ‰ÐµÐ³Ð¾ Ð°ÐºÑ‚ÐµÑ€Ð°
	float m_fDistanceToCurEntity;
protected:
	u32 m_ef_anomaly_type;
	u32 m_ef_weapon_type;
public:
	void CalcDistanceTo(const Fvector& P, float& dist, float& radius);
	virtual u32 ef_anomaly_type() const;
	virtual u32 ef_weapon_type() const;
	virtual bool register_schedule() const { return true; }

	// optimization FAST/SLOW mode
public:
	virtual BOOL AlwaysTheCrow();
	void o_switch_2_fast();
	void o_switch_2_slow();

	// Lain: adde
private:
	virtual bool light_in_slow_mode() { return true; }

	DECLARE_SCRIPT_REGISTER_FUNCTION
};
