///////////////////////////////////////////////////////////////
// BastArtifact.cpp
// BastArtefact - Ð°Ñ€Ñ‚ÐµÑ„Ð°ÐºÑ‚ Ð¼Ð¾Ñ‡Ð°Ð»ÐºÐ°
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BastArtifact.h"
#include "../xrphysics/PhysicsShell.h"
#include "../xrphysics/extendedgeom.h"
#include "ParticlesObject.h"


CBastArtefact::CBastArtefact(void)
{
	m_pHitedEntity = nullptr;
	m_fImpulseThreshold = 10.f;

	m_fRadius = 10.f;
	m_fStrikeImpulse = 15.f;

	m_bStrike = false;
	m_AttakingEntity = nullptr;

	m_fEnergy = 0.f;
	m_fEnergyMax = m_fStrikeImpulse * 100.f;
	m_fEnergyDecreasePerTime = 1.1f;
}

CBastArtefact::~CBastArtefact(void)
{
}

//Ð²Ñ‹Ð·Ñ‹Ð²Ð°ÐµÑ‚ÑÑ Ð¿Ñ€Ð¸ ÑÑ‚Ð¾Ð»ÐºÐ½Ð¾Ð²ÐµÐ½Ð¸Ð¸ Ð¼Ð¾Ñ‡Ð°Ð»ÐºÐ¸ Ñ Ñ‡ÐµÐ¼-Ñ‚Ð¾
void CBastArtefact::ObjectContactCallback(bool& /**do_colide/**/, bool bo1, dContact& c, SGameMtl* /*material_1*/,
                                          SGameMtl* /*material_2*/)
{
	dxGeomUserData* l_pUD1 = nullptr;
	dxGeomUserData* l_pUD2 = nullptr;
	l_pUD1 = PHRetrieveGeomUserData(c.geom.g1);
	l_pUD2 = PHRetrieveGeomUserData(c.geom.g2);

	if (!l_pUD1 || !l_pUD2) return;

	//Ð¾Ð¿Ñ€ÐµÐ´ÐµÐ»Ð¸Ñ‚ÑŒ ÐºÑ‚Ð¾ ÐµÑÑ‚ÑŒ ÐºÑ‚Ð¾, Ð¸Ð· Ð´Ð²ÑƒÑ… ÑÑ‚Ð¾Ð»ÐºÐ½ÑƒÐ²ÑˆÐ¸Ñ…ÑÑ Ð¿Ñ€ÐµÐ´Ð¼ÐµÑ‚Ð¾Ð²
	CBastArtefact* pBastArtefact = l_pUD1 ? smart_cast<CBastArtefact*>(l_pUD1->ph_ref_object) : NULL;
	if (!pBastArtefact) pBastArtefact = l_pUD2 ? smart_cast<CBastArtefact*>(l_pUD2->ph_ref_object) : NULL;
	if (!pBastArtefact) return;
	if (!pBastArtefact->IsAttacking()) return;

	CEntityAlive* pEntityAlive = nullptr;
	pEntityAlive = l_pUD1 ? smart_cast<CEntityAlive*>(l_pUD1->ph_ref_object) : NULL;
	if (!pEntityAlive) pEntityAlive = l_pUD2 ? smart_cast<CEntityAlive*>(l_pUD2->ph_ref_object) : NULL;

	pBastArtefact->BastCollision(pEntityAlive);
}

void CBastArtefact::BastCollision(CEntityAlive* pEntityAlive)
{
	//Ð¿Ð¾Ð¿Ð°Ð»Ð¸ Ð²Ð¾ Ñ‡Ñ‚Ð¾-Ñ‚Ð¾ Ð¶Ð¸Ð²Ð¾Ðµ
	if (pEntityAlive && pEntityAlive->g_Alive())
	{
		m_AttakingEntity = nullptr;
		m_pHitedEntity = pEntityAlive;


		if (m_AliveList.size() > 1)
		{
			m_bStrike = true;
		}
		else
		{
			m_bStrike = false;
		}

		m_bStrike = true;
		Fvector vel;
		vel.set(0, 0, 0);
		//	this->m_pPhysicsShell->set_LinearVel(vel);
		//	this->m_pPhysicsShell->set_AngularVel(vel);
	}
}

