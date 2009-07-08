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
SDName: Boss svala
SDAuthor: ckegg
SD%Complete: 95
SDComment: Check timer
SDCategory: Utgarde Pinnacle
EndScriptData */

#include "precompiled.h"
#include "def_pinnacle.h"

enum
{
    SAY_DIALOG_WITH_ARTHAS_1               = -1575015,
    SAY_DIALOG_WITH_ARTHAS_2               = -1575016,
    SAY_DIALOG_WITH_ARTHAS_3               = -1575017,
    SAY_AGGRO                              = -1575018,
    SAY_SLAY_1                             = -1575019,
    SAY_SLAY_2                             = -1575020,
    SAY_SLAY_3                             = -1575021,
    SAY_DEATH                              = -1575022,
    SAY_SACRIFICE_PLAYER_1                 = -1575023,
    SAY_SACRIFICE_PLAYER_2                 = -1575024,
    SAY_SACRIFICE_PLAYER_3                 = -1575025,
    SAY_SACRIFICE_PLAYER_4                 = -1575026,
    SAY_SACRIFICE_PLAYER_5                 = -1575027,

    SPELL_CALL_FLAMES                      = 48258,
    SPELL_RITUAL_OF_THE_SWORD              = 48276, //Effect #1 Teleport,  Effect #2 Dummy
    SPELL_SINSTER_STRIKE_N                 = 15667,
    SPELL_SINSTER_STRIKE_H                 = 59409,
    SPELL_SVALA_TRANSFORMING1              = 54205,
    SPELL_SVALA_TRANSFORMING2              = 54140,

    NPC_RITUAL_CHANNELER                   = 27281,

    SPELL_PARALYZE                         = 48278,
    SPELL_SHADOWS_IN_THE_DARK              = 59407
};

struct Locations
{
    float x, y, z;
    uint32 id;
};

static Locations RitualChannelerLoc[]=
{
    {296.42, -355.01, 90.94},
    {302.36, -352.01, 90.54},
    {291.39, -350.89, 90.54}
};

/*######
## Boss Svala
######*/
struct MANGOS_DLL_DECL boss_svalaAI : public ScriptedAI
{
    boss_svalaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
    	m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
    	Reset();

