#include "RushSquad.h"
#include "../Managers/AgentManager.h"
#include "../Managers/ExplorationManager.h"
#include "Commander.h"

RushSquad::RushSquad(int mId, string mName, int mPriority)
{
	this->id = mId;
	this->type = RUSH;
	this->moveType = AIR;
	this->name = mName;
	this->priority = mPriority;
	activePriority = priority;
	active = false;
	required = false;
	goal = Broodwar->self()->getStartLocation();
	goalSetFrame = 0;
	currentState = STATE_NOT_SET;
}

bool RushSquad::isActive()
{
	return active;
}

void RushSquad::defend(TilePosition mGoal)
{
	if (!active)
	{
		setGoal(mGoal);
	}
}

void RushSquad::attack(TilePosition mGoal)
{

}

void RushSquad::assist(TilePosition mGoal)
{
	if (!isUnderAttack())
	{
		currentState = STATE_ASSIST;
		setGoal(mGoal);
	}
}

void RushSquad::computeActions()
{
	if (!active)
	{
		//Check if we need workers in the squad
		for (int i = 0; i < (int)setup.size(); i++)
		{
			if (setup.at(i).current < setup.at(i).no && setup.at(i).type.isWorker())
			{
				int no =  setup.at(i).no - setup.at(i).current;
				for (int j = 0; j < no; j++)
				{
					BaseAgent* w = AgentManager::getInstance()->findClosestFreeWorker(Broodwar->self()->getStartLocation());
					if (w != NULL) addMember(w);
				}
			}
		}

		if (isFull())
		{
			active = true;
		}

		TilePosition defSpot = Commander::getInstance()->findChokePoint();
		if (defSpot.x != -1)
		{
			goal = defSpot;
		}
		return;
	}

	//First, remove dead agents
	removeDestroyed();

	if (active)
	{
		if (activePriority != priority)
		{
			priority = activePriority;
		}

		Unit target = findWorkerTarget();
		if (target != NULL)
		{
			for (auto &a : agents)
			{
				if (a->isAlive())
				{
					a->getUnit()->attack(target);
				}
			}
		}

		TilePosition ePos = ExplorationManager::getInstance()->getClosestSpottedBuilding(Broodwar->self()->getStartLocation());
		if (ePos.x != -1)
		{
			goal = ePos;
			setMemberGoals(goal);
		}
	}
}

Unit RushSquad::findWorkerTarget()
{
	try {
		double maxRange = 12 * 32;

		for (auto &a : agents)
		{
			for (auto &u : Broodwar->enemy()->getUnits())
			{
				if (u->exists())
				{
					if (u->getType().isWorker())
					{
						double dist = a->getUnit()->getDistance(u);
						if (dist <= maxRange)
						{
							return u;
						}
					}	
				}
			}
		}
	}
	catch (exception)
	{

	}

	return NULL;
}

void RushSquad::clearGoal()
{
	goal = TilePosition(-1, -1);
}

TilePosition RushSquad::getGoal()
{
	return goal;
}

bool RushSquad::hasGoal()
{
	if (goal.x < 0 || goal.y < 0)
	{
		return false;
	}
	return true;
}
