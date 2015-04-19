#include "HatcheryAgent.h"
#include "../../Managers/AgentManager.h"
#include "../../Managers/Constructor.h"
#include "../../Commander/Commander.h"
#include "../../Managers/Upgrader.h"
#include "../../Managers/ResourceManager.h"

HatcheryAgent::HatcheryAgent(Unit mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	
	hasSentWorkers = false;
	if (AgentManager::getInstance()->countNoBases() == 0)
	{
		//We dont do this for the first base.
		hasSentWorkers = true;
	}
	if (mUnit->getType().getID() != UnitTypes::Zerg_Hatchery.getID())
	{
		//Upgrade. Dont send workers.
		hasSentWorkers = true;
	}
	
	agentType = "HatcheryAgent";
	Constructor::getInstance()->commandCenterBuilt();
}

void HatcheryAgent::computeActions()
{
	if (!hasSentWorkers)
	{
		if (!unit->isBeingConstructed())
		{
			sendWorkers();
			hasSentWorkers = true;
			Constructor::getInstance()->addRefinery();
		}
	}

	if (!unit->isIdle()) return;

	//Check for base upgrades
	if (isOfType(UnitTypes::Zerg_Hatchery) && AgentManager::getInstance()->countNoUnits(UnitTypes::Zerg_Lair) == 0)
	{
		if (Broodwar->canMake(UnitTypes::Zerg_Lair, unit))
		{
			if (ResourceManager::getInstance()->hasResources(UnitTypes::Zerg_Lair))
			{
				ResourceManager::getInstance()->lockResources(UnitTypes::Zerg_Lair);
				unit->morph(UnitTypes::Zerg_Lair);
				return;
			}
		}
	}
	if (isOfType(UnitTypes::Zerg_Lair) && AgentManager::getInstance()->countNoUnits(UnitTypes::Zerg_Hive) == 0)
	{
		if (Broodwar->canMake(UnitTypes::Zerg_Hive, unit))
		{
			if (ResourceManager::getInstance()->hasResources(UnitTypes::Zerg_Hive))
			{
				ResourceManager::getInstance()->lockResources(UnitTypes::Zerg_Hive);
				unit->morph(UnitTypes::Zerg_Hive);
				return;
			}
		}
	}
	
	//Build Overlords for supply
	bool buildOL = false;
	int totSupply = Broodwar->self()->supplyTotal() / 2;
	int cSupply = Broodwar->self()->supplyUsed() / 2;
	if (cSupply >= totSupply - 2)
	{
		if (Constructor::getInstance()->noInProduction(UnitTypes::Zerg_Overlord) == 0) buildOL = true;
	}
	if (buildOL)
	{
		if (canBuild(UnitTypes::Zerg_Overlord))
		{
			unit->train(UnitTypes::Zerg_Overlord);
			return;
		}
	}

	//Build units
	if (checkBuildUnit(UnitTypes::Zerg_Queen)) return;
	if (checkBuildUnit(UnitTypes::Zerg_Mutalisk)) return;
	if (checkBuildUnit(UnitTypes::Zerg_Hydralisk)) return;
	if (checkBuildUnit(UnitTypes::Zerg_Zergling)) return;
	if (checkBuildUnit(UnitTypes::Zerg_Defiler)) return;
	if (checkBuildUnit(UnitTypes::Zerg_Ultralisk)) return;
	if (checkBuildUnit(UnitTypes::Zerg_Scourge)) return;

	//Create workers
	if (AgentManager::getInstance()->countNoUnits(Broodwar->self()->getRace().getWorker()) < Commander::getInstance()->getNoWorkers())
	{
		UnitType worker = Broodwar->self()->getRace().getWorker();
		if (canBuild(worker))
		{
			unit->train(worker);
		}
	}
	
	//Check for upgrades
	Upgrader::getInstance()->checkUpgrade(this);
}

bool HatcheryAgent::checkBuildUnit(UnitType type)
{
	if (canEvolveUnit(type))
	{
		unit->train(type);
		return true;
	}
	return false;
}

