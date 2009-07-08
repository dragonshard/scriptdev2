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
SDName: boss_ymiron
SDAuthor: ckegg
SD%Complete: 95
SDComment: Check timer
SDCategory: Utgarde Pinnacle
EndScriptData */

#include "precompiled.h"
#include "def_pinnacle.h"

enum
{
    SAY_AGGRO                               = -1575028,
    SAY_SLAY_1                              = -1575029,
    SAY_SLAY_2                              = -1575030,
    SAY_SLAY_3                              = -1575031,
    SAY_SLAY_4                              = -1575032,
    SAY_DEATH                               = -1575033,
    SAY_SUMMON_BJORN                        = -1575034,
    SAY_SUMMON_HALDOR                       = -1575035,
    SAY_SUMMON_RANULF                       = -1575036,
    SAY_SUMMON_TORGYN                       = -1575037,

    SPELL_BANE_N                            = 48294,
    SPELL_BANE_H                            = 59301,
    SPELL_DARK_SLASH                        = 48292,
    SPELL_FETID_ROT_N                       = 48291,
    SPELL_FETID_ROT_H                       = 59300,
    SPELL_SCREAMS_OF_THE_DEAD               = 51750,
    SPELL_SPIRIT_BURST_N                    = 48529, // when Ranulf
    SPELL_SPIRIT_BURST_H                    = 59305, // when Ranulf
    SPELL_SPIRIT_STRIKE_N                   = 48423, // when Haldor
    SPELL_SPIRIT_STRIKE_H                   = 59304, // when Haldor
    SPELL_ANCESTORS_VENGEANCE               = 16939, // 2 normal, 4 heroic

    SPELL_SUMMON_AVENGING_SPIRIT            = 48592,
    SPELL_SUMMON_SPIRIT_FOUNT               = 48386,

    SPELL_CHANNEL_SPIRIT_TO_YMIRON          = 48316,
    SPELL_CHANNEL_YMIRON_TO_SPIRIT          = 48307,

    SPELL_SPIRIT_FOUNT_N                    = 48380,
    SPELL_SPIRIT_FOUNT_H                    = 59320,

    NPC_BJORN                               = 27303, // Near Right Boat, summon Spirit Fount
    NPC_BJORN_VISUAL                        = 27304,
    NPC_HALDOR                              = 27307, // Near Left Boat, debuff Spirit Strike on player
    NPC_HALDOR_VISUAL                       = 27310,
    NPC_RANULF                              = 27308, // Far Left Boat, ability to cast spirit burst
    NPC_RANULF_VISUAL                       = 27311,
    NPC_TORGYN                              = 27309, // Far Right Boat, summon 4 Avenging Spirit
    NPC_TORGYN_VISUAL                       = 27312,

    NPC_SPIRIT_FOUNT                        = 27339,
    NPC_AVENGING_SPIRIT                     = 27386
};

struct ActiveBoatStruct
{
    uint32 npc;
    uint32 say;
    float MoveX,MoveY,MoveZ,SpawnX,SpawnY,SpawnZ,SpawnO;
};

static ActiveBoatStruct ActiveBot[4] =
{
    {NPC_BJORN_VISUAL,  SAY_SUMMON_BJORN,  404.379, -335.335, 104.756, 413.594, -335.408, 107.995, 3.157},
    {NPC_HALDOR_VISUAL, SAY_SUMMON_HALDOR, 380.813, -335.069, 104.756, 369.994, -334.771, 107.995, 6.232},
    {NPC_RANULF_VISUAL, SAY_SUMMON_RANULF, 381.546, -314.362, 104.756, 370.841, -314.426, 107.995, 6.232},
    {NPC_TORGYN_VISUAL, SAY_SUMMON_TORGYN, 404.310, -314.761, 104.756, 413.992, -314.703, 107.995, 3.157}
};

struct MANGOS_DLL_DECL boss_ymironAI : public ScriptedAI
{
    boss_ymironAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
    	m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
    	m_bIsHeroic = pCreature->GetMap()->IsHeroic();

        srand(time(NULL));
        for (int i=0; i<4; i++)
            m_uiActiveOrder[i] = i;
        for (int i=0; i<3; i++)
        {
            int r = i + (rand()%(4-i));
            int temp = m_uiActiveOrder[i];
            m_uiActiveOrder[i] = m_uiActiveOrder[r];
            m_uiActiveOrder[r] = temp;
        }

