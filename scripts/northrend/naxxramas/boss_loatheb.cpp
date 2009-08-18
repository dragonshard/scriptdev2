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
SDName: Boss_Loatheb
SD%Complete: 100
SDComment:
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "def_naxxramas.h"

#define SPELL_DEATHBLOOM            29865
#define SPELL_DEATHBLOOM_H          55053
#define SPELL_INEVITABLE_DOOM       29204
#define SPELL_INEVITABLE_DOOM_H     55052
#define SPELL_BERSERK               26662
#define SPELL_NECROTIC_AURA         55593

#define SPELL_NECROTIC_AURA_PRE_WARNING 60929
#define SPELL_NECROTIC_AURA_WARNING     59481

#define SPELL_FUNGAL_CREEP          29232

#define ADD_1X 2957.040
#define ADD_1Y -3997.590
#define ADD_1Z 274.280

#define ADD_2X 2909.130
#define ADD_2Y -4042.970
#define ADD_2Z 274.280

#define ADD_3X 2861.102
#define ADD_3Y -3997.901
#define ADD_3Z 274.280

struct MANGOS_DLL_DECL boss_loathebAI : public ScriptedAI
{
    boss_loathebAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroicMode = pCreature->GetMap()->IsHeroic();
        Reset();
    }
    ScriptedInstance* m_pInstance;
    bool m_bIsHeroicMode;

    uint32 Deathbloom_Timer;
    uint32 NecroticAura_Timer;
    uint32 InevitableDoom_Timer;
    uint32 InevitableDoom5mins_Timer;
    uint32 Enrage_Timer;
    uint32 Summon_Timer;

    void Reset()
    {
        Deathbloom_Timer = 30000;
        NecroticAura_Timer = 2500;
        InevitableDoom_Timer = 120000;
        InevitableDoom5mins_Timer = 300000;
        Enrage_Timer = 420000;
        Summon_Timer = 8000;

        if(m_pInstance)
            m_pInstance->SetData(TYPE_LOATHEB, NOT_STARTED);
    }

    void Aggro(Unit *who)
    {
        if(m_pInstance)
            m_pInstance->SetData(TYPE_LOATHEB, IN_PROGRESS);
    }

    void JustDied(Unit* Killer)
    {
        if(m_pInstance)
            m_pInstance->SetData(TYPE_LOATHEB, DONE);
    }

    void JustSummoned(Creature* summoned)
    {
        if (Unit* target = SelectUnit(SELECT_TARGET_TOPAGGRO,0))
            summoned->AI()->AttackStart(target);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

        //CorruptedMind_Timer
        if (Deathbloom_Timer < diff)
        {
            DoCast(SelectUnit(SELECT_TARGET_RANDOM,0), m_bIsHeroicMode ? SPELL_DEATHBLOOM_H : SPELL_DEATHBLOOM);
            Deathbloom_Timer = 30000;
        }else Deathbloom_Timer -= diff;

        //NecroticAura_Timer
        if (NecroticAura_Timer < diff)
        {
            DoCast(m_creature,SPELL_NECROTIC_AURA); // 17 seconds
            //DoCast(m_creature,SPELL_NECROTIC_AURA_PRE_WARNING); // 14 seconds
            //DoCast(m_creature,SPELL_NECROTIC_AURA_WARNING); // 17 seconds
            NecroticAura_Timer = 20000;
        }else NecroticAura_Timer -= diff;

        //InevitableDoom_Timer
        if (InevitableDoom_Timer < diff)
        {
            DoCast(m_creature->getVictim(),SPELL_INEVITABLE_DOOM);
            InevitableDoom_Timer = 120000;
        }else InevitableDoom_Timer -= diff;

        //InevitableDoom5mins_Timer
        if (InevitableDoom5mins_Timer < diff)
        {
            DoCast(m_creature->getVictim(),SPELL_INEVITABLE_DOOM);
            InevitableDoom5mins_Timer = 15000;
        }else InevitableDoom5mins_Timer -= diff;

        //RemoveCurse_Timer
        if (Enrage_Timer < diff)
        {
            DoCast(m_creature,SPELL_BERSERK);
            Enrage_Timer = 420000;
        }else Enrage_Timer -= diff;

        //Summon_Timer
        if (Summon_Timer < diff)
        {
            Unit* SummonedSpores = NULL;

            SummonedSpores = m_creature->SummonCreature(16286,ADD_1X,ADD_1Y,ADD_1Z,0,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,80000);
            SummonedSpores = m_creature->SummonCreature(16286,ADD_2X,ADD_2Y,ADD_2Z,0,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,80000);
            SummonedSpores = m_creature->SummonCreature(16286,ADD_3X,ADD_3Y,ADD_3Z,0,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,80000);

            Summon_Timer = 28000;
        } else Summon_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL npc_loatheb_sporesAI : public ScriptedAI
{
    npc_loatheb_sporesAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 DieDelay_Timer;

    void Reset()
    {
        DieDelay_Timer = 0;
    }

    void DamageTaken(Unit* done_by, uint32 &damage)
    {
        if (damage > m_creature->GetHealth() && !DieDelay_Timer)
        {
            m_creature->CastSpell(m_creature, SPELL_FUNGAL_CREEP, true);
            DieDelay_Timer = 500;
        }
        if (DieDelay_Timer)
        {
            damage = 0;
            return;
        }
    }

    void JustDied(Unit* Killer) {}

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

        if (DieDelay_Timer)
            if (DieDelay_Timer < diff)
            {
                m_creature->ForcedDespawn();
                DieDelay_Timer = 0;
            }else DieDelay_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_loatheb(Creature* pCreature)
{
    return new boss_loathebAI(pCreature);
}

CreatureAI* GetAI_npc_loatheb_spores(Creature* pCreature)
{
    return new npc_loatheb_sporesAI(pCreature);
}

void AddSC_boss_loatheb()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_loatheb";
    newscript->GetAI = &GetAI_boss_loatheb;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_loatheb_spores";
    newscript->GetAI = &GetAI_npc_loatheb_spores;
    newscript->RegisterSelf();
}
