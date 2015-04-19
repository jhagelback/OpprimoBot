#include "AgentManager.h"
#include "../MainAgents/AgentFactory.h"
#include "BuildingPlacer.h"
#include "../Commander/Commander.h"
#include "Constructor.h"
#include "ResourceManager.h"
#include "../MainAgents/WorkerAgent.h"
#include "../Utils/Profiler.h"
#include <windows.h>

#include "../Utils/Sets.h"

int AgentManager::StartFrame = 0;
AgentManager* AgentManager::instance = NULL;

AgentManager::AgentManager()
{
	lastCallFrame = Broodwar->getFrameCount();
}

AgentManager::~AgentManager()
{
	for (auto &a : agents)
	{
		delete a;
	}
	
	instance = NULL;
}

AgentManager* AgentManager::getInstance()
{
	if (instance == NULL)
	{
		instance = new AgentManager();
	}
	return instance;
}

const Agentset& AgentManager::getAgents()
{
	return agents;
}

BaseAgent* AgentManager::getAgent(int unitID)
{
	for (auto &a : agents)
	{
		if (a->getUnitID() == unitID)
		{
			return a;
		}
	}

	return NULL;
}

BaseAgent* AgentManager::getClosestBase(TilePosition pos)
{
	BaseAgent* agent = NULL;
	double bestDist = 100000;

	for (auto &a : agents)
	{
		if (a->getUnitType().isResourceDepot() && a->isAlive())
		{
			double dist = a->getUnit()->getDistance(Position(pos));
			if (dist < bestDist)
			{
				bestDist = dist;
				agent = a;
			}
		}
	}
	return agent;
}

BaseAgent* AgentManager::getClosestAgent(TilePosition pos, UnitType type)
{
	BaseAgent* agent = NULL;
	double bestDist = 100000;

	for (auto &a : agents)
	{
		if (a->isOfType(type) && a->isAlive())
		{
			double dist = a->getUnit()->getDistance(Position(pos));
			if (dist < bestDist)
			{
				bestDist = dist;
				agent = a;
			}
		}
	}
	return agent;
}

void AgentManager::addAgent(Unit unit)
{
	if (unit->getType().getID() == UnitTypes::Zerg_Larva.getID())
	{
		//Special case: Dont add Zerg larva as agents.
		return;
	}
	if (unit->getType().getID() == UnitTypes::Zerg_Egg.getID())
	{
		//Special case: Dont add Zerg eggs as agents.
		return;
	}
	if (unit->getType().getID() == UnitTypes::Zerg_Cocoon.getID())
	{
		//Special case: Dont add Zerg cocoons as agents.
		return;
	}
	if (unit->getType().getID() == UnitTypes::Zerg_Lurker_Egg.getID())
	{
		//Special case: Dont add Zerg eggs as agents.
		return;
	}
	
	bool found = false;
	for (auto &a : agents)
	{
		if (a->matches(unit))
		{
			found = true;
			break;
		}
	}

	if (!found)
	{
		BaseAgent* newAgent = AgentFactory::getInstance()->createAgent(unit);
		agents.insert(newAgent);
		
		if (newAgent->isBuilding())
		{
			BuildingPlacer::getInstance()->addConstructedBuilding(unit);
			Constructor::getInstance()->unlock(unit->getType());
			ResourceManager::getInstance()->unlockResources(unit->getType());
		}
		else
		{
			Commander::getInstance()->unitCreated(newAgent);
		}
	}
}

void AgentManager::removeAgent(Unit unit)
{
	for (auto &a : agents)
	{
		if (a->matches(unit))
		{
			if (a->isBuilding())
			{
				BuildingPlacer::getInstance()->buildingDestroyed(unit);
			}

			a->destroyed();
			Commander::getInstance()->unitDestroyed(a);

			//Special case: If a bunker is destroyed, we need to remove
			//the bunker squad.
			if (unit->getType().getID() == UnitTypes::Terran_Bunker.getID())
			{
				int squadID = a->getSquadID();
				Commander::getInstance()->removeSquad(squadID);
			}

			return;
		}
	}
}

void AgentManager::morphDrone(Unit unit)
{
	for (auto &a : agents)
	{
		if (a->matches(unit))
		{
			agents.erase(a);
			addAgent(unit);
			return;
		}
	}
	//No match found. Add it anyway.
	if (unit->exists())
	{
		addAgent(unit);
	}
}

