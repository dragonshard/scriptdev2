/* Copyright 2006 - 2008 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: boss_kiljaeden
SDAuthor: ckegg
SD%Complete: 80%
SDComment: Missing ending animation, and Sinister Reflection need to be implemented.
SDCategory: Sunwell_Plateau
EndScriptData */
#include "precompiled.h"
#include "def_sunwell_plateau.h"

enum
{
    /*** Speech and sounds***/
    // These are used throughout Sunwell and Magisters(?). Players can hear this while running through the instances.
    SAY_KJ_OFFCOMBAT1                       = -1580056,
    SAY_KJ_OFFCOMBAT2                       = -1580057,
    SAY_KJ_OFFCOMBAT3                       = -1580058,
    SAY_KJ_OFFCOMBAT4                       = -1580059,
    SAY_KJ_OFFCOMBAT5                       = -1580060,

    // Encounter speech and sounds
    SAY_KJ_EMERGE                           = -1580061,
    SAY_KJ_SLAY1                            = -1580062,
    SAY_KJ_SLAY2                            = -1580063,
    SAY_KJ_REFLECTION1                      = -1580064,
    SAY_KJ_REFLECTION2                      = -1580065,
    SAY_KJ_DARKNESS1                        = -1580066,
    SAY_KJ_DARKNESS2                        = -1580067,
    SAY_KJ_DARKNESS3                        = -1580068,
    SAY_KJ_CANNOT_WIN                       = -1580070,
    SAY_KJ_DENINE                           = -1580069,
    SAY_KJ_LOST_POWER                       = -1580071,

    /*** Kalecgos - Anveena speech at the beginning of Phase 5; Anveena's sacrifice ***/
    SAY_KALECGOS_INTRO                      = -1580072,
    SAY_KALECGOS_AWAKEN                     = -1580073,
    SAY_ANVEENA_IMPRISONED                  = -1580074,
    SAY_KALECGOS_LETGO                      = -1580075,
    SAY_ANVEENA_LOST                        = -1580076,
    SAY_KALECGOS_FOCUS                      = -1580077,
    SAY_ANVEENA_KALEC                       = -1580078,
    SAY_KALECGOS_FATE                       = -1580079,
    SAY_ANVEENA_GOODBYE                     = -1580080,
    SAY_KALECGOS_GOODBYE                    = -1580081,
    SAY_KALECGOS_ENCOURAGE                  = -1580082,
    SAY_KALECGOS_ORB1                       = -1580083,
    SAY_KALECGOS_ORB2                       = -1580084,
    SAY_KALECGOS_ORB3                       = -1580085,
    SAY_KALECGOS_ORB4                       = -1580086,

    /*** Spells used during the encounter ***/
    /* Hand of the Deceiver's spells and cosmetics */
    SPELL_SHADOW_BOLT_VOLLEY               = 45770, // ~30 yard range Shadow Bolt Volley for ~2k(?) damage
    SPELL_SHADOW_INFUSION                  = 45772, // They gain this at 20% - Immunity to Stun/Silence and makes them look angry!
    SPELL_FELFIRE_PORTAL                   = 46875, // Creates a portal that spawns Felfire Fiends (LIVE FOR THE SWARM!1 FOR THE OVERMIND!)
    SPELL_SHADOW_CHANNELING                = 46757, // Channeling animation out of combat

    /* Volatile Felfire Fiend's spells */
    SPELL_FELFIRE_FISSION                  = 45779, // Felfire Fiends explode when they die or get close to target.

    /* Kil'Jaeden's spells and cosmetics */
    SPELL_TRANS                            = 23188, // Surprisingly, this seems to be the right spell.. (Where is it used?)
    SPELL_REBIRTH                          = 44200, // Emerge from the Sunwell
    SPELL_SOUL_FLAY_DAMAGE                 = 45442, // 9k Shadow damage over 3 seconds. Spammed throughout all the fight.
    SPELL_SOUL_FLAY_MOVEMENT               = 47106, // -50% Movement component of the above. Players cast it on selves unless this spell is fixed.
    SPELL_LEGION_LIGHTNING                 = 45664, // Chain Lightning, 4 targets, ~3k Shadow damage, 1.5k mana burn
    SPELL_FIRE_BLOOM                       = 45641, // Places a debuff on 5 raid members, which causes them to deal 2k Fire damage to nearby allies and selves. MIGHT NOT WORK
    SPELL_SINISTER_REFLECTION              = 45785, // Summon shadow copies of 5 raid members that fight against KJ's enemies
    SPELL_COPY_WEAPON                      = 41055, // }
    SPELL_COPY_WEAPON2                     = 41054, // }
    SPELL_COPY_OFFHAND                     = 45206, // }- Spells used in Sinister Reflection creation
    SPELL_COPY_OFFHAND_WEAPON              = 45205, // }
    SPELL_SINISTER_REFLECTION_CLASS        = 45893,
    SPELL_SINISTER_REFLECTION_CLONE        = 45785, // Cause the target to become a clone of the caster.
    SPELL_SINISTER_REFLECTION_SUMMON       = 45891,
    SPELL_SHADOW_SPIKE                     = 46680, // Bombard random raid members with Shadow Spikes (Very similar to Void Reaver orbs)
    SPELL_SHADOW_SPIKE_VISUAL              = 46589,
    SPELL_SHADOW_SPIKE_EFFECT              = 45885, // Inflicts 5100 to 6900% Shadow damage to an enemy and leaves it wounded, reducing the effectiveness of any healing by 50% for 10 sec.
    SPELL_FLAME_DART                       = 45740, // Bombards the raid with flames every 3(?) seconds
    SPELL_DARKNESS_OF_A_THOUSAND_SOULS     = 46605, // Begins a 8-second channeling, after which he will deal 50'000 damage to the raid
    SPELL_DARKNESS_OF_A_THOUSAND_SOULS_EFFECT = 45657, // Deals 47500 to 52500 Shadow damage to all enemies within reach.
    SPELL_ARMAGEDDON                       = 45909, // Meteor spell
    SPELL_ARMAGEDDON_VISUAL                = 45911, // Does the hellfire visual to indicate where the meteor missle lands
    SPELL_SACRIFICE_OF_ANVEENA             = 46474, // Anveena sacrifices herself, causing Kil'jaeden to take 25% increased Holy damage.

    /* Anveena's spells and cosmetics (Or, generally, everything that has "Anveena" in name) */
    SPELL_ANVEENA_PRISON                   = 46367, // She hovers locked within a bubble
    SPELL_ANVEENA_ENERGY_DRAIN             = 46410, // Sunwell energy glow animation (Control mob uses this)

