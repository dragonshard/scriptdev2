/* Copyright ?2006,2007 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: Boss_Felmyst
SD%Complete: 0
SDComment:
EndScriptData */

#include "precompiled.h"
#include "def_sunwell_plateau.h"
#include "Player.h"

enum
{
    SAY_FELMYST_BIRTH               = -1580036,
    SAY_FELMYST_KILL1               = -1580037,
    SAY_FELMYST_KILL2               = -1580038,
    SAY_FELMYST_BREATH              = -1580039,
    SAY_FELMYST_TAKEOFF             = -1580040,
    SAY_FELMYST_BERSERK             = -1580041,
    SAY_FELMYST_DEATH               = -1580042,

    SAY_FELMYST_KALECGOS            = -1580043,

    // AURA
    SPELL_SUNWELL_RADIANCE          = 45769,
    SPELL_NOXIOUS_FUMES             = 47002,
    // LAND PHASE SPELL
    SPELL_CLEAVE                    = 19983,
    SPELL_CORROSION                 = 45866,
    SPELL_GAS_NOVA                  = 45855,
    SPELL_ENCAPSULATE_CHANNEL       = 45661,
    SPELL_ENCAPSULATE_EFFECT        = 45665,
    SPELL_ENCAPSULATE_AOE           = 45662,
    // FLIGHT PHASE SPELL
    SPELL_VAPOR_SELECT              = 45391,   // fel to player, force cast 45392, 50000y selete target
    SPELL_VAPOR_SUMMON              = 45392,   // player summon vapor, radius around caster, 5y,
    SPELL_VAPOR_FORCE               = 45388,   // vapor to fel, force cast 45389
    SPELL_VAPOR_CHANNEL             = 45389,   // fel to vapor, green beam channel
    SPELL_VAPOR_TRIGGER             = 45411,   // linked to 45389, vapor to self, trigger 45410 and 46931
    SPELL_VAPOR_DAMAGE              = 46931,   // vapor damage, 4000
    SPELL_TRAIL_SUMMON              = 45410,   // vapor summon trail
    SPELL_TRAIL_TRIGGER             = 45399,   // trail to self, trigger 45402
    SPELL_TRAIL_DAMAGE              = 45402,   // trail damage, 2000 + 2000 dot
    SPELL_DEAD_SUMMON               = 45400,   // summon blazing dead, 5min
    SPELL_DEAD_PASSIVE              = 45415,
    SPELL_FOG_BREATH                = 45495,   // fel to self, speed burst
    SPELL_FOG_TRIGGER               = 45582,   // fog to self, trigger 45782
    SPELL_FOG_FORCE                 = 45782,   // fog to player, force cast 45714
    SPELL_FOG_INFORM                = 45714,   // player let fel cast 45717, script effect
    SPELL_FOG_CHARM                 = 45717,   // fel to player
    SPELL_FOG_CHARM2                = 45726,   // link to 45717

    SPELL_TRANSFORM_TRIGGER         = 44885,   // madrigosa to self, trigger 46350
    SPELL_TRANSFORM_VISUAL          = 46350,   //46411stun?
    SPELL_TRANSFORM_FELMYST         = 45068,   // become fel
    SPELL_FELMYST_SUMMON            = 45069,
    // OTHER
    SPELL_BERSERK                   = 45078,
    SPELL_CLOUD_VISUAL              = 45212,
    SPELL_CLOUD_SUMMON              = 45884,

    NPC_FELMYST                     = 25038,
    //NPC_BRUTALLUS
    //NPC_KALECGOS
    NPC_DEAD                        = 25268,
    NPC_MADRIGOSA                   = 25160,
    NPC_FELMYST_VISUAL              = 25041,
    NPC_FLIGHT_LEFT                 = 25357,
    NPC_FLIGHT_RIGHT                = 25358,
    NPC_DEATH_CLOUD                 = 25703,
    NPC_VAPOR                       = 25265,
    NPC_VAPOR_TRAIL                 = 25267
};

enum PhaseFelmyst
{
    PHASE_NULL   = 0,
    PHASE_GROUND = 1,
    PHASE_FLIGHT = 2
};

