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
SDName: Boss General Bjarngrim
SDAuthor: ckegg
SD%Complete: 90%
SDComment: Waypoint needed.
SDCategory: Halls of Lightning
EndScriptData */

#include "precompiled.h"
#include "def_halls_of_lightning.h"

enum
{
    //Yell
    SAY_AGGRO                                = -1602000,
    SAY_SLAY_1                               = -1602001,
    SAY_SLAY_2                               = -1602002,
    SAY_SLAY_3                               = -1602003,
    SAY_DEATH                                = -1602004,
    SAY_BATTLE_STANCE                        = -1602005,
    SAY_BERSEKER_STANCE                      = -1602006,
    SAY_DEFENSIVE_STANCE                     = -1602007,

    SPELL_DEFENSIVE_STANCE                   = 53790,
    SPELL_DEFENSIVE_AURA                     = 41105,
    SPELL_SPELL_REFLECTION                   = 36096,
    SPELL_PUMMEL                             = 12555,
    SPELL_KNOCK_AWAY                         = 52029,
    SPELL_IRONFORM                           = 52022,

    SPELL_BERSEKER_STANCE                    = 53791,
    SPELL_BERSEKER_AURA                      = 41107,
    SPELL_INTERCEPT                          = 58769,
    SPELL_WHIRLWIND                          = 52027,
    SPELL_CLEAVE                             = 15284,

    SPELL_BATTLE_STANCE                      = 53792,
    SPELL_BATTLE_AURA                        = 41106,
    SPELL_MORTAL_STRIKE                      = 16856,
    SPELL_SLAM                               = 52026,

    //OTHER SPELLS
    SPELL_CHARGE_UP                          = 52098,
    SPELL_TEMPORARY_ELECTRICAL_CHARGE        = 52092,

    NPC_STORMFORGED_LIEUTENANT               = 29240,
    SPELL_ARC_WELD                           = 59085,
    SPELL_RENEW_STEEL_N                      = 52774,
    SPELL_RENEW_STEEL_H                      = 59160
};

/*######
## Boss General Bjarngrim
######*/
struct MANGOS_DLL_DECL boss_bjarngrimAI : public ScriptedAI
{
    boss_bjarngrimAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        m_bIsHeroic = pCreature->GetMap()->IsHeroic();

        SpellEntry* TempSpell = (SpellEntry*)GetSpellStore()->LookupEntry(SPELL_RENEW_STEEL_H); // Use script target
        if (TempSpell && TempSpell->EffectImplicitTargetB[0] != 38)
        {
               TempSpell->EffectImplicitTargetB[0] = 38;
               TempSpell->EffectImplicitTargetB[1] = 0;
        }

