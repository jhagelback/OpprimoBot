#include "BaseAgent.h"
#include "../Managers/Constructor.h"
#include "../Managers/AgentManager.h"
#include "../Managers/ResourceManager.h"
#include "../Managers/ExplorationManager.h"

BaseAgent::BaseAgent()
{
	alive = true;
	squadID = -1;
	type = UnitTypes::Unknown;
	goal = TilePosition(-1, -1);

	infoUpdateFrame = 0;
	infoUpdateTime = 20;
	sx = 0;
	sy = 0;

	lastOrderFrame = 0;
}

BaseAgent::BaseAgent(Unit mUnit)
{
	unit = mUnit;
	unitID = unit->getID();
	type = unit->getType();
	alive = true;
	squadID = -1;
	goal = TilePosition(-1, -1);
	agentType = "BaseAgent";

	lastOrderFrame = 0;
}

BaseAgent::~BaseAgent()
{
	
}

int BaseAgent::getLastOrderFrame()
{
	return lastOrderFrame;
}

string BaseAgent::getTypeName()
{
	return agentType;
}

void BaseAgent::printInfo()
{

}

int BaseAgent::getUnitID()
{
	return unitID;
}

string BaseAgent::format(string str)
{
	string res = str;

	int i = str.find("_");
	if (i >= 0)
	{
		res = str.substr(i + 1, str.length());
	}

	if (res == "Siege Tank Tank Mode") res = "Siege Tank";
	if (res == "Siege Tank Siege Mode") res = "Siege Tank (sieged)";

	return res;
}

UnitType BaseAgent::getUnitType()
{
	return type;
}

Unit BaseAgent::getUnit()
{
	return unit;
}

bool BaseAgent::matches(Unit mUnit)
{
	if (mUnit->getID() == unitID)
	{
		return true;
	}
	return false;
}

bool BaseAgent::isOfType(UnitType type)
{
	if (unit->getType().getID() == type.getID())
	{
		return true;
	}
	return false;
}

bool BaseAgent::isOfType(UnitType mType, UnitType toCheckType)
{
	if (mType.getID() == toCheckType.getID())
	{
		return true;
	}
	return false;
}

bool BaseAgent::isBuilding()
{
	if (unit->getType().isBuilding())
	{
		return true;
	}
	return false;
}

bool BaseAgent::isWorker()
{
	if (unit->getType().isWorker())
	{
		return true;
	}
	return false;
}

bool BaseAgent::isUnit()
{
	if (unit->getType().isBuilding() || unit->getType().isWorker() || unit->getType().isAddon())
	{
		return false;
	}
	return true;
}

bool BaseAgent::isUnderAttack()
{
	if (unit == NULL) return false;
	if (!unit->exists()) return false;

	if (unit->isAttacking()) return true;
	if (unit->isStartingAttack()) return true;

	double r = unit->getType().seekRange();
	if (unit->getType().sightRange() > r)
	{
		r = unit->getType().sightRange();
	}

	for (auto &u : Broodwar->enemy()->getUnits())
	{
		double dist = unit->getPosition().getDistance(u->getPosition());
		if (dist <= r)
		{
			return true;
		}
	}

	return false;
}

void BaseAgent::destroyed()
{
	alive = false;
}

bool BaseAgent::isAlive()
{
	if (!unit->exists())
	{
		return false;
	}
	return alive;
}

bool BaseAgent::isDamaged()
{
	if (unit->isBeingConstructed()) return false;
	if (unit->getRemainingBuildTime() > 0) return false;

	if (unit->getHitPoints() < unit->getType().maxHitPoints())
	{
		return true;
	}
	return false;
}

bool BaseAgent::isDetectorWithinRange(TilePosition pos, int range)
{
	for (auto &u : Broodwar->enemy()->getUnits())
	{
		if (u->getType().isDetector())
		{
			double dist = u->getDistance(Position(pos));
			if (dist <= range)
			{
				return true;
			}
		}
	}
	return false;
}

void BaseAgent::setSquadID(int id)
{
	squadID = id;
}

int BaseAgent::getSquadID()
{
	return squadID;
}

bool BaseAgent::enemyUnitsVisible()
{
	double r = unit->getType().sightRange();
	
	for (auto &u : Broodwar->enemy()->getUnits())
	{
		double dist = unit->getPosition().getDistance(u->getPosition());
		if (dist <= r)
		{
			return true;
		}
	}
	return false;
}

void BaseAgent::setGoal(TilePosition goal)
{
	if (unit->getType().isFlyer() || unit->getType().isFlyingBuilding())
	{
		//Flyers, can always move to goals.
		this->goal = goal;
	}
	else
	{
		//Ground units, check if we can reach goal.
		if (ExplorationManager::canReach(this, goal))
		{
			this->goal = goal;
		}
	}
}

void BaseAgent::clearGoal()
{
	goal = TilePosition(-1, -1);
}

TilePosition BaseAgent::getGoal()
{
	return goal;
}

void BaseAgent::addTrailPosition(WalkPosition wt)
{
	//Check if position already is in trail
	if (trail.size() > 0)
	{
		WalkPosition lwt = trail.at(trail.size() - 1);
		if (lwt.x == wt.x && lwt.y == wt.y) return;
	}

	trail.push_back(wt);
	if (trail.size() > 20)
	{
		trail.erase(trail.begin());
	}
}

vector<WalkPosition> BaseAgent::getTrail()
{
	return trail;
}