enum EventFelmyst
{
    EVENT_NULL          =   0,
    EVENT_BERSERK       =   1,

    EVENT_CLEAVE        =   2,
    EVENT_CORROSION     =   3,
    EVENT_GAS_NOVA      =   4,
    EVENT_ENCAPSULATE   =   5,
    EVENT_FLIGHT        =   6,

    EVENT_FLIGHT_SEQUENCE   =   2,
    EVENT_SUMMON_DEAD       =   3,
    EVENT_SUMMON_FOG        =   4
};

static EventFelmyst MaxTimer[]=
{
    EVENT_NULL,
    EVENT_FLIGHT,
    EVENT_SUMMON_FOG
};

/*######
## Boss Felmyst
######*/
struct MANGOS_DLL_DECL boss_felmystAI : public ScriptedAI
{
    boss_felmystAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        Reset();

        // wait for core patch be accepted
        SpellEntry *TempSpell = (SpellEntry*)GetSpellStore()->LookupEntry(SPELL_ENCAPSULATE_EFFECT);
        if(TempSpell->SpellIconID == 2294)
            TempSpell->SpellIconID = 2295;
        TempSpell = (SpellEntry*)GetSpellStore()->LookupEntry(SPELL_VAPOR_TRIGGER);
        if((TempSpell->Attributes & SPELL_ATTR_PASSIVE) == 0)
            TempSpell->Attributes |= SPELL_ATTR_PASSIVE;
        TempSpell = (SpellEntry*)GetSpellStore()->LookupEntry(SPELL_FOG_CHARM2);
        if((TempSpell->Attributes & SPELL_ATTR_PASSIVE) == 0)
            TempSpell->Attributes |= SPELL_ATTR_PASSIVE;

    }
    ScriptedInstance *m_pInstance;

    PhaseFelmyst enum_Phase;
    EventFelmyst enum_Event;
    uint32 ui_Timer[EVENT_FLIGHT + 1];

    uint32 ui_FlightCount;
    uint32 ui_BreathCount;

    float f_BreathX, f_BreathY;

    void Reset()
    {
        enum_Phase = PHASE_NULL;
        enum_Event = EVENT_NULL;
        ui_Timer[EVENT_BERSERK] = 600000;
        ui_FlightCount = 0;

        m_creature->RemoveMonsterMoveFlag(MONSTER_MOVE_LEVITATING);
        m_creature->SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, 10);
        m_creature->SetFloatValue(UNIT_FIELD_COMBATREACH, 10);

        DespawnSummons(NPC_VAPOR_TRAIL);

        if(m_pInstance)
            m_pInstance->SetData(TYPE_FELMYST, NOT_STARTED);
    }

    void Aggro(Unit *who)
    {
        m_creature->SetInCombatWithZone();
        m_creature->CastSpell(m_creature, SPELL_SUNWELL_RADIANCE, true);
        m_creature->CastSpell(m_creature, SPELL_NOXIOUS_FUMES, true);
        EnterPhase(PHASE_GROUND);

        if(m_pInstance)
            m_pInstance->SetData(TYPE_FELMYST, IN_PROGRESS);
    }

    void AttackStart(Unit *who)
    {
        if(enum_Phase != PHASE_FLIGHT)
            ScriptedAI::AttackStart(who);
    }

    void MoveInLineOfSight(Unit *who)
    {
        if (!who)
            return;

        if (who == m_creature || who->GetTypeId() != TYPEID_PLAYER)
            return;

        if(m_pInstance)
        {
            Creature* pBrutallus = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(DATA_BRUTALLUS)));
            if (pBrutallus && !pBrutallus->isAlive() && m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            {
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                m_creature->SetVisibility(VISIBILITY_ON);
                DoScriptText(SAY_FELMYST_BIRTH,m_creature);
            }
            else if (pBrutallus && pBrutallus->isAlive() && !m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            {
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                m_creature->SetVisibility(VISIBILITY_OFF);
                return;
            }
            else if (pBrutallus && pBrutallus->isAlive() && m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                return;
        }

        if(enum_Phase != PHASE_FLIGHT)
            ScriptedAI::MoveInLineOfSight(who);
    }

    void KilledUnit(Unit* victim)
    {
        switch(rand()%2)
        {
            case 0: DoScriptText(SAY_FELMYST_KILL1,m_creature); break;
            case 1: DoScriptText(SAY_FELMYST_KILL2,m_creature); break;
        }
    }

    void JustDied(Unit* Killer)
    {
        DoScriptText(SAY_FELMYST_DEATH,m_creature);
        if(m_pInstance)
            m_pInstance->SetData(TYPE_FELMYST, DONE);
    }

    void SpellHit(Unit *caster, const SpellEntry *spell)
    {
        // workaround for linked aura
        /*if(spell->Id == SPELL_VAPOR_FORCE)
        {
            caster->CastSpell(caster, SPELL_VAPOR_TRIGGER, true);
        }*/
        // workaround for mind control
        if(spell->Id == SPELL_FOG_INFORM)
        {
            float x, y, z;
            caster->GetPosition(x, y, z);
            if(Unit* pSummon = m_creature->SummonCreature(NPC_DEAD, x, y, z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000))
            {
                pSummon->SetMaxHealth(caster->GetMaxHealth());
                pSummon->SetHealth(caster->GetMaxHealth());
                pSummon->CastSpell(pSummon, SPELL_FOG_CHARM, true);
                pSummon->CastSpell(pSummon, SPELL_FOG_CHARM2, true);
            }
            m_creature->DealDamage(caster, caster->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        }
    }

    void JustSummoned(Creature *summon)
    {
        if(summon->GetEntry() == NPC_DEAD)
        {
            summon->AI()->AttackStart(SelectUnit(SELECT_TARGET_RANDOM, 0));
            summon->CastSpell(summon, SPELL_DEAD_PASSIVE, true);
        }
    }

    void MovementInform(uint32, uint32)
    {
        ui_Timer[EVENT_FLIGHT_SEQUENCE] = 1;
    }

    void DamageTaken(Unit*, uint32 &damage)
    {
        if(enum_Phase != PHASE_GROUND && damage >= m_creature->GetHealth())
            damage = 0;
    }

    void EnterPhase(PhaseFelmyst NextPhase)
    {
        switch(NextPhase)
        {
            case PHASE_GROUND:
                ui_Timer[EVENT_CLEAVE]       = 5000 + rand()%5 * 1000;
                ui_Timer[EVENT_CORROSION]    = 10000 + rand()%10 * 1000;
                ui_Timer[EVENT_GAS_NOVA]     = 15000 + rand()%5 * 1000;
                ui_Timer[EVENT_ENCAPSULATE]  = 20000 + rand()%5 * 1000;
                ui_Timer[EVENT_FLIGHT]       = 60000;
                break;
            case PHASE_FLIGHT:
                ui_Timer[EVENT_FLIGHT_SEQUENCE]  = 1000;
                ui_Timer[EVENT_SUMMON_DEAD]      = 0;
                ui_Timer[EVENT_SUMMON_FOG]       = 0;
                ui_FlightCount = 0;
                ui_BreathCount = 0;
                break;
            default:
                break;
        }
        enum_Phase = NextPhase;
    }

    void HandleFlightSequence()
    {
        switch(ui_FlightCount)
        {
            case 0:
                m_creature->AttackStop();
                m_creature->GetMotionMaster()->Clear(false);
                m_creature->HandleEmoteCommand(EMOTE_ONESHOT_LIFTOFF);
                m_creature->SetMonsterMoveFlags(MONSTER_MOVE_LEVITATING);
                m_creature->StopMoving();
                DoScriptText(SAY_FELMYST_TAKEOFF,m_creature);
                ui_Timer[EVENT_FLIGHT_SEQUENCE] = 2000;
                break;
            case 1:
                m_creature->AddMonsterMoveFlag(MONSTER_MOVE_FLY);
                m_creature->GetMotionMaster()->MovePoint(0, m_creature->GetPositionX()+1, m_creature->GetPositionY(), m_creature->GetPositionZ()+10);
                m_creature->RemoveMonsterMoveFlag(MONSTER_MOVE_FLY);
                ui_Timer[EVENT_FLIGHT_SEQUENCE] = 0;
                break;
            case 2:
                if(Player* pTarget = SelectRandomPlayer(50))
                {
                    if(Creature* pVapor = m_creature->SummonCreature(NPC_VAPOR, pTarget->GetPositionX()-5+rand()%10, pTarget->GetPositionY()-5+rand()%10, pTarget->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 9000))
                    {
                        pVapor->AI()->AttackStart(pTarget);
                        pVapor->CastSpell(pVapor, SPELL_VAPOR_TRIGGER, true);
                        m_creature->InterruptNonMeleeSpells(false);
                        m_creature->CastSpell(pVapor, SPELL_VAPOR_CHANNEL, false); // core bug
                    }
                }
                else
                    EnterEvadeMode();
                ui_Timer[EVENT_FLIGHT_SEQUENCE] = 10000;
                break;
            case 3:
                DespawnSummons(NPC_VAPOR_TRAIL);
                //m_creature->CastSpell(m_creature, SPELL_VAPOR_SELECT); need core support
                if(Player* pTarget = SelectRandomPlayer(50))
                {
                    //pTarget->CastSpell(pTarget, SPELL_VAPOR_SUMMON, true); need core support
                    if(Creature* pVapor = m_creature->SummonCreature(NPC_VAPOR, pTarget->GetPositionX()-5+rand()%10, pTarget->GetPositionY()-5+rand()%10, pTarget->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 9000))
                    {
                        pVapor->AI()->AttackStart(pTarget);
                        pVapor->CastSpell(pVapor, SPELL_VAPOR_TRIGGER, true);
                        m_creature->InterruptNonMeleeSpells(false);
                        m_creature->CastSpell(pVapor, SPELL_VAPOR_CHANNEL, false); // core bug
                    }
                }
                else
                    EnterEvadeMode();
                ui_Timer[EVENT_FLIGHT_SEQUENCE] = 10000;
                break;
            case 4:
                DespawnSummons(NPC_VAPOR_TRAIL);
                ui_Timer[EVENT_FLIGHT_SEQUENCE] = 1;
                break;
            case 5:
                if(Player* pTarget = SelectRandomPlayer(80))
                {
                    f_BreathX = pTarget->GetPositionX();
                    f_BreathY = pTarget->GetPositionY();
                    float x, y, z;
                    pTarget->GetContactPoint(m_creature, x, y, z, 40);
                    m_creature->AddMonsterMoveFlag(MONSTER_MOVE_FLY);
                    m_creature->GetMotionMaster()->MovePoint(0, x, y, z+10);
                    m_creature->RemoveMonsterMoveFlag(MONSTER_MOVE_FLY);
                }
                else
                    EnterEvadeMode();
                ui_Timer[EVENT_FLIGHT_SEQUENCE] = 0;
                break;
            case 6:
                m_creature->SetOrientation(m_creature->GetAngle(f_BreathX, f_BreathY));
                m_creature->StopMoving();
                ui_Timer[EVENT_FLIGHT_SEQUENCE] = 10000;
                break;
            case 7:
                m_creature->CastSpell(m_creature, SPELL_FOG_BREATH, true);
                {
                    float x, y, z;
                    m_creature->GetPosition(x, y, z);
                    x = 2 * f_BreathX - x;
                    y = 2 * f_BreathY - y;
                    m_creature->AddMonsterMoveFlag(MONSTER_MOVE_FLY);
                    m_creature->GetMotionMaster()->MovePoint(0, x, y, z);
                    m_creature->RemoveMonsterMoveFlag(MONSTER_MOVE_FLY);
                }
                ui_Timer[EVENT_SUMMON_FOG] = 1;
                ui_Timer[EVENT_FLIGHT_SEQUENCE] = 0;
                break;
            case 8:
                m_creature->RemoveAurasDueToSpell(SPELL_FOG_BREATH);
                ui_BreathCount++;
                ui_Timer[EVENT_SUMMON_FOG] = 0;
                ui_Timer[EVENT_FLIGHT_SEQUENCE] = 1;
                if (ui_BreathCount < 3)
                    ui_FlightCount = 4;
                break;
            case 9:
                if(Unit* pTarget = SelectUnit(SELECT_TARGET_TOPAGGRO, 0))
                {
                    float x, y, z;
                    pTarget->GetContactPoint(m_creature, x, y, z);
                    m_creature->AddMonsterMoveFlag(MONSTER_MOVE_FLY);
                    m_creature->GetMotionMaster()->MovePoint(0, x, y, z);
                    m_creature->RemoveMonsterMoveFlag(MONSTER_MOVE_FLY);
                }
                else
                    EnterEvadeMode();
                ui_Timer[EVENT_FLIGHT_SEQUENCE] = 0;
                break;
            case 10:
                m_creature->RemoveMonsterMoveFlag(MONSTER_MOVE_LEVITATING);
                m_creature->StopMoving();
                m_creature->HandleEmoteCommand(EMOTE_ONESHOT_LAND);
                EnterPhase(PHASE_GROUND);
                m_creature->AI()->AttackStart(SelectUnit(SELECT_TARGET_TOPAGGRO, 0));
                break;
            default:
                break;
        }
        ui_FlightCount++;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostilTarget() && !m_creature->getVictim())
        {
            if(enum_Phase == PHASE_FLIGHT && !m_creature->IsInEvadeMode())
                EnterEvadeMode();
            return;
        }

        enum_Event = EVENT_NULL;
        for(uint32 i = 1; i <= MaxTimer[enum_Phase]; i++)
        {
            if(ui_Timer[i])
                if(ui_Timer[i] <= diff)
                {
                    if(!enum_Event)
                        enum_Event = (EventFelmyst)i;
                }else ui_Timer[i] -= diff;
        }

        if(m_creature->IsNonMeleeSpellCasted(false))
            return;

        if(enum_Phase == PHASE_GROUND)
        {
            switch(enum_Event)
            {
                case EVENT_BERSERK:
                    DoScriptText(SAY_FELMYST_BERSERK,m_creature);
                    m_creature->CastSpell(m_creature, SPELL_BERSERK, true);
                    ui_Timer[EVENT_BERSERK] = 0;
                    break;
                case EVENT_CLEAVE:
                    m_creature->CastSpell(m_creature->getVictim(), SPELL_CLEAVE, false);
                    ui_Timer[EVENT_CLEAVE] = 5000 + rand()%5 * 1000;
                    break;
                case EVENT_CORROSION:
                    m_creature->CastSpell(m_creature->getVictim(), SPELL_CORROSION, false);
                    ui_Timer[EVENT_CORROSION] = 20000 + rand()%10 * 1000;
                    break;
                case EVENT_GAS_NOVA:
                    m_creature->CastSpell(m_creature, SPELL_GAS_NOVA, false);
                    ui_Timer[EVENT_GAS_NOVA] = 20000 + rand()%5 * 1000;
                    break;
                case EVENT_ENCAPSULATE:
                    if(Unit* pTarget = SelectRandomPlayer(50))
                    {
                        m_creature->CastSpell(pTarget, SPELL_ENCAPSULATE_CHANNEL, false);
                        pTarget->CastSpell(pTarget, SPELL_ENCAPSULATE_EFFECT, true);// linked aura, need core patch to remove this hack
                        ui_Timer[EVENT_ENCAPSULATE] = 25000 + rand()%5 * 1000;
                    }
                    break;
                case EVENT_FLIGHT:
                    EnterPhase(PHASE_FLIGHT);
                    break;
                default:
                    DoMeleeAttackIfReady();
                    break;
            }
        }

        if(enum_Phase == PHASE_FLIGHT)
        {
            switch(enum_Event)
            {
                case EVENT_BERSERK:
                    DoScriptText(SAY_FELMYST_BERSERK,m_creature);
                    m_creature->CastSpell(m_creature, SPELL_BERSERK, true);
                    ui_Timer[EVENT_BERSERK] = 0;
                    break;
                case EVENT_FLIGHT_SEQUENCE:
                    HandleFlightSequence();
                    break;
                case EVENT_SUMMON_FOG:
                    {
                        float x, y, z;
                        m_creature->GetPosition(x, y, z);
                        m_creature->UpdateGroundPositionZ(x, y, z);
                        if(Creature* pFog = m_creature->SummonCreature(NPC_VAPOR_TRAIL, x, y, z, 0, TEMPSUMMON_TIMED_DESPAWN, 10000))
                        {
                            pFog->RemoveAurasDueToSpell(SPELL_TRAIL_TRIGGER);
                            pFog->CastSpell(pFog, SPELL_FOG_TRIGGER, true);
                        }
                    }
                    ui_Timer[EVENT_SUMMON_FOG] = 1000;
                    break;
                default:
                    break;
            }
        }
    }

    void DespawnSummons(uint32 entry)
    {
        std::list<Creature*> templist;
        GetCreatureListWithEntryInGrid(templist,m_creature,entry,100.0f);

        if (!templist.empty())
        {
            for(std::list<Creature*>::iterator itr = templist.begin(); itr != templist.end(); ++itr)
            {
                if(entry == NPC_VAPOR_TRAIL && enum_Phase == PHASE_FLIGHT)
                {
                    if(Creature* pDeads = (*itr)->SummonCreature(NPC_DEAD, (*itr)->GetPositionX(), (*itr)->GetPositionY(), (*itr)->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000))
                        pDeads->AI()->AttackStart(SelectUnit(SELECT_TARGET_RANDOM, 0));
                }

                (*itr)->ForcedDespawn();
            }
        }
    }

    Player* SelectRandomPlayer(float range = 0.0f)
    {
        Map *map = m_creature->GetMap();
        if (map->IsDungeon())
        {
            Map::PlayerList const &PlayerList = map->GetPlayers();

            if (PlayerList.isEmpty())
                return NULL;

            for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
            {
                if((range == 0.0f || m_creature->IsWithinDistInMap(i->getSource(), range))
                    && i->getSource()->isTargetableForAttack())
                    return i->getSource();
            }
            return NULL;
        }
        else
            return NULL;
    }
};