        Reset();
    }

    ScriptedInstance *pInstance;

    bool m_bIsHeroic;
    bool IsChangingStance;

    uint8 m_uiChargingStatus;
    uint8 m_uiStance;

    uint32 m_uiCharge_Timer;
    uint32 m_uiChangeStance_Timer;

    uint32 m_uiReflection_Timer;
    uint32 m_uiKnockAway_Timer;
    uint32 m_uiPummel_Timer;
    uint32 m_uiIronform_Timer;

    uint32 m_uiIntercept_Timer;
    uint32 m_uiWhirlwind_Timer;
    uint32 m_uiCleave_Timer;

    uint32 m_uiMortalStrike_Timer;
    uint32 m_uiSlam_Timer;

    uint64 m_uiStormforgedLieutenantGUID[2];

    void Reset()
    {
        IsChangingStance = false;

        m_uiChargingStatus = 0;
        m_uiCharge_Timer = 1000;

        m_uiStance = 0;
        m_uiChangeStance_Timer = 20000 + rand()%5000;

        m_uiReflection_Timer = 8000;
        m_uiKnockAway_Timer = 20000;
        m_uiPummel_Timer = 10000;
        m_uiIronform_Timer = 25000;

        m_uiIntercept_Timer = 5000;
        m_uiWhirlwind_Timer = 10000;
        m_uiCleave_Timer = 8000;

        m_uiMortalStrike_Timer = 8000;
        m_uiSlam_Timer = 10000;

        for(uint8 i = 0; i<2; i++)
        {
            if (Creature* pStormforgedLieutenant = ((Creature*)Unit::GetUnit((*m_creature), m_uiStormforgedLieutenantGUID[i])))
            {
                if (!pStormforgedLieutenant->isAlive())
                    pStormforgedLieutenant->Respawn();
                pStormforgedLieutenant->GetMotionMaster()->MoveChase(m_creature);
            }
            else if (Creature* pStormforgedLieutenant = m_creature->SummonCreature(NPC_STORMFORGED_LIEUTENANT, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000))
            {
                m_uiStormforgedLieutenantGUID[i] = pStormforgedLieutenant->GetGUID();
                pStormforgedLieutenant->GetMotionMaster()->MoveChase(m_creature);
            }
        }

        DoCast(m_creature, SPELL_DEFENSIVE_AURA);
        DoCast(m_creature, SPELL_DEFENSIVE_STANCE);
        m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 0, uint32(33609));
        m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 1, uint32(39288));

        if(pInstance)
            pInstance->SetData(DATA_BJARNGRIM_EVENT, NOT_STARTED);
    }

    void Aggro(Unit* who)
    {
        DoScriptText(SAY_AGGRO, m_creature);
        m_creature->SetInCombatWithZone();

        for(uint8 i = 0; i<2; i++)
            if (Creature* pStormforgedLieutenant = ((Creature*)Unit::GetUnit((*m_creature), m_uiStormforgedLieutenantGUID[i])))
                pStormforgedLieutenant->AI()->AttackStart(who);

        if(pInstance)
            pInstance->SetData(DATA_BJARNGRIM_EVENT, IN_PROGRESS);
    }

    void UpdateAI(const uint32 diff)
    {
        // Charge up
        if(!m_creature->isInCombat())
        {
            if(m_uiCharge_Timer < diff)
            {
                if (m_uiChargingStatus == 0)
                {
                    DoCast(m_creature, SPELL_CHARGE_UP);
                    m_uiChargingStatus = 1;
                    m_uiCharge_Timer = 5500;
                }
                else if (m_uiChargingStatus == 1)
                {
                    m_creature->InterruptNonMeleeSpells(true);
                    DoCast(m_creature, SPELL_TEMPORARY_ELECTRICAL_CHARGE);
                    m_uiChargingStatus = 2;
                    m_uiCharge_Timer = 20000 + rand()%5000;
                }
                else if (m_uiChargingStatus == 2)
                {
                    if (m_creature->HasAura(SPELL_TEMPORARY_ELECTRICAL_CHARGE, 0))
                        m_creature->RemoveAurasDueToSpell(SPELL_TEMPORARY_ELECTRICAL_CHARGE);
                    m_uiChargingStatus = 0;
                    m_uiCharge_Timer = 5000 + rand()%1000;
                }
            }else m_uiCharge_Timer -= diff;
            return;
        }

        //Return since we have no target
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

        // Change stance
        if(m_uiChangeStance_Timer < diff)
        {
            if (!IsChangingStance)
            {
                uint8 m_uiTempStance;
                m_uiTempStance = rand()%3;
                while (m_uiStance == m_uiTempStance)
                    m_uiTempStance = rand()%3;
                m_uiStance = m_uiTempStance;

                m_creature->InterruptNonMeleeSpells(true);
                if (m_creature->HasAura(SPELL_DEFENSIVE_AURA, 0))
                    m_creature->RemoveAurasDueToSpell(SPELL_DEFENSIVE_AURA);
                if (m_creature->HasAura(SPELL_BERSEKER_AURA, 0))
                    m_creature->RemoveAurasDueToSpell(SPELL_BERSEKER_AURA);
                if (m_creature->HasAura(SPELL_BATTLE_AURA, 0))
                    m_creature->RemoveAurasDueToSpell(SPELL_BATTLE_AURA);

                switch(m_uiStance)
                {
                    case 0:
                        DoScriptText(SAY_DEFENSIVE_STANCE, m_creature);
                        DoCast(m_creature, SPELL_DEFENSIVE_STANCE);
                        m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 0, uint32(33609));
                        m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 1, uint32(39288));
                        break;
                    case 1:
                        DoScriptText(SAY_BERSEKER_STANCE, m_creature);
                        DoCast(m_creature, SPELL_BERSEKER_STANCE);
                        m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 0, uint32(33609));
                        m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 1, uint32(33609));
                        break;
                    case 2:
                        DoScriptText(SAY_BATTLE_STANCE, m_creature);
                        DoCast(m_creature, SPELL_BATTLE_STANCE);
                        m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 0, uint32(43623));
                        m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 1, uint32(0));
                        break;
                }
                IsChangingStance = true;
                m_uiChangeStance_Timer = 500;
            }
            else
            {
                switch(m_uiStance)
                {
                    case 0:
                        DoCast(m_creature, SPELL_DEFENSIVE_AURA);
                        break;
                    case 1:
                        DoCast(m_creature, SPELL_BERSEKER_AURA);
                        break;
                    case 2:
                        DoCast(m_creature, SPELL_BATTLE_AURA);
                        break;
                }
                IsChangingStance = false;
                m_uiChangeStance_Timer = 20000 + rand()%5000;
            }
            return;
        }else m_uiChangeStance_Timer -= diff;

        // Defensive
        if (m_uiStance == 0)
        {
            if(m_uiReflection_Timer < diff)
            {
                DoCast(m_creature, SPELL_SPELL_REFLECTION);
                m_uiReflection_Timer = 8000 + rand()%1000;
            }else m_uiReflection_Timer -= diff;

            if(m_uiKnockAway_Timer < diff)
            {
                DoCast(m_creature, SPELL_KNOCK_AWAY);
                m_uiKnockAway_Timer = 20000 + rand()%1000;
            }else m_uiKnockAway_Timer -= diff;

            if(m_uiPummel_Timer < diff)
            {
                DoCast(m_creature->getVictim(), SPELL_PUMMEL);
                m_uiPummel_Timer = 10000 + rand()%1000;
            }else m_uiPummel_Timer -= diff;

            if(m_uiIronform_Timer < diff)
            {
                DoCast(m_creature, SPELL_IRONFORM);
                m_uiIronform_Timer = 25000 + rand()%1000;
            }else m_uiIronform_Timer -= diff;
        }

        // Berseker
        if (m_uiStance == 1)
        {
            if(m_uiIntercept_Timer < diff)
            {
                DoCast(m_creature->getVictim(), SPELL_INTERCEPT);
                m_uiIntercept_Timer = 45000 + rand()%1000;
            }else m_uiIntercept_Timer -= diff;

            if(m_uiWhirlwind_Timer < diff)
            {
                DoCast(m_creature, SPELL_WHIRLWIND);
                m_uiWhirlwind_Timer = 10000 + rand()%1000;
            }else m_uiWhirlwind_Timer -= diff;

            if(m_uiCleave_Timer < diff)
            {
                DoCast(m_creature->getVictim(), SPELL_CLEAVE);
                m_uiCleave_Timer = 8000 + rand()%1000;
            }else m_uiCleave_Timer -= diff;
        }

        // Battle
        if (m_uiStance == 2)
        {
            if(m_uiMortalStrike_Timer < diff)
            {
                DoCast(m_creature->getVictim(), SPELL_MORTAL_STRIKE);
                m_uiMortalStrike_Timer = 20000 + rand()%1000;
            }else m_uiMortalStrike_Timer -= diff;

            if(m_uiSlam_Timer < diff)
            {
                DoCast(m_creature->getVictim(), SPELL_SLAM);
                m_uiSlam_Timer = 15000 + rand()%1000;
            }else m_uiSlam_Timer -= diff;
        }

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* killer)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if(pInstance)
            pInstance->SetData(DATA_BJARNGRIM_EVENT, DONE);
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
};

