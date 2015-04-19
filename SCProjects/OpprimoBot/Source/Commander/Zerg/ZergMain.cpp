#include "ZergMain.h"
#include "../../Managers/BuildplanEntry.h"
#include "../../Managers/AgentManager.h"
#include "../RushSquad.h"
#include "../ExplorationSquad.h"
#include "../../Managers/ExplorationManager.h"

ZergMain::ZergMain()
{
	buildplan.push_back(BuildplanEntry(UnitTypes::Zerg_Spawning_Pool, 5));
	buildplan.push_back(BuildplanEntry(UnitTypes::Zerg_Extractor, 5));
	buildplan.push_back(BuildplanEntry(UnitTypes::Zerg_Hydralisk_Den, 8));
	buildplan.push_back(BuildplanEntry(UnitTypes::Zerg_Hatchery, 14));
	
	mainSquad = new Squad(1, Squad::OFFENSIVE, "MainSquad", 10);
	mainSquad->addSetup(UnitTypes::Zerg_Zergling, 16);
	mainSquad->addSetup(UnitTypes::Zerg_Hydralisk, 10);
	mainSquad->setRequired(true);
	mainSquad->setBuildup(true);
	squads.push_back(mainSquad);
	
	sc1 = new ExplorationSquad(4, "ScoutingSquad", 8);
	sc1->addSetup(UnitTypes::Zerg_Overlord, 1);
	sc1->setRequired(false);
	sc1->setBuildup(false);
	sc1->setActivePriority(10);
	squads.push_back(sc1);

	sc2 = new RushSquad(5, "ScoutingSquad", 7);
	sc2->addSetup(UnitTypes::Zerg_Zergling, 2);
	sc2->setRequired(false);
	sc2->setBuildup(false);
	sc2->setActivePriority(1000);

	noWorkers = 8;
	noWorkersPerRefinery = 3;
}

ZergMain::~ZergMain()
{
	for (Squad* s : squads)
	{
		delete s;
	}
	instance = NULL;
}

void ZergMain::computeActions()
{
	computeActionsBase();

	noWorkers = AgentManager::getInstance()->countNoBases() * 6 + AgentManager::getInstance()->countNoUnits(UnitTypes::Zerg_Extractor) * 3;

	int cSupply = Broodwar->self()->supplyUsed() / 2;
	int min = Broodwar->self()->minerals();
	int gas = Broodwar->self()->gas();

	if (stage == 0 && AgentManager::getInstance()->countNoFinishedUnits(UnitTypes::Zerg_Lair) > 0)
	{
		buildplan.push_back(BuildplanEntry(UnitTypes::Zerg_Spire, cSupply));
		buildplan.push_back(BuildplanEntry(UnitTypes::Zerg_Creep_Colony, cSupply));
		buildplan.push_back(BuildplanEntry(UnitTypes::Zerg_Creep_Colony, cSupply));

		stage++;
	}
	if (stage == 1 && AgentManager::getInstance()->countNoFinishedUnits(UnitTypes::Zerg_Spire) > 0)
	{
		mainSquad->addSetup(UnitTypes::Zerg_Hydralisk, 14);
		mainSquad->addSetup(UnitTypes::Zerg_Mutalisk, 16);
		mainSquad->setBuildup(false);

		buildplan.push_back(BuildplanEntry(UnitTypes::Zerg_Queens_Nest, cSupply));
		
		stage++;
	}
	if (stage == 2 && min > 450)
	{
		buildplan.push_back(BuildplanEntry(UnitTypes::Zerg_Hatchery, cSupply));
	
		stage++;
	}
	if (stage == 3 && AgentManager::getInstance()->countNoFinishedUnits(UnitTypes::Zerg_Hive) > 0)
	{
		buildplan.push_back(BuildplanEntry(UnitTypes::Zerg_Defiler_Mound, cSupply));
		
		stage++;
	}
	if (stage == 4 && AgentManager::getInstance()->countNoFinishedUnits(UnitTypes::Zerg_Defiler_Mound) > 0)
	{
		mainSquad->addSetup(UnitTypes::Zerg_Defiler, 4);

		stage++;
	}
}
