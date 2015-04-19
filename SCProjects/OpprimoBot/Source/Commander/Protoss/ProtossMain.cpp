#include "ProtossMain.h"
#include "../../Managers/BuildplanEntry.h"
#include "../ExplorationSquad.h"
#include "../../Managers/ExplorationManager.h"
#include "../RushSquad.h"
#include "../../Managers/AgentManager.h"

ProtossMain::ProtossMain()
{
	buildplan.push_back(BuildplanEntry(UnitTypes::Protoss_Pylon, 8));
	buildplan.push_back(BuildplanEntry(UnitTypes::Protoss_Forge, 9));
	buildplan.push_back(BuildplanEntry(UnitTypes::Protoss_Gateway, 9));
	buildplan.push_back(BuildplanEntry(UnitTypes::Protoss_Pylon, 8));
	buildplan.push_back(BuildplanEntry(UnitTypes::Protoss_Assimilator, 10));
	buildplan.push_back(BuildplanEntry(UnitTypes::Protoss_Gateway, 12));
	buildplan.push_back(BuildplanEntry(UnitTypes::Protoss_Pylon, 14));
	buildplan.push_back(BuildplanEntry(UnitTypes::Protoss_Cybernetics_Core, 15));
	buildplan.push_back(BuildplanEntry(UnitTypes::Protoss_Stargate, 18));
	buildplan.push_back(BuildplanEntry(UnitTypes::Protoss_Citadel_of_Adun, 20));
	buildplan.push_back(BuildplanEntry(UnitTypes::Protoss_Pylon, 22));
	buildplan.push_back(BuildplanEntry(UnitTypes::Protoss_Templar_Archives, 24));
	
	mainSquad = new Squad(1, Squad::OFFENSIVE, "MainSquad", 10);
	mainSquad->addSetup(UnitTypes::Protoss_Dragoon, 10);
	mainSquad->setBuildup(true);
	mainSquad->setRequired(true);
	squads.push_back(mainSquad);

	stealthSquad = new Squad(2, Squad::OFFENSIVE, "StealthSquad", 11);
	stealthSquad->setRequired(false);
	stealthSquad->setBuildup(true);
	squads.push_back(stealthSquad);
	
	noWorkers = 16;
	noWorkersPerRefinery = 3;
}

ProtossMain::~ProtossMain()
{
	for (Squad* s : squads)
	{
		delete s;
	}
	instance = NULL;
}

void ProtossMain::computeActions()
{
	computeActionsBase();

	int cSupply = Broodwar->self()->supplyUsed() / 2;
	int min = Broodwar->self()->minerals();
	int gas = Broodwar->self()->gas();

	if (cSupply >= 17 && stage == 0)
	{
		buildplan.push_back(BuildplanEntry(UnitTypes::Protoss_Photon_Cannon, cSupply));

		mainSquad->addSetup(UnitTypes::Protoss_Dragoon, 8);
		mainSquad->addSetup(UnitTypes::Protoss_Scout, 5);
		buildplan.push_back(BuildplanEntry(UpgradeTypes::Singularity_Charge, cSupply));
		stage++;
	}
	if (cSupply >= 30 && stage == 1 && AgentManager::getInstance()->countNoFinishedUnits(UnitTypes::Protoss_Templar_Archives) > 0)
	{
		mainSquad->addSetup(UnitTypes::Protoss_High_Templar, 4);
		mainSquad->setBuildup(false);

		stealthSquad->addSetup(UnitTypes::Protoss_Dark_Templar, 6);
		stealthSquad->setBuildup(false);

		buildplan.push_back(BuildplanEntry(TechTypes::Psionic_Storm, cSupply));
		
		stage++;
	}
	if (min > 400 && stage == 2)
	{
		buildplan.push_back(BuildplanEntry(UnitTypes::Protoss_Nexus, cSupply));
		buildplan.push_back(BuildplanEntry(UnitTypes::Protoss_Gateway, cSupply));
		buildplan.push_back(BuildplanEntry(UnitTypes::Protoss_Stargate, cSupply));
		stage++;
	}
	if (stage == 4 && min > 400 && gas > 150 && AgentManager::getInstance()->countNoFinishedUnits(UnitTypes::Protoss_Gateway) >= 3)
	{
		buildplan.push_back(BuildplanEntry(UnitTypes::Protoss_Robotics_Facility, cSupply));
		buildplan.push_back(BuildplanEntry(UnitTypes::Protoss_Observatory, cSupply));

		mainSquad->addSetup(UnitTypes::Protoss_Observer, 1);

		buildplan.push_back(BuildplanEntry(UpgradeTypes::Protoss_Ground_Weapons, cSupply));
		buildplan.push_back(BuildplanEntry(UpgradeTypes::Protoss_Plasma_Shields, cSupply));
		buildplan.push_back(BuildplanEntry(UpgradeTypes::Protoss_Ground_Weapons, cSupply));
		stage++;
	}
}
