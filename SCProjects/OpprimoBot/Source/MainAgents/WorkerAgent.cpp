#include "WorkerAgent.h"
#include "../Managers/AgentManager.h"
#include "../Pathfinding/NavigationAgent.h"
#include "../Managers/BuildingPlacer.h"
#include "../Managers/Constructor.h"
#include "../Commander/Commander.h"
#include "../Managers/ResourceManager.h"

WorkerAgent::WorkerAgent(Unit mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	setState(GATHER_MINERALS);
	startBuildFrame = 0;
	startSpot = TilePosition(-1, -1);
	agentType = "WorkerAgent";

	toBuild = UnitTypes::None;
}

void WorkerAgent::destroyed()
{
	if (currentState == MOVE_TO_SPOT || currentState == CONSTRUCT || currentState == FIND_BUILDSPOT)
	{
		if (!Constructor::isZerg())
		{
			Constructor::getInstance()->handleWorkerDestroyed(toBuild, unitID);
			BuildingPlacer::getInstance()->clearTemp(toBuild, buildSpot);
			setState(GATHER_MINERALS);
		}
	}
}

void WorkerAgent::printInfo()
{
	int e = Broodwar->getFrameCount() - infoUpdateFrame;
	if (e >= infoUpdateTime || (sx == 0 && sy == 0))
	{
		infoUpdateFrame = Broodwar->getFrameCount();
		sx = unit->getPosition().x;
		sy = unit->getPosition().y;
	}

	Broodwar->drawBoxMap(sx-2,sy,sx+152,sy+90,Colors::Black,true);
	Broodwar->drawTextMap(sx+4,sy,"\x03%s", unit->getType().getName().c_str());
	Broodwar->drawLineMap(sx,sy+14,sx+150,sy+14,Colors::Blue);

	Broodwar->drawTextMap(sx+2,sy+15,"Id: \x11%d", unitID);
	Broodwar->drawTextMap(sx+2,sy+30,"Position: \x11(%d,%d)", unit->getTilePosition().x, unit->getTilePosition().y);
	Broodwar->drawTextMap(sx+2,sy+45,"Goal: \x11(%d,%d)", goal.x, goal.y);
	if (squadID == -1) Broodwar->drawTextMap(sx+2,sy+60,"Squad: \x15None");
	else Broodwar->drawTextMap(sx+2,sy+60,"Squad: \x11%d", squadID);
	Broodwar->drawTextMap(sx+2,sy+75,"State: \x11%s", getStateAsText().c_str());

	Broodwar->drawLineMap(sx,sy+89,sx+150,sy+89,Colors::Blue);
}

void WorkerAgent::debug_showGoal()
{
	if (!isAlive()) return;
	if (!unit->isCompleted()) return;
	
	if (currentState == GATHER_MINERALS || currentState == GATHER_GAS)
	{
		Unit target = unit->getTarget();
		if (target != NULL)
		{
			Position a = unit->getPosition();
			Position b = target->getPosition();
			Broodwar->drawLineMap(a.x,a.y,b.x,b.y,Colors::Teal);
		}
	}

	if (currentState == MOVE_TO_SPOT || currentState == CONSTRUCT)
	{
		if (buildSpot.x > 0)
		{
			int w = toBuild.tileWidth() * 32;
			int h = toBuild.tileHeight() * 32;

			Position a = unit->getPosition();
			Position b = Position(buildSpot.x*32 + w/2, buildSpot.y*32 + h/2);
			Broodwar->drawLineMap(a.x,a.y,b.x,b.y,Colors::Teal);

			Broodwar->drawBoxMap(buildSpot.x*32,buildSpot.y*32,buildSpot.x*32+w,buildSpot.y*32+h,Colors::Blue,false);
		}
	}

	if (unit->isRepairing())
	{
		Unit targ = unit->getOrderTarget();
		if (targ != NULL)
		{
			Position a = unit->getPosition();
			Position b = targ->getPosition();
			Broodwar->drawLineMap(a.x,a.y,b.x,b.y,Colors::Green);

			Broodwar->drawTextMap(unit->getPosition().x, unit->getPosition().y, "Repairing %s", targ->getType().getName().c_str());
		}
	}

	if (unit->isConstructing())
	{
		Unit targ = unit->getOrderTarget();
		if (targ != NULL)
		{
			Position a = unit->getPosition();
			Position b = targ->getPosition();
			Broodwar->drawLineMap(a.x,a.y,b.x,b.y,Colors::Green);

			Broodwar->drawTextMap(unit->getPosition().x, unit->getPosition().y, "Constructing %s", targ->getType().getName().c_str());
		}
	}
}

