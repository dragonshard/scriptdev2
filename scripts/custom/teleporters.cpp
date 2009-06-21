/* Copyright (C) 2006 - 2009 DragonShard <git://github.com/mangos/mangos.git> */

/* ScriptData
SDName: Teleporters_Scripts
SD%Complete: 100
SDComment: Make Tour Operators work
SDCategory: NPCs
EndScriptData */

/* ContentData
teleporter_ruttheran_village
teleporter_auberdine
teleporter_exodar
teleporter_menethil_harbor
teleporter_theramore_isle
teleporter_ferelas
teleporter_feathermoon_stronghold
teleporter_silvermoonteleporter_undercity
teleporter_orgrimmarteleporter_gromgol
teleporter_ratchetteleporter_booty_bay
EndContentData */

#include "precompiled.h"

/*####################
#  Transporter Ally  #
####################*/

/*######
## teleporter_rut'theran_village
######*/

bool GossipHello_teleporter_ruttheran_village(Player *player, Creature *_Creature)
{
	player->ADD_GOSSIP_ITEM(0, "Vorrei andare ad Auberdine", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
	player->ADD_GOSSIP_ITEM(0, "Vorrei andare ad Exodar", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
	player->PlayerTalkClass->SendGossipMenu(999999, _Creature->GetGUID());
    
	return true;
}

bool GossipSelect_teleporter_ruttheran_village(Player *player, Creature *_Creature, uint32 sender, uint32 action )
{
	if (action == GOSSIP_ACTION_INFO_DEF+4)
	{
		player->CLOSE_GOSSIP_MENU();
		return true;
	}

	if (player->getLevel() < 15)
	{
		player->ADD_GOSSIP_ITEM(0, "Arrivederci", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
		player->SEND_GOSSIP_MENU(1000000, _Creature->GetGUID());
		return true;
	}

	switch (action)
	{
		case GOSSIP_ACTION_INFO_DEF:
			player->CLOSE_GOSSIP_MENU();
			player->TeleportTo(1, 6499.1, 793, 7.94, 4.38);
			break;
		case GOSSIP_ACTION_INFO_DEF+1:
			player->CLOSE_GOSSIP_MENU();
			player->TeleportTo(530, -4067.1, -11994.7, -0.42, 1.29);
			break;
	}
	
	return true;
}

/*######
## teleporter_auberdine
######*/

bool GossipHello_teleporter_auberdine(Player *player, Creature *_Creature)
{
	player->ADD_GOSSIP_ITEM(0, "Vorrei andare a Rut'theran Village", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
	player->ADD_GOSSIP_ITEM(0, "Vorrei andare ad Exodar", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
	player->ADD_GOSSIP_ITEM(0, "Vorrei andare a Menethil Harbor", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
	player->PlayerTalkClass->SendGossipMenu(999999, _Creature->GetGUID());
    
	return true;
}

bool GossipSelect_teleporter_auberdine(Player *player, Creature *_Creature, uint32 sender, uint32 action )
{
	if (action == GOSSIP_ACTION_INFO_DEF+4)
	{
		player->CLOSE_GOSSIP_MENU();
		return true;
	}

	if (player->getLevel() < 15)
	{
		player->ADD_GOSSIP_ITEM(0, "Arrivederci", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
		player->SEND_GOSSIP_MENU(1000000, _Creature->GetGUID());
		return true;
	}

	switch (action)
	{
		case GOSSIP_ACTION_INFO_DEF:
			player->CLOSE_GOSSIP_MENU();
			player->TeleportTo(1, 8546.3, 1021.2, 5.79, 6.18);
			break;
		case GOSSIP_ACTION_INFO_DEF+1:
			player->CLOSE_GOSSIP_MENU();
			player->TeleportTo(530, -4067.1, -11994.7, -0.42, 1.29);
			break;
		case GOSSIP_ACTION_INFO_DEF+2:
			player->CLOSE_GOSSIP_MENU();
			player->TeleportTo(0, -3895, -599.8, 5.48, 5.23);
			break;
	}
	
	return true;
}

/*######
## teleporter_exodar
######*/

bool GossipHello_teleporter_exodar(Player *player, Creature *_Creature)
{
	player->ADD_GOSSIP_ITEM(0, "Vorrei andare a Rut'theran Village", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
	player->ADD_GOSSIP_ITEM(0, "Vorrei andare ad Auberdine", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
	player->PlayerTalkClass->SendGossipMenu(999999, _Creature->GetGUID());
    
	return true;
}

bool GossipSelect_teleporter_exodar(Player *player, Creature *_Creature, uint32 sender, uint32 action )
{
	if (action == GOSSIP_ACTION_INFO_DEF+4)
	{
		player->CLOSE_GOSSIP_MENU();
		return true;
	}

	if (player->getLevel() < 15)
	{
		player->ADD_GOSSIP_ITEM(0, "Arrivederci", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
		player->SEND_GOSSIP_MENU(1000000, _Creature->GetGUID());
		return true;
	}

	switch (action)
	{
		case GOSSIP_ACTION_INFO_DEF:
			player->CLOSE_GOSSIP_MENU();
			player->TeleportTo(1, 8546.3, 1021.2, 5.79, 6.18);
			break;
		case GOSSIP_ACTION_INFO_DEF+1:
			player->CLOSE_GOSSIP_MENU();
			player->TeleportTo(1, 6499.1, 793, 7.94, 4.38);
			break;
	}
	
	return true;
}

/*######
## teleporter_menethil_harbor
######*/

bool GossipHello_teleporter_menethil_harbor(Player *player, Creature *_Creature)
{
	player->ADD_GOSSIP_ITEM(0, "Vorrei andare ad Auberdine", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
	player->ADD_GOSSIP_ITEM(0, "Vorrei andare a Theramore Isle", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
	player->PlayerTalkClass->SendGossipMenu(999999, _Creature->GetGUID());
    
	return true;
}

bool GossipSelect_teleporter_menethil_harbor(Player *player, Creature *_Creature, uint32 sender, uint32 action )
{
	if (action == GOSSIP_ACTION_INFO_DEF+4)
	{
		player->CLOSE_GOSSIP_MENU();
		return true;
	}

	if (player->getLevel() < 15)
	{
		player->ADD_GOSSIP_ITEM(0, "Arrivederci", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
		player->SEND_GOSSIP_MENU(1000000, _Creature->GetGUID());
		return true;
	}

	switch (action)
	{
		case GOSSIP_ACTION_INFO_DEF:
			player->CLOSE_GOSSIP_MENU();
			player->TeleportTo(1, 6499.1, 793, 7.94, 4.38);
			break;
		case GOSSIP_ACTION_INFO_DEF+1:
			player->CLOSE_GOSSIP_MENU();
			player->TeleportTo(1, -4004.1, -4729.1, 5.14, 0.82);
			break;
	}
	
	return true;
}

/*######
## teleporter_theramore_isle
######*/

bool GossipHello_teleporter_theramore_isle(Player *player, Creature *_Creature)
{
	player->ADD_GOSSIP_ITEM(0, "Vorrei andare a Menethil Harbor", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
	player->PlayerTalkClass->SendGossipMenu(999999, _Creature->GetGUID());
    
	return true;
}

bool GossipSelect_teleporter_theramore_isle(Player *player, Creature *_Creature, uint32 sender, uint32 action )
{
	if (action == GOSSIP_ACTION_INFO_DEF+4)
	{
		player->CLOSE_GOSSIP_MENU();
		return true;
	}

	if (player->getLevel() < 15)
	{
		player->ADD_GOSSIP_ITEM(0, "Arrivederci", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
		player->SEND_GOSSIP_MENU(1000000, _Creature->GetGUID());
		return true;
	}

	switch (action)
	{
		case GOSSIP_ACTION_INFO_DEF:
			player->CLOSE_GOSSIP_MENU();
			player->TeleportTo(0, -3895, -599.8, 5.48, 5.23);
			break;
	}
	
	return true;
}

/*######
## teleporter_ferelas
######*/

bool GossipHello_teleporter_ferelas(Player *player, Creature *_Creature)
{
	player->ADD_GOSSIP_ITEM(0, "Vorrei andare a Feathermoon Stronghold", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
	player->PlayerTalkClass->SendGossipMenu(999999, _Creature->GetGUID());
    
	return true;
}

bool GossipSelect_teleporter_ferelas(Player *player, Creature *_Creature, uint32 sender, uint32 action )
{
	if (action == GOSSIP_ACTION_INFO_DEF+4)
	{
		player->CLOSE_GOSSIP_MENU();
		return true;
	}

	if (player->getLevel() < 15)
	{
		player->ADD_GOSSIP_ITEM(0, "Arrivederci", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
		player->SEND_GOSSIP_MENU(1000000, _Creature->GetGUID());
		return true;
	}

	switch (action)
	{
		case GOSSIP_ACTION_INFO_DEF:
			player->CLOSE_GOSSIP_MENU();
			player->TeleportTo(1, -4217.7, 3285.4, 7, 3.16);
			break;
	}
	
	return true;
}

/*######
## teleporter_feathermoon_stronghold
######*/

bool GossipHello_teleporter_feathermoon_stronghold(Player *player, Creature *_Creature)
{
	player->ADD_GOSSIP_ITEM(0, "Vorrei andare a Ferelas", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
	player->PlayerTalkClass->SendGossipMenu(999999, _Creature->GetGUID());
    
	return true;
}

bool GossipSelect_teleporter_feathermoon_stronghold(Player *player, Creature *_Creature, uint32 sender, uint32 action )
{
	if (action == GOSSIP_ACTION_INFO_DEF+4)
	{
		player->CLOSE_GOSSIP_MENU();
		return true;
	}

	if (player->getLevel() < 15)
	{
		player->ADD_GOSSIP_ITEM(0, "Arrivederci", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
		player->SEND_GOSSIP_MENU(1000000, _Creature->GetGUID());
		return true;
	}

	switch (action)
	{
		case GOSSIP_ACTION_INFO_DEF:
			player->CLOSE_GOSSIP_MENU();
			player->TeleportTo(1, -4347.6, 2425, 7, 4.71);
			break;
	}
	
	return true;
}

/*######################
#  Transporter Ordosi  #
######################*/

/*######
## teleporter_silvermoon
######*/

bool GossipHello_teleporter_silvermoon(Player *player, Creature *_Creature)
{
	player->ADD_GOSSIP_ITEM(0, "Vorrei andare ad Undercity", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
	player->PlayerTalkClass->SendGossipMenu(999999, _Creature->GetGUID());
    
	return true;
}

bool GossipSelect_teleporter_silvermoon(Player *player, Creature *_Creature, uint32 sender, uint32 action )
{
	if (action == GOSSIP_ACTION_INFO_DEF+4)
	{
		player->CLOSE_GOSSIP_MENU();
		return true;
	}

	if (player->getLevel() < 15)
	{
		player->ADD_GOSSIP_ITEM(0, "Arrivederci", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
		player->SEND_GOSSIP_MENU(1000000, _Creature->GetGUID());
		return true;
	}

	switch (action)
	{
		case GOSSIP_ACTION_INFO_DEF:
			player->CLOSE_GOSSIP_MENU();
			player->TeleportTo(0, 2057.6, 241.4, 99.76, 1.36);
			break;
	}
	
	return true;
}

/*######
## teleporter_undercity
######*/

bool GossipHello_teleporter_undercity(Player *player, Creature *_Creature)
{
	player->ADD_GOSSIP_ITEM(0, "Vorrei andare a Silvermoon", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
	player->ADD_GOSSIP_ITEM(0, "Vorrei andare ad Orgrimmar", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
	player->ADD_GOSSIP_ITEM(0, "Vorrei andare a Grom'gol", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
	player->PlayerTalkClass->SendGossipMenu(999999, _Creature->GetGUID());
    
	return true;
}

bool GossipSelect_teleporter_undercity(Player *player, Creature *_Creature, uint32 sender, uint32 action )
{
	if (action == GOSSIP_ACTION_INFO_DEF+4)
	{
		player->CLOSE_GOSSIP_MENU();
		return true;
	}

	if (player->getLevel() < 15)
	{
		player->ADD_GOSSIP_ITEM(0, "Arrivederci", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
		player->SEND_GOSSIP_MENU(1000000, _Creature->GetGUID());
		return true;
	}

	switch (action)
	{
		case GOSSIP_ACTION_INFO_DEF:
			player->CLOSE_GOSSIP_MENU();
			player->TeleportTo(530, 9352.4, -7277.9, 14.24, 0);
			break;
		case GOSSIP_ACTION_INFO_DEF+1:
			player->CLOSE_GOSSIP_MENU();
			player->TeleportTo(1, 1325.36, -4649.16, 54.08, 6.19);
			break;
		case GOSSIP_ACTION_INFO_DEF+2:
			player->CLOSE_GOSSIP_MENU();
			player->TeleportTo(0, -12411.7, 208.2, 31.56, 3.16);
			break;
	}
	
	return true;
}

/*######
## teleporter_orgrimmar
######*/

bool GossipHello_teleporter_orgrimmar(Player *player, Creature *_Creature)
{
	player->ADD_GOSSIP_ITEM(0, "Vorrei andare ad Undercity", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
	player->ADD_GOSSIP_ITEM(0, "Vorrei andare a Grom'gol", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
	player->PlayerTalkClass->SendGossipMenu(999999, _Creature->GetGUID());
    
	return true;
}

bool GossipSelect_teleporter_orgrimmar(Player *player, Creature *_Creature, uint32 sender, uint32 action )
{
	if (action == GOSSIP_ACTION_INFO_DEF+4)
	{
		player->CLOSE_GOSSIP_MENU();
		return true;
	}

	if (player->getLevel() < 15)
	{
		player->ADD_GOSSIP_ITEM(0, "Arrivederci", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
		player->SEND_GOSSIP_MENU(1000000, _Creature->GetGUID());
		return true;
	}

	switch (action)
	{
		case GOSSIP_ACTION_INFO_DEF:
			player->CLOSE_GOSSIP_MENU();
			player->TeleportTo(0, 2057.6, 241.4, 99.76, 1.36);
			break;
		case GOSSIP_ACTION_INFO_DEF+1:
			player->CLOSE_GOSSIP_MENU();
			player->TeleportTo(0, -12411.7, 208.2, 31.56, 3.16);
	}
	
	return true;
}

/*######
## teleporter_gromgol
######*/

bool GossipHello_teleporter_gromgol(Player *player, Creature *_Creature)
{
	player->ADD_GOSSIP_ITEM(0, "Vorrei andare ad Undercity", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
	player->ADD_GOSSIP_ITEM(0, "Vorrei andare ad Orgrimmar", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
	player->PlayerTalkClass->SendGossipMenu(999999, _Creature->GetGUID());
    
	return true;
}

bool GossipSelect_teleporter_gromgol(Player *player, Creature *_Creature, uint32 sender, uint32 action )
{
	if (action == GOSSIP_ACTION_INFO_DEF+4)
	{
		player->CLOSE_GOSSIP_MENU();
		return true;
	}

	if (player->getLevel() < 15)
	{
		player->ADD_GOSSIP_ITEM(0, "Arrivederci", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
		player->SEND_GOSSIP_MENU(1000000, _Creature->GetGUID());
		return true;
	}

	switch (action)
	{
		case GOSSIP_ACTION_INFO_DEF:
			player->CLOSE_GOSSIP_MENU();
			player->TeleportTo(0, 2057.6, 241.4, 99.76, 1.36);
			break;
		case GOSSIP_ACTION_INFO_DEF+1:
			player->CLOSE_GOSSIP_MENU();
			player->TeleportTo(1, 1325.36, -4649.16, 54.08, 6.19);
	}
	
	return true;
}

/*########################
#  Transporter Neutrali  #
########################*/

/*######
## teleporter_ratchet
######*/

bool GossipHello_teleporter_ratchet(Player *player, Creature *_Creature)
{
	player->ADD_GOSSIP_ITEM(0, "Vorrei andare a Booty Bay", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
	player->PlayerTalkClass->SendGossipMenu(999999, _Creature->GetGUID());
    
	return true;
}

bool GossipSelect_teleporter_ratchet(Player *player, Creature *_Creature, uint32 sender, uint32 action )
{
	if (action == GOSSIP_ACTION_INFO_DEF+4)
	{
		player->CLOSE_GOSSIP_MENU();
		return true;
	}

	if (player->getLevel() < 15)
	{
		player->ADD_GOSSIP_ITEM(0, "Arrivederci", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
		player->SEND_GOSSIP_MENU(1000000, _Creature->GetGUID());
		return true;
	}

	switch (action)
	{
		case GOSSIP_ACTION_INFO_DEF:
			player->CLOSE_GOSSIP_MENU();
			player->TeleportTo(0, -14282, 556, 8.90, 4.18);
			break;
	}
	
	return true;
}

/*######
## teleporter_booty_bay
######*/

bool GossipHello_teleporter_booty_bay(Player *player, Creature *_Creature)
{
	player->ADD_GOSSIP_ITEM(0, "Vorrei andare a Ratchet", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
	player->PlayerTalkClass->SendGossipMenu(999999, _Creature->GetGUID());
    
	return true;
}

bool GossipSelect_teleporter_booty_bay(Player *player, Creature *_Creature, uint32 sender, uint32 action )
{
	if (action == GOSSIP_ACTION_INFO_DEF+4)
	{
		player->CLOSE_GOSSIP_MENU();
		return true;
	}

	if (player->getLevel() < 15)
	{
		player->ADD_GOSSIP_ITEM(0, "Arrivederci", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
		player->SEND_GOSSIP_MENU(1000000, _Creature->GetGUID());
		return true;
	}

	switch (action)
	{
		case GOSSIP_ACTION_INFO_DEF:
			player->CLOSE_GOSSIP_MENU();
			player->TeleportTo(1, -994, -3830, 5.60, 1.65);
			break;
	}
	
	return true;
}

/*----------*/

void AddSC_tele_scripts()
{
    Script *newscript;

	/*##################################
	#  Teleporters References Scripts  #
	##################################*/

	/*-----Ally-----*/

	newscript = new Script;
	newscript->Name="teleporter_ruttheran_village";
	newscript->pGossipHello          = &GossipHello_teleporter_ruttheran_village;
	newscript->pGossipSelect         = &GossipSelect_teleporter_ruttheran_village;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name="teleporter_auberdine";
	newscript->pGossipHello          = &GossipHello_teleporter_auberdine;
	newscript->pGossipSelect         = &GossipSelect_teleporter_auberdine;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name="teleporter_exodar";
	newscript->pGossipHello          = &GossipHello_teleporter_exodar;
	newscript->pGossipSelect         = &GossipSelect_teleporter_exodar;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name="teleporter_menethil_harbor";
	newscript->pGossipHello          = &GossipHello_teleporter_menethil_harbor;
	newscript->pGossipSelect         = &GossipSelect_teleporter_menethil_harbor;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name="teleporter_theramore_isle";
	newscript->pGossipHello          = &GossipHello_teleporter_theramore_isle;
	newscript->pGossipSelect         = &GossipSelect_teleporter_theramore_isle;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name="teleporter_ferelas";
	newscript->pGossipHello          = &GossipHello_teleporter_ferelas;
	newscript->pGossipSelect         = &GossipSelect_teleporter_ferelas;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name="teleporter_feathermoon_stronghold";
	newscript->pGossipHello          = &GossipHello_teleporter_feathermoon_stronghold;
	newscript->pGossipSelect         = &GossipSelect_teleporter_feathermoon_stronghold;
	newscript->RegisterSelf();

	/*-----Orda-----*/

	newscript = new Script;
	newscript->Name="teleporter_silvermoon";
	newscript->pGossipHello          = &GossipHello_teleporter_silvermoon;
	newscript->pGossipSelect         = &GossipSelect_teleporter_silvermoon;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name="teleporter_undercity";
	newscript->pGossipHello          = &GossipHello_teleporter_undercity;
	newscript->pGossipSelect         = &GossipSelect_teleporter_undercity;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name="teleporter_orgrimmar";
	newscript->pGossipHello          = &GossipHello_teleporter_orgrimmar;
	newscript->pGossipSelect         = &GossipSelect_teleporter_orgrimmar;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name="teleporter_gromgol";
	newscript->pGossipHello          = &GossipHello_teleporter_gromgol;
	newscript->pGossipSelect         = &GossipSelect_teleporter_gromgol;
	newscript->RegisterSelf();

	/*-----Neutral-----*/

	newscript = new Script;
	newscript->Name="teleporter_ratchet";
	newscript->pGossipHello          = &GossipHello_teleporter_ratchet;
	newscript->pGossipSelect         = &GossipSelect_teleporter_ratchet;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name="teleporter_booty_bay";
	newscript->pGossipHello          = &GossipHello_teleporter_booty_bay;
	newscript->pGossipSelect         = &GossipSelect_teleporter_booty_bay;
	newscript->RegisterSelf();
}