/*######
## Mob Felmyst Vapor
######*/
struct MANGOS_DLL_DECL mob_felmyst_vaporAI : public ScriptedAI
{
    mob_felmyst_vaporAI(Creature *c) : ScriptedAI(c)
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetSpeed(MOVE_RUN, 0.8);
    }
    void Reset() {}

    void Aggro(Unit* who)
    {
        //m_creature->CastSpell(m_creature, SPELL_VAPOR_FORCE, true); //core bug
    }

    void UpdateAI(const uint32 diff)
    {
        if(!m_creature->getVictim())
            AttackStart(SelectUnit(SELECT_TARGET_RANDOM, 0));
    }
};

/*######
## Mob Felmyst Trial
######*/
struct MANGOS_DLL_DECL mob_felmyst_trailAI : public ScriptedAI
{
    mob_felmyst_trailAI(Creature *c) : ScriptedAI(c)
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->CastSpell(m_creature, SPELL_TRAIL_TRIGGER, true);
        m_creature->SetUInt64Value(UNIT_FIELD_TARGET, m_creature->GetGUID());
        m_creature->SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, 0.01); // core bug
    }
    void Reset() {}
    void AttackStart(Unit* who) {}
    void MoveInLineOfSight(Unit* who) {}
    void UpdateAI(const uint32 diff) {}
};

CreatureAI* GetAI_boss_felmyst(Creature *_Creature)
{
    return new boss_felmystAI(_Creature);
}

CreatureAI* GetAI_mob_felmyst_vapor(Creature *_Creature)
{
    return new mob_felmyst_vaporAI(_Creature);
}

CreatureAI* GetAI_mob_felmyst_trail(Creature *_Creature)
{
    return new mob_felmyst_trailAI(_Creature);
}

void AddSC_boss_felmyst()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_felmyst";
    newscript->GetAI = &GetAI_boss_felmyst;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_felmyst_vapor";
    newscript->GetAI = &GetAI_mob_felmyst_vapor;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_felmyst_trail";
    newscript->GetAI = &GetAI_mob_felmyst_trail;
    newscript->RegisterSelf();
}
