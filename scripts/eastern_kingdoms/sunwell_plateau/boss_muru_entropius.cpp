/* Copyright  2006,2007 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

/* ScriptData
SDName: boss_muru_entropius
SDAuthor: Ebrithil
SD%Complete: 60
SDComment: Needs Test XD
EndScriptData */

#include "precompiled.h"
#include "def_sunwell_plateau.h"

enum
{
    // Boss spells
    SPELL_ENRAGE                      = 26662,
    SPELL_DARKNESS                    = 45996,
    SPELL_SHADOW_PORTAL               = 40280,
    SPELL_ENTROPIUS_EFFECT            = 46223,
    SPELL_SUMMON_BERSERKER            = 46037,
    SPELL_SUMMON_ENTROPIUS            = 46217,
    SPELL_SUMMON_FURY_MAGE            = 46038,
    SPELL_SUMMON_VOID_SENTINEL        = 45988,
    SPELL_NEGATIVE_ENERGY_TRIGGER     = 46009,

    SPELL_ENTROPIUS_DARKNESS          = 46269, //Need's core support
    SPELL_ENTROPIUS_VOID_ZONE         = 46263,
    SPELL_ENTROPIUS_NEGATIVE_ENERGY_TRIGGER = 46284, //Triggered spell need's core support

    //Spells casting time (in msecs)
    ENRAGE_TIMER                      = 360000,
    DARKNESS_TIMER                    = 45000,
    SUMMON_HUMANOIDS_TIMER            = 60000,
    SUMMON_VOID_SENTINEL_TIMER        = 30000,
    SINGULARITY_TIMER                 = 60000,

    //Boss Summons
    NPC_SHADOWSWORD_FURY_MAGE         = 25799,
    NPC_SHADOWSWORD_BERSERKER         = 25798,
    NPC_DARK_FIEND                    = 25744,
    NPC_VOID_SENTINEL                 = 25772,
    NPC_SINGULARITY                   = 25855,

    SPELL_DARK_FIEND_AURA             = 45934,
    SPELL_DARK_FIEND_DEBUFF           = 45944,

    //Boss sounds
    SOUND_CHANGE_PHASE                = 12560,

    //Shadow Portal Spawns
    SP_SPAWN_Z                        = 78,
    NPC_WORLD_TRIGGER                 = 12999
};

float ShadowPortalSpawn[5][3] =
{
    {1797.644,613.744,0.56290},
    {1822.956,646.437,4.40262},
    {1794.275,626.539,6.23522},
    {1809.726,646.494,5.01348},
    {1798.854,638.952,5.62435}
};

uint32 DarkFiendsSpell[8] =
{
    46000,46001,46002,46003,
    46004,46005,46006,46007
};

uint32 ui_EnrageTimer;
uint32 ui_DarknessTimer;

