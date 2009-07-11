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
SDName: Boss_Eredar_Twins
SDAuthor: ckegg
SD%Complete: 90
SDComment:
    - Blaze Spell doenst Work
    - Some trigger Spells are only (good) workarounds
EndScriptData */

#include "precompiled.h"
#include "def_sunwell_plateau.h"

enum
{
    // Lady Sacrolash
    SAY_INTRO                                = -1580044,
    SAY_SACROLASH_SHADOW_NOVA                = -1580045, //only if Alythess is not dead
    SAY_SACROLASH_SISTER_ALYTHESS_DEAD       = -1580046,
    SAY_SACROLASH_SAC_KILL_1                 = -1580047,
    SAY_SACROLASH_SAC_KILL_2                 = -1580048,
    SAY_SAC_DEAD                             = -1580049,
    SAY_SACROLASH_ENRAGE                     = -1580050,

    SPELL_DARK_TOUCHED                       = 45347,
    SPELL_SHADOW_BLADES                      = 45248, //10 secs
    SPELL_DARK_STRIKE                        = 45271,
    SPELL_SHADOW_NOVA                        = 45329, //30-35 secs
    SPELL_CONFOUNDING_BLOW                   = 45256, //25 secs

    SPELL_SHADOW_FURY                        = 45270,
    SPELL_IMAGE_VISUAL                       = 45263,

    SPELL_ENRAGE                             = 46587, //enrage 6 minutes
    SPELL_EMPOWER                            = 45366, //empower after sister is death

    //Grand Warlock Alythess
    SAY_ALYTHESS_CANFLAGRATION               = -1580051, //only if Sacrolash is not dead
    SAY_ALYTHESS_SISTER_SACROLASH_DEAD       = -1580052,
    SAY_ALYTHESS_ALY_KILL_1                  = -1580053,
    SAY_ALYTHESS_ALY_KILL_2                  = -1580054,
    SAY_ALYTHESS_ALY_DEAD                    = -1580055,
    SAY_ALYTHESS_BERSERK                     = -1580056,

    SPELL_PYROGENICS                         = 45230, //15secs
    SPELL_FLAME_TOUCHED                      = 45348,
    SPELL_CONFLAGRATION                      = 45342, //30-35 secs
    SPELL_BLAZE                              = 45235, //on main target every 3 secs
    SPELL_FLAME_SEAR                         = 46771,
    SPELL_BLAZE_SUMMON                       = 45236, //187366
    SPELL_BLAZE_BURN                         = 45246,

    NPC_SHADOW_IMAGE                         = 25214,

    HOME_POS_X                               = 1816,
    HOME_POS_Y                               = 625,
    HOME_POS_Z                               = 33
};

