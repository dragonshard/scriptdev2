/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* ScriptData
SDName: Boss palehoof
SDAuthor: ckegg
SD%Complete: 90%
SDComment: Should start by the object trigger!!!!!!!
SDCategory: Utgarde Pinnacle
EndScriptData */

#include "precompiled.h"
#include "def_pinnacle.h"

enum
{
    SAY_AGGRO                                = -1575000,
    SAY_SLAY_1                               = -1575001,
    SAY_SLAY_2                               = -1575002,
    SAY_DEATH                                = -1575003,

    SPELL_ARCING_SMASH                       = 48260,
    SPELL_IMPALE_N                           = 48261,
    SPELL_IMPALE_H                           = 59268,
    SPELL_WITHERING_ROAR_N                   = 48256,
    SPELL_WITHERING_ROAR_H                   = 59267,

    SPELL_FREEZE_ANIM                        = 16245,

    // Massive Jormungar
    SPELL_ACID_SPIT                          = 48132,
    SPELL_ACID_SPLATTER_N                    = 48136,
    SPELL_ACID_SPLATTER_H                    = 59272,
    SPELL_POISON_BREATH_N                    = 48133,
    SPELL_POISON_BREATH_H                    = 59271,
    NPC_JORMUNGAR_WORM                       = 27228,

    // Ferocious Rhino
    SPELL_GORE_N                             = 48130,
    SPELL_GORE_H                             = 59264,
    SPELL_GRIEVOUS_WOUND_N                   = 48105,
    SPELL_GRIEVOUS_WOUND_H                   = 59263,
    SPELL_STOMP                              = 48131,

    // Ravenous Furbolg
    SPELL_CHAIN_LIGHTING_N                   = 48140,
    SPELL_CHAIN_LIGHTING_H                   = 59273,
    SPELL_CRAZED                             = 48139,
    SPELL_TERRIFYING_ROAD                    = 48144,

    // Frenzied Worgen
    SPELL_MORTAL_WOUND_N                     = 48137,
    SPELL_MORTAL_WOUND_H                     = 59265,
    SPELL_ENRAGE_1                           = 48138,
    SPELL_ENRAGE_2                           = 48142
};

/*######
## Mob Gortok Palehoof
######*/