        SpellEntry* TempSpell = (SpellEntry*)GetSpellStore()->LookupEntry(SPELL_PARALYZE);
        if (TempSpell && TempSpell->EffectImplicitTargetB[0] != 0)
        {
               TempSpell->EffectImplicitTargetA[0] = 77;
               TempSpell->EffectImplicitTargetA[1] = 0;
               TempSpell->EffectImplicitTargetB[0] = 0;
               TempSpell->EffectImplicitTargetB[1] = 0;
        }
    }

    ScriptedInstance *m_pInstance;
    bool m_bIsIntro;

    uint8 m_uiIntro_Phase;

    uint32 m_uiSpeech_Timer;

    void Reset()
    {
    	m_bIsIntro = false;
    	m_uiIntro_Phase = 0;
    	m_uiSpeech_Timer = 1000;

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }
    void Aggro(Unit* pWho) { return; }
    void AttackStart(Unit* pWho) { return; }
    void MoveInLineOfSight(Unit* pWho)
    {
    	if (!pWho)
    	    return;

        if (pWho->isTargetableForAttack() && pWho->isInAccessablePlaceFor(m_creature) && m_creature->IsHostileTo(pWho) &&
        	!m_bIsIntro && pWho->GetTypeId() == TYPEID_PLAYER && m_creature->IsWithinDistInMap(pWho, 40))
        {
        	m_bIsIntro = true;

            // Display Arthas (29280)
            if (Creature* pArthas = m_creature->SummonCreature(24266, 295.81, -366.16, 92.57, 1.58, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 16000))
            {
                pArthas->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                pArthas->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                pArthas->SetFloatValue(OBJECT_FIELD_SCALE_X, 5);
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_creature->getVictim())
            return;

        if (m_bIsIntro)
        {
            if(m_uiSpeech_Timer < uiDiff)
            {
                switch(m_uiIntro_Phase)
                {
                    case 0:
                        DoScriptText(SAY_DIALOG_WITH_ARTHAS_1, m_creature);
                        m_uiIntro_Phase++;
                        m_uiSpeech_Timer = 7000;
                        break;
                    case 1:
                        DoScriptText(SAY_DIALOG_WITH_ARTHAS_2, m_creature);
                        m_uiIntro_Phase++;
                        m_uiSpeech_Timer = 7000;
                        break;
                    case 2:
                        DoScriptText(SAY_DIALOG_WITH_ARTHAS_3, m_creature);
                        DoCast(m_creature, SPELL_SVALA_TRANSFORMING2);
                        m_uiIntro_Phase++;
                        m_uiSpeech_Timer = 2800;
                        break;
                    case 3:
                        DoCast(m_creature, SPELL_SVALA_TRANSFORMING1);
                        m_uiIntro_Phase++;
                        m_uiSpeech_Timer = 200;
                        break;
                    case 4:
                        if (Creature* pSvalaSorrowgrave = m_creature->SummonCreature(26668, 296.632, -346.075, 90.6307, 1.58, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000))
                        {
                            if (m_pInstance)
                                m_pInstance->SetData(DATA_SVALA_SORROWGRAVE, m_creature->GetGUID());

                            m_uiIntro_Phase++;
                            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            m_creature->SetDisplayId(11686);
                        }
                        else
                            Reset();
                        break;
                    default:
                        m_uiSpeech_Timer = 100000;
                }
            }else m_uiSpeech_Timer -= uiDiff;
        }
    }
    void JustDied(Unit* pKiller) { }
    void KilledUnit(Unit* pVictim) { }
};

/*######
## Mob Ritual Channeler
######*/
struct MANGOS_DLL_DECL mob_ritual_channelerAI : public ScriptedAI
{
    mob_ritual_channelerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
    	m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
    	Reset();
    }

    ScriptedInstance *m_pInstance;

    uint64 m_uiPlayer;

    void Reset()
    {
    	DoCast(m_creature, SPELL_SHADOWS_IN_THE_DARK);
    }

    void AttackStart(Unit* pWho)
    {
        if (m_uiPlayer)
            if (Unit* pPlayer = (Unit::GetUnit((*m_creature), m_uiPlayer)))
                if (!(pPlayer->HasAura(SPELL_PARALYZE,0)))
        	        m_creature->CastSpell(pPlayer,SPELL_PARALYZE,false);
    	return;
    }

    void UpdateAI(const uint32 uiDiff) {}
    void JustDied(Unit* pKiller) {}
};

