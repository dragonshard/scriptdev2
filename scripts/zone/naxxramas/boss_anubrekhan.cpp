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
SDName: Boss_Anubrekhan
SD%Complete: 70
SDComment:
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "def_naxxramas.h"

enum
{
    SAY_GREET                   = -1533000,
    SAY_AGGRO1                  = -1533001,
    SAY_AGGRO2                  = -1533002,
    SAY_AGGRO3                  = -1533003,
    SAY_TAUNT1                  = -1533004,
    SAY_TAUNT2                  = -1533005,
    SAY_TAUNT3                  = -1533006,
    SAY_TAUNT4                  = -1533007,
    SAY_SLAY                    = -1533008,

    SPELL_IMPALE                = 28783,                    //May be wrong spell id. Causes more dmg than I expect
    SPELL_IMPALE_H              = 56090,
    SPELL_LOCUSTSWARM           = 28785,                    //This is a self buff that triggers the dmg debuff
    SPELL_LOCUSTSWARM_H         = 54021,
    SPELL_BERSERK               = 26662,

    //spellId invalid
    SPELL_SUMMONGUARD           = 29508,                    //Summons 1 crypt guard at targeted location

    SPELL_SELF_SPAWN_5          = 29105,                    //This spawns 5 corpse scarabs ontop of us (most likely the pPlayer casts this on death)
    SPELL_SELF_SPAWN_10         = 28864,                    //This is used by the crypt guards when they die

    NPC_CRYPT_GUARD             = 16573,
    SPELL_ACID_SPIT             = 28969,
    SPELL_ACID_SPIT_H           = 56098,
    SPELL_CLEAVE                = 40504,
    SPELL_FRENZY                = 8269
};

struct MANGOS_DLL_DECL boss_anubrekhanAI : public ScriptedAI
{
    boss_anubrekhanAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroicMode = pCreature->GetMap()->IsHeroic();
        HasTaunted = false;
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsHeroicMode;

    uint32 Impale_Timer;
    uint32 LocustSwarm_Timer;
    uint32 Summon_Timer;
    uint32 Berserk_Timer;

    uint64 CryptGuardGUID[2];
    bool HasTaunted;

    void Reset()
    {
        Impale_Timer = 15000;                               //15 seconds
        LocustSwarm_Timer = 80000 + (rand()%40000);         //Random time between 80 seconds and 2 minutes for initial cast
        Summon_Timer = LocustSwarm_Timer + 20000;           //45 seconds after initial locust swarm
        Berserk_Timer = 300000;

        for(uint8 i = 0; i < (m_bIsHeroicMode ? 2 : 1); i++)
        {
            if (Creature* pCryptGuard = ((Creature*)Unit::GetUnit((*m_creature), CryptGuardGUID[i])))
            {
                if (!pCryptGuard->isAlive())
                    pCryptGuard->Respawn();
            }
            else if (Creature* pCryptGuard = m_creature->SummonCreature(NPC_CRYPT_GUARD, m_creature->GetPositionX(), m_creature->GetPositionY()+10, m_creature->GetPositionZ(), m_creature->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 300000))
            {
                CryptGuardGUID[i] = pCryptGuard->GetGUID();
            }
        }

        if (m_pInstance)
            m_pInstance->SetData(TYPE_ANUB_REKHAN, NOT_STARTED);
    }

    void KilledUnit(Unit* pVictim)
    {
        //Force the player to spawn corpse scarabs via spell
        if (pVictim->GetTypeId() == TYPEID_PLAYER)
            pVictim->CastSpell(pVictim, SPELL_SELF_SPAWN_5, true);

        if (rand()%5)
            return;

        DoScriptText(SAY_SLAY, m_creature);
    }

