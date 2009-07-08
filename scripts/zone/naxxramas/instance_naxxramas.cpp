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
SDName: Instance_Naxxramas
SD%Complete: 10
SDComment:
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "def_naxxramas.h"

#define ENCOUNTERS              15
/* Encounters
0 Patchwerk
1 Grobbulus
2 Gluth
3 Thaddius
4 Anub'Rekhan
5 Grand Widow Faerlina
6 Maexxna
7 Instructor Razuvious
8 Gothik the Harvester
9 Four Horsemen
10 Noth the Plaguebringer
11
12
13
14
*/

enum
{
    GO_ARAC_ANUB_GATE           = 181195,                   //open when anub dead?
    GO_ARAC_ANUB_DOOR           = 181126,                   //encounter door
    GO_ARAC_FAER_WEB            = 181235,                   //encounter door
    GO_ARAC_DOOD_DOOR_2         = 181235,                   //after faerlina, to outer ring
    GO_ARAC_FAER_DOOR           = 194022,                   //encounter door
    GO_ARAC_MAEX_OUTER_DOOR     = 181209,                   //right before maex
    GO_ARAC_MAEX_INNER_DOOR     = 181197,                   //encounter door
    GO_ARAC_EYE_RAMP            = 181212,

    GO_PLAG_SLIME01_DOOR        = 181198,
    GO_PLAG_SLIME02_DOOR        = 181199,
    GO_PLAG_NOTH_ENTRY_DOOR     = 181200,                   //encounter door
    GO_PLAG_NOTH_EXIT_DOOR      = 181201,                   //exit, open when boss dead
    GO_PLAG_HEIG_ENTRY_DOOR     = 181202,
    GO_PLAG_HEIG_EXIT_DOOR      = 181203,                   //exit, open when boss dead
    GO_PLAG_LOAT_DOOR           = 181241,                   //encounter door
    GO_PLAG_EYE_RAMP            = 181211,

    GO_MILI_GOTH_ENTRY_GATE     = 181124,
    GO_MILI_GOTH_EXIT_GATE      = 181125,
    GO_MILI_GOTH_COMBAT_GATE    = 181170,                   //encounter door (?)
    GO_MILI_HORSEMEN_DOOR       = 181119,                   //encounter door
    GO_MILI_EYE_RAMP            = 181210,                   //encounter door

    GO_CHEST_HORSEMEN_NORM      = 181366,                   //four horsemen event, DoRespawnGameObject() when event == DONE
    GO_CHEST_HORSEMEN_HERO      = 193426,

    GO_CONS_PATH_EXIT_DOOR      = 181123,
    GO_CONS_GLUT_EXIT_DOOR      = 181120,                   //encounter door (?)
    GO_CONS_THAD_DOOR           = 181121,
    GO_CONS_EYE_RAMP            = 181213,

    NPC_ZELIEK                  = 16063,
    NPC_THANE                   = 16064,
    NPC_BLAUMEUX                = 16065,
    NPC_RIVENDARE               = 30549,

    NPC_ANUB_REKHAN             = 15956,

    NPC_THADDIUS                = 15928,
    NPC_STALAGG                 = 15929,
    NPC_FEUGEN                  = 15930,

    AREATRIGGER_FROSTWYRM       = 4120                      //not needed here, but AT to be scripted
};

struct MANGOS_DLL_DECL instance_naxxramas : public ScriptedInstance
{
    instance_naxxramas(Map* pMap) : ScriptedInstance(pMap) {Initialize();}

    std::string strInstData;
    uint32 m_uiEncounter[ENCOUNTERS];

    uint32 StalaggFeugen_Target_Timer;

    uint64 AracEyeRamp;
    uint64 PlagEyeRamp;
    uint64 MiliEyeRamp;
    uint64 ConsEyeRamp;

    uint64 Patchwerk;
    uint64 Grobbulus;
    uint64 Gluth;
    uint64 Thaddius;
    uint64 Stalagg;
    uint64 Feugen;

    uint64 AnubRekhan;
    uint64 Faerlina;
    uint64 Maexxna;

    uint64 Razuvious;
    uint64 Gothik;
    uint64 Zeliek;
    uint64 Thane;
    uint64 Blaumeux;
    uint64 Rivendare;

    uint64 PathExitDoor;
    uint64 GlutExitDoor;
    uint64 ThadDoor;

    uint64 AnubDoor;
    uint64 AnubGate;
    uint64 FaerDoor;
    uint64 MaexOuter;
    uint64 MaexInner;

    uint64 GothCombatGate;
    uint64 GothikEntryDoor;
    uint64 GothikExitDoor;
    uint64 HorsemenDoor;
    uint64 HorsemenChestN;

    uint64 NothEntryDoor;
    uint64 NothExitDoor;
    uint64 HeigEntryDoor;
    uint64 HeigExitDoor;
    uint64 LoathebDoor;

