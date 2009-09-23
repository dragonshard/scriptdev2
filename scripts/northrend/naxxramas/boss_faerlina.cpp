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
SDName: Boss_Faerlina
SD%Complete: 50
SDComment:
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "def_naxxramas.h"

#define SAY_GREET                   -1533009
#define SAY_AGGRO1                  -1533010
#define SAY_AGGRO2                  -1533011
#define SAY_AGGRO3                  -1533012
#define SAY_AGGRO4                  -1533013
#define SAY_SLAY1                   -1533014
#define SAY_SLAY2                   -1533015
#define SAY_DEATH                   -1533016

//#define SOUND_RANDOM_AGGRO  8955                            //soundId containing the 4 aggro sounds, we not using this

#define SPELL_POSIONBOLT_VOLLEY     28796
#define H_SPELL_POSIONBOLT_VOLLEY   54098
#define SPELL_ENRAGE                28798
#define H_SPELL_ENRAGE              54100

#define SPELL_FIREBALL              54095
#define SPELL_FIREBALL_H            54096
#define SPELL_WIDOWS_EMBRACE        28732

#define SPELL_RAINOFFIRE            28794                   //Not sure if targeted AoEs work if casted directly upon a pPlayer

struct MANGOS_DLL_DECL boss_faerlinaAI : public ScriptedAI
{
    boss_faerlinaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroicMode = pCreature->GetMap()->IsHeroic();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsHeroicMode;

    uint32 PoisonBoltVolley_Timer;
    uint32 RainOfFire_Timer;
    uint32 Enrage_Timer;
    bool HasTaunted;

    void Reset()
    {
        PoisonBoltVolley_Timer = 8000;
        RainOfFire_Timer = 16000;
        Enrage_Timer = 60000;
        HasTaunted = false;

        std::list<Creature*> lUnitList;
        GetCreatureListWithEntryInGrid(lUnitList, m_creature, 16506, 100.0f);
        if (!lUnitList.empty())
        {
            for(std::list<Creature*>::iterator iter = lUnitList.begin(); iter != lUnitList.end(); ++iter)
                if ((*iter)->isDead())
                    (*iter)->Respawn();
        }

        if (m_pInstance)
            m_pInstance->SetData(TYPE_FAERLINA, NOT_STARTED);
    }

    void Aggro(Unit *who)
    {
        switch (rand()%4)
        {
            case 0: DoScriptText(SAY_AGGRO1, m_creature); break;
            case 1: DoScriptText(SAY_AGGRO2, m_creature); break;
            case 2: DoScriptText(SAY_AGGRO3, m_creature); break;
            case 3: DoScriptText(SAY_AGGRO4, m_creature); break;
        }

        if (m_pInstance)
            m_pInstance->SetData(TYPE_FAERLINA, IN_PROGRESS);
    }

    void MoveInLineOfSight(Unit *who)
    {
        if (!HasTaunted && m_creature->IsWithinDistInMap(who, 60.0f))
        {
            DoScriptText(SAY_GREET, m_creature);
            HasTaunted = true;
        }

        ScriptedAI::MoveInLineOfSight(who);
    }

    void KilledUnit(Unit* victim)
    {
        switch (rand()%2)
        {
            case 0: DoScriptText(SAY_SLAY1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY2, m_creature); break;
        }
    }

    void JustDied(Unit* Killer)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_FAERLINA, DONE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

        //PoisonBoltVolley_Timer
        if (PoisonBoltVolley_Timer < diff)
        {
            DoCast(m_creature->getVictim(), m_bIsHeroicMode ? H_SPELL_POSIONBOLT_VOLLEY : SPELL_POSIONBOLT_VOLLEY);
            PoisonBoltVolley_Timer = 9000 + rand()%2000;
        }else PoisonBoltVolley_Timer -= diff;

        //RainOfFire_Timer
        if (RainOfFire_Timer < diff)
        {
            if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
                DoCast(target,SPELL_RAINOFFIRE);

            RainOfFire_Timer = 14000 + rand()%2000;
        }else RainOfFire_Timer -= diff;

        //Enrage_Timer
        if (Enrage_Timer < diff)
        {
            DoCast(m_creature, m_bIsHeroicMode ? H_SPELL_ENRAGE : SPELL_ENRAGE);
            Enrage_Timer = 60000 + rand()%20000;
        }else Enrage_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL mob_worshippersAI : public ScriptedAI
{
    mob_worshippersAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroicMode = pCreature->GetMap()->IsHeroic();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsHeroicMode;
    bool m_bIsDead;

    uint32 m_uiFireball_Timer;
    uint32 m_uiDeathDelay_Timer;

    void Reset()
    {
        m_bIsDead = false;
        m_uiFireball_Timer = 0;
        m_uiDeathDelay_Timer = 0;
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (m_bIsDead)
        {
            uiDamage = 0;
            return;
        }

        if (uiDamage > m_creature->GetHealth())
        {
            if (m_creature->IsNonMeleeSpellCasted(false))
                m_creature->InterruptNonMeleeSpells(false);

            m_creature->RemoveAllAuras();
            m_creature->AttackStop();

            DoCast(m_creature, SPELL_WIDOWS_EMBRACE);

            if (m_pInstance)
                if (Creature* pFaerlina = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(DATA_FAERLINA))))
                    if (m_creature->GetDistance2d(pFaerlina) <= 5 && pFaerlina->HasAura(m_bIsHeroicMode ? H_SPELL_ENRAGE : SPELL_ENRAGE))
                        pFaerlina->RemoveAurasDueToSpell(m_bIsHeroicMode ? H_SPELL_ENRAGE : SPELL_ENRAGE);

            m_bIsDead = true;
            m_uiDeathDelay_Timer = 500;

            uiDamage = 0;
            return;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiDeathDelay_Timer)
            if (m_uiDeathDelay_Timer < uiDiff)
            {
                m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                m_uiDeathDelay_Timer = 0;
            }
            else m_uiDeathDelay_Timer -= uiDiff;

        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim() || m_bIsDead)
            return;

        if (m_uiFireball_Timer < uiDiff)
        {
            DoCast(m_creature->getVictim(), m_bIsHeroicMode ? SPELL_FIREBALL_H : SPELL_FIREBALL);
            m_uiFireball_Timer = 7000 + rand()%4000;
        }
        else m_uiFireball_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_worshippers(Creature* pCreature)
{
    return new mob_worshippersAI(pCreature);
}

CreatureAI* GetAI_boss_faerlina(Creature* pCreature)
{
    return new boss_faerlinaAI(pCreature);
}

void AddSC_boss_faerlina()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_faerlina";
    newscript->GetAI = &GetAI_boss_faerlina;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_worshippers";
    newscript->GetAI = &GetAI_mob_worshippers;
    newscript->RegisterSelf();
}