    void JustDied(Unit* Killer)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ANUB_REKHAN, DONE);
    }

    void Aggro(Unit *who)
    {
        switch(rand()%3)
        {
            case 0: DoScriptText(SAY_AGGRO1, m_creature); break;
            case 1: DoScriptText(SAY_AGGRO2, m_creature); break;
            case 2: DoScriptText(SAY_AGGRO3, m_creature); break;
        }

        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_ANUB_REKHAN, IN_PROGRESS);

        for(uint8 i = 0; i < (m_bIsHeroicMode ? 2 : 1); i++)
        {
            if (Creature* pCryptGuard = ((Creature*)Unit::GetUnit((*m_creature), CryptGuardGUID[i])))
                if (pCryptGuard->isAlive())
                    pCryptGuard->AI()->AttackStart(who);
        }
        }
    }

    void MoveInLineOfSight(Unit *who)
    {
        if (!HasTaunted && m_creature->IsWithinDistInMap(who, 60.0f))
        {
            switch(rand()%5)
            {
                case 0: DoScriptText(SAY_GREET, m_creature); break;
                case 1: DoScriptText(SAY_TAUNT1, m_creature); break;
                case 2: DoScriptText(SAY_TAUNT2, m_creature); break;
                case 3: DoScriptText(SAY_TAUNT3, m_creature); break;
                case 4: DoScriptText(SAY_TAUNT4, m_creature); break;
            }
            HasTaunted = true;
        }

        ScriptedAI::MoveInLineOfSight(who);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

        if (Berserk_Timer)
            if (Berserk_Timer < diff)
            {
                DoCast(m_creature, SPELL_BERSERK);
                Berserk_Timer = 0;
            }else Berserk_Timer -= diff;

        //Impale_Timer
        if (Impale_Timer < diff)
        {
            //Cast Impale on a random target
            //Do NOT cast it when we are afflicted by locust swarm
            if (!m_creature->HasAura(SPELL_LOCUSTSWARM) || !m_creature->HasAura(SPELL_LOCUSTSWARM_H))
            {
                if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
                    DoCast(target, m_bIsHeroicMode ? SPELL_IMPALE_H : SPELL_IMPALE);
            }

            Impale_Timer = 15000;
        }else Impale_Timer -= diff;

        //LocustSwarm_Timer
        if (LocustSwarm_Timer < diff)
        {
            DoCast(m_creature, m_bIsHeroicMode ? SPELL_LOCUSTSWARM_H : SPELL_LOCUSTSWARM);
            if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
                if (Creature* pTemp = m_creature->SummonCreature(NPC_CRYPT_GUARD, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000))
                    pTemp->SetInCombatWithZone();
            LocustSwarm_Timer = 90000 + (rand()%30000);
            Summon_Timer += 20000;
        }else LocustSwarm_Timer -= diff;

        //Summon_Timer
        if (Summon_Timer < diff)
        {
            //DoCast(m_creature, SPELL_SUMMONGUARD);
            if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
                if (Creature* pTemp = m_creature->SummonCreature(NPC_CRYPT_GUARD, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000))
                    pTemp->SetInCombatWithZone();
            Summon_Timer = 20000;
        }else Summon_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL mob_crypt_guardAI : public ScriptedAI
{
    mob_crypt_guardAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroicMode = pCreature->GetMap()->IsHeroic();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsHeroicMode;

    uint32 AcidSpit_Timer;
    uint32 Cleave_Timer;
    uint32 Berserk_Timer;

    void Reset()
    {
        AcidSpit_Timer = 10000 + rand()%1000;
        Cleave_Timer = 5000 + rand()%5000;
        Berserk_Timer = 120000;
    }

    void KilledUnit(Unit* pVictim)
    {
        //Force the player to spawn corpse scarabs via spell
        if (pVictim->GetTypeId() == TYPEID_PLAYER)
            pVictim->CastSpell(pVictim, SPELL_SELF_SPAWN_5, true);
    }

    void JustDied(Unit* Killer)
    {
        m_creature->CastSpell(m_creature, SPELL_SELF_SPAWN_10, true);
    }

    void Aggro(Unit *who)
    {
        if (m_pInstance)
        {
            if (Creature* pAnubRekhan = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(DATA_ANUB_REKHAN))))
                if (pAnubRekhan->isAlive() && !pAnubRekhan->getVictim())
                    pAnubRekhan->AI()->AttackStart(who);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

        if (Berserk_Timer)
            if (Berserk_Timer < diff)
            {
                DoCast(m_creature, SPELL_FRENZY);
                Berserk_Timer = 0;
            }else Berserk_Timer -= diff;

        if (AcidSpit_Timer < diff)
        {
            DoCast(m_creature->getVictim(), m_bIsHeroicMode ? SPELL_ACID_SPIT_H : SPELL_ACID_SPIT);
            AcidSpit_Timer = 10000 + rand()%1000;
        }else AcidSpit_Timer -= diff;

        if (Cleave_Timer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_CLEAVE);
            Cleave_Timer = 5000 + rand()%5000;
        }else Cleave_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_anubrekhan(Creature* pCreature)
{
    return new boss_anubrekhanAI(pCreature);
}

CreatureAI* GetAI_mob_crypt_guard(Creature* pCreature)
{
    return new mob_crypt_guardAI(pCreature);
}

void AddSC_boss_anubrekhan()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_anubrekhan";
    newscript->GetAI = &GetAI_boss_anubrekhan;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_crypt_guard";
    newscript->GetAI = &GetAI_mob_crypt_guard;
    newscript->RegisterSelf();
}