    /* Shield Orb spells */
//    SPELL_SHADOW_BOLT                      = 45680, // constantly shooting Shadow Bolts at the raid (up to 3 bolts within 1 second)
    SPELL_SHADOW_BOLT                      = 30505, // 45680 doesn't work, use 36714(1275~1725) for worksaround, or 30505 (1063~1437)

    /* Orb of the Blue Dragonflight */
    SPELL_BLINK                            = 45862,
    SPELL_BREATH_REVITALIZE                = 45860,
    SPELL_BREATH_HASTE                     = 45856,
    SPELL_SHIELD_OF_THE_BLUE               = 45848,

    /*** Other Spells (used by players, etc) ***/
    SPELL_VENGEANCE_OF_THE_BLUE_FLIGHT     = 45839, // Possess the blue dragon from the orb to help the raid.
    SPELL_POWER_OF_THE_BLUE_FLIGHT         = 45833, // Empowers a humanoid with the essence of the Blue Flight.
    SPELL_SUMMON_BLUE_DRAKE                = 45836, // Summon (25653)
    SPELL_ENTROPIUS_BODY                   = 46819, // Visual for Entropius at the Epilogue
    SPELL_RING_OF_BLUE_FLAME               = 45825, // Blue Flame Ring on actived orb

    /*** Creatures used in the encounter ***/
    NPC_ANVEENA                            = 26046, // Embodiment of the Sunwell
    NPC_KILJAEDEN                          = 25315, // Give it to 'em KJ!
    NPC_HAND_OF_THE_DECEIVER               = 25588, // Adds found before KJ emerges
    NPC_FELFIRE_PORTAL                     = 25603, // Portal spawned be Hand of the Deceivers
    NPC_VOLATILE_FELFIRE_FIEND             = 25598, // Fiends spawned by the above portal
    NPC_ARMAGEDDON_TARGET                  = 25735, // This mob casts meteor on itself.. I think
    NPC_SHIELD_ORB                         = 25502, // Shield orbs circle the room raining shadow bolts on raid
    NPC_THE_CORE_OF_ENTROPIUS              = 26262, // Used in the ending cinematic?
    NPC_POWER_OF_THE_BLUE_DRAGONFLIGHT     = 25653, // NPC that players possess when using the Orb of the Blue Dragonflight

    /*** GameObjects ***/
    GO_ORB_OF_THE_BLUE_DRAGONFLIGHT        = 188415,
/* there are several more, probably changes its looking with ring of flame
    GO_ORB_OF_THE_BLUE_DRAGONFLIGHT        = 188114,
    GO_ORB_OF_THE_BLUE_DRAGONFLIGHT        = 188115,
    GO_ORB_OF_THE_BLUE_DRAGONFLIGHT        = 188116,
    GO_ORB_OF_THE_BLUE_DRAGONFLIGHT        = 187869,
*/

    /*** Others ***/
    FLOOR_Z                                = 28,
    SHIELD_ORB_Z                           = 39 // TODO: Find correct height.
};

enum Phase
{
    PHASE_NOTSTART   = 0, //
    PHASE_DECEIVERS  = 1, // Fight 3 adds
    PHASE_NORMAL     = 2, // Kil'Jaeden emerges from the sunwell
    PHASE_DARKNESS   = 3, // At 85%, he gains few abilities; Kalecgos joins the fight
    PHASE_ARMAGEDDON = 4, // At 55%, he gains even more abilities
    PHASE_SACRIFICE  = 5 // At 25%, Anveena sacrifices herself into the Sunwell; at this point he becomes enraged and has *significally* shorter cooldowns.
};

/*######
## Boss Kalecgos
######*/
struct MANGOS_DLL_DECL mob_kalecgosAI : public ScriptedAI
{
    mob_kalecgosAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        Reset();
    }
    ScriptedInstance* m_pInstance;

    uint32 ui_OrbsEmpowered;
    uint32 ui_EmpowerTimer;
    uint64 ui_Orb[4];

    void Reset()
    {
        ui_OrbsEmpowered = 0;
        ui_EmpowerTimer = 0;

        for(uint8 i = 0; i < 4; ++i)
            ui_Orb[i] = 0;

        FindOrbs();

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetMonsterMoveFlags(MONSTER_MOVE_LEVITATING);
    }

    void UpdateAI(const uint32 diff) {
        // Empower a orb
        if (ui_EmpowerTimer)
            if(ui_EmpowerTimer < diff)
            {
                switch(ui_OrbsEmpowered)
                {
                    case 0: DoScriptText(SAY_KALECGOS_ORB1, m_creature);break;
                    case 1: DoScriptText(SAY_KALECGOS_ORB2, m_creature);break;
                    case 2: DoScriptText(SAY_KALECGOS_ORB3, m_creature);break;
                    case 3: DoScriptText(SAY_KALECGOS_ORB4, m_creature);break;
                }

                EmpowerOrb(ui_OrbsEmpowered);
                ui_OrbsEmpowered++;

                if (ui_OrbsEmpowered == 3)
                    ui_EmpowerTimer = 20000;
                else
                    ui_EmpowerTimer = 0;

            }else ui_EmpowerTimer -= diff;
    }

    void FindOrbs()
    {
        std::list<GameObject*> orbList;
        GetGameObjectListWithEntryInGrid(orbList,m_creature, GO_ORB_OF_THE_BLUE_DRAGONFLIGHT,100.0f);

        if(orbList.empty())
            return;

        uint8 i = 0;
        for(std::list<GameObject*>::iterator itr = orbList.begin(); itr != orbList.end(); ++itr)
        {
            ui_Orb[i] = (*itr)->GetGUID();
            (*itr)->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_IN_USE);
            (*itr)->Refresh();
            i++;
        }
    }

    void EmpowerOrb(uint32 ui_ActiveOrb)
    {
        if (ui_ActiveOrb < 4)
    	{
            GameObject* pOrb = m_pInstance->instance->GetGameObject(ui_Orb[ui_ActiveOrb]);
            if(!pOrb)
                return;

            // workaround effect
            if (Unit* pSummon = m_creature->SummonCreature(25708, pOrb->GetPositionX(), pOrb->GetPositionY(), pOrb->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 300000))
            {
                pSummon->SetDisplayId(11686);
                pSummon->setFaction(35);
                pSummon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                pSummon->CastSpell(pSummon, SPELL_RING_OF_BLUE_FLAME, true);
            }

            //m_creature->CastSpell(orb, SPELL_RING_OF_BLUE_FLAME, true);
            pOrb->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_IN_USE);
            pOrb->Refresh();
        }
    }
};

