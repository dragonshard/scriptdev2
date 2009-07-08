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
SDName: Boss Ionar
SDAuthor: ckegg
SD%Complete: 99%
SDComment: Timer check
SDCategory: Halls of Lightning
EndScriptData */

#include "precompiled.h"
#include "def_halls_of_lightning.h"

enum
{
    SAY_AGGRO                              = -1602008,
    SAY_SLAY_1                             = -1602009,
    SAY_SLAY_2                             = -1602010,
    SAY_SLAY_3                             = -1602011,
    SAY_DEATH                              = -1602012,
    SAY_SPLIT_1                            = -1602013,
    SAY_SPLIT_2                            = -1602014,

    SPELL_BALL_LIGHTNING_N                 = 52780,
    SPELL_BALL_LIGHTNING_H                 = 59800,
    SPELL_DISPERSE                         = 52770,
    SPELL_STATIC_OVERLOAD_N                = 52658,
    SPELL_STATIC_OVERLOAD_H                = 59795,

    //Spark of Ionar
    NPC_SPARK_OF_IONAR                     = 28926,
    SPELL_ARCING_BURN_N                    = 52671,
    SPELL_ARCING_BURN_TRIGGER_N            = 52667,
    SPELL_ARCING_BURN_H                    = 59834,
    SPELL_ARCING_BURN_TRIGGER_H            = 59833
};