/*######
## Boss Svala Sorrowgrave
######*/
struct MANGOS_DLL_DECL boss_svala_sorrowgraveAI : public ScriptedAI
{
    boss_svala_sorrowgraveAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
    	m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
    	m_bIsHeroic = pCreature->GetMap()->IsHeroic();
    	Reset();
    }

    ScriptedInstance *m_pInstance;
    bool m_bIsHeroic;
    bool m_bIsOnSky;

    uint32 m_uiCallFlames_Timer;
    uint32 m_uiRitualSword_Timer;
    uint32 m_uiSinisterStrike_Timer;
    uint32 m_uiRitualChanneler_Timer;
    uint32 m_uiSacrifice_Timer;

    uint64 m_uiRitualChannelerGUID[3];
    uint64 m_uiSacrificerGUID;

    void Reset()
    {
        m_bIsOnSky = false;

        m_uiCallFlames_Timer = 10000;
        m_uiRitualSword_Timer = 20000;
        m_uiSinisterStrike_Timer = 7000;
        m_uiRitualChanneler_Timer = 1000;
        m_uiSacrifice_Timer = 20000;

        m_uiSacrificerGUID = 0;

        for(uint8 i = 0; i<3; i++)
            m_uiRitualChannelerGUID[i] = 0;

        if (m_pInstance)
            m_pInstance->SetData(DATA_SVALA_SORROWGRAVE_EVENT, NOT_STARTED);

        m_creature->GetMap()->CreatureRelocation(m_creature, 296.632, -346.075, 90.6307, 0.0f);
        m_creature->SendMonsterMove(296.632, -346.075, 90.6307, 0, MONSTER_MOVE_NONE, 0);
        m_creature->RemoveMonsterMoveFlag(MONSTER_MOVE_LEVITATING);
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (!pWho || m_creature->getVictim())
            return;

        if (pWho->isTargetableForAttack() && pWho->isInAccessablePlaceFor(m_creature) && m_creature->IsHostileTo(pWho))
            AttackStart(pWho);

        if(m_pInstance)
            m_pInstance->SetData(DATA_SVALA_SORROWGRAVE_EVENT, IN_PROGRESS);
    }

    void AttackStart(Unit* pWho)
    {
        if (m_bIsOnSky)
            return;

        if (!pWho || pWho == m_creature)
            return;

        if (m_creature->Attack(pWho, true))
        {
            m_creature->SetInCombatWithZone();
            DoStartMovement(pWho);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

        if (!m_bIsOnSky)
        {
            // Sinister Strike
            if(m_uiSinisterStrike_Timer < uiDiff)
            {
                DoCast(m_creature, m_bIsHeroic ? SPELL_SINSTER_STRIKE_H : SPELL_SINSTER_STRIKE_N);
                m_uiSinisterStrike_Timer = 5000 + rand()%4000;
            }else m_uiSinisterStrike_Timer -= uiDiff;

            // Call Flames
            if(m_uiCallFlames_Timer < uiDiff)
            {
                DoCast(SelectUnit(SELECT_TARGET_RANDOM, 0), SPELL_CALL_FLAMES);
                m_uiCallFlames_Timer = 8000 + rand()%4000;
            }else m_uiCallFlames_Timer -= uiDiff;

            // Ritual Sword
            if(m_uiRitualSword_Timer < uiDiff)
            {
            	if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0))
            	{
                    m_creature->AddMonsterMoveFlag(MONSTER_MOVE_LEVITATING);

                    m_creature->GetMap()->CreatureRelocation(m_creature, 296.632, -346.075, 120.85, 0.0f);
                    //m_creature->SendMonsterMove(296.632, -346.075, 120.85, 0, MONSTER_MOVE_NONE, 0);
                    DoCast(target, SPELL_RITUAL_OF_THE_SWORD);

                    switch(rand()%5)
                    {
                        case 0: DoScriptText(SAY_SACRIFICE_PLAYER_1, m_creature);break;
                        case 1: DoScriptText(SAY_SACRIFICE_PLAYER_2, m_creature);break;
                        case 2: DoScriptText(SAY_SACRIFICE_PLAYER_3, m_creature);break;
                        case 3: DoScriptText(SAY_SACRIFICE_PLAYER_4, m_creature);break;
                        case 4: DoScriptText(SAY_SACRIFICE_PLAYER_5, m_creature);break;
                    }

                    // worksaround below
                    DoTeleportPlayer(target, 296.632, -346.075, 90.63, 4.6);
                    m_uiSacrificerGUID = target->GetGUID();
                    m_uiSacrifice_Timer = 25000;
                    m_bIsOnSky = true;

                    for(uint8 i = 0; i<3; i++)
                    {
                        if (Creature* pRitualChanneler = m_creature->SummonCreature(NPC_RITUAL_CHANNELER, RitualChannelerLoc[i].x, RitualChannelerLoc[i].y, RitualChannelerLoc[i].z, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 360000))
                        {
                            m_uiRitualChannelerGUID[i] = pRitualChanneler->GetGUID();
                            ((mob_ritual_channelerAI*)pRitualChanneler->AI())->m_uiPlayer = target->GetGUID();
                        }
                    }
                }
                m_uiRitualSword_Timer = 18000 + rand()%4000;
            }else m_uiRitualSword_Timer -= uiDiff;

            DoMeleeAttackIfReady();
        }
        else
        {
            // check if Ritual Channeler dies
            if(m_uiRitualChanneler_Timer < uiDiff && m_bIsOnSky)
            {
            	bool RitualChannelerAlive;
                for(uint8 i = 0; i<3; i++)
                {
                    if (Creature* pRitualChanneler = ((Creature*)Unit::GetUnit((*m_creature), m_uiRitualChannelerGUID[i])))
                    	if (pRitualChanneler->isAlive())
                    	    RitualChannelerAlive = true;
                }

                if (!RitualChannelerAlive)
                {
                	m_creature->InterruptNonMeleeSpells(false);
                    m_creature->RemoveMonsterMoveFlag(MONSTER_MOVE_LEVITATING);
                    m_creature->GetMotionMaster()->MoveChase(SelectUnit(SELECT_TARGET_RANDOM, 0));
                	m_bIsOnSky = false;
                	m_uiSacrifice_Timer = 25000;
                	m_uiSacrificerGUID = 0;
                }

                m_uiRitualChanneler_Timer = 1000;
            }else m_uiRitualChanneler_Timer -= uiDiff;

            if(m_uiSacrifice_Timer < uiDiff && m_bIsOnSky)
            {
               	m_creature->InterruptNonMeleeSpells(false);
                m_creature->GetMotionMaster()->MoveChase(SelectUnit(SELECT_TARGET_RANDOM, 0));

                for(uint8 i = 0; i<3; i++)
                {
                    if (Creature* pRitualChanneler = ((Creature*)Unit::GetUnit((*m_creature), m_uiRitualChannelerGUID[i])))
                    {
                    	if (pRitualChanneler->isAlive())
                    	    pRitualChanneler->ForcedDespawn();
                    }
                }

                if (m_uiSacrificerGUID)
                    if (Unit* pPlayer = (Unit::GetUnit((*m_creature), m_uiSacrificerGUID)))
                        if (pPlayer->isAlive())
                            pPlayer->DealDamage(pPlayer, pPlayer->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);

                m_bIsOnSky = false;
                m_uiSacrificerGUID = 0;
                m_uiSacrifice_Timer = 25000;
            }else m_uiSacrifice_Timer -= uiDiff;
        }
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);
        if(m_pInstance)
        {
            m_pInstance->SetData(DATA_SVALA_SORROWGRAVE_EVENT, DONE);

            if (Creature* pSvala = (Creature*)Unit::GetUnit(*m_creature, m_pInstance->GetData64(DATA_SVALA_SORROWGRAVE)))
            {
                if (pSvala->isAlive())
                    pKiller->DealDamage(pSvala, pSvala->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            }
        }
    }

    void KilledUnit(Unit* pVictim)
    {
        if(pVictim == m_creature)
            return;
        switch(rand()%3)
        {
            case 0: DoScriptText(SAY_SLAY_1, m_creature);break;
            case 1: DoScriptText(SAY_SLAY_2, m_creature);break;
            case 2: DoScriptText(SAY_SLAY_3, m_creature);break;
        }
    }
};

CreatureAI* GetAI_boss_svala(Creature* pCreature)
{
    return new boss_svalaAI (pCreature);
}

CreatureAI* GetAI_boss_svala_sorrowgrave(Creature* pCreature)
{
    return new boss_svala_sorrowgraveAI (pCreature);
}

CreatureAI* GetAI_mob_ritual_channeler(Creature* pCreature)
{
    return new mob_ritual_channelerAI (pCreature);
}

void AddSC_boss_svala()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name="boss_svala";
    newscript->GetAI = GetAI_boss_svala;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="boss_svala_sorrowgrave";
    newscript->GetAI = GetAI_boss_svala_sorrowgrave;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="mob_ritual_channeler";
    newscript->GetAI = GetAI_mob_ritual_channeler;
    newscript->RegisterSelf();
}