/*######
## Mob Shield Orb
######*/
struct MANGOS_DLL_DECL mob_shield_orbAI : public ScriptedAI
{
    mob_shield_orbAI(Creature* pCreature) : ScriptedAI(pCreature) {
        m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        Reset();
    }
    ScriptedInstance* m_pInstance;

    uint32 ui_ShadowBoltTimer;
    uint32 ui_MovementTimer;
    uint32 ui_MovingSteps;

    void Reset() {
        ui_ShadowBoltTimer = 1000 + rand()%3000;
        ui_MovementTimer = 1000 + rand()%2000;
        ui_MovingSteps = 0;
    }

    void Aggro(Unit* who)
    {
        m_creature->SetInCombatWithZone();
    }

    void UpdateAI(const uint32 diff) {

        if(ui_ShadowBoltTimer < diff)
        {
        	for(uint8 i = 0; i < rand()%3; ++i)
                DoCast(SelectUnit(SELECT_TARGET_RANDOM,0), SPELL_SHADOW_BOLT);
            ui_ShadowBoltTimer = 1000 + rand()%3000;
        }else ui_ShadowBoltTimer -= diff;

        if(ui_MovementTimer < diff)
        {
            if (m_pInstance)
            {
                if(Creature* pKiljaeden = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(DATA_KILJAEDEN))))
                {
                    float angle = (M_PI / 10) * ui_MovingSteps;
                    float X = pKiljaeden->GetPositionX() + 22 * cos(angle);
                    float Y = pKiljaeden->GetPositionY() + 22 * sin(angle);

                    m_creature->GetMotionMaster()->MovePoint(0, X, Y, SHIELD_ORB_Z);

                    ui_MovingSteps++;
                    if (ui_MovingSteps > 19)
                        ui_MovingSteps = 0;
                }
            }
            ui_MovementTimer = 1000;
        }else ui_MovementTimer -= diff;
    }
};