struct MANGOS_DLL_DECL boss_palehoofAI : public ScriptedAI
{
    boss_palehoofAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
    	m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
    	m_bIsHeroic = pCreature->GetMap()->IsHeroic();
    	Reset();
    }

    ScriptedInstance *m_pInstance;
    bool m_bIsHeroic;

    uint32 m_uiArcingSmash_Timer;
    uint32 m_uiImpale_Timer;
    uint32 m_uiWitheringRoar_Timer;
    uint32 m_uiAnimalCheck_Timer;
    uint32 m_uiAnimalCounter;
    Unit* pPlayer;

    void Reset() {
        m_uiArcingSmash_Timer = 15000;
        m_uiImpale_Timer = 12000;
        m_uiWitheringRoar_Timer = 10000;
        m_uiAnimalCheck_Timer = 1000;
        m_uiAnimalCounter = 0;
        pPlayer = NULL;
        DoCast(m_creature, SPELL_FREEZE_ANIM);

        if (m_pInstance)
        {
            if(m_creature->isAlive())
            {
        	    m_pInstance->SetData(DATA_GORTOK_PALEHOOF_EVENT, NOT_STARTED);
        	    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        	    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        	}

            Unit* pTemp = NULL;
            if (pTemp = Unit::GetUnit((*m_creature),m_pInstance->GetData64(DATA_MOB_FRENZIED_WORGEN)))
            {
                if (pTemp->isDead())
                    ((Creature*)pTemp)->Respawn();
        	    pTemp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        	    pTemp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }

            if (pTemp = Unit::GetUnit((*m_creature),m_pInstance->GetData64(DATA_MOB_RAVENOUS_FURBOLG)))
            {
                if (pTemp->isDead())
                    ((Creature*)pTemp)->Respawn();
        	    pTemp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        	    pTemp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }

            if (pTemp = Unit::GetUnit((*m_creature),m_pInstance->GetData64(DATA_MOB_MASSIVE_JORMUNGAR)))
            {
                if (pTemp->isDead())
                    ((Creature*)pTemp)->Respawn();
        	    pTemp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        	    pTemp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }

            if (pTemp = Unit::GetUnit((*m_creature),m_pInstance->GetData64(DATA_MOB_FEROCIOUS_RHINO)))
            {
                if (pTemp->isDead())
                    ((Creature*)pTemp)->Respawn();
        	    pTemp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        	    pTemp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }
        }
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void AttackStart(Unit* pWho)
    {
        if (m_uiAnimalCounter < 5)
            return;

        if (!pWho || pWho == m_creature)
            return;

        if (m_creature->Attack(pWho, true))
        {
            m_creature->SetInCombatWithZone();
            DoStartMovement(pWho);
        }
    }

    void MoveInLineOfSight(Unit* pWho)
    {
    	if (!pWho)
    	    return;

        if (pWho->isTargetableForAttack() && pWho->isInAccessablePlaceFor(m_creature) && m_creature->IsHostileTo(pWho) &&
        	!m_uiAnimalCounter && pWho->GetTypeId() == TYPEID_PLAYER && m_creature->IsWithinDistInMap(pWho, 25))
        {
            if(m_pInstance)
                m_pInstance->SetData(DATA_GORTOK_PALEHOOF_EVENT, IN_PROGRESS);

            pPlayer = pWho;
        	m_uiAnimalCounter++;
        }
	}

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiAnimalCounter)
        {
            if(m_uiAnimalCheck_Timer < uiDiff)
            {
            	Creature* pTemp = NULL;

                if (m_uiAnimalCounter == 1)
                	pTemp = (Creature*)Unit::GetUnit(*m_creature, m_pInstance->GetData64(DATA_MOB_FRENZIED_WORGEN));
                if (m_uiAnimalCounter == 2)
                	pTemp = (Creature*)Unit::GetUnit(*m_creature, m_pInstance->GetData64(DATA_MOB_RAVENOUS_FURBOLG));
                if (m_uiAnimalCounter == 3)
                	pTemp = (Creature*)Unit::GetUnit(*m_creature, m_pInstance->GetData64(DATA_MOB_MASSIVE_JORMUNGAR));
                if (m_uiAnimalCounter == 4)
                	pTemp = (Creature*)Unit::GetUnit(*m_creature, m_pInstance->GetData64(DATA_MOB_FEROCIOUS_RHINO));

                if (pTemp)
                {
                    if (!pTemp->isAlive())
                        m_uiAnimalCounter++;
                    if (pTemp->isAlive() && !pTemp->getVictim())
                    {
                        if (pTemp->HasAura(SPELL_FREEZE_ANIM, 0))
                            pTemp->RemoveAurasDueToSpell(SPELL_FREEZE_ANIM);
                        pTemp->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        pTemp->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        ((Unit*)pTemp)->SetStandState(UNIT_STAND_STATE_STAND);
                        pTemp->SetInCombatWithZone();
                        pTemp->AI()->AttackStart(pPlayer);
                    }
                }

                if (m_uiAnimalCounter == 5)
                {
                    if (m_creature->HasAura(SPELL_FREEZE_ANIM, 0))
                        m_creature->RemoveAurasDueToSpell(SPELL_FREEZE_ANIM);
                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    ((Unit*)m_creature)->SetStandState(UNIT_STAND_STATE_STAND);
                    m_uiAnimalCheck_Timer = 10000000;
                    AttackStart(pPlayer);
                }
                else
                {
                    m_uiAnimalCheck_Timer = 1000;
                    return;
                }
            }else m_uiAnimalCheck_Timer -= uiDiff;
        }

        //Return since we have no target
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

        // Withering Roar
        if(m_uiWitheringRoar_Timer < uiDiff)
        {
            DoCast(m_creature, m_bIsHeroic ? SPELL_WITHERING_ROAR_H : SPELL_WITHERING_ROAR_N);
            m_uiWitheringRoar_Timer = 8000 + rand()%4000;
        }else m_uiWitheringRoar_Timer -= uiDiff;

        // Impale
        if(m_uiImpale_Timer < uiDiff)
        {
            DoCast(SelectUnit(SELECT_TARGET_RANDOM, 0), m_bIsHeroic ? SPELL_IMPALE_H : SPELL_IMPALE_N);
            m_uiImpale_Timer = 8000 + rand()%4000;
        }else m_uiImpale_Timer -= uiDiff;

        // Arcing Smash
        if(m_uiArcingSmash_Timer < uiDiff)
        {
            DoCast(m_creature, SPELL_ARCING_SMASH);
            m_uiArcingSmash_Timer = 13000 + rand()%4000;
        }else m_uiArcingSmash_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);
        if(m_pInstance)
            m_pInstance->SetData(DATA_GORTOK_PALEHOOF_EVENT, DONE);
    }

    void KilledUnit(Unit* pVictim)
    {
        if (pVictim == m_creature)
            return;

        switch(rand()%2)
        {
            case 0:
                DoScriptText(SAY_SLAY_1, m_creature);
                break;
            case 1:
                DoScriptText(SAY_SLAY_2, m_creature);
                break;
        }
    }
};

