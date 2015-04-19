#include "ComsatAgent.h"
#include "../../Managers/AgentManager.h"
#include "../../Influencemap/MapManager.h"
#include "../../Commander/Commander.h"

ComsatAgent::ComsatAgent(Unit mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "ComsatAgent";
	lastSweepPos = TilePosition(-1, -1);
	lastSweepFrame = 0;
}

void ComsatAgent::computeActions()
{
	if (!unit->isIdle()) return;

	if (Broodwar->getFrameCount() - lastSweepFrame > 100 && unit->getEnergy() >= 50)
	{
		for (auto &u : Broodwar->enemy()->getUnits())
		{
			//Enemy seen
			if (u->exists())
			{
				if ((u->isCloaked() || u->isBurrowed()) && !u->isDetected() && u->getType().getID() != UnitTypes::Protoss_Observer.getID())
				{
					if (friendlyUnitsWithinRange(u->getPosition()) > 0 && !anyHasSweeped(u->getTilePosition()))
					{
						Broodwar << "Use Scanner Sweep at (" << u->getTilePosition().x << "," << u->getTilePosition().y << ") " << u->getType().getName() << " detected" << endl;
						bool ok = unit->useTech(TechTypes::Scanner_Sweep, u->getPosition());
						if (ok)
						{
							lastSweepFrame = Broodwar->getFrameCount();
							lastSweepPos = u->getTilePosition();
							return;
						}
					}
				}
			}
		}

		//Uncomment if you want the Comsat to scan for enemy location.
		/*if (Commander::getInstance()->isAttacking())
		{
			TilePosition pos = MapManager::getInstance()->findAttackPosition();
			if (pos.x == -1)
			{
				//No attack position found. Sweep a base area
				for (BWTA::BaseLocation* r : BWTA::getBaseLocations())
				{
					if (!anyHasSweeped(r->getTilePosition()) && !Broodwar->isVisible(r->getTilePosition()))
					{
						bool ok = unit->useTech(TechTypes::Scanner_Sweep, r->getPosition());
						if (ok)
						{
							Broodwar << "Use Scanner Sweep to find enemy at (" << r->getTilePosition().x << "," << r->getTilePosition().y << ")" << endl;
							lastSweepFrame = Broodwar->getFrameCount();
							lastSweepPos = r->getTilePosition();
							return;
						}
					}
				}
			}
		}*/
	}
}

int ComsatAgent::friendlyUnitsWithinRange(Position pos)
{
	int fCnt = 0;
	double maxRange = 384; //Range of sieged tanks
	Agentset agents = AgentManager::getInstance()->getAgents();
	for (auto &a : agents)
	{
		if (a->isUnit() && a->isAlive() && a->getUnitType().canAttack())
		{
			double dist = a->getUnit()->getPosition().getDistance(pos);
			if (dist <= maxRange)
			{
				fCnt++;
			}
		}
	}
	return fCnt;
}

bool ComsatAgent::anyHasSweeped(TilePosition pos)
{
	Agentset agents = AgentManager::getInstance()->getAgents();
	for (auto &a : agents)
	{
		if (a->isAlive() && a->getUnitType().getID() == UnitTypes::Terran_Comsat_Station.getID())
		{
			ComsatAgent* ca = (ComsatAgent*)a;
			if (ca->hasSweeped(pos))
			{
				return true;
			}
		}
	}
	return false;
}

bool ComsatAgent::hasSweeped(TilePosition pos)
{
	if (Broodwar->getFrameCount() - lastSweepFrame > 100)
	{
		return false;
	}
	
	if (pos.getDistance(lastSweepPos) <= 5)
	{
		return true;
	}
	
	return false;
}
