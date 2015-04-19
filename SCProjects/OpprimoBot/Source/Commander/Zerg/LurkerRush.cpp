#include "LurkerRush.h"
#include "../../Managers/BuildplanEntry.h"
#include "../../Managers/AgentManager.h"
#include "../RushSquad.h"
#include "../ExplorationSquad.h"
#include "../../Managers/ExplorationManager.h"

LurkerRush::LurkerRush()
{
	buildplan.push_back(BuildplanEntry(UnitTypes::Zerg_Spawning_Pool, 5));
	buildplan.push_back(BuildplanEntry(UnitTypes::Zerg_Extractor, 5));
	buildplan.push_back(BuildplanEntry(UnitTypes::Zerg_Hydralisk_Den, 8));
	buildplan.push_back(BuildplanEntry(TechTypes::Lurker_Aspect, 13));
	
	mainSquad = new Squad(1, Squad::OFFENSIVE, "MainSquad", 10);
	mainSquad->setRequired(true);
	mainSquad->setBuildup(true);
	squads.push_back(mainSquad);
	
	l1 = new RushSquad(2, "LurkerSquad", 8);
	l1->addSetup(UnitTypes::Zerg_Hydralisk, 4);
	l1->setBuildup(false);
	l1->setRequired(false);
	l1->setActivePriority(11);
	l1->setMorphsTo(UnitTypes::Zerg_Lurker);
	squads.push_back(l1);

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

LurkerRush::~LurkerRush()
{
	for (Squad* s : squads)
	{
		delete s;
	}
	instance = NULL;
}

void LurkerRush::computeActions()
{
	computeActionsBase();

	noWorkers = AgentManager::getInstance()->countNoBases() * 6 + AgentManager::getInstance()->countNoUnits(UnitTypes::Zerg_Extractor) * 3;

	int cSupply = Broodwar->self()->supplyUsed() / 2;

	if (stage == 0 && AgentManager::getInstance()->countNoFinishedUnits(UnitTypes::Zerg_Lair) > 0)
	{
		//Check if we have spotted any enemy buildings. If not, send
		//out two Zerglings to rush base locations. Only needed for
		//2+ player maps.
		//This is needed to find out where the enemy is before we
		//send out the Lurkers.
		TilePosition tp = ExplorationManager::getInstance()->getClosestSpottedBuilding(Broodwar->self()->getStartLocation());
		if (tp.x == -1 && Broodwar->getStartLocations().size() > 2)
		{
			squads.push_back(sc2);
		}
		stage++;
	}
	if (stage == 1 && AgentManager::getInstance()->countNoFinishedUnits(UnitTypes::Zerg_Lurker) > 0)
	{
		buildplan.push_back(BuildplanEntry(UnitTypes::Zerg_Spire, cSupply));
		buildplan.push_back(BuildplanEntry(UnitTypes::Zerg_Hatchery, cSupply));

		mainSquad->addSetup(UnitTypes::Zerg_Mutalisk, 20);
		mainSquad->setBuildup(false);
	
		stage++;
	}
	if (stage == 2 && AgentManager::getInstance()->countNoBases() > 1)
	{
		buildplan.push_back(BuildplanEntry(UnitTypes::Zerg_Creep_Colony, cSupply));
		buildplan.push_back(BuildplanEntry(UnitTypes::Zerg_Creep_Colony, cSupply));

		stage++;
	}
	if (stage == 3 && AgentManager::getInstance()->countNoFinishedUnits(UnitTypes::Zerg_Sunken_Colony) > 0)
	{		
		buildplan.push_back(BuildplanEntry(UnitTypes::Zerg_Hatchery, cSupply));
		
		stage++;
	}
}