/*######
## Mob Massive Jormungar
######*/
struct MANGOS_DLL_DECL mob_massive_jormungarAI : public ScriptedAI
{
    mob_massive_jormungarAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
    	m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
    	m_bIsHeroic = pCreature->GetMap()->IsHeroic();
    	Reset();
    }

    ScriptedInstance *m_pInstance;
    bool m_bIsHeroic;

    uint32 AcidSpit_timer;
    uint32 AcidSplatter_timer;
    uint32 PoisonBreath_timer;

    void Reset() {
    	AcidSpit_timer = 3000;
    	AcidSplatter_timer = 12000;
    	PoisonBreath_timer = 10000;
        if (m_pInstance)
            if (Creature* pPalehoof = (Creature*)Unit::GetUnit(*m_creature, m_pInstance->GetData64(DATA_GORTOK_PALEHOOF)))
                ((boss_palehoofAI*)pPalehoof->AI())->EnterEvadeMode();

        DoCast(m_creature, SPELL_FREEZE_ANIM);
    }

    void AttackStart(Unit* pWho)
    {
        if (!pWho || pWho == m_creature)
            return;

        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
        	return;

        if (m_creature->Attack(pWho, true))
        {
            m_creature->AddThreat(pWho, 0.0f);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);
            DoStartMovement(pWho);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

        // Acid Spit
        if(AcidSpit_timer < uiDiff)
        {
            DoCast(SelectUnit(SELECT_TARGET_RANDOM, 0), SPELL_ACID_SPIT);
            AcidSpit_timer = 2000 + rand()%2000;
        }else AcidSpit_timer -= uiDiff;

        // Acid Splatter
        if(AcidSplatter_timer < uiDiff)
        {
            DoCast(m_creature, m_bIsHeroic ? SPELL_ACID_SPLATTER_H : SPELL_ACID_SPLATTER_N);

            // spell doesn't work, summon 6 Jormungar Worm instead
            float x, y, z;
            m_creature->GetPosition(x,y,z);
            for(uint8 i = 0; i<6; i++)
            {
                Creature* pJormungarWorm = m_creature->SummonCreature(NPC_JORMUNGAR_WORM, x+rand()%10, y+rand()%10, z, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 360000);

                Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0);
                if (target && pJormungarWorm) {
                    pJormungarWorm->AddThreat(target, 0.0f);
                    pJormungarWorm->AI()->AttackStart(target);
                }
            }

            AcidSplatter_timer = 10000 + rand()%4000;
        }else AcidSplatter_timer -= uiDiff;

        // Poison Breath
        if(PoisonBreath_timer < uiDiff)
        {
            DoCast(SelectUnit(SELECT_TARGET_RANDOM, 0), m_bIsHeroic ? SPELL_POISON_BREATH_H : SPELL_POISON_BREATH_N);
            PoisonBreath_timer = 8000 + rand()%4000;
        }else PoisonBreath_timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
    void JustDied(Unit* pKiller) {}
};