    void Initialize()
    {
        StalaggFeugen_Target_Timer = 0;

        AracEyeRamp = 0;
        PlagEyeRamp = 0;
        MiliEyeRamp = 0;
        ConsEyeRamp = 0;

        Patchwerk = 0;
        Grobbulus = 0;
        Gluth     = 0;
        Thaddius  = 0;
        Stalagg   = 0;
        Feugen    = 0;

        AnubRekhan = 0;
        Faerlina   = 0;
        Maexxna    = 0;

        Razuvious  = 0;
        Gothik     = 0;
        Zeliek     = 0;
        Thane      = 0;
        Blaumeux   = 0;
        Rivendare  = 0;

        PathExitDoor = 0;
        GlutExitDoor = 0;
        ThadDoor     = 0;

        AnubDoor     = 0;
        AnubGate     = 0;
        FaerDoor     = 0;
        MaexOuter    = 0;
        MaexInner    = 0;

        GothCombatGate  = 0;
        GothikEntryDoor = 0;
        GothikExitDoor  = 0;
        HorsemenDoor    = 0;
        HorsemenChestN  = 0;

        NothEntryDoor = 0;
        NothExitDoor  = 0;
        HeigEntryDoor = 0;
        HeigExitDoor  = 0;
        LoathebDoor   = 0;

        for(uint8 i = 0; i < ENCOUNTERS; i++)
            m_uiEncounter[i] = NOT_STARTED;
    }

    void OnCreatureCreate(Creature* pCreature)
    {
        switch(pCreature->GetEntry())
        {
            case NPC_ANUB_REKHAN:  AnubRekhan   = pCreature->GetGUID(); break;
            case NPC_THADDIUS:     Thaddius     = pCreature->GetGUID(); break;
            case NPC_STALAGG:      Stalagg      = pCreature->GetGUID(); break;
            case NPC_FEUGEN:       Feugen       = pCreature->GetGUID(); break;
            case NPC_ZELIEK:       Zeliek       = pCreature->GetGUID(); break;
            case NPC_THANE:        Thane        = pCreature->GetGUID(); break;
            case NPC_BLAUMEUX:     Blaumeux     = pCreature->GetGUID(); break;
            case NPC_RIVENDARE:    Rivendare    = pCreature->GetGUID(); break;
        }
    }