/*######
## Boss Lady Sacrolash
######*/
struct MANGOS_DLL_DECL boss_sacrolashAI : public ScriptedAI
{
    boss_sacrolashAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        Reset();
    }

    ScriptedInstance *m_pInstance;
    bool b_SisterIsDeath;
    bool b_UsedBlades;

    uint32 ui_Darkstrike_timer;
    uint32 ui_Shadowblades_timer;
    uint32 ui_Shadownova_timer;
    uint32 ui_Confoundingblow_timer;
    uint32 ui_Shadowimage_timer;
    uint32 ui_Conflagration_timer;
    uint32 ui_Enrage_timer;

    void Reset()
    {
        if(m_pInstance)
        {
            if (Unit* Temp =  Unit::GetUnit((*m_creature),m_pInstance->GetData64(DATA_ALYTHESS)))
                if (Temp->isDead())
                {
                    Temp->setDeathState(ALIVE);
                    ((Creature*)Temp)->AI()->EnterEvadeMode();
                }

            m_pInstance->SetData(TYPE_EREDAR_TWINS, NOT_STARTED);
        }

        b_SisterIsDeath = false;
        b_UsedBlades = false;

        ui_Shadowblades_timer = 10000;
        ui_Shadownova_timer = 30000;
        ui_Confoundingblow_timer = 25000;
        ui_Shadowimage_timer = 30000;
        ui_Conflagration_timer = 30000;
        ui_Darkstrike_timer = 5000;
        ui_Enrage_timer = 360000;
    }

    void Aggro(Unit *who)
    {
        m_creature->SetInCombatWithZone();
        if(m_pInstance)
        {
            if (Unit* Temp =  Unit::GetUnit((*m_creature),m_pInstance->GetData64(DATA_ALYTHESS)))
            {
                Temp->getThreatManager().addThreat(who,0.0f);
                ((Creature*)Temp)->AI()->AttackStart(who);
            }
            m_pInstance->SetData(TYPE_EREDAR_TWINS, IN_PROGRESS);
        }
    }

    void AttackStart(Unit* who)
    {
        if (!who)
            return;

        if (who == m_creature || who->GetTypeId() != TYPEID_PLAYER)
            return;

        if (m_creature->Attack(who, true))
        {
            m_creature->AddThreat(who, 0.0f);
            m_creature->SetInCombatWith(who);
            who->SetInCombatWith(m_creature);
            DoStartMovement(who);
        }
    }

    void KilledUnit(Unit *victim)
    {
        if(rand()%4 == 0)
        {
            switch (rand()%2)
            {
                case 0: DoScriptText(SAY_SACROLASH_SAC_KILL_1,m_creature); break;
                case 1: DoScriptText(SAY_SACROLASH_SAC_KILL_2,m_creature); break;
            }
        }
    }

    void JustDied(Unit* Killer)
    {
         // only if ALY death
        if (b_SisterIsDeath)
        {
            DoScriptText(SAY_SAC_DEAD,m_creature);
            if(m_pInstance)
                m_pInstance->SetData(TYPE_EREDAR_TWINS, DONE);
        }
        else
            m_creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
    }

    void UpdateAI(const uint32 diff)
    {
        if(!b_SisterIsDeath)
        {
            if (m_pInstance)
            {
                Unit* Temp = Unit::GetUnit((*m_creature),m_pInstance->GetData64(DATA_ALYTHESS));
                if (Temp && Temp->isDead())
                {
                    DoScriptText(SAY_SACROLASH_SISTER_ALYTHESS_DEAD,m_creature);
                    b_SisterIsDeath = true;

                    m_creature->InterruptSpell(CURRENT_GENERIC_SPELL);
                    DoCast(m_creature,SPELL_EMPOWER);
                }
                else if(Temp && Temp->isAlive())
                    Temp->AddThreat(m_creature->getVictim(), 0.0f);
            }
        }

        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

        if (m_creature->GetDistance2d(HOME_POS_X, HOME_POS_Y) > 40 || m_creature->GetDistance(m_creature->GetPositionX(),m_creature->GetPositionY(),HOME_POS_Z) > 5)
        {
            EnterEvadeMode();
            return;
        }

        if(b_SisterIsDeath)
        {
            if (ui_Conflagration_timer < diff)
            {
                if (!m_creature->IsNonMeleeSpellCasted(false))
                {
                    m_creature->InterruptSpell(CURRENT_GENERIC_SPELL);
                    DoCast(SelectUnit(SELECT_TARGET_RANDOM, 0),SPELL_CONFLAGRATION);
                    ui_Conflagration_timer = 30000+(rand()%5000);
                }
            }else ui_Conflagration_timer -= diff;
        }
        else
        {
            if(ui_Shadownova_timer < diff)
            {
                if (!m_creature->IsNonMeleeSpellCasted(false))
                {
                    DoCast(SelectUnit(SELECT_TARGET_RANDOM, 0),SPELL_SHADOW_NOVA);

                    if(!b_SisterIsDeath)
                        DoScriptText(SAY_SACROLASH_SHADOW_NOVA,m_creature);

                    ui_Shadownova_timer= 30000+(rand()%5000);
                }
            }else ui_Shadownova_timer -=diff;
        }

        if(ui_Confoundingblow_timer < diff)
        {
            if (!m_creature->IsNonMeleeSpellCasted(false))
            {
                DoCast(SelectUnit(SELECT_TARGET_RANDOM, 0),SPELL_CONFOUNDING_BLOW);
                ui_Confoundingblow_timer = 20000 + (rand()%5000);
            }
        }else ui_Confoundingblow_timer -=diff;

        if(ui_Shadowimage_timer < diff)
        {
            for(int i = 0;i<3;i++)
            {
                if (Creature* pTemp = DoSpawnCreature(NPC_SHADOW_IMAGE,0,0,0,0,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,10000))
                    pTemp->AI()->AttackStart(SelectUnit(SELECT_TARGET_RANDOM, 0));
            }
            ui_Shadowimage_timer = 30000;
        }else ui_Shadowimage_timer -=diff;

        if(ui_Shadowblades_timer < diff)
        {
            if (!m_creature->IsNonMeleeSpellCasted(false))
            {
                DoCast(m_creature,SPELL_SHADOW_BLADES);
                b_UsedBlades = true;
                ui_Shadowblades_timer = 10000;
                ui_Darkstrike_timer = 1000;
            }
        }else ui_Shadowblades_timer -=diff;

        if(b_UsedBlades && !m_creature->IsNonMeleeSpellCasted(false))
        {
            if (ui_Darkstrike_timer < diff)
            {
                std::list<HostilReference*>::iterator i = m_creature->getThreatManager().getThreatList().begin();
                for (; i != m_creature->getThreatManager().getThreatList().end(); ++i)
                {
                    Unit* pTemp = Unit::GetUnit((*m_creature),(*i)->getUnitGuid());
                    if (pTemp && pTemp->GetTypeId() == TYPEID_PLAYER && m_creature->IsWithinDistInMap(pTemp, 20))
                         DoCast(pTemp, SPELL_DARK_STRIKE);
                }
                b_UsedBlades = false;
                ui_Darkstrike_timer = 5000;
            }else ui_Darkstrike_timer -= diff;
        }

        if (ui_Enrage_timer < diff)
        {
            m_creature->InterruptSpell(CURRENT_GENERIC_SPELL);
            DoScriptText(SAY_SACROLASH_ENRAGE,m_creature);
            DoCast(m_creature,SPELL_ENRAGE);
            ui_Enrage_timer = 300000;
        }else ui_Enrage_timer -= diff;

        DoMeleeAttackIfReady();
    }
};