/*######
## Mob Ferocious Rhino
######*/
struct MANGOS_DLL_DECL mob_ferocious_rhinoAI : public ScriptedAI
{
    mob_ferocious_rhinoAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
    	m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
    	m_bIsHeroic = pCreature->GetMap()->IsHeroic();
    	Reset();
    }

    ScriptedInstance *m_pInstance;
    bool m_bIsHeroic;

    uint32 Stomp_timer;
    uint32 Gore_timer;
    uint32 GrievousWound_timer;

    void Reset() {
    	Stomp_timer = 10000;
    	Gore_timer = 15000;
    	GrievousWound_timer = 20000;

        if (m_pInstance)
            if (Creature* pPalehoof = (Creature*)Unit::GetUnit(*m_creature, m_pInstance->GetData64(DATA_GORTOK_PALEHOOF)))
                ((boss_palehoofAI*)pPalehoof->AI())->EnterEvadeMode();

        DoCast(m_creature, SPELL_FREEZE_ANIM);
    }

    void AttackStart(Unit* pWho)
    {
        if (!pWho || pWho == m_creature)
            return;

        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
        	return;

        if (m_creature->Attack(pWho, true))
        {
            m_creature->AddThreat(pWho, 0.0f);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);
            DoStartMovement(pWho);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

    	// Stomp
        if(Stomp_timer < uiDiff)
        {
            DoCast(m_creature->getVictim(), SPELL_STOMP);
            Stomp_timer = 8000 + rand()%4000;
        }else Stomp_timer -= uiDiff;

    	// Gore
        if(Gore_timer < uiDiff)
        {
            DoCast(m_creature->getVictim(), SPELL_STOMP);
            Gore_timer = 13000 + rand()%4000;
        }else Gore_timer -= uiDiff;

        // Grievous Wound
        if(GrievousWound_timer < uiDiff)
        {
            DoCast(SelectUnit(SELECT_TARGET_RANDOM, 0), m_bIsHeroic ? SPELL_GRIEVOUS_WOUND_H : SPELL_GRIEVOUS_WOUND_N);
            GrievousWound_timer = 18000 + rand()%4000;
        }else GrievousWound_timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
    void JustDied(Unit* pKiller) {}
};

/*######
## Mob Ravenous Furbolg
######*/
struct MANGOS_DLL_DECL mob_ravenous_furbolgAI : public ScriptedAI
{
    mob_ravenous_furbolgAI(Creature* pCreature) : ScriptedAI(pCreature) {
    	m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
    	m_bIsHeroic = pCreature->GetMap()->IsHeroic();
    	Reset();
    }

    ScriptedInstance *m_pInstance;
    bool m_bIsHeroic;

    uint32 ChainLightning_timer;
    uint32 Crazed_timer;
    uint32 TerrifyingRoar_timer;

    void Reset() {
    	ChainLightning_timer = 5000;
    	Crazed_timer = 10000;
    	TerrifyingRoar_timer = 15000;

        if (m_pInstance)
            if (Creature* pPalehoof = (Creature*)Unit::GetUnit(*m_creature, m_pInstance->GetData64(DATA_GORTOK_PALEHOOF)))
                ((boss_palehoofAI*)pPalehoof->AI())->EnterEvadeMode();

        DoCast(m_creature, SPELL_FREEZE_ANIM);
    }

