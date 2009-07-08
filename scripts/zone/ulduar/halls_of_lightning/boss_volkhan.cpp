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
SDName: Boss Volkhan
SDAuthor: ckegg
SD%Complete: 98%
SDComment: Golem die after shatter or not?
SDCategory: Halls of Lightning
EndScriptData */

#include "precompiled.h"
#include "def_halls_of_lightning.h"

enum
{
    SAY_AGGRO                                = -1602028,
    SAY_SLAY_1                               = -1602029,
    SAY_SLAY_2                               = -1602030,
    SAY_SLAY_3                               = -1602031,
    SAY_DEATH                                = -1602032,
    SAY_STOMP_1                              = -1602033,
    SAY_STOMP_2                              = -1602034,
    SAY_FORGE_1                              = -1602035,
    SAY_FORGE_2                              = -1602036,

    SPELL_HEAT_N                             = 52387,
    SPELL_HEAT_H                             = 59528,
    SPELL_SHATTERING_STOMP_N                 = 52237,
    SPELL_SHATTERING_STOMP_H                 = 59529,
    SPELL_TEMPER                             = 52238,
    SPELL_TEMPER2                            = 52654, // not sure
    SPELL_TEMPER_VISUAL                      = 52661,

    NPC_VOLKHANS_ANVIL                       = 28823,

    //Molten Golem
    NPC_MOLTEN_GOLEM                         = 28695,
    SPELL_BLAST_WAVE                         = 23113,
    SPELL_IMMOLATION_STRIKE_N                = 52433,
    SPELL_IMMOLATION_STRIKE_H                = 59530,
    SPELL_SHATTER_N                          = 52429,
    SPELL_SHATTER_H                          = 59527
};

