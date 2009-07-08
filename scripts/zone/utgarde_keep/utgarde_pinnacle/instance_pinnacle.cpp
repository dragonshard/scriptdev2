#include "precompiled.h"
#include "def_pinnacle.h"

#define ENCOUNTERS     4

/* Utgarde Pinnacle encounters:
0 - Svala Sorrowgrave
1 - Gortok Palehoof
2 - Skadi the Ruthless
3 - King Ymiron
*/

struct MANGOS_DLL_DECL instance_pinnacle : public ScriptedInstance
{
    instance_pinnacle(Map *Map) : ScriptedInstance(Map) {Initialize();};

    uint64 SvalaSorrowgrave;
    uint64 GortokPalehoof;
    uint64 SkadiRuthless;
    uint64 KingYmiron;
    uint64 FrenziedWorgen;
    uint64 RavenousFurbolg;
    uint64 MassiveJormungar;
    uint64 FerociousRhino;
    uint64 RitualTarget;

    uint64 SkadiRuthlessDoor;
    uint64 YmironDoor;

    uint32 Encounters[ENCOUNTERS];
    std::string str_data;

   void Initialize()
   {
       SvalaSorrowgrave   = 0;
       GortokPalehoof     = 0;
       SkadiRuthless      = 0;
       KingYmiron         = 0;
   	   FrenziedWorgen     = 0;
   	   RavenousFurbolg    = 0;
   	   MassiveJormungar   = 0;
   	   FerociousRhino     = 0;
   	   RitualTarget       = 0;

   	   SkadiRuthlessDoor  = 0;
   	   YmironDoor         = 0;

       for(uint8 i = 0; i < ENCOUNTERS; ++i)
           Encounters[i] = NOT_STARTED;
    }

    bool IsEncounterInProgress() const
    {
        for(uint8 i = 0; i < ENCOUNTERS; ++i)
            if(Encounters[i] == IN_PROGRESS) return true;

        return false;
    }

    void OnCreatureCreate(Creature *creature)
    {
        switch(creature->GetEntry())
        {
            case 26668:    SvalaSorrowgrave = creature->GetGUID();         break;
            case 26687:    GortokPalehoof = creature->GetGUID();           break;
            case 26693:    SkadiRuthless = creature->GetGUID();            break;
            case 26861:    KingYmiron = creature->GetGUID();               break;
            case 26683:    FrenziedWorgen = creature->GetGUID();           break;
            case 26684:    RavenousFurbolg = creature->GetGUID();          break;
            case 26685:    MassiveJormungar = creature->GetGUID();         break;
            case 26686:    FerociousRhino = creature->GetGUID();           break;
            case 27327:    RitualTarget = creature->GetGUID();             break;
        }
    }

    void OnObjectCreate(GameObject* gobj)
    {
        switch(gobj->GetEntry())
        {
            case 192173: SkadiRuthlessDoor     = gobj->GetGUID(); break;
            case 192174: YmironDoor            = gobj->GetGUID(); break;
        }
    }

    uint64 GetData64(uint32 identifier)
    {
        switch(identifier)
        {
            case DATA_SVALA_SORROWGRAVE:          return SvalaSorrowgrave;
            case DATA_GORTOK_PALEHOOF:            return GortokPalehoof;
            case DATA_SKADI_THE_RUTHLESS:         return SkadiRuthless;
            case DATA_KING_YMIRON:                return KingYmiron;
            case DATA_MOB_FRENZIED_WORGEN:        return FrenziedWorgen;
            case DATA_MOB_RAVENOUS_FURBOLG:       return RavenousFurbolg;
            case DATA_MOB_MASSIVE_JORMUNGAR:      return MassiveJormungar;
            case DATA_MOB_FEROCIOUS_RHINO:        return FerociousRhino;
        }
        return 0;
    }

    void SetData(uint32 type, uint32 data)
    {
        switch(type)
        {
        case DATA_SVALA_SORROWGRAVE:
            SvalaSorrowgrave = data; break;
        case DATA_SVALA_SORROWGRAVE_EVENT:
            if(data == DONE)
            {
                //
            }
            Encounters[0] = data;break;
        case DATA_GORTOK_PALEHOOF_EVENT:
            if(data == DONE)
            {
                //
            }
            Encounters[1] = data; break;
        case DATA_SKADI_THE_RUTHLESS_EVENT:
            if(data == DONE)
            {
                OpenDoor(SkadiRuthlessDoor);
            }
            Encounters[2] = data; break;
        case DATA_KING_YMIRON_EVENT:
            if(data == DONE)
            {
                OpenDoor(YmironDoor);
            }
            Encounters[3] = data; break;
        }
    }

    uint32 GetData(uint32 type)
    {
        switch(type)
        {
            case DATA_SVALA_SORROWGRAVE_EVENT:        return Encounters[0];
            case DATA_GORTOK_PALEHOOF_EVENT:          return Encounters[1];
            case DATA_SKADI_THE_RUTHLESS_EVENT:       return Encounters[2];
            case DATA_KING_YMIRON_EVENT:              return Encounters[3];
        }
        return 0;
    }

    void OpenDoor(uint64 DoorGUID)
    {
        if (GameObject* pDoors = instance->GetGameObject(DoorGUID))
            pDoors->SetGoState(GO_STATE_ACTIVE);
    }
};

InstanceData* GetInstanceData_instance_pinnacle(Map* map)
{
    return new instance_pinnacle(map);
}

void AddSC_instance_pinnacle()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "instance_pinnacle";
    newscript->GetInstanceData = GetInstanceData_instance_pinnacle;
    newscript->RegisterSelf();
}