/*######
## Boss Grand Warlock Alythess
######*/
struct MANGOS_DLL_DECL boss_alythessAI : public ScriptedAI
{
    boss_alythessAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        Reset();
    }

    ScriptedInstance *m_pInstance;
    bool b_SisterIsDeath;
    bool b_IsBlazeSummoned;

    uint64 ui_BlazeTarget;

    uint32 ui_Conflagration_timer;
    uint32 ui_Blaze_timer;
    uint32 ui_Pyrogenics_timer;
    uint32 ui_Blazesummon_timer;
    uint32 ui_Shadownova_timer;
    uint32 ui_Flamesear_timer;
    uint32 ui_Enrage_timer;

    void Reset()
    {
        if(m_pInstance)
        {
            if (Unit* Temp =  Unit::GetUnit((*m_creature),m_pInstance->GetData64(DATA_SACROLASH)))
                if (Temp->isDead())
                {
                    Temp->setDeathState(ALIVE);
                    ((Creature*)Temp)->AI()->EnterEvadeMode();
                }

            m_pInstance->SetData(TYPE_EREDAR_TWINS, NOT_STARTED);
        }

        b_SisterIsDeath = false;

        ui_BlazeTarget = 0;

        ui_Conflagration_timer = 45000;
        ui_Blaze_timer = 1000;
        ui_Pyrogenics_timer = 15000;
        ui_Shadownova_timer = 40000;
        ui_Enrage_timer = 360000;
        ui_Flamesear_timer = 15000;
    }

    void Aggro(Unit *who)
    {
        DoScriptText(SAY_INTRO,m_creature);
        m_creature->SetInCombatWithZone();
        if(m_pInstance)
        {
            Unit* Temp =  Unit::GetUnit((*m_creature),m_pInstance->GetData64(DATA_SACROLASH));
            if (Temp && Temp->isAlive())
            {
                Temp->getThreatManager().addThreat(who,0.0f);
                ((Creature*)Temp)->AI()->AttackStart(who);
            }

            m_pInstance->SetData(TYPE_EREDAR_TWINS, IN_PROGRESS);
        }
    }

    void AttackStart(Unit* who)
    {
        if (!who)
            return;

        if (who == m_creature || who->GetTypeId() != TYPEID_PLAYER)
            return;

        if (m_creature->Attack(who, true))
        {
            m_creature->AddThreat(who, 0.0f);
            m_creature->SetInCombatWith(who);
            who->SetInCombatWith(m_creature);
            DoStartMovement(who);
        }
    }

    void KilledUnit(Unit *victim)
    {
        if(rand()%4 == 0)
        {
            switch (rand()%2)
            {
                case 0: DoScriptText(SAY_ALYTHESS_ALY_KILL_1,m_creature); break;
                case 1: DoScriptText(SAY_ALYTHESS_ALY_KILL_2,m_creature); break;
            }
        }
    }

    void JustDied(Unit* Killer)
    {
        if (b_SisterIsDeath)
        {
            DoScriptText(SAY_ALYTHESS_ALY_DEAD,m_creature);
            if(m_pInstance)
                m_pInstance->SetData(TYPE_EREDAR_TWINS, DONE);
        }
        else
            m_creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
    }

    void UpdateAI(const uint32 diff)
    {
        if(!b_SisterIsDeath)
        {
            if (m_pInstance)
            {
                Unit* Temp = Unit::GetUnit((*m_creature),m_pInstance->GetData64(DATA_SACROLASH));
                if (Temp && Temp->isDead())
                {
                    DoScriptText(SAY_ALYTHESS_SISTER_SACROLASH_DEAD,m_creature);
                    b_SisterIsDeath = true;

                    m_creature->InterruptSpell(CURRENT_GENERIC_SPELL);
                    DoCast(m_creature,SPELL_EMPOWER);
                }
                else if(Temp && Temp->isAlive())
                    Temp->AddThreat(m_creature->getVictim(), 0.0f);
            }
        }

        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

        if (m_creature->GetDistance2d(HOME_POS_X, HOME_POS_Y) > 40 || m_creature->GetDistance(m_creature->GetPositionX(),m_creature->GetPositionY(),HOME_POS_Z) > 5)
        {
            EnterEvadeMode();
            return;
        }

        if(b_SisterIsDeath)
        {
            if(ui_Shadownova_timer < diff)
            {
                if (!m_creature->IsNonMeleeSpellCasted(false))
                {
                    DoCast(SelectUnit(SELECT_TARGET_RANDOM, 0),SPELL_SHADOW_NOVA);
                    ui_Shadownova_timer= 30000+(rand()%5000);
                }
            }else ui_Shadownova_timer -=diff;
        }
        else
        {
            if (ui_Conflagration_timer < diff)
            {
                if (!m_creature->IsNonMeleeSpellCasted(false))
                {
                    m_creature->InterruptSpell(CURRENT_GENERIC_SPELL);
                    DoCast(SelectUnit(SELECT_TARGET_RANDOM, 0),SPELL_CONFLAGRATION);
                    ui_Conflagration_timer = 30000+(rand()%5000);

                    if(!b_SisterIsDeath)
                        DoScriptText(SAY_ALYTHESS_CANFLAGRATION,m_creature);

                    ui_Blaze_timer = 4000;
                }
            }else ui_Conflagration_timer -= diff;
        }

        if (ui_Flamesear_timer < diff)
        {
            if (!m_creature->IsNonMeleeSpellCasted(false))
            {
                DoCast(m_creature,SPELL_FLAME_SEAR);
                ui_Flamesear_timer = 15000;
            }
        }else ui_Flamesear_timer -=diff;

        if (ui_Pyrogenics_timer < diff)
        {
            if (!m_creature->IsNonMeleeSpellCasted(false))
            {
                DoCast(m_creature,SPELL_PYROGENICS,true);
                ui_Pyrogenics_timer = 15000;
            }
        }else ui_Pyrogenics_timer -= diff;

        if (ui_Blaze_timer < diff)
        {
            if (!m_creature->IsNonMeleeSpellCasted(false))
            {
                ui_BlazeTarget = m_creature->getVictim()->GetGUID();
                DoCast(m_creature->getVictim(),SPELL_BLAZE);
                b_IsBlazeSummoned = false;

                ui_Blaze_timer = 3800;
                ui_Blazesummon_timer = 2600;
            }
        }else ui_Blaze_timer -= diff;

        if(!b_IsBlazeSummoned)
        {
            if (ui_Blazesummon_timer < diff)
            {
                Unit* Temp = NULL;
                Temp = Unit::GetUnit((*m_creature),ui_BlazeTarget);
                //if(Temp && Temp->isAlive())
                    //m_creature->SummonCreature(187366,Temp->GetPositionX(),Temp->GetPositionY(),Temp->GetPositionZ(),0,TEMPSUMMON_TIMED_DESPAWN,10000);
                Temp = SelectUnit(SELECT_TARGET_RANDOM, 0);
                if (rand()%2 == 0)
                    Temp->CastSpell(Temp,SPELL_FLAME_TOUCHED,true);
                b_IsBlazeSummoned = true;
                ui_Blazesummon_timer = 2500;
            }else ui_Blazesummon_timer -= diff;
        }

        if (ui_Enrage_timer < diff)
        {
            m_creature->InterruptSpell(CURRENT_GENERIC_SPELL);
            DoScriptText(SAY_ALYTHESS_BERSERK,m_creature);
            DoCast(m_creature,SPELL_ENRAGE);
            ui_Enrage_timer = 300000;
        }else ui_Enrage_timer -= diff;
    }
};

