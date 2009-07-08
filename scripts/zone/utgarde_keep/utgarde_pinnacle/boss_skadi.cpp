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
SDName: Boss skadi
SDAuthor: ckegg
SD%Complete: 50%
SDComment: Missing breath
SDCategory: Utgarde Pinnacle
EndScriptData */

#include "precompiled.h"
#include "def_pinnacle.h"

//Phase 0 "gauntlet even" Skadi on a flying mount, waves of adds charging to the group periodicaly carrying harpoons
//Phase 1 Kill the Skadi drake mount with harppons launcher
//Phase 2 Kill the Skadi

enum
{
    SAY_AGGRO                             = -1575004,
    SAY_KILL_1                            = -1575005,
    SAY_KILL_2                            = -1575006,
    SAY_KILL_3                            = -1575007,
    SAY_DEATH                             = -1575008,
    SAY_DRAKE_DEATH                       = -1575009,
    SAY_DRAKE_HIT_1                       = -1575010,
    SAY_DRAKE_HIT_2                       = -1575011,
    SAY_DRAKE_BREATH_1                    = -1575012,
    SAY_DRAKE_BREATH_2                    = -1575013,
    SAY_DRAKE_BREATH_3                    = -1575014,

    SPELL_CRUSH_N                         = 50234,
    SPELL_CRUSH_H                         = 59330,
    SPELL_POISONED_SPEAR_N                = 50225,
    SPELL_POISONED_SPEAR_H                = 59331,
    SPELL_WHIRLWIND_N                     = 50228, //random target,  but not the tank approx. every 20s
    SPELL_WHIRLWIND_H                     = 59332, //random target,  but not the tank approx. every 20s

    NPC_YMIRJAR_WARRIOR                   = 26690,
    NPC_YMIRJAR_WITCH_DOCTOR              = 26691,
    NPC_YMIRJAR_HARPOONER                 = 26692
};

struct Locations
{
    float x, y, z;
    uint32 id;
};

static Locations SpawnLoc[]=
{
    {340.556, -511.493, 104.352},
    {367.741, -512.865, 104.828},
    {399.546, -512.755, 104.834},
    {430.551, -514.320, 105.055},
    {468.931, -513.555, 104.723}
};