void AgentManager::cleanup()
{
	for (auto &a : agents)
	{
		if (!a->isAlive())
		{
			agents.erase(a);
			//delete a;
			return cleanup();
		}
	}
}

void AgentManager::computeActions()
{
	//Start time
	LARGE_INTEGER li;
	QueryPerformanceFrequency(&li);    
    double PCFreq = double(li.QuadPart)/1000.0;
    QueryPerformanceCounter(&li);
    __int64 CounterStart = li.QuadPart;

	for (auto &a : agents)
	{
		if (a->isAlive() && Broodwar->getFrameCount() - a->getLastOrderFrame() > 30)
		{
			a->computeActions();
		}
		
		LARGE_INTEGER le;
		QueryPerformanceCounter(&le);
		double elapsed = (le.QuadPart-CounterStart)/PCFreq;
		if (elapsed >= 30.0) return;
	}
}

int AgentManager::getNoWorkers()
{
	int wCnt = 0;
	for (auto &a : agents)
	{
		if (a->isAlive() && a->isWorker())
		{
			wCnt++;
		}
	}
	return wCnt;
}

int AgentManager::noMiningWorkers()
{
	int cnt = 0;
	for (auto &a : agents)
	{
		if (a->isAlive() && a->isWorker())
		{
			WorkerAgent* w = (WorkerAgent*)a;
			if (w->getState() == WorkerAgent::GATHER_MINERALS)
			{
				cnt++;
			}
		}
	}
	return cnt;
}

BaseAgent* AgentManager::findClosestFreeWorker(TilePosition pos)
{
	BaseAgent* bestAgent = NULL;
	double bestDist = 10000;

	for (auto &a : agents)
	{
		if (a->isFreeWorker())
		{
			double cDist = a->getUnit()->getDistance(Position(pos));
			if (cDist < bestDist)
			{
				bestDist = cDist;
				bestAgent = a;
			}
		}
	}
	return bestAgent;
}

bool AgentManager::isAnyAgentRepairingThisAgent(BaseAgent* repairedAgent)
{
	for (auto &a : agents)
	{
		if (a->isAlive() && a->isWorker())
		{
			Unit unit = a->getUnit();
			if (unit->getTarget() != NULL && unit->getTarget()->getID() == repairedAgent->getUnitID())
			{
				//Already have an assigned builder
				return true;
			}
		}
	}
	return false;
}

int AgentManager::noInProduction(UnitType type)
{
	int cnt = 0;
	for (auto &a : agents)
	{
		if (a->isAlive())
		{
			if (a->isOfType(type) && a->getUnit()->isBeingConstructed())
			{
				cnt++;
			}
		}
	}
	return cnt;
}

bool AgentManager::hasBuilding(UnitType type)
{
	for (auto &a : agents)
	{
		if (a->isOfType(type) && a->isAlive())
		{
			if (!a->getUnit()->isBeingConstructed())
			{
				return true;
			}
		}
	}
	return false;
}

int AgentManager::countNoUnits(UnitType type)
{
	int cnt = 0;
	for (auto &a : agents)
	{
		if (a->isAlive())
		{
			if (a->isOfType(type) && a->isAlive())
			{
				cnt++;
			}
		}
	}
	return cnt;
}

int AgentManager::countNoFinishedUnits(UnitType type)
{
	int cnt = 0;
	for (auto &a : agents)
	{
		if (a->isAlive())
		{
			if (a->isOfType(type) &&a->isAlive() && !a->getUnit()->isBeingConstructed())
			{
				cnt++;
			}
		}
	}
	return cnt;
}

int AgentManager::countNoBases()
{
	int cnt = 0;
	for (auto &a : agents)
	{
		if (a->isAlive())
		{
			if (a->getUnitType().isResourceDepot() && !a->getUnit()->isBeingConstructed())
			{
				cnt++;
			}
		}
	}
	return cnt;
}

bool AgentManager::workerIsTargeting(Unit target)
{
	for (auto &a : agents)
	{
		if (a->isWorker() && a->isAlive())
		{
			Unit unit = a->getUnit();
			Unit unitTarget = a->getUnit()->getTarget();
			if (unitTarget != NULL && unitTarget->getID() == target->getID())
			{
				return true;
			}
		}
	}
	return false;
}