bool WorkerAgent::checkRepair()
{
	if (unit->getType().getID() != UnitTypes::Terran_SCV.getID()) return false;
	if (unit->isRepairing()) return true;

	//Find closest unit that needs repairing
	BaseAgent* toRepair = NULL;
	double bestDist = 10000;
	Agentset agents = AgentManager::getInstance()->getAgents();
	for (auto &a : agents)
	{
		if (a->isAlive() && a->isDamaged() && a->getUnitType().isMechanical() && a->getUnitID() != unitID)
		{
			double cDist = a->getUnit()->getPosition().getDistance(unit->getPosition());
			if (cDist < bestDist)
			{
				bestDist = cDist;
				toRepair = a;
			}
		}
	}

	//Repair it
	if (toRepair != NULL)
	{
		unit->repair(toRepair->getUnit());
		return true;
	}

	return false;
}

void WorkerAgent::computeSquadWorkerActions()
{
	//Repairing
	if (checkRepair()) return;

	//No repairing. Gather minerals
	Squad* sq = Commander::getInstance()->getSquad(squadID);
	if (sq != NULL)
	{
		//If squad is not ative, let the worker gather
		//minerals while not doing any repairs
		if (!sq->isActive())
		{
			if (unit->isIdle())
			{
				Unit mineral = BuildingPlacer::getInstance()->findClosestMineral(unit->getTilePosition());
				if (mineral != NULL)
				{
					unit->rightClick(mineral);
					return;
				}
			}
		}
		else
		{
			NavigationAgent::getInstance()->computeMove(this, goal);
			return;
		}
	}	
}

bool WorkerAgent::isFreeWorker()
{
	if (currentState != GATHER_MINERALS) return false;
	if (toBuild.getID() != UnitTypes::None.getID()) return false;
	if (unit->isConstructing()) return false;
	Unit b = unit->getTarget();
	if (b != NULL) if (b->isBeingConstructed()) return false;
	if (unit->isRepairing()) return false;
	if (squadID != -1) return false;
	
	return true;
}


void WorkerAgent::computeActions()
{
	//To prevent order spamming
	lastOrderFrame = Broodwar->getFrameCount();

	if (squadID != -1)
	{
		computeSquadWorkerActions();
		return;
	}
	//Check if workers are too far away from a base when attacking
	if (currentState == ATTACKING)
	{
		if (unit->getTarget() != NULL)
		{
			BaseAgent* base = AgentManager::getInstance()->getClosestBase(unit->getTilePosition());
			if (base != NULL)
			{
				double dist = base->getUnit()->getTilePosition().getDistance(unit->getTilePosition());
				if (dist > 25)
				{
					//Stop attacking. Return home
					unit->stop();
					unit->rightClick(base->getUnit());
					setState(GATHER_MINERALS);
					return;
				}
			}
		}
		else
		{
			//No target, return to gather minerals
			setState(GATHER_MINERALS);
			return;
		}
	}

	if (currentState == GATHER_GAS)
	{
		if (unit->isIdle())
		{
			//Not gathering gas. Reset.
			setState(GATHER_MINERALS);
		}
	}
	
	if (currentState == REPAIRING)
	{
		Unit target = unit->getTarget();
		bool cont = true;
		if (target == NULL) cont = false;
		if (target != NULL && target->getHitPoints() >= target->getInitialHitPoints()) cont = false;
		
		if (!cont)
		{
			reset();
		}
		return;
	}

	if (currentState == GATHER_MINERALS)
	{
		if (unit->isIdle())
		{
			Unit mineral = BuildingPlacer::getInstance()->findClosestMineral(unit->getTilePosition());
			if (mineral != NULL)
			{
				unit->rightClick(mineral);
			}
		}
	}

	if (currentState == FIND_BUILDSPOT)
	{
		if (buildSpot.x == -1)
		{
			buildSpot = BuildingPlacer::getInstance()->findBuildSpot(toBuild);
		}
		if (buildSpot.x >= 0)
		{
			setState(MOVE_TO_SPOT);
			startBuildFrame = Broodwar->getFrameCount();
			if (toBuild.isResourceDepot())
			{
				Commander::getInstance()->updateGoals();
			}
		}
	}

	if (currentState == MOVE_TO_SPOT)
	{
		if (!buildSpotExplored())
		{
			Position toMove = Position(buildSpot.x*32 + 16, buildSpot.y*32 + 16);
			if (toBuild.isRefinery()) toMove = Position(buildSpot);
			unit->rightClick(toMove);
		}

		if (buildSpotExplored() && !unit->isConstructing())
		{
			bool ok = unit->build(toBuild, buildSpot);
			if (!ok)
			{
				BuildingPlacer::getInstance()->blockPosition(buildSpot);
				BuildingPlacer::getInstance()->clearTemp(toBuild, buildSpot);
				//Cant build at selected spot, get a new one.
				setState(FIND_BUILDSPOT);
			}
		}

		if (unit->isConstructing())
		{
			setState(CONSTRUCT);
			startSpot = TilePosition(-1, -1);
		}
	}

	if (currentState == CONSTRUCT)
	{
		if (isBuilt())
		{
			//Build finished.
			BaseAgent* agent = AgentManager::getInstance()->getClosestBase(unit->getTilePosition());
			if (agent != NULL)
			{
				unit->rightClick(agent->getUnit()->getPosition());
			}
			setState(GATHER_MINERALS);
		}
	}
}