/*######
## Mob shadow image
######*/
struct MANGOS_DLL_DECL mob_shadow_imageAI : public ScriptedAI
{
    mob_shadow_imageAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 ui_Shadowfury_timer;
    uint32 ui_Touched_timer;
    uint32 ui_Kill_timer;

    void Reset()
    {
        ui_Shadowfury_timer = 5000 + (rand()%15000);
        ui_Touched_timer = 1000;
        ui_Kill_timer = 15000;
        DoCast(m_creature,SPELL_IMAGE_VISUAL);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

        if(ui_Touched_timer < diff)
        {
            DoCast(m_creature->getVictim(),SPELL_DARK_TOUCHED,true);
            ui_Touched_timer = 3000;
        }else ui_Touched_timer -= diff;

        if(ui_Shadowfury_timer < diff)
        {
            DoCast(m_creature,SPELL_SHADOW_FURY);
            ui_Shadowfury_timer = 10000;
        }else ui_Shadowfury_timer -=diff;

        if(ui_Kill_timer < diff)
        {
            m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            ui_Kill_timer = 9999999;
        }else ui_Kill_timer -=diff;

        DoMeleeAttackIfReady();
    }
};

/*######
## Mob blaze fire
######*/
struct MANGOS_DLL_DECL mob_blaze_fireAI : public ScriptedAI
{
    mob_blaze_fireAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 ui_Burn_timer;

