#include "NexusAgent.h"
#include "../../Managers/AgentManager.h"
#include "../../Managers/Constructor.h"
#include "../../Commander/Commander.h"

NexusAgent::NexusAgent(Unit mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "NexusAgent";
	
	hasSentWorkers = false;
	if (AgentManager::getInstance()->countNoUnits(UnitTypes::Protoss_Nexus) == 0)
	{
		//We dont do this for the first Nexus.
		hasSentWorkers = true;
	}
	
	Constructor::getInstance()->commandCenterBuilt();
}

void NexusAgent::computeActions()
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

	if (AgentManager::getInstance()->countNoUnits(Broodwar->self()->getRace().getWorker()) < Commander::getInstance()->getNoWorkers())
	{
		UnitType worker = Broodwar->self()->getRace().getWorker();
		if (canBuild(worker))
		{
			unit->train(worker);
		}
	}
}
