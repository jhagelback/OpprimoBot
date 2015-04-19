#include "CommandCenterAgent.h"
#include "../../Commander/Commander.h"
#include "../../Managers/AgentManager.h"
#include "../../Managers/Constructor.h"

CommandCenterAgent::CommandCenterAgent(Unit mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	
	hasSentWorkers = false;
	if (AgentManager::getInstance()->countNoUnits(UnitTypes::Terran_Command_Center) == 0)
	{
		//We dont do this for the first Command Center.
		hasSentWorkers = true;
	}

	agentType = "CommandCenterAgent";
	Constructor::getInstance()->commandCenterBuilt();
}

void CommandCenterAgent::computeActions()
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

	//Build comsat addon
	if (unit->getAddon() == NULL)
	{
		if (Broodwar->canMake(UnitTypes::Terran_Comsat_Station, unit))
		{
			unit->buildAddon(UnitTypes::Terran_Comsat_Station);
			return;
		}
	}

	if (AgentManager::getInstance()->countNoUnits(Broodwar->self()->getRace().getWorker()) < Commander::getInstance()->getNoWorkers())
	{
		UnitType worker = Broodwar->self()->getRace().getWorker();
		if (canBuild(worker))
		{
			unit->train(worker);
		}
	}

	if (Commander::getInstance()->needUnit(UnitTypes::Terran_SCV))
	{
		if (canBuild(UnitTypes::Terran_SCV))
		{
			unit->train(UnitTypes::Terran_SCV);
		}
	}
}