    void Reset()
    {
        ui_Burn_timer = 3000;
    }

    void AttackStart(Unit *who) {}
    void MoveInLineOfSight(Unit *who){}

    void UpdateAI(const uint32 diff)
    {
        if(ui_Burn_timer < diff)
        {
            DoCast(m_creature,SPELL_BLAZE_BURN);
            ui_Burn_timer = 3000;
        }else ui_Burn_timer -=diff;
    }
};

CreatureAI* GetAI_boss_sacrolash(Creature *_Creature)
{
    return new boss_sacrolashAI (_Creature);
};

CreatureAI* GetAI_boss_alythess(Creature *_Creature)
{
    return new boss_alythessAI (_Creature);
};

CreatureAI* GetAI_mob_shadow_image(Creature *_Creature)
{
    return new mob_shadow_imageAI (_Creature);
};

CreatureAI* GetAI_mob_blaze_fire(Creature *_Creature)
{
    return new mob_blaze_fireAI (_Creature);
};

void AddSC_boss_eredar_twins()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_sacrolash";
    newscript->GetAI = &GetAI_boss_sacrolash;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_alythess";
    newscript->GetAI = &GetAI_boss_alythess;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_shadow_image";
    newscript->GetAI = &GetAI_mob_shadow_image;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_blaze_fire";
    newscript->GetAI = &GetAI_mob_blaze_fire;
    newscript->RegisterSelf();
}