bool WorkerAgent::isBuilt()
{
	if (unit->isConstructing()) return false;

	Unit b = unit->getTarget();
	if (b != NULL) if (b->isBeingConstructed()) return false;

	return true;
}

bool WorkerAgent::buildSpotExplored()
{
	int sightDist = 64;
	if (toBuild.isRefinery())
	{
		sightDist = 160; //5 tiles
	}

	double dist = unit->getPosition().getDistance(Position(buildSpot));
	if (dist > sightDist)
	{
		return false;
	}
	return true;
}

int WorkerAgent::getState()
{
	return currentState;
}

void WorkerAgent::setState(int state)
{
	currentState = state;
	
	if (state == GATHER_MINERALS)
	{
		startSpot = TilePosition(-1, -1);
		buildSpot = TilePosition(-1, -1);
		toBuild = UnitTypes::None;
	}
}

bool WorkerAgent::canBuild(UnitType type)
{
	if (unit->isIdle())
	{
		return true;
	}
	if (unit->isGatheringMinerals())
	{
		return true;
	}
	return false;
}

bool WorkerAgent::assignToBuild(UnitType type)
{
	toBuild = type;
	buildSpot = BuildingPlacer::getInstance()->findBuildSpot(toBuild);
	if (buildSpot.x >= 0)
	{
		ResourceManager::getInstance()->lockResources(toBuild);
		BuildingPlacer::getInstance()->fillTemp(toBuild, buildSpot);
		setState(FIND_BUILDSPOT);
		return true;
	}
	else
	{
		startSpot = TilePosition(-1, -1);
		return false;
	}
}

void WorkerAgent::reset()
{
	if (currentState == MOVE_TO_SPOT)
	{
		//The buildSpot is probably not reachable. Block it.	
		BuildingPlacer::getInstance()->blockPosition(buildSpot);
		BuildingPlacer::getInstance()->clearTemp(toBuild, buildSpot);
	}

	if (unit->isConstructing())
	{
		unit->cancelConstruction();
		BuildingPlacer::getInstance()->clearTemp(toBuild, buildSpot);
	}
	
	if (unit->isRepairing())
	{
		unit->cancelConstruction();
	}

	setState(GATHER_MINERALS);
	unit->stop();
	BaseAgent* base = AgentManager::getInstance()->getClosestBase(unit->getTilePosition());
	if (base != NULL)
	{
		unit->rightClick(base->getUnit()->getPosition());
	}
}

bool WorkerAgent::isConstructing(UnitType type)
{
	if (currentState == FIND_BUILDSPOT || currentState == MOVE_TO_SPOT || currentState == CONSTRUCT)
	{
		if (toBuild.getID() == type.getID())
		{
			return true;
		}
	}
	return false;
}

/** Returns the state of the agent as text. Good for printouts. */
string WorkerAgent::getStateAsText()
{
	string strReturn = "";
	switch(currentState)
	{
	case GATHER_MINERALS:
		strReturn = "GATHER_MINERALS";
		break;
	case GATHER_GAS:
		strReturn = "GATHER_GAS";
		break;
	case FIND_BUILDSPOT:
		strReturn = "FIND_BUILDSPOT";
		break;
	case MOVE_TO_SPOT:
		strReturn = "MOVE_TO_SPOT";
		break;
	case CONSTRUCT:
		strReturn = "CONSTRUCT";
		break;
	case REPAIRING:
		strReturn = "REPAIRING";
		break;
	};
	return strReturn;
}

bool WorkerAgent::assignToFinishBuild(Unit building)
{
	if (isFreeWorker())
	{
		setState(REPAIRING);
		unit->rightClick(building);
		return true;
	}
	return false;
}

bool WorkerAgent::assignToRepair(Unit building)
{
	if (isFreeWorker())
	{
		setState(REPAIRING);
		bool ok = unit->repair(building);
		unit->rightClick(building);
		return true;
	}
	return false;
}