        SpellEntry* TempSpell = (SpellEntry*)GetSpellStore()->LookupEntry(SPELL_CHANNEL_SPIRIT_TO_YMIRON);
        if (TempSpell && TempSpell->EffectImplicitTargetB[0] != 16)
        {
               TempSpell->EffectImplicitTargetA[0] = 53;
               TempSpell->EffectImplicitTargetA[1] = 0;
               TempSpell->EffectImplicitTargetB[0] = 16;
               TempSpell->EffectImplicitTargetB[1] = 0;
        }
        Reset();
    }

    ScriptedInstance *m_pInstance;

    bool m_bIsHeroic;
    bool m_bIsPause;
    bool m_bIsActiveWithBJORN;
    bool m_bIsActiveWithHALDOR;
    bool m_bIsActiveWithRANULF;
    bool m_bIsActiveWithTORGYN;

    uint8 m_uiHealthCheck[4];
    uint8 m_uiActiveOrder[4];
    uint8 m_uiActivedNumber;

    uint32 m_uiFetidRot_Timer;
    uint32 m_uiBane_Timer;
    uint32 m_uiDarkSlash_Timer;
    uint32 m_uiAncestors_Vengeance_Timer;

    uint32 m_uiAbility_BJORN_Timer;
    uint32 m_uiAbility_HALDOR_Timer;
    uint32 m_uiAbility_RANULF_Timer;
    uint32 m_uiAbility_TORGYN_Timer;

    uint32 m_uiHealCheck_Timer;
    uint32 m_uiPause_Timer;

    uint64 m_uiActivedCreatureGUID;
    uint64 m_uiOrbGUID;

    void Reset()
    {
        m_bIsPause = false;
        m_bIsActiveWithBJORN = false;
        m_bIsActiveWithHALDOR = false;
        m_bIsActiveWithRANULF = false;
        m_bIsActiveWithTORGYN = false;

        m_uiFetidRot_Timer            = 8000+rand()%5000;
        m_uiBane_Timer                = 18000+rand()%5000;
        m_uiDarkSlash_Timer           = 28000+rand()%5000;
        m_uiAncestors_Vengeance_Timer = (m_bIsHeroic ? 60000 : 45000);
        m_uiHealCheck_Timer           = 1000;
        m_uiPause_Timer               = 0;

        m_uiAbility_BJORN_Timer  = 0;
        m_uiAbility_HALDOR_Timer = 0;
        m_uiAbility_RANULF_Timer = 0;
        m_uiAbility_TORGYN_Timer = 0;

        m_uiActivedNumber        = 0;

        DespawnBoatGhosts(m_uiActivedCreatureGUID);
        DespawnBoatGhosts(m_uiOrbGUID);

        if (!m_bIsHeroic)
        {
            m_uiHealthCheck[0] = 75;
            m_uiHealthCheck[1] = 50;
            m_uiHealthCheck[2] = 25;
            m_uiHealthCheck[3] = 0;
        }
        else
        {
            m_uiHealthCheck[0] = 80;
            m_uiHealthCheck[1] = 60;
            m_uiHealthCheck[2] = 40;
            m_uiHealthCheck[3] = 20;
        }

        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        if(m_pInstance)
            m_pInstance->SetData(DATA_KING_YMIRON_EVENT, NOT_STARTED);
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (!pWho || m_creature->getVictim())
            return;

        if (pWho->isTargetableForAttack() && pWho->isInAccessablePlaceFor(m_creature) && m_creature->IsHostileTo(pWho))
            AttackStart(pWho);

        if(m_pInstance)
            m_pInstance->SetData(DATA_KING_YMIRON_EVENT, IN_PROGRESS);
    }

    void AttackStart(Unit* pWho)
    {
        if (m_bIsPause)
            return;

        if (!pWho || pWho == m_creature)
            return;

        if (m_creature->Attack(pWho, true))
        {
            m_creature->SetInCombatWithZone();
            DoStartMovement(pWho);
        }
    }

    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if(uiType != POINT_MOTION_TYPE)
                return;

        switch(uiPointId)
        {
            case 0:
                DoScriptText(ActiveBot[m_uiActiveOrder[m_uiActivedNumber]].say, m_creature);
                DoCast(m_creature, SPELL_CHANNEL_YMIRON_TO_SPIRIT); // should be on spirit
                if (Creature* pTemp = m_creature->SummonCreature(ActiveBot[m_uiActiveOrder[m_uiActivedNumber]].npc, ActiveBot[m_uiActiveOrder[m_uiActivedNumber]].SpawnX, ActiveBot[m_uiActiveOrder[m_uiActivedNumber]].SpawnY, ActiveBot[m_uiActiveOrder[m_uiActivedNumber]].SpawnZ, ActiveBot[m_uiActiveOrder[m_uiActivedNumber]].SpawnO, TEMPSUMMON_CORPSE_DESPAWN, 0))
                {
                    m_uiActivedCreatureGUID = pTemp->GetGUID();
                    pTemp->CastSpell(m_creature, SPELL_CHANNEL_SPIRIT_TO_YMIRON, true);
                    pTemp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    pTemp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    pTemp->SetMonsterMoveFlags(MONSTER_MOVE_LEVITATING);
                    switch(m_uiActiveOrder[m_uiActivedNumber])
                    {
                        case 0: m_bIsActiveWithBJORN  = true; break;
                        case 1: m_bIsActiveWithHALDOR = true; break;
                        case 2: m_bIsActiveWithRANULF = true; break;
                        case 3: m_bIsActiveWithTORGYN = true; break;
                    }
                }
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                m_uiPause_Timer = 5000;
                break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_bIsPause)
        {
            if (m_uiPause_Timer < uiDiff)
            {
                m_uiAbility_BJORN_Timer = 5000;
                m_uiAbility_HALDOR_Timer = 5000;
                m_uiAbility_RANULF_Timer = 5000;
                m_uiAbility_TORGYN_Timer = 5000;

                m_bIsPause = false;
                m_uiPause_Timer = 0;
            } else m_uiPause_Timer -= uiDiff;
            return;
        }

        //Return since we have no target
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

        if (!m_bIsPause)
        {
            // Normal spells ------------------------------------------------------------------------
            if (m_uiBane_Timer < uiDiff)
            {
                DoCast(m_creature, m_bIsHeroic ? SPELL_BANE_H : SPELL_BANE_N);
                m_uiBane_Timer = 20000+rand()%5000;
            } else m_uiBane_Timer -= uiDiff;

            if (m_uiFetidRot_Timer < uiDiff)
            {
                DoCast(m_creature->getVictim(), m_bIsHeroic ? SPELL_FETID_ROT_H : SPELL_FETID_ROT_N);
                m_uiFetidRot_Timer = 10000+rand()%5000;
            } else m_uiFetidRot_Timer -= uiDiff;

            if (m_uiDarkSlash_Timer < uiDiff)
            {
                //DoCast(m_creature->getVictim(), SPELL_DARK_SLASH); // not working
                int32 dmg = m_creature->getVictim()->GetHealth() / 50; // workaround for damage
                m_creature->CastCustomSpell(m_creature->getVictim(), SPELL_DARK_SLASH, &dmg, 0, 0, false);
                m_uiDarkSlash_Timer = 30000+rand()%5000;
            } else m_uiDarkSlash_Timer -= uiDiff;

            if (m_uiAncestors_Vengeance_Timer < uiDiff)
            {
                DoCast(m_creature, SPELL_ANCESTORS_VENGEANCE);
                m_uiAncestors_Vengeance_Timer = (m_bIsHeroic ? 60000+rand()%5000 : 45000+rand()%5000);
            } else m_uiAncestors_Vengeance_Timer -= uiDiff;

            // Abilities ------------------------------------------------------------------------------
            if (m_bIsActiveWithBJORN && m_uiAbility_BJORN_Timer < uiDiff)
            {
                //DoCast(m_creature, SPELL_SUMMON_SPIRIT_FOUNT); // works fine, but using summon has better control
                if (Creature* pTemp = m_creature->SummonCreature(NPC_SPIRIT_FOUNT, 385+rand()%10, -330+rand()%10, 104.756, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 180000))
                {
                    pTemp->SetSpeed(MOVE_RUN, 0.4f);
                    pTemp->CastSpell(pTemp, m_bIsHeroic ? SPELL_SPIRIT_FOUNT_H : SPELL_SPIRIT_FOUNT_N, true);
                    pTemp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    pTemp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    pTemp->SetDisplayId(11686);
                    pTemp->GetMotionMaster()->MoveChase(m_creature->getVictim());
                    m_uiOrbGUID = pTemp->GetGUID();
                }
                m_bIsActiveWithBJORN = false; // only one orb
            } else m_uiAbility_BJORN_Timer -= uiDiff;

            if (m_bIsActiveWithHALDOR && m_uiAbility_HALDOR_Timer < uiDiff)
            {
                DoCast(m_creature->getVictim(), m_bIsHeroic ? SPELL_SPIRIT_STRIKE_H : SPELL_SPIRIT_STRIKE_N);
                m_uiAbility_HALDOR_Timer = 5000; // overtime
            } else m_uiAbility_HALDOR_Timer -= uiDiff;

            if (m_bIsActiveWithRANULF && m_uiAbility_RANULF_Timer < uiDiff)
            {
                DoCast(m_creature, m_bIsHeroic ? SPELL_SPIRIT_BURST_H : SPELL_SPIRIT_BURST_N);
                m_uiAbility_RANULF_Timer = 10000; // overtime
            } else m_uiAbility_RANULF_Timer -= uiDiff;

            if (m_bIsActiveWithTORGYN && m_uiAbility_TORGYN_Timer < uiDiff)
            {
                float x,y,z;
                x = m_creature->GetPositionX()-5;
                y = m_creature->GetPositionY()-5;
                z = m_creature->GetPositionZ();
                for(uint8 i = 0; i < 4; ++i)
                {
                    //DoCast(m_creature, SPELL_SUMMON_AVENGING_SPIRIT); // works fine, but using summon has better control
                    if (Creature* pTemp = m_creature->SummonCreature(NPC_AVENGING_SPIRIT, x+rand()%10, y+rand()%10, z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000))
                    {
                        pTemp->SetInCombatWithZone();
                        pTemp->AI()->AttackStart(SelectUnit(SELECT_TARGET_RANDOM, 0));
                    }
                }
                m_uiAbility_TORGYN_Timer = 15000; // overtime
            } else m_uiAbility_TORGYN_Timer -= uiDiff;

            // Health check -----------------------------------------------------------------------------
            if (m_uiHealCheck_Timer < uiDiff)
            {
                for(uint8 i = 0; i < 4; i++)
                {
                    if (!m_bIsPause && m_uiHealthCheck[i] && (m_creature->GetHealth()*100 / m_creature->GetMaxHealth()) <= m_uiHealthCheck[i])
                    {
                        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        m_creature->InterruptNonMeleeSpells(true);
                        DoCast(m_creature, SPELL_SCREAMS_OF_THE_DEAD);
                        m_creature->GetMotionMaster()->Clear(false);
                        m_creature->StopMoving();
                        m_creature->AttackStop();
                        m_creature->GetMotionMaster()->MovePoint(0, ActiveBot[m_uiActiveOrder[i]].MoveX, ActiveBot[m_uiActiveOrder[i]].MoveY, ActiveBot[m_uiActiveOrder[i]].MoveZ);

                        DespawnBoatGhosts(m_uiActivedCreatureGUID);
                        DespawnBoatGhosts(m_uiOrbGUID);

                        m_bIsActiveWithBJORN  = false;
                        m_bIsActiveWithHALDOR = false;
                        m_bIsActiveWithRANULF = false;
                        m_bIsActiveWithTORGYN = false;

                        m_uiBane_Timer                += 8000;
                        m_uiFetidRot_Timer            += 8000;
                        m_uiDarkSlash_Timer           += 8000;
                        m_uiAncestors_Vengeance_Timer += 8000;

                        m_uiActivedNumber = i;
                        m_uiHealthCheck[i] = 0; // deactive
                        m_bIsPause = true;
                        return;
                    }
                }
                m_uiHealCheck_Timer = 1000;
            } else m_uiHealCheck_Timer -= uiDiff;

            DoMeleeAttackIfReady();
        }
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);
        DespawnBoatGhosts(m_uiActivedCreatureGUID);
        DespawnBoatGhosts(m_uiOrbGUID);

        if(m_pInstance)
            m_pInstance->SetData(DATA_KING_YMIRON_EVENT, DONE);
    }

    void KilledUnit(Unit* pVictim)
    {
        if(pVictim == m_creature)
            return;
        switch(rand()%4)
        {
            case 0: DoScriptText(SAY_SLAY_1, m_creature);break;
            case 1: DoScriptText(SAY_SLAY_2, m_creature);break;
            case 2: DoScriptText(SAY_SLAY_3, m_creature);break;
            case 3: DoScriptText(SAY_SLAY_4, m_creature);break;
        }
    }

    void DespawnBoatGhosts(uint64& m_uiCreatureGUID)
    {
        if (m_uiCreatureGUID)
            if (Creature* pTemp = (Creature*)Unit::GetUnit(*m_creature, m_uiCreatureGUID))
                pTemp->ForcedDespawn();
        m_uiCreatureGUID = 0;
    }
};

CreatureAI* GetAI_boss_ymiron(Creature* pCreature)
{
    return new boss_ymironAI (pCreature);
}

void AddSC_boss_ymiron()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name="boss_ymiron";
    newscript->GetAI = GetAI_boss_ymiron;
    newscript->RegisterSelf();
}