/*######
## Boss Kil'jaeden
######*/
struct MANGOS_DLL_DECL boss_kiljaedenAI : public Scripted_NoMovementAI
{
    boss_kiljaedenAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        Reset();
    }

    /* General */
    ScriptedInstance* m_pInstance;
    uint32 ui_Phase;

    /* Phase 2+ spells */
    uint32 ui_SoulFlay_Timer;
    uint32 ui_LegionLightning_Timer;
    uint32 ui_FireBloom_Timer;
    uint32 ui_SummonShieldOrb_Timer;

    /* Phase 3+ spells */
    uint32 ui_FlameDart_Timer;
    uint32 ui_Darkness_Timer;
    uint32 ui_DarknessBomb_Timer;
    uint32 ui_ShadowSpike_Timer;
    uint32 ui_ShadowSpikeCount;

    /* Phase 4+ spells */
    uint32 ui_Armageddon_Timer;

    void Reset()
    {
        // TODO: Fix timers
        ui_SoulFlay_Timer         = 15000;
        ui_LegionLightning_Timer  = 30000;
        ui_FireBloom_Timer        = 30000;
        ui_SummonShieldOrb_Timer  = 45000;

        ui_FlameDart_Timer        = 3000;
        ui_Darkness_Timer         = 60000;
        ui_DarknessBomb_Timer     = 0;
        ui_ShadowSpike_Timer      = 5000;
        ui_ShadowSpikeCount       = 30000;

        ui_Armageddon_Timer       = 10000;

        ui_Phase = PHASE_NOTSTART;
    }

    void EnterEvadeMode()
    {
        Scripted_NoMovementAI::EnterEvadeMode();
        ui_Phase = PHASE_NOTSTART;

        // Reset the controller
        if(m_pInstance)
            m_pInstance->SetData(TYPE_KILJAEDEN, NOT_STARTED);
    }

    void Aggro(Unit* who)
    {
        m_creature->SetInCombatWithZone();
    }

    void MoveInLineOfSight(Unit* who)
    {
        if (who->isTargetableForAttack() && who->isInAccessablePlaceFor(m_creature) && m_creature->IsHostileTo(who) && !m_creature->getVictim())
            AttackStart(who);
    }

    void CastSinisterReflection()
    {
//        for (uint8 i = 0; i < 4; ++i) // disadvantage is it may be duplicated target
//            DoCast(SelectUnit(SELECT_TARGET_RANDOM, 0), SPELL_SINISTER_REFLECTION);
        // workaround
        for (uint8 i = 0; i < 4; ++i)
        {
            if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
            {
                Unit* pSummon = pTarget->SummonCreature(25708, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 45000);
                pSummon->SetDisplayId(pTarget->GetDisplayId());
                pSummon->setFaction(m_creature->getFaction());
                pSummon->SetMaxHealth(pTarget->GetMaxHealth());
                pSummon->SetHealth(pTarget->GetMaxHealth());
                pSummon->CastSpell(pTarget, SPELL_SINISTER_REFLECTION_CLASS, true);
                pSummon->CastSpell(pTarget, SPELL_SINISTER_REFLECTION_CLONE, true);
                pSummon->CastSpell(pTarget, SPELL_COPY_WEAPON, true);
                pSummon->CastSpell(pTarget, SPELL_COPY_WEAPON2, true);
                pSummon->CastSpell(pTarget, SPELL_COPY_OFFHAND, true);
                pSummon->CastSpell(pTarget, SPELL_COPY_OFFHAND_WEAPON, true);
            }
        }

        switch(rand()%2)
        {
            case 0: DoScriptText(SAY_KJ_REFLECTION1, m_creature);break;
            case 1: DoScriptText(SAY_KJ_REFLECTION2, m_creature);break;
        }
    } // TODO: Create copies of 5 random raid members.

    void UpdateAI(const uint32 diff)
    {
        if(!m_creature->SelectHostilTarget() || !m_creature->getVictim() || ui_Phase < PHASE_NORMAL)
            return;

        // *****************************************
        // *********** Phase 2 spells **************
        // *****************************************

        // Soul Flay : 9k damage over 3 seconds on highest aggro
        if(ui_SoulFlay_Timer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_SOUL_FLAY_DAMAGE);
            m_creature->getVictim()->CastSpell(m_creature->getVictim(), SPELL_SOUL_FLAY_MOVEMENT, true);
            ui_SoulFlay_Timer = 3500;
        }else ui_SoulFlay_Timer -= diff;

        // Legion Lightning : Lightning on random target, jumps to 3 additional players, burns 1500 mana and deals 3k damage
        if(ui_LegionLightning_Timer < diff)
        {
            m_creature->InterruptNonMeleeSpells(true);
            DoCast(SelectUnit(SELECT_TARGET_RANDOM, 0), SPELL_LEGION_LIGHTNING);
            ui_LegionLightning_Timer = (ui_Phase == PHASE_SACRIFICE) ? 18000 : 30000; // 18 seconds in PHASE_SACRIFICE
        }else ui_LegionLightning_Timer -= diff;

        // Fire Bloom : Places a debuff on 5 random targets; targets and all nearby allies suffer ~1.7k fire damage every 2 seconds
        if(ui_FireBloom_Timer < diff)
        {
            m_creature->InterruptNonMeleeSpells(true);
            for (uint8 i = 0; i < 5; ++i) // disadvantage is it may be duplicated target
                DoCast(SelectUnit(SELECT_TARGET_RANDOM, 0), SPELL_FIRE_BLOOM);
            ui_FireBloom_Timer = (ui_Phase == PHASE_SACRIFICE) ? 25000 : 40000; // 25 seconds in PHASE_SACRIFICE
        }else ui_FireBloom_Timer -= diff;

        // Summon Shield Orb : Spawns a Shield Orb add and constantly shooting Shadow Bolts at the raid
        if(ui_SummonShieldOrb_Timer < diff)
        {
            // northeast of Kil'jaeden
            for(uint8 i = 1; i < ui_Phase; ++i) {
                if (Creature* SOrb = m_creature->SummonCreature(NPC_SHIELD_ORB, 1709.000, 615.000, SHIELD_ORB_Z, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 45000))
                    if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                        SOrb->AI()->AttackStart(pTarget);
            }
            ui_SummonShieldOrb_Timer = (ui_Phase == PHASE_SACRIFICE) ? 30000 : 60000; // 30 seconds in PHASE_SACRIFICE
        }else ui_SummonShieldOrb_Timer -= diff;

        // *****************************************
        // *********** Phase 3 spells **************
        // *****************************************
        if(ui_Phase == PHASE_NORMAL && ((m_creature->GetHealth()*100 / m_creature->GetMaxHealth()) < 85))
        {
            m_creature->InterruptNonMeleeSpells(true);

            // Sinister Reflection
            CastSinisterReflection();

            // Shadow Spike
            DoCast(SelectUnit(SELECT_TARGET_RANDOM, 0), SPELL_SHADOW_SPIKE);

            ui_SoulFlay_Timer         = 30000; // Don't let other spells
            ui_LegionLightning_Timer += 30000; // interrupt Shadow Spikes
            ui_FireBloom_Timer       += 30000;
            ui_SummonShieldOrb_Timer += 30000;
            ui_FlameDart_Timer       += 30000;
            //ui_Darkness_Timer        += 30000;
            ui_ShadowSpikeCount      = 30000; // shadow spike visual timer

            // Active orb after 35 seconds
            if (Creature* pKalecgos = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(DATA_KALECGOS))))
                ((mob_kalecgosAI*)pKalecgos->AI())->ui_EmpowerTimer = 35000;

            ui_Phase = PHASE_DARKNESS;
        }
        else if(ui_Phase <= PHASE_NORMAL) return;

        // Shadow Spike Effect Count
        if (ui_ShadowSpikeCount)
            if(ui_ShadowSpike_Timer < diff)
            {
                // workaround
                if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                    m_creature->CastSpell(pTarget, SPELL_SHADOW_SPIKE_VISUAL, true);

                ui_ShadowSpike_Timer = 5000;
                ui_ShadowSpikeCount -= ui_ShadowSpike_Timer;
            }else ui_ShadowSpike_Timer -= diff;

        // Flame Dart Explosion
        if(ui_FlameDart_Timer < diff)
        {
            DoCast(SelectUnit(SELECT_TARGET_RANDOM, 0), SPELL_FLAME_DART);
            ui_FlameDart_Timer = 3000;
        }else ui_FlameDart_Timer -= diff;

        // Darkness of a Thousand Souls : Begins to channel for 8 seconds, then deals 50'000 damage to all raid members.
        if(ui_Darkness_Timer < diff)
        {
            m_creature->InterruptNonMeleeSpells(true);
            //DoCast(m_creature, SPELL_DARKNESS_OF_A_THOUSAND_SOULS);
            m_creature->CastSpell(m_creature, SPELL_DARKNESS_OF_A_THOUSAND_SOULS, true);

            ui_SummonShieldOrb_Timer += 9000; // Don't let other spells
            ui_FireBloom_Timer       += 9000; // interrupt Darkness of a Thousand Souls
            ui_LegionLightning_Timer += 9000;
            ui_FlameDart_Timer       += 9000;
            if(ui_Phase >= PHASE_ARMAGEDDON)
                ui_Armageddon_Timer  += 9000; // Armageddon on the other hand, can be casted now, if Anveena has already sacrificed
            ui_SoulFlay_Timer         = 9000;
            ui_DarknessBomb_Timer     = 8000; // Darkness of a Thousand Souls effect timer

            switch(rand()%3)
            {
                case 0: DoScriptText(SAY_KJ_DARKNESS1, m_creature);break;
                case 1: DoScriptText(SAY_KJ_DARKNESS2, m_creature);break;
                case 2: DoScriptText(SAY_KJ_DARKNESS3, m_creature);break;
            }

            ui_Darkness_Timer = (ui_Phase == PHASE_SACRIFICE) ? 20000 + rand()%15000 : 40000 + rand()%30000;
        }else ui_Darkness_Timer -= diff;

        // Darkness of a Thousand Souls Effect
        if (ui_DarknessBomb_Timer)
            if(ui_DarknessBomb_Timer < diff)
            {
                DoCast(m_creature, SPELL_DARKNESS_OF_A_THOUSAND_SOULS_EFFECT);
                ui_DarknessBomb_Timer = 0;
            }else ui_DarknessBomb_Timer -= diff;

        // *****************************************
        // *********** Phase 4 spells **************
        // *****************************************
        if(ui_Phase == PHASE_DARKNESS && ((m_creature->GetHealth()*100 / m_creature->GetMaxHealth()) < 55))
        {
            m_creature->InterruptNonMeleeSpells(true);

            // Sinister Reflection
            CastSinisterReflection();

            // Shadow Spike
            DoCast(SelectUnit(SELECT_TARGET_RANDOM, 0), SPELL_SHADOW_SPIKE);

            ui_SoulFlay_Timer         = 30000; // Don't let other spells
            ui_LegionLightning_Timer += 30000; // interrupt Shadow Spikes
            ui_FireBloom_Timer       += 30000;
            ui_SummonShieldOrb_Timer += 30000;
            ui_FlameDart_Timer       += 30000;
            ui_Darkness_Timer        += 30000;
            ui_Armageddon_Timer      += 30000;
            ui_ShadowSpikeCount      = 30000; // shadow spike visual timer

            // Active orb after 35 seconds
            if (Creature* pKalecgos = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(DATA_KALECGOS))))
                ((mob_kalecgosAI*)pKalecgos->AI())->ui_EmpowerTimer = 35000;

            ui_Phase = PHASE_ARMAGEDDON;
        }
        else if(ui_Phase <= PHASE_DARKNESS) return;

        // Armageddon
        if(ui_Armageddon_Timer < diff)
        {
            if(Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
            {
                float x, y, z;
                pTarget->GetPosition(x, y, z);
                if(Creature* Armageddon = m_creature->SummonCreature(NPC_ARMAGEDDON_TARGET, x, y, z, 0, TEMPSUMMON_TIMED_DESPAWN, 5000))
                {
                    Armageddon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    DoCast(Armageddon, SPELL_ARMAGEDDON, true);
                    Armageddon->CastSpell(Armageddon, SPELL_ARMAGEDDON_VISUAL, true);
                }
            }
            ui_Armageddon_Timer = 2000; // No, I'm not kidding
        }else ui_Armageddon_Timer -= diff;


        // *****************************************
        // *********** Phase 5 spells **************
        // *****************************************
        if(ui_Phase == PHASE_ARMAGEDDON && ((m_creature->GetHealth()*100 / m_creature->GetMaxHealth()) < 25))
        {
            m_creature->InterruptNonMeleeSpells(true);

            // Sinister Reflection
            CastSinisterReflection();

            // Shadow Spike
            DoCast(SelectUnit(SELECT_TARGET_RANDOM, 0), SPELL_SHADOW_SPIKE);

            ui_SoulFlay_Timer         = 30000; // Don't let other spells
            ui_LegionLightning_Timer += 30000; // interrupt Shadow Spikes
            ui_FireBloom_Timer       += 30000;
            ui_SummonShieldOrb_Timer += 30000;
            ui_FlameDart_Timer       += 30000;
            ui_Darkness_Timer        += 30000;
            ui_Armageddon_Timer      += 30000;
            ui_ShadowSpikeCount      = 30000; // shadow spike visual timer

            // Active orb after 35 seconds
            if (Creature* pKalecgos = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(DATA_KALECGOS))))
                ((mob_kalecgosAI*)pKalecgos->AI())->ui_EmpowerTimer = 35000;

            ui_Phase = PHASE_SACRIFICE;
        }
        else if(ui_Phase <= PHASE_ARMAGEDDON) return;
    }

    void KilledUnit(Unit *victim)
    {
        if (victim == m_creature)
            return;

        switch(rand()%2)
        {
            case 0: DoScriptText(SAY_KJ_SLAY1, m_creature);break;
            case 1: DoScriptText(SAY_KJ_SLAY2, m_creature);break;
        }
    }

    void JustDied(Unit* killer)
	{
        if (m_pInstance)
            m_pInstance->SetData(TYPE_KILJAEDEN, DONE);
	}
};