/*######
## Boss M'uru
######*/
struct MANGOS_DLL_DECL boss_muruAI : public Scripted_NoMovementAI
{
    boss_muruAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        Reset();
    }
    ScriptedInstance *m_pInstance;

    bool b_EntropiusPhase;
    uint8 ui_ShadowPortalNumber;

    uint32 ui_SummonHumanoids_Timer;
    uint32 ui_SummonVoidSentinel_Timer;
    uint32 ui_EntropiusCheck_Timer;

    uint64 ui_ShadowPortalGUID[6];
    uint64 ui_EntropiusGUID;

    void Reset()
    {
        b_EntropiusPhase = false;

        ui_ShadowPortalNumber = 0;

        ui_EnrageTimer = ENRAGE_TIMER;
        ui_DarknessTimer = DARKNESS_TIMER;
        ui_SummonHumanoids_Timer = SUMMON_HUMANOIDS_TIMER;
        ui_SummonVoidSentinel_Timer = SUMMON_VOID_SENTINEL_TIMER;
        ui_EntropiusCheck_Timer = 3000;

        ui_EntropiusGUID = 0;

        for (uint8 i = 0; i < 5; i++)
        {
            if (Creature* pMob = ((Creature*)Unit::GetUnit((*m_creature), ui_ShadowPortalGUID[i])))
                if (pMob->isAlive())
                    pMob->ForcedDespawn();
            ui_ShadowPortalGUID[i] = 0;
        }

        m_creature->RemoveAurasDueToSpell(SPELL_NEGATIVE_ENERGY_TRIGGER);

        if(m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        if(m_pInstance)
            m_pInstance->SetData(TYPE_MURU, NOT_STARTED);
    }

    void Aggro(Unit *who)
    {
        m_creature->SetInCombatWithZone();

        if(!m_creature->HasAura(SPELL_NEGATIVE_ENERGY_TRIGGER,0))
            m_creature->CastSpell(m_creature, SPELL_NEGATIVE_ENERGY_TRIGGER, true);

        if(m_pInstance)
            m_pInstance->SetData(TYPE_MURU, IN_PROGRESS);
    }

    void DamageTaken(Unit* done_by, uint32 &damage)
    {
        if (damage > m_creature->GetHealth() && !b_EntropiusPhase)
        {
            m_creature->CastSpell(m_creature, SPELL_ENTROPIUS_EFFECT, true);
            m_creature->CastSpell(m_creature, SPELL_SUMMON_ENTROPIUS, true);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            DoPlaySoundToSet(m_creature, SOUND_CHANGE_PHASE);

            for (uint8 i = 0; i < 5; i++)
            {
                if (Creature* pMob = ((Creature*)Unit::GetUnit((*m_creature), ui_ShadowPortalGUID[i])))
                    if (pMob->isAlive())
                        pMob->ForcedDespawn();
                ui_ShadowPortalGUID[i] = 0;
            }
            ui_ShadowPortalNumber = 0;
            b_EntropiusPhase = true;
        }
        if (b_EntropiusPhase)
        {
            damage = 0;
            return;
        }
    }

    void KilledUnit(Unit *Victim) {}

    void UpdateAI(const uint32 diff)
    {
        if (b_EntropiusPhase)
        {
            if(ui_EntropiusCheck_Timer < diff)
            {
                if (ui_EntropiusGUID)
                {
                    if (Creature* pEntropius = ((Creature*)Unit::GetUnit((*m_creature), ui_EntropiusGUID)))
                        if (pEntropius->isAlive() && !pEntropius->isInCombat())
                        {
                            pEntropius->ForcedDespawn();
                            b_EntropiusPhase = false;
                            EnterEvadeMode();
                        }
                        else if (!pEntropius->isAlive())
                        {
                            m_creature->ForcedDespawn();
                            if (m_pInstance)
                                m_pInstance->SetData(TYPE_MURU, DONE);
                        }
                }
                else
                {
                    if (Creature* pEntropius = GetClosestCreatureWithEntry(m_creature, 25840, 100))
                        if (pEntropius->isAlive())
                            ui_EntropiusGUID = pEntropius->GetGUID();
                }
                ui_EntropiusCheck_Timer = 1000;
            }
            else ui_EntropiusCheck_Timer -= diff;
            return;
        }

        //Return since we have no target
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

        //Goes in enrage
        if(ui_EnrageTimer < diff)
        {
            m_creature->CastSpell(m_creature, SPELL_ENRAGE, true);
            ui_EnrageTimer = 20000;
        }
        else ui_EnrageTimer -= diff;

        //Cast Darkness
        if(ui_DarknessTimer < diff)
        {
            m_creature->CastSpell(m_creature, SPELL_DARKNESS, false);

            for(uint8 i = 0; i < 4; i++) 
                m_creature->CastSpell(m_creature, DarkFiendsSpell[i], true);

            ui_DarknessTimer = DARKNESS_TIMER;
        }
        else ui_DarknessTimer -= diff;

        //Spawns Shadow portals and then one Void Sentinel
        if(ui_SummonVoidSentinel_Timer < diff)
        {
            float SP_SPAWN_X, SP_SPAWN_Y, SP_SPAWN_O = 0;

            if(ui_ShadowPortalNumber < 5) //Not every portal is spawned
            {
                SP_SPAWN_X = ShadowPortalSpawn[ui_ShadowPortalNumber][0];
                SP_SPAWN_Y = ShadowPortalSpawn[ui_ShadowPortalNumber][1];
                SP_SPAWN_O = ShadowPortalSpawn[ui_ShadowPortalNumber][2];

                if (Unit* pTemp = m_creature->SummonCreature(NPC_WORLD_TRIGGER, SP_SPAWN_X, SP_SPAWN_Y, SP_SPAWN_Z, SP_SPAWN_O, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 10000))
                {
                    pTemp->StopMoving();
                    pTemp->GetMotionMaster()->Clear(false); // No moving!
                    pTemp->CastSpell(pTemp, SPELL_SHADOW_PORTAL, true);
                    ui_ShadowPortalGUID[ui_ShadowPortalNumber] = pTemp->GetGUID();
                }
                ui_ShadowPortalNumber++;
            }
            else // Choose 1 random portal in wich spawn Void Sentinel
            {
                uint8 RandSpwn = rand()%5;

                SP_SPAWN_X = ShadowPortalSpawn[RandSpwn][0];
                SP_SPAWN_Y = ShadowPortalSpawn[RandSpwn][1];
                SP_SPAWN_O = ShadowPortalSpawn[RandSpwn][2];
            }

            Creature* VoidSentinel = m_creature->SummonCreature(NPC_VOID_SENTINEL, SP_SPAWN_X, SP_SPAWN_Y, m_creature->GetPositionZ(), SP_SPAWN_O, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 10000);
            if(VoidSentinel)
                VoidSentinel->AI()->AttackStart(m_creature->getVictim());

            ui_SummonVoidSentinel_Timer = SUMMON_VOID_SENTINEL_TIMER + rand()%15000;
        }
        else ui_SummonVoidSentinel_Timer -= diff;

        //Summon 6 humanoids every min (1mage & 2berserkers)
        if(ui_SummonHumanoids_Timer < diff)
        {
            for(uint8 i = 0; i < 6; i++)
            {
                if(i == 2)
                    m_creature->CastSpell(m_creature, SPELL_SUMMON_FURY_MAGE, true);
                else
                    m_creature->CastSpell(m_creature, SPELL_SUMMON_BERSERKER, true);
            }
            ui_SummonHumanoids_Timer = SUMMON_HUMANOIDS_TIMER;
        }
        else ui_SummonHumanoids_Timer -= diff;
    }
};

