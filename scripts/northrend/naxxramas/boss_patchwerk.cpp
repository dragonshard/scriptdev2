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
SDName: Boss_Patchwerk
SD%Complete: 80
SDComment: Some issues with hateful strike inturrupting the melee swing timer.
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "def_naxxramas.h"

#define SAY_AGGRO1              -1533017
#define SAY_AGGRO2              -1533018
#define SAY_SLAY                -1533019
#define SAY_DEATH               -1533020

#define EMOTE_BERSERK           -1533021
#define EMOTE_ENRAGE            -1533022

#define SPELL_HATEFULSTRIKE     28308
#define H_SPELL_HATEFULSTRIKE   59192
#define SPELL_ENRAGE            28131
#define SPELL_BERSERK           26662
#define SPELL_SLIMEBOLT         32309

struct MANGOS_DLL_DECL boss_patchwerkAI : public ScriptedAI
{
    boss_patchwerkAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 HatefullStrike_Timer;
    uint32 Enrage_Timer;
    uint32 Slimebolt_Timer;
    bool Enraged;

    void Reset()
    {
        HatefullStrike_Timer = 1200;                        //1.2 seconds
        Enrage_Timer = 420000;                              //7 minutes 420,000
        Slimebolt_Timer = 450000;                           //7.5 minutes 450,000
        Enraged = false;

        if (m_pInstance)
            m_pInstance->SetData(TYPE_PATCHWERK, NOT_STARTED);
    }

    void KilledUnit(Unit* Victim)
    {
        if (urand(0, 4))
            return;

        DoScriptText(SAY_SLAY, m_creature);
    }

    void JustDied(Unit* Killer)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_PATCHWERK, DONE);
    }

    void Aggro(Unit *who)
    {
        if (urand(0, 1))
            DoScriptText(SAY_AGGRO1, m_creature);
        else
            DoScriptText(SAY_AGGRO2, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_PATCHWERK, IN_PROGRESS);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

        //HatefullStrike_Timer
        if (HatefullStrike_Timer < diff)
        {
            //Cast Hateful strike on the player with the highest
            //amount of HP within melee distance
            uint32 MostHP = 0;
            Unit* pMostHPTarget = NULL;
            Unit* pTemp = NULL;
            std::list<HostilReference*>::iterator i = m_creature->getThreatManager().getThreatList().begin();

            for (i = m_creature->getThreatManager().getThreatList().begin(); i!=m_creature->getThreatManager().getThreatList().end(); ++i)
            {
                pTemp = Unit::GetUnit((*m_creature),(*i)->getUnitGuid());
                if (pTemp && pTemp->isAlive() && pTemp->GetHealth() > MostHP && m_creature->IsWithinDist(pTemp, 5.0f, false))
                {
                    MostHP = pTemp->GetHealth();
                    pMostHPTarget = pTemp;
                }
            }

            if (pMostHPTarget)
                DoCast(pMostHPTarget, SPELL_HATEFULSTRIKE);

            HatefullStrike_Timer = 3000;
        }else HatefullStrike_Timer -= diff;

        //Enrage_Timer
        if (Enrage_Timer < diff)
        {
            DoCast(m_creature, SPELL_BERSERK);
            DoScriptText(EMOTE_BERSERK, m_creature);
            Enrage_Timer = 300000;
        }else Enrage_Timer -= diff;

        //Slimebolt_Timer
        if (Slimebolt_Timer < diff)
        {
            DoCast(m_creature->getVictim(),SPELL_SLIMEBOLT);
            Slimebolt_Timer = 5000;
        }else Slimebolt_Timer -= diff;

        //Enrage if not already enraged and below 5%
        if (!Enraged && (m_creature->GetHealth()*100 / m_creature->GetMaxHealth()) < 5)
        {
            DoCast(m_creature,SPELL_ENRAGE);
            DoScriptText(EMOTE_ENRAGE,NULL);
            Enraged = true;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_patchwerk(Creature* pCreature)
{
    return new boss_patchwerkAI(pCreature);
}

void AddSC_boss_patchwerk()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_patchwerk";
    newscript->GetAI = &GetAI_boss_patchwerk;
    newscript->RegisterSelf();
}