/*######
## Mob Kil'jaeden Controller , controls most of the event
######*/
struct MANGOS_DLL_DECL mob_kiljaeden_controllerAI : public Scripted_NoMovementAI
{
    mob_kiljaeden_controllerAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        Reset();

        // hack spell 45885
        SpellEntry* TempSpell = (SpellEntry*)GetSpellStore()->LookupEntry(SPELL_SHADOW_SPIKE_EFFECT);
        if (TempSpell && TempSpell->EffectImplicitTargetB[0] != 16)
        {
               TempSpell->EffectImplicitTargetA[0] = 53;
               TempSpell->EffectImplicitTargetA[1] = 0;
               TempSpell->EffectImplicitTargetB[0] = 16;
               TempSpell->EffectImplicitTargetB[1] = 0;
        }
        TempSpell = (SpellEntry*)GetSpellStore()->LookupEntry(SPELL_SHIELD_OF_THE_BLUE);
        if (TempSpell && TempSpell->EffectImplicitTargetB[0] != 22)
        {
               TempSpell->EffectImplicitTargetB[0] = 22;
        }
    }

    ScriptedInstance* m_pInstance;
    std::list<uint64> ui_HandDeceivers;

    bool b_DevicerChanneling;
    bool b_AnveenaSpellCheck;
    bool b_KalecgosResetCheck;

    uint32 ui_Phase;
    uint32 ui_Event_Timer;
    uint32 ui_AnveenaSpeech_Timer;
    uint32 ui_AnveenaSpeechCounter;

    void Reset()
    {
        b_DevicerChanneling = false;
        b_AnveenaSpellCheck = false;
        b_KalecgosResetCheck = false;

        ui_Phase = PHASE_NOTSTART;
        ui_Event_Timer          = 1000;
        ui_AnveenaSpeech_Timer  = 0;
        ui_AnveenaSpeechCounter = 0;

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_KILJAEDEN, NOT_STARTED);

            if (Creature* pKalecgosAnveena = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(DATA_KALECGOS))))
                pKalecgosAnveena->Respawn();

            if (Creature* pKalecgos = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(DATA_KALECGOS))))
                ((mob_kalecgosAI*)pKalecgos->AI())->Reset();
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if(ui_Event_Timer < diff)
        {
            if (ui_Phase == PHASE_NOTSTART)
            {
                Unit* pTarget = NULL;
                uint32 ui_DeciverCount = 0;
                uint64 ui_DeciverGUID[3];

                for(uint8 i = 0; i < 3; ++i)
                    ui_DeciverGUID[i] = 0;

            	FindHandDeceivers();
                for(std::list<uint64>::iterator itr = ui_HandDeceivers.begin(); itr != ui_HandDeceivers.end(); ++itr)
                {
                    if (Creature* pCreature = ((Creature*)Unit::GetUnit(*m_creature, *itr)))
                    {
                        // alive & no target
                        if (pCreature->isAlive() && !pCreature->getVictim())
                        {
                            if (ui_DeciverCount < 3)
                                ui_DeciverGUID[ui_DeciverCount] = pCreature->GetGUID();
                            ui_DeciverCount++;
                        }
                        // alive & has target
                        else if (pCreature->isAlive() && pCreature->getVictim())
                            pTarget = pCreature->getVictim();
                        // dead, respawn
                        else if (pCreature->isDead())
                        {
                            pCreature->RemoveCorpse();
                            pCreature->Respawn();
                            if (ui_DeciverCount < 3)
                                ui_DeciverGUID[ui_DeciverCount] = pCreature->GetGUID();
                            ui_DeciverCount++;
                        }
                    }
                }

                // aggro, link to others
                if (pTarget)
                {
                    for(uint8 i = 0; i < 2; ++i)
                        if (Unit* pDeciver = Unit::GetUnit((*m_creature), ui_DeciverGUID[i]))
                            ((Creature*)pDeciver)->AI()->AttackStart(pTarget);

                    if (m_pInstance)
                        if (Creature* pAnveena = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(DATA_ANVEENA))))
                        {
                            pAnveena->GetMap()->CreatureRelocation(pAnveena, 1698.45, 628.03, 70.1989, 0.0f);
                            pAnveena->SendMonsterMove(1698.45, 628.03, 70.1989, 0, MONSTER_MOVE_NONE, 0);
                        }

                    ui_Phase = PHASE_DECEIVERS;
                }
                // no aggro, do channeling
                else if (!pTarget && !b_DevicerChanneling)
                {
                    for(uint8 i = 0; i < 3; ++i)
                        if (Unit* pDeciver = Unit::GetUnit((*m_creature), ui_DeciverGUID[i]))
                            pDeciver->CastSpell(pDeciver, SPELL_SHADOW_CHANNELING, false);
                    b_DevicerChanneling = true;
                }
            }

            if (ui_Phase == PHASE_DECEIVERS)
            {
                bool b_DeciverDead;
                b_DeciverDead = false;

                FindHandDeceivers();
                for(std::list<uint64>::iterator itr = ui_HandDeceivers.begin(); itr != ui_HandDeceivers.end(); ++itr)
                {
                    if (Creature* pCreature = ((Creature*)Unit::GetUnit(*m_creature, *itr)))
                    {
                        // if any is alive
                        if (pCreature->isAlive())
                        {
                        	b_DeciverDead = true;

                        	// no target
                            if (!pCreature->getVictim())
                            {
                                ui_Phase = PHASE_NOTSTART;
                                Reset();
                                return;
                            }
                        }
                    }
                }

                // if all dead, KILJAEDEN
                if (!b_DeciverDead)
                {
                    m_creature->RemoveAurasDueToSpell(SPELL_ANVEENA_ENERGY_DRAIN);
                    ui_Phase = PHASE_NORMAL;
                    ui_AnveenaSpeech_Timer = 25000; // first speech after 25 secs

                    if(Creature* KJ = DoSpawnCreature(NPC_KILJAEDEN, 0, 0,0, 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 45000))
                    {
                        KJ->CastSpell(KJ, SPELL_REBIRTH, false);
                        //KJ->AddThreat(m_creature->getVictim(), 1.0f);
                        //((boss_kiljaedenAI*)KJ->AI())->AttackStart(m_creature->getVictim());
                        DoScriptText(SAY_KJ_EMERGE, KJ);
                        ((boss_kiljaedenAI*)KJ->AI())->ui_Phase = PHASE_NORMAL;
                        if (m_pInstance)
                            m_pInstance->SetData(TYPE_KILJAEDEN, IN_PROGRESS);
                    }
                }
            }

            if (ui_Phase >= PHASE_NORMAL)
            {
                if(Creature* pKiljaeden = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(DATA_KILJAEDEN))))
                {
                    if (pKiljaeden->isAlive())
                    	if (m_pInstance && m_pInstance->GetData(TYPE_KILJAEDEN) == NOT_STARTED)
                    	{
                            ui_Phase = PHASE_NOTSTART;
                            Reset();
                            return;
                        }
                }
            }

            // Anveena and Kil'jaeden spawn check
            if (m_pInstance)
            {
                Creature* pKiljaeden = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(DATA_KILJAEDEN)));
                Creature* pAnveena = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(DATA_ANVEENA)));
                Creature* pKalecgosAnveena = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(DATA_KALECGOS)));

                if (pKiljaeden && pKiljaeden->isAlive() && m_pInstance->GetData(TYPE_KILJAEDEN) == NOT_STARTED)
                {
                    pKiljaeden->ForcedDespawn();
/*
                    std::list<Creature*> BlueFlightDragonList;
                    GetCreatureListWithEntryInGrid(BlueFlightDragonList, m_creature, NPC_POWER_OF_THE_BLUE_DRAGONFLIGHT, 50.0f);
                    if (!BlueFlightDragonList.empty())
                        for(std::list<Creature*>::iterator itr = BlueFlightDragonList.begin(); itr != BlueFlightDragonList.end(); ++itr)
                            m_creature->DealDamage((*itr), (*itr)->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);*/
                }
                if (pKiljaeden && pKiljaeden->isAlive() && m_pInstance->GetData(TYPE_KILJAEDEN) == IN_PROGRESS)
                {
                    if((pKiljaeden->GetHealth()*100 / pKiljaeden->GetMaxHealth()) < 24 && ui_Phase < PHASE_SACRIFICE)
                    {
                        ui_Phase = PHASE_SACRIFICE;
                        ui_AnveenaSpeechCounter = 0;
                        ui_AnveenaSpeech_Timer = 5000;
                    }
                    else if((pKiljaeden->GetHealth()*100 / pKiljaeden->GetMaxHealth()) < 54 && ui_Phase < PHASE_ARMAGEDDON)
                    {
                        ui_Phase = PHASE_ARMAGEDDON;
                        ui_AnveenaSpeechCounter = 0;
                        ui_AnveenaSpeech_Timer = 5000;
                    }
                    else if((pKiljaeden->GetHealth()*100 / pKiljaeden->GetMaxHealth()) < 84 && ui_Phase < PHASE_DARKNESS)
                    {
                        ui_Phase = PHASE_DARKNESS;
                        ui_AnveenaSpeechCounter = 0;
                        ui_AnveenaSpeech_Timer = 5000;
                    }
                }
                if (m_pInstance->GetData(TYPE_KILJAEDEN) == DONE)
                {
                    m_creature->ForcedDespawn();
                    return;
                }

                // Phase 2 speech
                if (ui_Phase == PHASE_NORMAL && ui_AnveenaSpeech_Timer)
                {
                    if(ui_AnveenaSpeech_Timer < diff)
                    {
                        if (pKalecgosAnveena && pKalecgosAnveena->isAlive())
                            DoScriptText(SAY_KALECGOS_INTRO, pKalecgosAnveena);
                        ui_AnveenaSpeech_Timer = 0;
                    }else ui_AnveenaSpeech_Timer -= diff;
                }
                // Phase 3 speech
                else if (ui_Phase == PHASE_DARKNESS && ui_AnveenaSpeech_Timer)
                {
                    if(ui_AnveenaSpeech_Timer < diff)
                    {
                        switch(ui_AnveenaSpeechCounter)
                        {
                            case 0:
                                    if (pKalecgosAnveena)
                                        DoScriptText(SAY_KALECGOS_AWAKEN, pKalecgosAnveena);
                                        ui_AnveenaSpeech_Timer = 5000;
                                    break;
                            case 1:
                                    if (pAnveena)
                                        DoScriptText(SAY_ANVEENA_IMPRISONED, pAnveena);
                                        ui_AnveenaSpeech_Timer = 5000;
                                    break;
                            case 2:
                                    if (pKiljaeden)
                                        DoScriptText(SAY_KJ_DENINE, pKiljaeden);
                                        ui_AnveenaSpeech_Timer = 0;
                                    break;
                        }
                        ui_AnveenaSpeechCounter++;
                    }else ui_AnveenaSpeech_Timer -= diff;
                }
                // Phase 4 speech
                else if (ui_Phase == PHASE_ARMAGEDDON && ui_AnveenaSpeech_Timer)
                {
                    if(ui_AnveenaSpeech_Timer < diff)
                    {
                        switch(ui_AnveenaSpeechCounter)
                        {
                            case 0:
                                    if (pKalecgosAnveena)
                                        DoScriptText(SAY_KALECGOS_LETGO, pKalecgosAnveena);
                                        ui_AnveenaSpeech_Timer = 7000;
                                    break;
                            case 1:
                                    if (pAnveena)
                                        DoScriptText(SAY_ANVEENA_LOST, pAnveena);
                                        ui_AnveenaSpeech_Timer = 5000;
                                    break;
                            case 2:
                                    if (pKiljaeden)
                                        DoScriptText(SAY_KJ_CANNOT_WIN, pKiljaeden);
                                        ui_AnveenaSpeech_Timer = 0;
                                    break;
                        }
                        ui_AnveenaSpeechCounter++;
                    }else ui_AnveenaSpeech_Timer -= diff;
                }
                // Phase 5 speech
                else if (ui_Phase == PHASE_SACRIFICE && ui_AnveenaSpeech_Timer)
                {
                    if(ui_AnveenaSpeech_Timer < diff)
                    {
                        switch(ui_AnveenaSpeechCounter)
                        {
                            case 0:
                                if (pKalecgosAnveena)
                                    DoScriptText(SAY_KALECGOS_FOCUS, pKalecgosAnveena);
                                ui_AnveenaSpeech_Timer = 10000;
                                break;
                            case 1:
                                if (pAnveena)
                                    DoScriptText(SAY_ANVEENA_KALEC, pAnveena);
                                ui_AnveenaSpeech_Timer = 3000;
                                break;
                            case 2:
                                if (pKalecgosAnveena)
                                    DoScriptText(SAY_KALECGOS_FATE, pKalecgosAnveena);
                                ui_AnveenaSpeech_Timer = 7000;
                                break;
                            case 3:
                                if (pAnveena)
                                    DoScriptText(SAY_ANVEENA_GOODBYE, pAnveena);
                                ui_AnveenaSpeech_Timer = 6000;
                                break;
                            case 4:
                                if (pKiljaeden && pKiljaeden->isAlive() && pAnveena && pAnveena->isAlive())
                                {
                                    pAnveena->CastSpell(pKiljaeden, SPELL_SACRIFICE_OF_ANVEENA, true);

                                    pAnveena->ForcedDespawn();
                                    DoScriptText(SAY_KJ_LOST_POWER, pKiljaeden);
                                }
                                ui_AnveenaSpeech_Timer = 13000;
                                break;
                            case 5:
                                if (pKalecgosAnveena)
                                    DoScriptText(SAY_KALECGOS_GOODBYE, pKalecgosAnveena);
                                ui_AnveenaSpeech_Timer = 14000;
                                break;
                            case 6:
                                if (pKalecgosAnveena)
                                    DoScriptText(SAY_KALECGOS_ENCOURAGE, pKalecgosAnveena);
                                ui_AnveenaSpeech_Timer = 0;
                                break;
                        }
                        ui_AnveenaSpeechCounter++;
                    }else ui_AnveenaSpeech_Timer -= diff;
                }

                // respawn check
                if (ui_Phase != PHASE_SACRIFICE)
                {
                    if (pAnveena && !pAnveena->isAlive())
                        pAnveena->Respawn();

                    if (pAnveena && pAnveena->isAlive() && b_AnveenaSpellCheck == false)
                    {
                        DoCast(m_creature, SPELL_ANVEENA_ENERGY_DRAIN);
                        pAnveena->SetMonsterMoveFlags(MONSTER_MOVE_LEVITATING);
                        pAnveena->CastSpell(pAnveena, SPELL_ANVEENA_PRISON, true);
                        pAnveena->GetMap()->CreatureRelocation(m_creature, 1698.45, 628.03, 70.1989, 0.0f);
                        pAnveena->SendMonsterMove(1698.45, 628.03, 70.1989, 0, MONSTER_MOVE_NONE, 0);
                        b_AnveenaSpellCheck = true;
                    }
                }
                // reset Kalecgo
                if (ui_Phase == PHASE_NOTSTART)
                {
                	if (pKalecgosAnveena && pKalecgosAnveena->isAlive() && !b_KalecgosResetCheck)
                    {
                	    ((mob_kalecgosAI*)pKalecgosAnveena->AI())->Reset();
                        b_KalecgosResetCheck = true;
                    }
                }
            }
        }else ui_Event_Timer -=diff;
    }

    void FindHandDeceivers()
    {
        std::list<Creature*> DeceiverList;
        GetCreatureListWithEntryInGrid(DeceiverList,m_creature,NPC_HAND_OF_THE_DECEIVER,50.0f);

        if (!DeceiverList.empty())
        {
            ui_HandDeceivers.clear();
            for(std::list<Creature*>::iterator itr = DeceiverList.begin(); itr != DeceiverList.end(); ++itr)
                ui_HandDeceivers.push_back((*itr)->GetGUID());
        }
    }
};