/*######
## Boss Entropius
######*/
struct MANGOS_DLL_DECL boss_entropiusAI : public ScriptedAI
{
    boss_entropiusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        Reset();
    }
    ScriptedInstance *m_pInstance;

    uint32 ui_SingularityTimer;

    void Reset()
    {
        ui_DarknessTimer = DARKNESS_TIMER;
        ui_SingularityTimer = SINGULARITY_TIMER;
    }

    void Aggro(Unit *who)
    {
        m_creature->SetInCombatWithZone();

        if(!m_creature->HasAura(SPELL_ENTROPIUS_NEGATIVE_ENERGY_TRIGGER,0))
            m_creature->CastSpell(m_creature, SPELL_ENTROPIUS_NEGATIVE_ENERGY_TRIGGER, true);
    }

    void JustDied(Unit* Killer) {}
    void KilledUnit(Unit *Victim) {}

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

        //Goes in enrage
        if(ui_EnrageTimer < diff)
        {
            m_creature->CastSpell(m_creature, SPELL_ENRAGE, true);
            ui_EnrageTimer = 20000;
        }
        else ui_EnrageTimer -= diff;

        //Cast Darkness
        if(ui_DarknessTimer < diff)
        {
            m_creature->CastSpell(SelectUnit(SELECT_TARGET_RANDOM, 0), SPELL_ENTROPIUS_VOID_ZONE, false);
            ui_DarknessTimer = DARKNESS_TIMER;
        }
        else ui_DarknessTimer -= diff;

        //Summons Singularity
        if(ui_SingularityTimer < diff)
        {
            Unit* sTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);

            Creature* Singularity = m_creature->SummonCreature(NPC_SINGULARITY, sTarget->GetPositionX(), sTarget->GetPositionY(), sTarget->GetPositionZ(), m_creature->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, SINGULARITY_TIMER-5000);
            if(Singularity)
            {
                Singularity->CastSpell(Singularity, 46228, true);  //Sigularity aura
                Singularity->AI()->AttackStart(sTarget);
            }
            ui_SingularityTimer = SINGULARITY_TIMER;
        }
        else ui_SingularityTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

/*######
## Mob Dark Fiend
######*/
struct MANGOS_DLL_DECL dark_fiendAI : public ScriptedAI
{
    dark_fiendAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    void Reset() {}

    void Aggro(Unit *who)
    {
        if(!m_creature->HasAura(SPELL_DARK_FIEND_AURA,0))
            m_creature->CastSpell(m_creature, SPELL_DARK_FIEND_AURA, true);
    }

    void JustDied(Unit* Killer) {}
    void KilledUnit(Unit *Victim) {}

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

        if( m_creature->isAttackReady() && !m_creature->IsNonMeleeSpellCasted(false))
        {
            //If we are within range melee the target
            if( m_creature->IsWithinDistInMap(m_creature->getVictim(), ATTACK_DISTANCE))
            {
                m_creature->CastSpell(m_creature->getVictim(), SPELL_DARK_FIEND_DEBUFF, true);
                //m_creature->AttackerStateUpdate(m_creature->getVictim());
                //m_creature->resetAttackTimer();
            }
        }
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_muru(Creature *_Creature)
{
    return new boss_muruAI(_Creature);
}

CreatureAI* GetAI_boss_entropius(Creature *_Creature)
{
    return new boss_entropiusAI(_Creature);
}

CreatureAI* GetAI_dark_fiend(Creature *_Creature)
{
    return new dark_fiendAI(_Creature);
}

void AddSC_boss_muru_entropius()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_muru";
    newscript->GetAI = &GetAI_boss_muru;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_entropius";
    newscript->GetAI = &GetAI_boss_entropius;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "dark_fiend";
    newscript->GetAI = &GetAI_dark_fiend;
    newscript->RegisterSelf();
}