/*######
## Boss Volkhan
######*/
struct MANGOS_DLL_DECL boss_volkhanAI : public ScriptedAI
{
    boss_volkhanAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
    	pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
    	m_bIsHeroic = pCreature->GetMap()->IsHeroic();
        Reset();
    }

    ScriptedInstance *pInstance;

    std::list<uint64> m_lGolemGUIDList;

    bool m_bIsHeroic;
    bool m_bIsStriking;

    uint8 m_uiHealthCheck[4];

    uint32 m_uiPause_Timer;
    uint32 m_uiShatteringStomp_Timer;
    uint32 m_uiShatter_Timer;
    uint32 m_uiHealCheck_Timer;

    void Reset()
    {
    	m_bIsStriking = false;

        m_uiPause_Timer = 0;
        m_uiShatteringStomp_Timer = 80000;
        m_uiShatter_Timer = 0;
        m_uiHealCheck_Timer = 1000;

        m_uiHealthCheck[0] = 80;
        m_uiHealthCheck[1] = 60;
        m_uiHealthCheck[2] = 40;
        m_uiHealthCheck[3] = 20;

        DespawnGolem();
        m_lGolemGUIDList.clear();

        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

        if(pInstance)
            pInstance->SetData(DATA_VOLKHAN_EVENT, NOT_STARTED);
    }

    void Aggro(Unit* who)
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (!who || m_creature->getVictim())
            return;

        if (who->isTargetableForAttack() && who->isInAccessablePlaceFor(m_creature) && m_creature->IsHostileTo(who))
            AttackStart(who);

        if(pInstance)
            pInstance->SetData(DATA_VOLKHAN_EVENT, IN_PROGRESS);
    }

    void AttackStart(Unit* who)
    {
        if (m_bIsStriking)
            return;

        if (!who || who == m_creature)
            return;

        if (m_creature->Attack(who, true))
        {
            m_creature->SetInCombatWithZone();
            DoStartMovement(who);
        }
    }

    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if(uiType != POINT_MOTION_TYPE)
                return;

        switch(uiPointId)
        {
            case 0:
            	m_creature->SetSpeed(MOVE_RUN, 1.0f);
                DoCast(m_creature, SPELL_TEMPER);
                m_uiPause_Timer = 1000;
                break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // Striking workaround (SPELL_TEMPER is not spawn adds)
        if (m_bIsStriking)
        {
            if (m_uiPause_Timer < uiDiff)
            {
                for(uint8 i = 0; i < 2; ++i)
                {
                    if (Creature* pTemp = m_creature->SummonCreature(NPC_MOLTEN_GOLEM, 1323.061, -90.179, 56.717, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000))
                    {
                        m_lGolemGUIDList.push_back(pTemp->GetGUID());
                        pTemp->AI()->AttackStart(SelectUnit(SELECT_TARGET_RANDOM, 0));
                        DoCast(pTemp, m_bIsHeroic ? SPELL_HEAT_H : SPELL_HEAT_N);
                    }
                }
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                m_bIsStriking = false;
                m_uiPause_Timer = 0;
            } else m_uiPause_Timer -= uiDiff;
            return;
        }

        //Return since we have no target
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

        if(m_uiShatteringStomp_Timer < uiDiff)
        {
            switch(rand()%2)
            {
                case 0: DoScriptText(SAY_STOMP_1, m_creature);break;
                case 1: DoScriptText(SAY_STOMP_2, m_creature);break;
            }
            DoCast(m_creature, m_bIsHeroic ? SPELL_SHATTERING_STOMP_H : SPELL_SHATTERING_STOMP_N);
            m_uiShatteringStomp_Timer = 80000;
            m_uiShatter_Timer = 3000;
        }else m_uiShatteringStomp_Timer -= uiDiff;

        // Shatter Golems 3 seconds after Shattering Stomp
        if (m_uiShatter_Timer)
            if(m_uiShatter_Timer < uiDiff)
            {
                ShatterGolem();
                m_uiShatter_Timer = 0;
            }else m_uiShatter_Timer -= uiDiff;

        // Health check -----------------------------------------------------------------------------
        if (m_uiHealCheck_Timer < uiDiff)
        {
            for(uint8 i = 0; i < 4; i++)
            {
                if (m_uiHealthCheck[i] && (m_creature->GetHealth()*100 / m_creature->GetMaxHealth()) <= m_uiHealthCheck[i])
                {
                    switch(rand()%2)
                    {
                        case 0: DoScriptText(SAY_FORGE_1, m_creature);break;
                        case 1: DoScriptText(SAY_FORGE_2, m_creature);break;
                    }
                    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE); // If don't do this, Volkhan won't run because players keep hitting him
                    m_creature->InterruptNonMeleeSpells(true);
                    m_creature->GetMotionMaster()->Clear(false);
                    m_creature->StopMoving();
                    m_creature->AttackStop();
                    // workaround for SPELL_TEMPER
                    m_creature->SetSpeed(MOVE_RUN, 2.0f);
                    m_creature->GetMotionMaster()->MovePoint(0, 1325.911, -93.036, 56.717);
                    m_bIsStriking = true;
                    m_uiHealthCheck[i] = 0; // deactive
                }
            }
            m_uiHealCheck_Timer = 1000;
        } else m_uiHealCheck_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* killer)
    {
        DoScriptText(SAY_DEATH, m_creature);
        DespawnGolem();

        if(pInstance)
            pInstance->SetData(DATA_VOLKHAN_EVENT, DONE);
    }

    void KilledUnit(Unit *victim)
    {
        if(victim == m_creature)
            return;
        switch(rand()%3)
        {
            case 0: DoScriptText(SAY_SLAY_1, m_creature);break;
            case 1: DoScriptText(SAY_SLAY_2, m_creature);break;
            case 2: DoScriptText(SAY_SLAY_3, m_creature);break;
        }
    }

    void DespawnGolem()
    {
        if (m_lGolemGUIDList.empty())
            return;

        for(std::list<uint64>::iterator itr = m_lGolemGUIDList.begin(); itr != m_lGolemGUIDList.end(); ++itr)
        	if (Creature* pTemp = (Creature*)Unit::GetUnit(*m_creature, *itr))
                if (pTemp->isAlive())
                    pTemp->ForcedDespawn();

        m_lGolemGUIDList.clear();
    }

    void ShatterGolem()
    {
        if (m_lGolemGUIDList.empty())
            return;

        for(std::list<uint64>::iterator itr = m_lGolemGUIDList.begin(); itr != m_lGolemGUIDList.end(); ++itr)
            if (Creature* pTemp = (Creature*)Unit::GetUnit(*m_creature, *itr))
                if (pTemp->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE)) // only Shatters frozen golems
                    pTemp->CastSpell(pTemp, m_bIsHeroic ? SPELL_SHATTER_H : SPELL_SHATTER_N, true);
    }
};