/*######
## Mob Stormforged Lieutenant
######*/
struct MANGOS_DLL_DECL mob_stormforged_lieutenantAI : public ScriptedAI
{
    mob_stormforged_lieutenantAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        m_bIsHeroic = pCreature->GetMap()->IsHeroic();
        Reset();
    }

    ScriptedInstance *pInstance;
    bool m_bIsHeroic;

    uint32 ArcWeld_Timer;
    uint32 RenewSteel_Timer;

    void Reset()
    {
        ArcWeld_Timer = 20000 + rand()%1000;
        RenewSteel_Timer = 10000 + rand()%1000;
    }

    void Aggro(Unit* who)
    {
        if(pInstance)
            if (Creature* pBjarngrim = (Creature*)Unit::GetUnit(*m_creature, pInstance->GetData64(DATA_BJARNGRIM)))
                if (!pBjarngrim->getVictim())
                    pBjarngrim->AI()->AttackStart(who);
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

        if(ArcWeld_Timer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_ARC_WELD);
            ArcWeld_Timer = 20000 + rand()%1000;
        }else ArcWeld_Timer -= diff;

        if(RenewSteel_Timer < diff)
        {
            if (pInstance)
                if (Creature* pBjarngrim = (Creature*)Unit::GetUnit(*m_creature, pInstance->GetData64(DATA_BJARNGRIM)))
                    if (pBjarngrim->isAlive())
                        DoCast(pBjarngrim, m_bIsHeroic ? SPELL_RENEW_STEEL_H : SPELL_RENEW_STEEL_N);
            RenewSteel_Timer = 10000 + rand()%4000;
        }else RenewSteel_Timer -= diff;

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* killer) {}
    void KilledUnit(Unit *victim) {}
};

CreatureAI* GetAI_boss_bjarngrim(Creature *_Creature)
{
    return new boss_bjarngrimAI (_Creature);
}

CreatureAI* GetAI_mob_stormforged_lieutenant(Creature *_Creature)
{
    return new mob_stormforged_lieutenantAI (_Creature);
}

void AddSC_boss_bjarngrim()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name="boss_bjarngrim";
    newscript->GetAI = GetAI_boss_bjarngrim;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="mob_stormforged_lieutenant";
    newscript->GetAI = GetAI_mob_stormforged_lieutenant;
    newscript->RegisterSelf();
}