    void OnObjectCreate(GameObject* pGo)
    {
        switch(pGo->GetEntry())
        {
            case GO_ARAC_ANUB_GATE:         AnubGate      = pGo->GetGUID(); break;
            case GO_ARAC_ANUB_DOOR:
                AnubDoor = pGo->GetGUID();
                if (m_uiEncounter[4] == DONE)
                    pGo->SetGoState(GO_STATE_ACTIVE);
                break;
            //case GO_ARAC_FAER_WEB:            = pGo->GetGUID(); break;
            //case GO_ARAC_DOOD_DOOR_2:         = pGo->GetGUID(); break;
            case GO_ARAC_FAER_DOOR:
                FaerDoor = pGo->GetGUID();
                if (m_uiEncounter[5] == DONE)
                    pGo->SetGoState(GO_STATE_ACTIVE);
                break;
            case GO_ARAC_MAEX_OUTER_DOOR:
                MaexOuter = pGo->GetGUID();
                if (m_uiEncounter[5] == DONE)
                    pGo->SetGoState(GO_STATE_ACTIVE);
                break;
            case GO_ARAC_MAEX_INNER_DOOR:   MaexInner     = pGo->GetGUID(); break;
            case GO_ARAC_EYE_RAMP:
                AracEyeRamp = pGo->GetGUID();
                if (m_uiEncounter[6] == DONE)
                    pGo->SetGoState(GO_STATE_ACTIVE);
                break;
            //case GO_PLAG_SLIME01_DOOR:        = pGo->GetGUID(); break;
            //case GO_PLAG_SLIME02_DOOR:        = pGo->GetGUID(); break;
            case GO_PLAG_NOTH_ENTRY_DOOR:   NothEntryDoor = pGo->GetGUID(); break;
            case GO_PLAG_NOTH_EXIT_DOOR:
                NothExitDoor = pGo->GetGUID();
                if (m_uiEncounter[10] == DONE)
                    pGo->SetGoState(GO_STATE_ACTIVE);
                break;
            case GO_PLAG_HEIG_ENTRY_DOOR:
                HeigEntryDoor = pGo->GetGUID();
                if (m_uiEncounter[10] == DONE)
                    pGo->SetGoState(GO_STATE_ACTIVE);
                break;
            case GO_PLAG_HEIG_EXIT_DOOR:
                HeigExitDoor = pGo->GetGUID();
                if (m_uiEncounter[11] == DONE)
                    pGo->SetGoState(GO_STATE_ACTIVE);
                break;
            case GO_PLAG_LOAT_DOOR:
                LoathebDoor = pGo->GetGUID();
                if (m_uiEncounter[11] == DONE)
                    pGo->SetGoState(GO_STATE_ACTIVE);
                break;
            case GO_PLAG_EYE_RAMP:          PlagEyeRamp   = pGo->GetGUID(); break;
            case GO_MILI_GOTH_ENTRY_GATE:
                GothikEntryDoor = pGo->GetGUID();
                if (m_uiEncounter[7] == DONE)
                    pGo->SetGoState(GO_STATE_ACTIVE);
                break;
            case GO_MILI_GOTH_EXIT_GATE:
                GothikExitDoor = pGo->GetGUID();
                if (m_uiEncounter[8] == DONE)
                    pGo->SetGoState(GO_STATE_ACTIVE);
                break;
            case GO_MILI_GOTH_COMBAT_GATE:
                GothCombatGate = pGo->GetGUID();
                break;
            case GO_MILI_HORSEMEN_DOOR:
                HorsemenDoor  = pGo->GetGUID();
                if (m_uiEncounter[8] == DONE)
                    pGo->SetGoState(GO_STATE_ACTIVE);
                break;
            case GO_MILI_EYE_RAMP:
                MiliEyeRamp = pGo->GetGUID();
                if (m_uiEncounter[9] == DONE)
                    pGo->SetGoState(GO_STATE_ACTIVE);
                break;
            case GO_CHEST_HORSEMEN_NORM:    HorsemenChestN   = pGo->GetGUID(); break;
            //case GO_CHEST_HORSEMEN_HERO:      = pGo->GetGUID(); break;
            case GO_CONS_PATH_EXIT_DOOR:
                PathExitDoor = pGo->GetGUID();
                if (m_uiEncounter[0] == DONE)
                    pGo->SetGoState(GO_STATE_ACTIVE);
                break;
            case GO_CONS_GLUT_EXIT_DOOR:
                GlutExitDoor = pGo->GetGUID();
                if (m_uiEncounter[2] == DONE)
                    pGo->SetGoState(GO_STATE_ACTIVE);
                break;
            case GO_CONS_THAD_DOOR:
                ThadDoor = pGo->GetGUID();
                if (m_uiEncounter[2] == DONE)
                    pGo->SetGoState(GO_STATE_ACTIVE);
                break;
            case GO_CONS_EYE_RAMP:
                ConsEyeRamp = pGo->GetGUID();
                if (m_uiEncounter[3] == DONE)
                    pGo->SetGoState(GO_STATE_ACTIVE);
                break;
        }
    }