struct MANGOS_DLL_DECL boss_skadiAI : public ScriptedAI
{
    boss_skadiAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
    	m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
    	m_bIsHeroic = pCreature->GetMap()->IsHeroic();
        Reset();
    }

    ScriptedInstance *m_pInstance;

    bool m_bIsHeroic;

    uint8 m_uiphase;
    uint8 m_uiSpawn_counter;

    uint32 m_uiWaypointId;

    uint32 m_uiCrush_timer;
    uint32 m_uiSpear_timer;
    uint32 m_uiWhirlwind_timer;
    uint32 m_uiMoveNext_Timer;
    uint32 m_uiPlayerCheck_Timer;

    uint64 m_uiGrauf;

    void Reset()
    {
        m_uiphase = 0;
        m_uiSpawn_counter = 0;

        m_uiWaypointId = 0;
        m_uiCrush_timer = 8000;
        m_uiSpear_timer = 10000;
        m_uiWhirlwind_timer = 20000;
        m_uiMoveNext_Timer = 25000;
        m_uiPlayerCheck_Timer = 1000;

        m_creature->Unmount();
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        if(m_pInstance)
            m_pInstance->SetData(DATA_SKADI_THE_RUTHLESS_EVENT, NOT_STARTED);
    }

    void AttackStart(Unit* pWho)
    {
        if (m_uiphase < 2)
            return;

        if (!pWho || pWho == m_creature)
            return;

        if (m_creature->Attack(pWho, true))
        {
            //m_creature->SetInCombatWithZone();
            DoStartMovement(pWho);
        }
    }

    void MoveInLineOfSight(Unit* pWho)
    {
    	if (!pWho)
    	    return;

        if (pWho->isTargetableForAttack() && pWho->isInAccessablePlaceFor(m_creature) && m_creature->IsHostileTo(pWho) &&
        	!m_uiphase && pWho->GetTypeId() == TYPEID_PLAYER && m_creature->IsWithinDistInMap(pWho, 20))
        {
            if(m_pInstance)
                m_pInstance->SetData(DATA_SKADI_THE_RUTHLESS_EVENT, IN_PROGRESS);

            DoScriptText(SAY_AGGRO, m_creature);

            m_creature->SetInCombatWithZone();
            m_creature->Mount(27043);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            if (m_creature->HasMonsterMoveFlag(MONSTER_MOVE_WALK))
                m_creature->RemoveMonsterMoveFlag(MONSTER_MOVE_WALK);
            m_creature->GetMotionMaster()->MovePoint(m_uiWaypointId, 340.259, -510.541, 120.869);
        	m_uiphase = 1;
        }
	}

    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if(uiType != POINT_MOTION_TYPE)
                return;

        if ((m_uiSpawn_counter >= 4 && !m_bIsHeroic) || (m_uiSpawn_counter >= 5 && m_bIsHeroic))
        {
            m_uiWaypointId = 200;
            m_uiMoveNext_Timer = 3000;
        }
        else
        {
            switch(uiPointId)
            {
                case 0:
                    SpawnMobs(m_uiSpawn_counter);
                    m_uiWaypointId = 1;
                    m_uiSpawn_counter++;
                    m_uiMoveNext_Timer = 3000;
                    break;
                case 1:
                    SpawnMobs(m_uiSpawn_counter);
                    m_uiWaypointId = 0;
                    m_uiSpawn_counter++;
                    m_uiMoveNext_Timer = 3000;
                    break;
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiphase == 0)
            return;
        // Flying & adds
        else if (m_uiphase == 1)
        {
            if (m_uiPlayerCheck_Timer < uiDiff)
            {
                Map *map = m_creature->GetMap();
                if (map->IsDungeon() && m_pInstance->GetData(DATA_SKADI_THE_RUTHLESS_EVENT) == IN_PROGRESS)
                {
                    Map::PlayerList const &PlayerList = map->GetPlayers();

                    if (PlayerList.isEmpty())
                        return;

                    bool bIsAlive = false;
                    for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                        if (i->getSource()->isAlive() && i->getSource()->isTargetableForAttack())
                        	 bIsAlive = true;

                    if (!bIsAlive)
                    {
                        m_creature->GetMotionMaster()->Clear(false);
                        m_creature->StopMoving();
                        EnterEvadeMode();
                        return;
                    }
                }
                m_uiPlayerCheck_Timer = 1000;
            } else m_uiPlayerCheck_Timer -= uiDiff;

            if (m_uiMoveNext_Timer < uiDiff)
            {
                switch(m_uiWaypointId)
                {
                    case 0: m_creature->GetMotionMaster()->MovePoint(m_uiWaypointId, 340.259, -510.541, 120.869); break;
                    case 1: m_creature->GetMotionMaster()->MovePoint(m_uiWaypointId, 472.977, -513.636, 120.869); break;
                    case 200:
                        m_creature->GetMotionMaster()->Clear();
                        m_creature->Unmount();
                        m_uiphase = 2;
                        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        AttackStart(SelectUnit(SELECT_TARGET_RANDOM, 0));
                    break;
                }
                m_uiMoveNext_Timer = 25000; // prevent stuck
            } else m_uiMoveNext_Timer -= uiDiff;
        }
        // Land & attack
        else if (m_uiphase == 2)
        {
            //Return since we have no target
            if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            {
                EnterEvadeMode();
                return;
            }

            if (m_uiCrush_timer < uiDiff)
            {
                DoCast(m_creature->getVictim(), m_bIsHeroic ? SPELL_CRUSH_H : SPELL_CRUSH_N);
                m_uiCrush_timer = 8000;
            } else m_uiCrush_timer -= uiDiff;

            if (m_uiSpear_timer < uiDiff)
            {
                DoCast(SelectUnit(SELECT_TARGET_RANDOM, 0), m_bIsHeroic ? SPELL_POISONED_SPEAR_H : SPELL_POISONED_SPEAR_N);
                m_uiSpear_timer = 10000;
            } else m_uiSpear_timer -= uiDiff;

            if (m_uiWhirlwind_timer < uiDiff)
            {
                DoCast(SelectUnit(SELECT_TARGET_RANDOM, 0), m_bIsHeroic ? SPELL_WHIRLWIND_H : SPELL_WHIRLWIND_N);
                m_uiWhirlwind_timer = 20000;
            } else m_uiWhirlwind_timer -= uiDiff;

            DoMeleeAttackIfReady();
        }
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if(m_pInstance)
            m_pInstance->SetData(DATA_SKADI_THE_RUTHLESS_EVENT, DONE);
    }

    void KilledUnit(Unit* pVictim)
    {
        if(pVictim == m_creature)
            return;
        switch(rand()%3)
        {
            case 0: DoScriptText(SAY_KILL_1, m_creature);break;
            case 1: DoScriptText(SAY_KILL_2, m_creature);break;
            case 2: DoScriptText(SAY_KILL_3, m_creature);break;
        }
    }

    void SpawnMobs(uint32 uiSpot)
    {
        uint8 maxSpawn = (m_bIsHeroic ? 6 : 5);
        for(uint8 i = 0; i < maxSpawn; ++i)
        {
            Creature* pTemp;
            switch (rand()%3)
            {
                case 0: pTemp = m_creature->SummonCreature(NPC_YMIRJAR_WARRIOR, SpawnLoc[uiSpot].x+rand()%5, SpawnLoc[uiSpot].y+rand()%5, SpawnLoc[uiSpot].z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000); break;
                case 1: pTemp = m_creature->SummonCreature(NPC_YMIRJAR_WITCH_DOCTOR, SpawnLoc[uiSpot].x+rand()%5, SpawnLoc[uiSpot].y+rand()%5, SpawnLoc[uiSpot].z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000); break;
                case 2: pTemp = m_creature->SummonCreature(NPC_YMIRJAR_HARPOONER, SpawnLoc[uiSpot].x+rand()%5, SpawnLoc[uiSpot].y+rand()%5, SpawnLoc[uiSpot].z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000); break;
            }
            if (pTemp)
            {
                pTemp->SetInCombatWithZone();
                //pTemp->AI()->AttackStart(pPlayer);
                pTemp->AI()->AttackStart(SelectUnit(SELECT_TARGET_RANDOM, 0));
            }
        }
    }
};

CreatureAI* GetAI_boss_skadi(Creature* pCreature)
{
    return new boss_skadiAI (pCreature);
}

void AddSC_boss_skadi()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name="boss_skadi";
    newscript->GetAI = GetAI_boss_skadi;
    newscript->RegisterSelf();
}