BOOL CBastArtefact::net_Spawn(CSE_Abstract* DC)
{
	BOOL result = inherited::net_Spawn(DC);
	if (!result) return FALSE;

	m_bStrike = false;
	m_AttakingEntity = nullptr;
	m_pHitedEntity = nullptr;
	m_AliveList.clear();

	return TRUE;
}

void CBastArtefact::net_Destroy()
{
	inherited::net_Destroy();

	m_bStrike = false;
	m_AttakingEntity = nullptr;
	m_pHitedEntity = nullptr;
	m_AliveList.clear();
}

void CBastArtefact::Load(LPCSTR section)
{
	inherited::Load(section);

	m_fImpulseThreshold = pSettings->r_float(section, "impulse_threshold");
	m_fRadius = pSettings->r_float(section, "radius");
	m_fStrikeImpulse = pSettings->r_float(section, "strike_impulse");

	m_fEnergyMax = pSettings->r_float(section, "energy_max");
	m_fEnergyDecreasePerTime = pSettings->r_float(section, "energy_decrease_speed");

	m_sParticleName = pSettings->r_string(section, "particle");
}

void CBastArtefact::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);

	Fvector P;
	P.set(Position());
	feel_touch_update(P, m_fRadius);
}


void CBastArtefact::UpdateCLChild()
{
	//Log						("--- A - CBastArtefact",*cName());
	//Log						("--- A - CBastArtefact",renderable.xform);

	//ÑÐ¾Ð²Ñ€ÐµÐ¼ÐµÐ½ÐµÐ¼ ÑÐ½ÐµÑ€Ð³Ð¸Ñ Ð¿Ð¾ Ð½ÐµÐ¼Ð½Ð¾Ð³Ñƒ Ñ‚Ð¾Ð¶Ðµ ÑƒÐ¼ÐµÐ½ÑŒÑˆÐ°ÐµÑ‚ÑÑ
	if (m_fEnergy > 0) m_fEnergy -= m_fEnergyDecreasePerTime * Device.fTimeDelta;

	if (getVisible() && m_pPhysicsShell)
	{
		if (m_bStrike)
		{
			//Ð²Ñ‹Ð±Ñ€Ð°Ñ‚ÑŒ Ð¶ÐµÑ€Ñ‚Ð²Ñƒ, ÐµÑÐ»Ð¸ Ð¾Ð½Ð° ÐµÑ‰Ðµ Ð½Ðµ Ð²Ñ‹Ð±Ñ€Ð°Ð½Ð°
			if (!m_AliveList.empty() && m_AttakingEntity == nullptr)
			{
				CEntityAlive* pEntityToHit = nullptr;
				if (m_AliveList.size() > 1)
				{
					do
					{
						int rnd = ::Random.randI(m_AliveList.size());
						pEntityToHit = m_AliveList[rnd];
					}
					while (pEntityToHit == m_pHitedEntity);
				}
				else
				{
					pEntityToHit = m_AliveList.front();
				}

				m_AttakingEntity = pEntityToHit;
			}
		}

		if (m_AttakingEntity)
		{
			if (m_AttakingEntity->g_Alive() && m_fEnergy > m_fStrikeImpulse)
			{
				m_fEnergy -= m_fStrikeImpulse;

				//Ð±Ñ€Ð¾ÑÐ¸Ñ‚ÑŒ Ð°Ñ€Ñ‚ÐµÑ„Ð°ÐºÑ‚ Ð½Ð° Ð²Ñ‹Ð±Ñ€Ð°Ð½Ð½ÑƒÑŽ Ñ†ÐµÐ»ÑŒ
				Fvector dir;
				m_AttakingEntity->Center(dir);
				dir.sub(this->Position());
				dir.y += ::Random.randF(-0.05f, 0.5f);

				m_pPhysicsShell->applyImpulse(dir,
				                              m_fStrikeImpulse * Device.fTimeDelta *
				                              m_pPhysicsShell->getMass());
			}
			else
			{
				m_AttakingEntity = nullptr;
				m_bStrike = false;
			}
		}


		if (m_fEnergy > 0 && ::Random.randF(0.f, 1.0f) < (m_fEnergy / (m_fStrikeImpulse * 100.f)))
		{
			CParticlesObject* pStaticPG;
			pStaticPG = CParticlesObject::Create(*m_sParticleName,TRUE);
			Fmatrix pos;
			pos.set(XFORM());
			Fvector vel;
			//vel.sub(Position(),ps_Element(0).vPosition); 
			//vel.div((Level().timeServer()-ps_Element(0).dwTime)/1000.f);
			vel.set(0, 0, 0);
			pStaticPG->UpdateParent(pos, vel);
			pStaticPG->Play(false);
		}
	}
	else if (H_Parent()) XFORM().set(H_Parent()->XFORM());
}