/*######
## Mob Hand of the Deceiver
######*/
struct MANGOS_DLL_DECL mob_hand_of_the_deceiverAI : public ScriptedAI
{
    mob_hand_of_the_deceiverAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 ui_ShadowBoltVolley_Timer;
    uint32 ui_FelfirePortal_Timer;

    void Reset()
    {
        // TODO: Timers!
        ui_ShadowBoltVolley_Timer = 8000 + rand()%6000; // So they don't all cast it in the same moment.
        ui_FelfirePortal_Timer    = 20000;

        m_creature->CastSpell(m_creature, SPELL_SHADOW_CHANNELING, false);
    }

    void Aggro(Unit* who)
    {
        m_creature->SetInCombatWithZone();
    }
/*
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
            m_creature->InterruptNonMeleeSpells(true);
            DoStartMovement(who);
        }
    }
*/
    void JustDied(Unit* killer) { }

    void UpdateAI(const uint32 diff)
    {
        if(!m_creature->getVictim() || !m_creature->SelectHostilTarget())
            return;

        // Gain Shadow Infusion at 20% health
        if(((m_creature->GetHealth()*100 / m_creature->GetMaxHealth()) < 20) && !m_creature->HasAura(SPELL_SHADOW_INFUSION, 0))
            DoCast(m_creature, SPELL_SHADOW_INFUSION, true);

        // Shadow Bolt Volley - Shoots Shadow Bolts at all enemies within 30 yards, for ~2k Shadow damage.
        if(ui_ShadowBoltVolley_Timer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_SHADOW_BOLT_VOLLEY);
            ui_ShadowBoltVolley_Timer = 12000;
        }else ui_ShadowBoltVolley_Timer -= diff;

