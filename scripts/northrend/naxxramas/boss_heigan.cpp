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
SDName: Boss_Heigan
SD%Complete: 0
SDComment: Place Holder
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "def_naxxramas.h"

#define SAY_AGGRO1          -1533109
#define SAY_AGGRO2          -1533110
#define SAY_AGGRO3          -1533111
#define SAY_SLAY            -1533112
#define SAY_TAUNT1          -1533113
#define SAY_TAUNT2          -1533114
#define SAY_TAUNT3          -1533115
#define SAY_TAUNT4          -1533116
#define SAY_TAUNT5          -1533117
#define SAY_DEATH           -1533118

//Spell used by floor peices to cause damage to players
#define SPELL_ERUPTION      29371

//Spells by boss
#define SPELL_DISRUPTION    29310
#define SPELL_FEAVER        29998
#define H_SPELL_FEAVER      55011

//Spell by eye stalks
#define SPELL_MIND_FLAY     26143


struct MANGOS_DLL_DECL boss_heiganAI : public ScriptedAI
{
    boss_heiganAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroicMode = pCreature->GetMap()->IsHeroic();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsHeroicMode;

    uint32 Disruption_Timer;
    uint32 Feaver_Timer;

    void Reset()
    {
        Disruption_Timer = 5000+rand()%10000;
        Feaver_Timer = 40000;

        if(m_pInstance)
            m_pInstance->SetData(TYPE_HEIGAN, NOT_STARTED);
    }

    void Aggro(Unit *who)
    {
        switch (rand()%3)
        {
            case 0: DoScriptText(SAY_AGGRO1, m_creature); break;
            case 1: DoScriptText(SAY_AGGRO2, m_creature); break;
            case 2: DoScriptText(SAY_AGGRO3, m_creature); break;
        }

        if(m_pInstance)
            m_pInstance->SetData(TYPE_HEIGAN, IN_PROGRESS);
    }

    void KilledUnit(Unit* victim)
    {
        DoScriptText(SAY_SLAY, m_creature);
    }

    void JustDied(Unit* Killer)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if(m_pInstance)
            m_pInstance->SetData(TYPE_HEIGAN, DONE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

        if (Disruption_Timer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_DISRUPTION);
            Disruption_Timer = 5000+rand()%10000;
        }else Disruption_Timer -= diff;

        if (Feaver_Timer < diff)
        {
            DoCast(m_creature->getVictim(), m_bIsHeroicMode ? H_SPELL_FEAVER : SPELL_FEAVER);
            Feaver_Timer = 40000;
        }else Feaver_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_heigan(Creature* pCreature)
{
    return new boss_heiganAI(pCreature);
}

void AddSC_boss_heigan()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_heigan";
    newscript->GetAI = &GetAI_boss_heigan;
    newscript->RegisterSelf();
}