//void CBastArtefact::Hit(float P, Fvector &dir,	
//						CObject* who, s16 element,
//						Fvector position_in_object_space, 
//						float impulse, 
//						ALife::EHitType hit_type)
void CBastArtefact::Hit(SHit* pHDS)
{
	SHit HDS = *pHDS;
	if (HDS.impulse > m_fImpulseThreshold && !m_AliveList.empty())
	{
		m_bStrike = true;
		m_AttakingEntity = m_pHitedEntity = nullptr;

		m_fEnergy += m_fStrikeImpulse * HDS.impulse;

		if (m_fEnergy > m_fEnergyMax) m_fEnergy = m_fEnergyMax;

		//Ñ‡Ñ‚Ð¾Ð± Ð²Ñ‹ÑÑ‚Ñ€ÐµÐ» Ð½Ðµ Ð¿Ð¾Ð²Ð»Ð¸ÑÐ» Ð½Ð° Ñ‚Ñ€Ð°ÐµÐºÑ‚Ð¾Ñ€Ð¸ÑŽ Ð¿Ð¾Ð»ÐµÑ‚Ð° Ð°Ñ€Ñ‚ÐµÑ„Ð°ÐºÑ‚Ð°
		HDS.impulse = 0;
	}

	//	inherited::Hit(P, dir, who, element, position_in_object_space, impulse, hit_type);
	inherited::Hit(&HDS);
}


//Ð¾Ð±ÑŠÐµÐºÑ‚ Ð¼Ð¾Ð¶Ð½Ð¾ Ð¿Ð¾Ð´Ð½ÑÑ‚ÑŒ Ñ‚Ð¾Ð»ÑŒÐºÐ¾ Ð² ÑÐ¿Ð¾ÐºÐ¾Ð¹Ð½Ð¾Ð¼ ÑÐ¾ÑÑ‚Ð¾ÑÐ½Ð¸Ð¸
bool CBastArtefact::Useful() const
{
	if (m_fEnergy > 0)
		return false;
	else
		return true;
}

void CBastArtefact::feel_touch_new(CObject* O)
{
	CEntityAlive* pEntityAlive = smart_cast<CEntityAlive*>(O);

	if (pEntityAlive && pEntityAlive->g_Alive())
	{
		m_AliveList.push_back(pEntityAlive);
	}
}

void CBastArtefact::feel_touch_delete(CObject* O)
{
	CEntityAlive* pEntityAlive = smart_cast<CEntityAlive*>(O);

	if (pEntityAlive)
	{
		m_AliveList.erase(std::find(m_AliveList.begin(),
		                            m_AliveList.end(),
		                            pEntityAlive));
	}
}

bool CBastArtefact::feel_touch_contact(CObject* O)
{
	CEntityAlive* pEntityAlive = smart_cast<CEntityAlive*>(O);

	if (pEntityAlive && pEntityAlive->g_Alive())
		return true;
	else
		return false;
}

void CBastArtefact::setup_physic_shell()
{
	inherited::setup_physic_shell();
	m_pPhysicsShell->set_PhysicsRefObject(this);
	m_pPhysicsShell->set_ObjectContactCallback(ObjectContactCallback);
	m_pPhysicsShell->set_ContactCallback(nullptr);
}