        // Felfire Portal - Creatres a portal, that spawns Volatile Felfire Fiends, which do suicide bombing.
        if(ui_FelfirePortal_Timer < diff)
        {
            if(Creature* pPortal = DoSpawnCreature(NPC_FELFIRE_PORTAL, 0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN, 20000))
            {
                // why don't use m_creature->getVictim() ???
                std::list<HostilReference*>::iterator itr;
                for(itr = m_creature->getThreatManager().getThreatList().begin(); itr != m_creature->getThreatManager().getThreatList().end(); ++itr)
                {
                    if(Unit* pUnit = Unit::GetUnit(*m_creature, (*itr)->getUnitGuid()))
                        pPortal->AddThreat(pUnit, 1.0f);
                }
            }
            ui_FelfirePortal_Timer = 20000;
        }else ui_FelfirePortal_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

/*######
## Mob Felfire Portal
######*/
struct MANGOS_DLL_DECL mob_felfire_portalAI : public Scripted_NoMovementAI
{
    mob_felfire_portalAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        Reset();
    }

    uint32 ui_SpawnFiend_Timer;

    void Reset()
    {
        ui_SpawnFiend_Timer = 5000;
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    void UpdateAI(const uint32 diff)
    {
        if(!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

        if(ui_SpawnFiend_Timer < diff)
        {
            if(Creature* pFiend = DoSpawnCreature(NPC_VOLATILE_FELFIRE_FIEND, 0, 0, 0, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 20000))
                pFiend->AddThreat(SelectUnit(SELECT_TARGET_RANDOM,0), 100000.0f);
            ui_SpawnFiend_Timer = 4000 + rand()%4000;
        }else ui_SpawnFiend_Timer -= diff;
    }
};

/*######
## Mob Volatile Felfire Fiend
######*/
struct MANGOS_DLL_DECL mob_volatile_felfire_fiendAI : public ScriptedAI
{
    mob_volatile_felfire_fiendAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 ui_Explode_Timer;

    void Reset()
    {
        ui_Explode_Timer = 2000;
    }

    void Aggro(Unit* who)
    {
        m_creature->AddThreat(who, 10000000.0f);
    }

    void DamageTaken(Unit *done_by, uint32 &damage)
    {
        if(damage > m_creature->GetHealth())
            DoCast(m_creature, SPELL_FELFIRE_FISSION, true);
    }

    void UpdateAI(const uint32 diff)
    {
        if(!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

        if(ui_Explode_Timer) // Just so it doesn't explode as soon as it spawn
        {
            if(ui_Explode_Timer < diff)
                ui_Explode_Timer = 0;
            else
                ui_Explode_Timer -= diff;
        }
        else if(m_creature->IsWithinDistInMap(m_creature->getVictim(), 3)) // Explode if it's close enough to it's target
        {
            DoCast(m_creature->getVictim(), SPELL_FELFIRE_FISSION);
            m_creature->ForcedDespawn();
        }
    }
};

CreatureAI* GetAI_boss_kiljaeden(Creature *_Creature)
{
    return new boss_kiljaedenAI (_Creature);
}

CreatureAI* GetAI_mob_kiljaeden_controller(Creature *_Creature)
{
    return new mob_kiljaeden_controllerAI (_Creature);
}

CreatureAI* GetAI_mob_hand_of_the_deceiver(Creature *_Creature)
{
    return new mob_hand_of_the_deceiverAI (_Creature);
}

CreatureAI* GetAI_mob_felfire_portal(Creature *_Creature)
{
    return new mob_felfire_portalAI (_Creature);
}

CreatureAI* GetAI_mob_volatile_felfire_fiend(Creature *_Creature)
{
    return new mob_volatile_felfire_fiendAI (_Creature);
}

CreatureAI* GetAI_mob_shield_orb(Creature *_Creature)
{
    return new mob_shield_orbAI (_Creature);
}

CreatureAI* GetAI_mob_kalecgos(Creature *_Creature)
{
    return new mob_kalecgosAI (_Creature);
}

bool GOHello_orb_of_the_blue_flight(Player* pPlayer, GameObject* pGo)
{
    ScriptedInstance *m_pInstance = (pPlayer->GetInstanceData()) ? ((ScriptedInstance*)pPlayer->GetInstanceData()) : NULL;

    if (m_pInstance && m_pInstance->GetData(TYPE_KILJAEDEN) == IN_PROGRESS && !pGo->HasFlag(GAMEOBJECT_FLAGS, GO_FLAG_IN_USE))
    {
        pPlayer->CastSpell(pPlayer, SPELL_SUMMON_BLUE_DRAKE, true);
        pPlayer->CastSpell(pPlayer, SPELL_VENGEANCE_OF_THE_BLUE_FLIGHT, true);
    }

    return false;
}

void AddSC_boss_Kiljaeden()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_kiljaeden";
    newscript->GetAI = &GetAI_boss_kiljaeden;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_kiljaeden_controller";
    newscript->GetAI = &GetAI_mob_kiljaeden_controller;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_kalecgos";
    newscript->GetAI = &GetAI_mob_kalecgos;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_hand_of_the_deceiver";
    newscript->GetAI = &GetAI_mob_hand_of_the_deceiver;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_felfire_portal";
    newscript->GetAI = &GetAI_mob_felfire_portal;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_volatile_felfire_fiend";
    newscript->GetAI = &GetAI_mob_volatile_felfire_fiend;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_shield_orb";
    newscript->GetAI = &GetAI_mob_shield_orb;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_orb_of_the_blue_flight";
    newscript->pGOHello = &GOHello_orb_of_the_blue_flight;
    newscript->RegisterSelf();
};