    void SetData(uint32 uiType, uint32 uiData)
    {
        switch(uiType)
        {
            case TYPE_PATCHWERK:
                m_uiEncounter[0] = uiData;
                if (uiData == DONE)
                    DoorControl(PathExitDoor, uiData);
                break;
            case TYPE_GROBBULUS:
                m_uiEncounter[1] = uiData;
                break;
            case TYPE_GLUTH:
                m_uiEncounter[2] = uiData;
                if (uiData == DONE)
                {
                    DoorControl(GlutExitDoor, uiData);
                    DoorControl(ThadDoor, uiData);
                }
                break;
            case TYPE_THADDIUS:
                m_uiEncounter[3] = uiData;
                DoorControl(ThadDoor, uiData);
                if (uiData == DONE)
                    ActiveRamp(ConsEyeRamp);
                break;
            case TYPE_ANUB_REKHAN:
                m_uiEncounter[4] = uiData;
                if (uiData == DONE)
                    DoorControl(AnubDoor, uiData);
                break;
            case TYPE_FAERLINA:
                m_uiEncounter[5] = uiData;
                if (uiData == DONE)
                {
                    DoorControl(FaerDoor, uiData);
                    DoorControl(MaexOuter, uiData);
                }
                break;
            case TYPE_MAEXXNA:
                m_uiEncounter[6] = uiData;
                DoorControl(MaexOuter, uiData);
                DoorControl(MaexInner, uiData);
                if (uiData == DONE)
                    ActiveRamp(AracEyeRamp);
                break;
            case TYPE_RAZUVIOUS:
                m_uiEncounter[7] = uiData;
                if (uiData == DONE)
                     DoorControl(GothikEntryDoor, uiData);
                break;
            case TYPE_GOTHIK:
                m_uiEncounter[8] = uiData;
                if (uiData == DONE)
                {
                     DoorControl(GothikExitDoor, uiData);
                     DoorControl(HorsemenDoor, uiData);
                }
                break;
            case TYPE_FOUR_HORSEMEN:
                m_uiEncounter[9] = uiData;
                DoorControl(HorsemenDoor, uiData);
                if (uiData == DONE)
                {
                    ActiveRamp(MiliEyeRamp);
                    if (GameObject* pChest = instance->GetGameObject(HorsemenChestN)) // how about heroic
                        if (pChest && !pChest->isSpawned())
                            pChest->SetRespawnTime(180000);
                }
                break;
            case TYPE_NOTH:
                m_uiEncounter[10] = uiData;
                DoorControl(NothEntryDoor, uiData);
                if (uiData == DONE)
                {
                     DoorControl(NothExitDoor, uiData);
                     DoorControl(HeigEntryDoor, uiData);
                }
                break;
            case TYPE_HEIGAN:
                m_uiEncounter[11] = uiData;
                DoorControl(HeigEntryDoor, uiData);
                if (uiData == DONE)
                {
                     DoorControl(HeigExitDoor, uiData);
                     DoorControl(LoathebDoor, uiData);
                }
                break;
            case TYPE_LOATHEB:
                m_uiEncounter[12] = uiData;
                DoorControl(LoathebDoor, uiData);
                if (uiData == DONE)
                    ActiveRamp(PlagEyeRamp);
                break;
        }

        if (uiData == DONE)
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;
            saveStream << m_uiEncounter[0] << " " << m_uiEncounter[1] << " " << m_uiEncounter[2] << " "
                << m_uiEncounter[3] << " " << m_uiEncounter[4] << " " << m_uiEncounter[5] << " "
                << m_uiEncounter[6];
                //<< m_uiEncounter[6] << " " << m_uiEncounter[7] << " " << m_uiEncounter[8];

            strInstData = saveStream.str();

            SaveToDB();
            OUT_SAVE_INST_DATA_COMPLETE;
        }
    }

    uint32 GetData(uint32 uiType)
    {
        switch(uiType)
        {
            case TYPE_PATCHWERK:   return m_uiEncounter[0];
            case TYPE_GROBBULUS:   return m_uiEncounter[1];
            case TYPE_GLUTH:       return m_uiEncounter[2];
            case TYPE_THADDIUS:    return m_uiEncounter[3];
            case TYPE_ANUB_REKHAN: return m_uiEncounter[4];
            case TYPE_FAERLINA:    return m_uiEncounter[5];
            case TYPE_MAEXXNA:     return m_uiEncounter[6];
        }
        return 0;
    }

    uint64 GetData64(uint32 uiData)
    {
        switch(uiData)
        {
            case DATA_ANUB_REKHAN:   return AnubRekhan;   break;
            case DATA_GOTHIK_GATE:   return GothCombatGate; break;
            case DATA_THADDIUS:      return Thaddius;     break;
            case DATA_STALAGG:       return Stalagg;      break;
            case DATA_FEUGEN:        return Feugen;       break;

            case DATA_ZELIEK:        return Zeliek;       break;
            case DATA_KORTHAZZ:      return Thane;       break;
            case DATA_BLAUMEUX:      return Blaumeux;       break;
            case DATA_RIVENDARE:     return Rivendare;       break;
        }
        return 0;
    }

    const char* Save()
    {
        return strInstData.c_str();
    }

    void Load(const char* chrIn)
    {
        if (!chrIn)
        {
            OUT_LOAD_INST_DATA_FAIL;
            return;
        }

        OUT_LOAD_INST_DATA(chrIn);

        std::istringstream loadStream(chrIn);
        loadStream >> m_uiEncounter[0] >> m_uiEncounter[1] >> m_uiEncounter[2] >> m_uiEncounter[3]
            >> m_uiEncounter[4] >> m_uiEncounter[5] >> m_uiEncounter[6];

        for(uint8 i = 0; i < ENCOUNTERS; ++i)
            if (m_uiEncounter[i] == IN_PROGRESS)            // Do not load an encounter as "In Progress" - reset it instead.
                m_uiEncounter[i] = NOT_STARTED;

        OUT_LOAD_INST_DATA_COMPLETE;
    }

    void DoorControl(uint64 uiDoor, uint32 uiData)
    {
        if (GameObject* pDoors = instance->GetGameObject(uiDoor))
        {
            if(uiData == IN_PROGRESS)
                pDoors->SetGoState(GO_STATE_READY);
            else
                pDoors->SetGoState(GO_STATE_ACTIVE);
        }
    }
    void ActiveRamp(uint64 uiRamp)
    {
        if (GameObject* pRamp = instance->GetGameObject(uiRamp))
            pRamp->SetGoState(GO_STATE_ACTIVE);
    }
};

InstanceData* GetInstanceData_instance_naxxramas(Map* pMap)
{
    return new instance_naxxramas(pMap);
}

void AddSC_instance_naxxramas()
{
    Script* pNewScript;
    pNewScript = new Script;
    pNewScript->Name = "instance_naxxramas";
    pNewScript->GetInstanceData = &GetInstanceData_instance_naxxramas;
    pNewScript->RegisterSelf();
}