    void AttackStart(Unit* pWho)
    {
        if (!pWho || pWho == m_creature)
            return;

        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
        	return;

        if (m_creature->Attack(pWho, true))
        {
            m_creature->AddThreat(pWho, 0.0f);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);
            DoStartMovement(pWho);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

    	// Chain Lightning
        if(ChainLightning_timer < uiDiff)
        {
            DoCast(m_creature->getVictim(), m_bIsHeroic ? SPELL_CHAIN_LIGHTING_H : SPELL_CHAIN_LIGHTING_N);
            ChainLightning_timer = 5000 + rand()%5000;
        }else ChainLightning_timer -= uiDiff;

    	// Crazed
        if(Crazed_timer < uiDiff)
        {
            DoCast(m_creature, SPELL_CRAZED);
            Crazed_timer = 8000 + rand()%4000;
        }else Crazed_timer -= uiDiff;

    	// Terrifying Roar
        if(TerrifyingRoar_timer < uiDiff)
        {
            DoCast(m_creature, SPELL_TERRIFYING_ROAD);
            TerrifyingRoar_timer = 10000 + rand()%10000;
        }else TerrifyingRoar_timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
    void JustDied(Unit* pKiller) {}
};

/*######
## Mob Frenzied Worgen
######*/
struct MANGOS_DLL_DECL mob_frenzied_worgenAI : public ScriptedAI
{
    mob_frenzied_worgenAI(Creature* pCreature) : ScriptedAI(pCreature) {
    	m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
    	m_bIsHeroic = pCreature->GetMap()->IsHeroic();
    	Reset();
    }

    ScriptedInstance *m_pInstance;
    bool m_bIsHeroic;

    uint32 MortalWound_timer;
    uint32 Enrage1_timer;
    uint32 Enrage2_timer;

    void Reset() {
    	MortalWound_timer = 5000;
    	Enrage1_timer = 15000;
    	Enrage2_timer = 10000;

        if (m_pInstance)
            if (Creature* pPalehoof = (Creature*)Unit::GetUnit(*m_creature, m_pInstance->GetData64(DATA_GORTOK_PALEHOOF)))
                ((boss_palehoofAI*)pPalehoof->AI())->EnterEvadeMode();

        DoCast(m_creature, SPELL_FREEZE_ANIM);
    }

    void AttackStart(Unit* pWho)
    {
        if (!pWho || pWho == m_creature)
            return;

        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
        	return;

        if (m_creature->Attack(pWho, true))
        {
            m_creature->AddThreat(pWho, 0.0f);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);
            DoStartMovement(pWho);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

    	// Mortal Wound
        if(MortalWound_timer < uiDiff)
        {
            DoCast(m_creature->getVictim(), m_bIsHeroic ? SPELL_MORTAL_WOUND_H : SPELL_MORTAL_WOUND_H);
            MortalWound_timer = 3000 + rand()%4000;
        }else MortalWound_timer -= uiDiff;

    	// Enrage1
        if(Enrage1_timer < uiDiff)
        {
            DoCast(m_creature, SPELL_ENRAGE_1);
            Enrage1_timer = 15000;
        }else Enrage1_timer -= uiDiff;

    	// Enrage2
        if(Enrage2_timer < uiDiff)
        {
            DoCast(m_creature, SPELL_ENRAGE_2);
            Enrage2_timer = 10000;
        }else Enrage2_timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
    void JustDied(Unit* pKiller) {}
};

CreatureAI* GetAI_boss_palehoof(Creature* pCreature)
{
    return new boss_palehoofAI (pCreature);
}

CreatureAI* GetAI_mob_massive_jormungar(Creature* pCreature)
{
    return new mob_massive_jormungarAI (pCreature);
}

CreatureAI* GetAI_mob_ferocious_rhino(Creature* pCreature)
{
    return new mob_ferocious_rhinoAI (pCreature);
}

CreatureAI* GetAI_mob_ravenous_furbolg(Creature* pCreature)
{
    return new mob_ravenous_furbolgAI (pCreature);
}

CreatureAI* GetAI_mob_frenzied_worgen(Creature* pCreature)
{
    return new mob_frenzied_worgenAI (pCreature);
}

void AddSC_boss_palehoof()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name="boss_palehoof";
    newscript->GetAI = GetAI_boss_palehoof;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="mob_massive_jormungar";
    newscript->GetAI = GetAI_mob_massive_jormungar;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="mob_ferocious_rhino";
    newscript->GetAI = GetAI_mob_ferocious_rhino;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="mob_ravenous_furbolg";
    newscript->GetAI = GetAI_mob_ravenous_furbolg;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="mob_frenzied_worgen";
    newscript->GetAI = GetAI_mob_frenzied_worgen;
    newscript->RegisterSelf();

}