/*######
## Npc Volkhans Anvil
######*/
struct MANGOS_DLL_DECL npc_volkhans_anvilAI : public ScriptedAI
{
    npc_volkhans_anvilAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }
    void Reset() {}
    void Aggro(Unit* who) { return; } // don't aggro
    void AttackStart(Unit* who) { return; }
    void UpdateAI(const uint32 diff) {}
    void JustDied(Unit* killer) {}
};

/*######
## Mob Molten Golem
######*/
struct MANGOS_DLL_DECL mob_molten_golemAI : public ScriptedAI
{
    mob_molten_golemAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
    	pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
    	m_bIsHeroic = pCreature->GetMap()->IsHeroic();
        Reset();
    }

    ScriptedInstance *pInstance;

    bool m_bIsHeroic;
    bool m_bIsFrozen;

    uint32 Blast_Timer;
    uint32 Immolation_Timer;

    void Reset()
    {
        m_bIsFrozen = false;

        Blast_Timer = 20000;
        Immolation_Timer = 5000;
    }

    void AttackStart(Unit* who)
    {
        if (m_bIsFrozen)
            return;

        if (!who || who == m_creature)
            return;

        if (m_creature->Attack(who, true))
        {
            m_creature->SetInCombatWithZone();
            DoStartMovement(who);
        }
    }
/*
    void SpellHit(Unit* pCaster, const SpellEntry* pSpell)
    {
        if (m_bIsFrozen && (pSpell->Id == SPELL_SHATTERING_STOMP_N || pSpell->Id == SPELL_SHATTERING_STOMP_H))
        {
            DoCast(m_creature, m_bIsHeroic ? SPELL_SHATTER_H : SPELL_SHATTER_N);
            //m_creature->ForcedDespawn(); // should they despawn?
        }
    }
*/
    void DamageTaken(Unit* done_by, uint32 &damage)
    {
        if (damage > m_creature->GetHealth() && !m_bIsFrozen)
        {
            m_bIsFrozen = true;
            m_creature->RemoveAllAuras();
            m_creature->InterruptNonMeleeSpells(true);
            m_creature->GetMotionMaster()->Clear(false);
            m_creature->StopMoving();
            m_creature->AttackStop();
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            m_creature->SetStandState(UNIT_STAND_STATE_SIT);
        }
        if (m_bIsFrozen)
        {
            damage = 0;
            return;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // Is frozen
    	if (m_bIsFrozen)
    	    return;

        //Return since we have no target
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

        if(Blast_Timer < uiDiff)
        {
            DoCast(m_creature, SPELL_BLAST_WAVE);
            Blast_Timer = 20000;
        }else Blast_Timer -= uiDiff;

        if(Immolation_Timer < uiDiff)
        {
            DoCast(m_creature->getVictim(), m_bIsHeroic ? SPELL_IMMOLATION_STRIKE_H : SPELL_IMMOLATION_STRIKE_N);
            Immolation_Timer = 5000;
        }else Immolation_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
    void JustDied(Unit* killer) {}
};

CreatureAI* GetAI_mob_molten_golem(Creature *_Creature)
{
    return new mob_molten_golemAI (_Creature);
}

CreatureAI* GetAI_npc_volkhans_anvil(Creature *_Creature)
{
    return new npc_volkhans_anvilAI (_Creature);
}

CreatureAI* GetAI_boss_volkhan(Creature *_Creature)
{
    return new boss_volkhanAI (_Creature);
}

void AddSC_boss_volkhan()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name="boss_volkhan";
    newscript->GetAI = GetAI_boss_volkhan;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_volkhans_anvil";
    newscript->GetAI = GetAI_npc_volkhans_anvil;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="mob_molten_golem";
    newscript->GetAI = GetAI_mob_molten_golem;
    newscript->RegisterSelf();
}