/*######
## Boss Ionar
######*/
struct MANGOS_DLL_DECL boss_ionarAI : public ScriptedAI
{
    boss_ionarAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
    	pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
    	m_bIsHeroic = pCreature->GetMap()->IsHeroic();
        Reset();
    }

    ScriptedInstance *pInstance;

    std::list<uint64> m_lSparkGUIDList;

    bool m_bIsHeroic;
    bool m_bIsSplited;

    float m_fLastPosition[3];

    uint8 m_uiHealthCheck[4];
    uint8 m_bIsSplitPhase;

    uint32 m_uiStaticOverload_Timer;
    uint32 m_uiBallLightning_Timer;
    uint32 m_uiSplit_Timer;
    uint32 m_uiHealCheck_Timer;

    void Reset()
    {
        m_bIsSplited = false;

        m_fLastPosition[0] = 0;
        m_fLastPosition[1] = 0;
        m_fLastPosition[2] = 0;

        m_bIsSplitPhase = 0;

        m_uiStaticOverload_Timer = 5000 + rand()%1000;
        m_uiBallLightning_Timer = 10000 + rand()%1000;
        m_uiSplit_Timer = 0;
        m_uiHealCheck_Timer = 1000;

        m_uiHealthCheck[0] = 80;
        m_uiHealthCheck[1] = 60;
        m_uiHealthCheck[2] = 40;
        m_uiHealthCheck[3] = 20;

        m_lSparkGUIDList.clear();

        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
        {
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            m_creature->SetDisplayId(27484);
        }

        if(pInstance)
            pInstance->SetData(DATA_IONAR_EVENT, NOT_STARTED);
    }

    void Aggro(Unit* who)
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (!who || m_creature->getVictim())
            return;

        if (who->isTargetableForAttack() && who->isInAccessablePlaceFor(m_creature) && m_creature->IsHostileTo(who))
            AttackStart(who);

        if(pInstance)
            pInstance->SetData(DATA_IONAR_EVENT, IN_PROGRESS);
    }

    void AttackStart(Unit* who)
    {
        if (m_bIsSplited)
            return;

        if (!who || who == m_creature)
            return;

        if (m_creature->Attack(who, true))
        {
            m_creature->SetInCombatWithZone();
            DoStartMovement(who);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // Splited
        if (m_bIsSplited)
        {
            if (m_uiSplit_Timer < uiDiff)
            {
                // Split into 5 sparks
                if (m_bIsSplitPhase == 0)
                {
                    m_creature->InterruptNonMeleeSpells(true);
                    m_creature->SetDisplayId(11686);
                    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    m_creature->GetMotionMaster()->Clear(false);
                    m_creature->StopMoving();
                    m_creature->AttackStop();
                    m_fLastPosition[0] = m_creature->GetPositionX();
                    m_fLastPosition[1] = m_creature->GetPositionY();
                    m_fLastPosition[2] = m_creature->GetPositionZ();

                    for(uint8 i = 0; i < 5; i++)
                        if (Creature* pTemp = m_creature->SummonCreature(NPC_SPARK_OF_IONAR, m_fLastPosition[0]-5+rand()%10, m_fLastPosition[1]-5+rand()%10, m_fLastPosition[2], 0, TEMPSUMMON_TIMED_DESPAWN, 25000))
                        {
                            pTemp->AI()->AttackStart(SelectUnit(SELECT_TARGET_RANDOM, 0));
                            m_lSparkGUIDList.push_back(pTemp->GetGUID());
                        }
                    m_bIsSplitPhase = 1;
                    m_uiSplit_Timer = 16000;
                }
                // Return to where Ionar splited
                else if (m_bIsSplitPhase == 1)
                {
                    if (m_lSparkGUIDList.empty())
                        return;

                    for(std::list<uint64>::iterator itr = m_lSparkGUIDList.begin(); itr != m_lSparkGUIDList.end(); ++itr)
                        if (Creature* pTemp = (Creature*)Unit::GetUnit(*m_creature, *itr))
                            if (pTemp->isAlive())
                            {
                                pTemp->GetMotionMaster()->Clear(false);
                                pTemp->StopMoving();
                                pTemp->SetSpeed(MOVE_RUN, 1.5f);
                	            pTemp->GetMotionMaster()->MovePoint(0, m_fLastPosition[0], m_fLastPosition[1], m_fLastPosition[2]);
                	        }
                    m_bIsSplitPhase = 2;
                    m_uiSplit_Timer = 3500;
                }
                // Lightning effect and restore Ionar
                else if (m_bIsSplitPhase == 2)
                {
                    if (m_lSparkGUIDList.empty())
                        return;

                    for(std::list<uint64>::iterator itr = m_lSparkGUIDList.begin(); itr != m_lSparkGUIDList.end(); ++itr)
                        if (Creature* pTemp = (Creature*)Unit::GetUnit(*m_creature, *itr))
                            if (pTemp->isAlive())
                	            pTemp->CastSpell(pTemp, 47693, true);

                    m_creature->SetDisplayId(27484);
                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    m_creature->CastSpell(m_creature, 47693, true);

                    m_bIsSplitPhase = 3;
                    m_uiSplit_Timer = 500;
                }
                // despawn sparks
                else if (m_bIsSplitPhase == 3)
                {
                    DespawnSpark();
                    m_bIsSplited = false;
                    m_bIsSplitPhase = 0;
                    m_uiSplit_Timer = 0;
                }
            }else m_uiSplit_Timer -= uiDiff;
            return;
        }

        //Return since we have no target
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

        if(m_uiStaticOverload_Timer < uiDiff)
        {
            DoCast(SelectUnit(SELECT_TARGET_RANDOM, 0), m_bIsHeroic ? SPELL_STATIC_OVERLOAD_H : SPELL_STATIC_OVERLOAD_N);
            m_uiStaticOverload_Timer = 5000 + rand()%1000;
        }else m_uiStaticOverload_Timer -= uiDiff;

        if(m_uiBallLightning_Timer < uiDiff)
        {
            DoCast(m_creature->getVictim(), m_bIsHeroic ? SPELL_BALL_LIGHTNING_H : SPELL_BALL_LIGHTNING_N);
            m_uiBallLightning_Timer = 10000 + rand()%1000;
        }else m_uiBallLightning_Timer -= uiDiff;

        // Health check -----------------------------------------------------------------------------
        if (m_uiHealCheck_Timer < uiDiff)
        {
            for(uint8 i = 0; i < 4; i++)
            {
                if (m_uiHealthCheck[i] && (m_creature->GetHealth()*100 / m_creature->GetMaxHealth()) <= m_uiHealthCheck[i])
                {
                    switch(rand()%2)
                    {
                        case 0: DoScriptText(SAY_SPLIT_1, m_creature);break;
                        case 1: DoScriptText(SAY_SPLIT_2, m_creature);break;
                    }
                    m_creature->InterruptNonMeleeSpells(true);
                    DoCast(m_creature, SPELL_DISPERSE);
                    m_bIsSplited = true;
                    m_bIsSplitPhase = 0;
                    m_uiSplit_Timer = 3000;
                    m_uiHealthCheck[i] = 0; // deactive
                }
            }
            m_uiHealCheck_Timer = 1000;
        } else m_uiHealCheck_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* killer)
    {
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
        {
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            m_creature->SetDisplayId(27484);
        }

        DoScriptText(SAY_DEATH, m_creature);
        DespawnSpark();

        if(pInstance)
            pInstance->SetData(DATA_IONAR_EVENT, DONE);
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

    void DespawnSpark()
    {
        if (m_lSparkGUIDList.empty())
            return;

        for(std::list<uint64>::iterator itr = m_lSparkGUIDList.begin(); itr != m_lSparkGUIDList.end(); ++itr)
        {
        	if (Creature* pTemp = (Creature*)Unit::GetUnit(*m_creature, *itr))
            {
                if (pTemp->isAlive())
                    pTemp->ForcedDespawn();
            }
        }

        m_lSparkGUIDList.clear();
    }
};

/*######
## NPC Spark of Ionar
######*/
struct MANGOS_DLL_DECL npc_spark_of_ionarAI : public ScriptedAI
{
    npc_spark_of_ionarAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
    	m_bIsHeroic = pCreature->GetMap()->IsHeroic();
        Reset();
    }

    bool m_bIsHeroic;

    void Reset()
    {
        m_creature->SetSpeed(MOVE_RUN, 0.5f);
        m_creature->CastSpell(m_creature, m_bIsHeroic ? SPELL_ARCING_BURN_TRIGGER_H : SPELL_ARCING_BURN_TRIGGER_N, true);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void AttackStart(Unit* who)
    {
        if (!who || who == m_creature)
            return;

        if (m_creature->Attack(who, false))
        {
            m_creature->AddThreat(who, 0.0f);
            m_creature->SetInCombatWith(who);
            m_creature->GetMotionMaster()->MoveChase(who);
        }
    }
    void UpdateAI(const uint32 uiDiff) {}
    void JustDied(Unit* killer) {}
};

CreatureAI* GetAI_boss_ionar(Creature *_Creature)
{
    return new boss_ionarAI (_Creature);
}

CreatureAI* GetAI_npc_spark_of_ionar(Creature *_Creature)
{
    return new npc_spark_of_ionarAI (_Creature);
}

void AddSC_boss_ionar()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name="boss_ionar";
    newscript->GetAI = GetAI_boss_ionar;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_spark_of_ionar";
    newscript->GetAI = GetAI_npc_spark_of_ionar;
    newscript->RegisterSelf();
}
