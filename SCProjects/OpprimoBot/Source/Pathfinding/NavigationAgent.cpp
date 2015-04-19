#include "NavigationAgent.h"
#include "PFFunctions.h"
#include "../Managers/AgentManager.h"
#include "../Influencemap/MapManager.h"
#include "../Commander/Commander.h"
#include "../Utils/Profiler.h"
#include <math.h>

bool NavigationAgent::instanceFlag = false;
int NavigationAgent::pathfinding_version = 1;
NavigationAgent* NavigationAgent::instance = NULL;

NavigationAgent::NavigationAgent()
{
	checkRange = 5;
	mapW = Broodwar->mapWidth() * 4;
	mapH = Broodwar->mapHeight() * 4;
}

NavigationAgent::~NavigationAgent()
{
	instanceFlag = false;
	instance = NULL;
}

NavigationAgent* NavigationAgent::getInstance()
{
	if (!instanceFlag)
	{
		instance = new NavigationAgent();
		instanceFlag = true;
	}
	return instance;
}

bool NavigationAgent::computeMove(BaseAgent* agent, TilePosition goal)
{
	bool cmd = false;
	double r = agent->getUnitType().seekRange();
	if (agent->getUnitType().sightRange() > r)
	{
		r = agent->getUnitType().sightRange();
	}
	
	bool enemyInRange = false;
	for (auto &u : Broodwar->enemy()->getUnits())
	{
		double dist = agent->getUnit()->getPosition().getDistance(u->getPosition());
		if (dist <= r)
		{
			enemyInRange = true;
			break;
		}
	}

	//Retreat to center of the squad if the enemy
	//is overwhelming.
	if (agent->isUnderAttack() && (agent->getUnit()->isIdle() || agent->getUnit()->isInterruptible()))
	{
		int ownI = MapManager::getInstance()->getOwnGroundInfluenceIn(agent->getUnit()->getTilePosition());
		int enI = MapManager::getInstance()->getEnemyGroundInfluenceIn(agent->getUnit()->getTilePosition());
		if (enI > ownI)
		{
			//Broodwar << "Retreat from (" << agent->getUnit()->getTilePosition().x << "," << agent->getUnit()->getTilePosition().y << " " << ownI << "<" << enI << endl;
			Squad* sq = Commander::getInstance()->getSquad(agent->getSquadID());
			if (sq != NULL && !sq->isExplorer())
			{
				TilePosition center = sq->getCenter();
				if (center.getDistance(agent->getUnit()->getTilePosition()) >= 4 && !agent->getUnit()->isCloaked())
				{
					if (agent->getUnit()->isSieged())
					{
						agent->getUnit()->unsiege();
						return true;
					}
					if (agent->getUnit()->isBurrowed())
					{
						agent->getUnit()->unburrow();
						return true;
					}
					agent->getUnit()->rightClick(Position(center));
					return true;
				}
			}
		}
	}

	if (enemyInRange)
	{
		if (pathfinding_version == 0)
		{
			Profiler::getInstance()->start("NormMove");
		    cmd = computePathfindingMove(agent, goal);
		    Profiler::getInstance()->end("NormMove");
		}

		if (pathfinding_version == 1)
		{
			Profiler::getInstance()->start("BoidsMove");
			cmd = computeBoidsMove(agent);
			Profiler::getInstance()->end("BoidsMove");
		}

		if (pathfinding_version == 2)
		{
			Profiler::getInstance()->start("PFmove");
			computePotentialFieldMove(agent);
			Profiler::getInstance()->end("PFmove");
		}
	}
	else
	{
		Profiler::getInstance()->start("NormMove");
		cmd = computePathfindingMove(agent, goal);
		Profiler::getInstance()->end("NormMove");
	}
	return cmd;
}

int NavigationAgent::getMaxUnitSize(UnitType type)
{
	int size = type.dimensionDown();
	if (type.dimensionLeft() > size) size = type.dimensionLeft();
	if (type.dimensionRight() > size) size = type.dimensionRight();
	if (type.dimensionUp() > size) size = type.dimensionUp();
	return size;
}


bool NavigationAgent::computeBoidsMove(BaseAgent* agent)
{
	if (!agent->getUnit()->isIdle() && !agent->getUnit()->isMoving()) return false;

	Unit unit = agent->getUnit();
	if (unit->isSieged() || unit->isBurrowed() || unit->isLoaded())
	{
		return false;
	}

	//The difference from current position the agent
	//shall move to.
	double aDiffX = 0;
	double aDiffY = 0;

	//Apply goal
	if (agent->getGoal().x != -1 && agent->getGoal().y != -1)
	{
		Position goal = Position(agent->getGoal());
		double addX = ((double)goal.x - (double)agent->getUnit()->getPosition().x) / 100.0;
		double addY = ((double)goal.y - (double)agent->getUnit()->getPosition().y) / 100.0;
		
		aDiffX += addX;
		aDiffY += addY;
	}

	//Apply average position for the squad
	double totDX = 0;
	double totDY = 0;
	Squad* sq = Commander::getInstance()->getSquad(agent->getSquadID());
	if (sq != NULL)
	{
		Agentset agents = sq->getMembers();
		int no = 0;
		for (auto &a : agents)
		{
			if (a->isAlive() && a->getUnitID() != agent->getUnitID())
			{
				totDX += (double)a->getUnit()->getPosition().x;
				totDY += (double)a->getUnit()->getPosition().y;
				no++;
			}
		}

		totDX = totDX / (double)(no - 1);
		totDY = totDY / (double)(no - 1);

		double addX = (totDX - (double)agent->getUnit()->getPosition().x) / 100.0;
		double addY = (totDY - (double)agent->getUnit()->getPosition().y) / 100.0;
		
		aDiffX += addX;
		aDiffY += addY;
	}

	//Apply average heading for the squad
	totDX = 0;
	totDY = 0;
	if (sq != NULL)
	{
		Agentset agents = sq->getMembers();
		int no = 0;
		for (auto &a : agents)
		{
			if (a->isAlive() && a->getUnitID() != agent->getUnitID())
			{
				totDX += cos(a->getUnit()->getAngle());
				totDY += sin(a->getUnit()->getAngle());
				no++;
			}
		}

		totDX = totDX / (double)(no - 1);
		totDY = totDY / (double)(no - 1);

		double addX = (totDX - cos(agent->getUnit()->getAngle())) / 5.0;
		double addY = (totDY - sin(agent->getUnit()->getAngle())) / 5.0;
		
		aDiffX += addX;
		aDiffY += addY;
	}

	//Apply separation from own units. Does not apply for air units
	totDX = 0;
	totDY = 0;
	double detectionLimit = 10.0;
	
	if (sq != NULL && !agent->getUnitType().isFlyer())
	{
		Agentset agents = sq->getMembers();
		int cnt = 0;
		for (auto &a : agents)
		{
			//Set detection limit to be the radius of both units + 2
			detectionLimit = (double)(getMaxUnitSize(agent->getUnitType()) + getMaxUnitSize(a->getUnitType()) + 2);

			if (a->isAlive() && a->getUnitID() != agent->getUnitID())
			{
				double d = agent->getUnit()->getPosition().getDistance(a->getUnit()->getPosition());
				if (d <= detectionLimit)
				{
					totDX -= (a->getUnit()->getPosition().x - agent->getUnit()->getPosition().x);
					totDY -= (a->getUnit()->getPosition().y - agent->getUnit()->getPosition().y);
					cnt++;
				}
			}
		}
		if (cnt > 0)
		{
			double addX = totDX / 5.0;
			double addY = totDY / 5.0;

			aDiffX += addX;
			aDiffY += addY;
		}
	}

	//Apply separation from enemy units
	totDX = 0;
	totDY = 0;

	//Check if the agent targets ground and/or air
	//Check range of weapons
	bool targetsGround = false;
	int groundRange = 0;
	if (agent->getUnitType().groundWeapon().targetsGround())
	{
		targetsGround = true;
		if (agent->getUnitType().groundWeapon().maxRange() > groundRange)
		{
			groundRange = agent->getUnitType().groundWeapon().maxRange();
		}
	}
	if (agent->getUnitType().airWeapon().targetsGround())
	{
		targetsGround = true;
		if (agent->getUnitType().airWeapon().maxRange() > groundRange)
		{
			groundRange = agent->getUnitType().airWeapon().maxRange();
		}
	}

	bool targetsAir = false;
	int airRange = 0;
	if (agent->getUnitType().groundWeapon().targetsAir())
	{
		targetsAir = true;
		if (agent->getUnitType().groundWeapon().maxRange() > groundRange)
		{
			airRange = agent->getUnitType().groundWeapon().maxRange();
		}
	}
	if (agent->getUnitType().airWeapon().targetsAir())
	{
		targetsAir = true;
		if (agent->getUnitType().airWeapon().maxRange() > groundRange)
		{
			airRange = agent->getUnitType().airWeapon().maxRange();
		}
	}
	//Unit cannot attack
	if (!agent->getUnitType().canAttack())
	{
		groundRange = 6 * 32;
		airRange = 6 * 32;
	}

	//If agent is retreating from an enemy unit or not
	bool retreat = false;

	//Iterate through enemies
	for (auto &u : Broodwar->enemy()->getUnits())
	{
		int cnt = 0;
		if (u->exists())
		{
			UnitType t = u->getType();
			double detectionLimit = 100000;
			if (t.isFlyer() && targetsAir) detectionLimit = (double)airRange - getMaxUnitSize(agent->getUnitType())-2;
			if (!t.isFlyer() && targetsGround) detectionLimit = (double)groundRange - getMaxUnitSize(agent->getUnitType())-2;
			if (!agent->getUnitType().canAttack())
			{
				retreat = true;
				detectionLimit = t.sightRange();
			}
			if (unit->getGroundWeaponCooldown() >= 20 || unit->getAirWeaponCooldown() >= 20)
			{
				retreat = true;
				detectionLimit = t.sightRange();
			}
			if (detectionLimit < 5) detectionLimit = 5; //Minimum separation

			double d = agent->getUnit()->getPosition().getDistance(u->getPosition());
			if (d <= detectionLimit)
			{
				totDX -= (u->getPosition().x - agent->getUnit()->getPosition().x);
				totDY -= (u->getPosition().y - agent->getUnit()->getPosition().y);
				cnt++;
			}
		}
		if (cnt > 0)
		{
			double addX = totDX;
			double addY = totDY;

			aDiffX += addX;
			aDiffY += addY;
		}
	}

	//Apply separation from terrain
	totDX = 0;
	totDY = 0;
	int cnt = 0;
	WalkPosition unitWT = WalkPosition(agent->getUnit()->getPosition());
	detectionLimit = (double)(getMaxUnitSize(agent->getUnitType()) + 16);
	for (int tx = unitWT.x - 2; tx <= unitWT.x + 2; tx++)
	{
		for (int ty = unitWT.y - 2; ty <= unitWT.y + 2; ty++)
		{
			if (!Broodwar->isWalkable(tx, ty))
			{
				WalkPosition terrainWT = WalkPosition(tx, ty);
				WalkPosition uWT = WalkPosition(agent->getUnit()->getPosition());
				double d = terrainWT.getDistance(uWT);
				if (d <= detectionLimit)
				{
					Position tp = Position(terrainWT);
					totDX -= (tp.x - agent->getUnit()->getPosition().x);
					totDY -= (tp.y - agent->getUnit()->getPosition().y);
					cnt++;
				}
			}
		}
	}
	if (cnt > 0)
	{
		double addX = totDX / 10.0;
		double addY = totDY / 10.0;

		aDiffX += addX;
		aDiffY += addY;
	}

	//Update new position
	int newX = (int)(agent->getUnit()->getPosition().x + aDiffX);
	int newY = (int)(agent->getUnit()->getPosition().y + aDiffY);
	Position toMove = Position(newX, newY);

	if (agent->getUnit()->getPosition().getDistance(toMove) >= 1)
	{
		if (retreat)
		{
			return agent->getUnit()->rightClick(toMove);
		}
		else
		{
			return agent->getUnit()->attack(toMove);
		}
	}

	return false;
}


bool NavigationAgent::computePotentialFieldMove(BaseAgent* agent)
{
	if (!agent->getUnit()->isIdle() && !agent->getUnit()->isMoving()) return false;

	Unit unit = agent->getUnit();

	if (unit->isSieged() || unit->isBurrowed() || unit->isLoaded())
	{
		return false;
	}
	
	WalkPosition unitWT = WalkPosition(unit->getPosition());
	int wtX = unitWT.x;
	int wtY = unitWT.y;

	float bestP = getAttackingUnitP(agent, unitWT);
	//bestP += PFFunctions::getGoalP(Position(unitX,unitY), goal);
	//bestP += PFFunctions::getTrailP(agent, unitX, unitY);
	bestP += PFFunctions::getTerrainP(agent, unitWT);

	float cP = 0;
	
	float startP = bestP;
    int bestX = wtX;
    int bestY = wtY;

	for (int cX = wtX - checkRange; cX <= wtX + checkRange; cX++)
	{
        for (int cY = wtY - checkRange; cY <= wtY + checkRange; cY++)
		{
			if (cX >= 0 && cY >= 0 && cX <= mapW && cY <= mapH)
			{
				WalkPosition wt = WalkPosition(cX, cY);
				cP = getAttackingUnitP(agent, wt);
				//cP += PFFunctions::getGoalP(Position(cX,cY), goal);
				//cP += PFFunctions::getTrailP(agent, cX, cY);
				cP += PFFunctions::getTerrainP(agent, wt);
				
                if (cP > bestP)
				{
					bestP = cP;
					bestX = cX;
					bestY = cY;
				}
            }
        }
    }
	
	if (bestX != wtX || bestY != wtY)
	{
		WalkPosition wt = WalkPosition(bestX, bestY);
		Position toMove = Position(wt);

		return agent->getUnit()->attack(toMove);
    }

	return false;
}

bool NavigationAgent::computePathfindingMove(BaseAgent* agent, TilePosition goal)
{
	TilePosition checkpoint = goal;
	if (agent->getSquadID() >= 0)
	{
		Squad* sq = Commander::getInstance()->getSquad(agent->getSquadID());
		if (sq != NULL)
		{
			checkpoint = sq->nextMovePosition();
			if (agent->isOfType(UnitTypes::Terran_SCV))
			{
				checkpoint = sq->nextFollowMovePosition();
				agent->setGoal(checkpoint);
			}
		}
	}
	
	if (goal.x != -1)
	{
		moveToGoal(agent, checkpoint, goal);
		return true;
	}
	return false;
}

void NavigationAgent::displayPF(BaseAgent* agent)
{
	Unit unit = agent->getUnit();
	if (unit->isBeingConstructed()) return;

	//PF
	WalkPosition w = WalkPosition(agent->getUnit()->getPosition());
	int tileX = w.x;
	int tileY = w.y;
	int range = 10*3;

	for (int cTileX = tileX - range; cTileX < tileX + range; cTileX+=3)
	{
        for (int cTileY = tileY - range; cTileY < tileY + range; cTileY+=3)
		{
            if (cTileX >= 0 && cTileY >= 0 && cTileX < mapW && cTileY < mapH)
			{
				WalkPosition wt = WalkPosition(cTileX+1, cTileY+1);
				float p = getAttackingUnitP(agent, wt);
				//cP += PFFunctions::getGoalP(Position(cX,cY), goal);
				//cP += PFFunctions::getTrailP(agent, cX, cY);
				p += PFFunctions::getTerrainP(agent, wt);
					
				//print box
				if (p > -950)
				{
					Position pos = Position(wt);
					Broodwar->drawBoxMap(pos.x-8,pos.y-8,pos.x+8,pos.y+8,getColor(p),true);
				}
            }
        }
    }
}

Color NavigationAgent::getColor(float p)
{
	if (p >= 0)
	{
		int v = (int)(p * 3);
		int halfV = (int)(p * 0.6);

		if (v > 255) v = 255;
		if (halfV > 255) halfV = 255;

		return Color(halfV, halfV, v);
	}
	else
	{
		p = -p;
		int v = (int)(p * 1.6);
		
		int v1 = 255 - v;
		if (v1 <= 0) v1 = 40;
		int halfV1 = (int)(v1 * 0.6);
		
		return Color(v1, halfV1, halfV1);
	}
}

bool NavigationAgent::moveToGoal(BaseAgent* agent,  TilePosition checkpoint, TilePosition goal)
{
	if (checkpoint.x == -1 || goal.x == -1) return false;
	Unit unit = agent->getUnit();

	if (unit->isStartingAttack() || unit->isAttacking())
	{
		return false;
	}

	Position toReach = Position(checkpoint.x*32+16, checkpoint.y*32+16);
	double distToReach = toReach.getDistance(unit->getPosition());

	int engageDist = (int)(unit->getType().groundWeapon() * 0.5);
	if (engageDist < 2*32) engageDist = 2*32;

	//Explorer units shall have
	//less engage dist to avoid getting
	//stuck at waypoints.
	Squad* sq = Commander::getInstance()->getSquad(agent->getSquadID());
	if (sq != NULL)
	{
		if (sq->isExplorer())
		{
			engageDist = 32;
		}
		else
		{
			//Add additional distance to avoid clogging
			//choke points.
			if (!sq->isActive())
			{
				engageDist += 4*32;
			}
		}
	}

	if (distToReach <= engageDist)
	{
		//Dont stop close to chokepoints
		TilePosition tp = unit->getTilePosition();
		Chokepoint* cp = getNearestChokepoint(tp);
		double d = tp.getDistance(TilePosition(cp->getCenter()));
		if (d > 4)
		{
			if (unit->isMoving()) unit->stop();
			return true;
		}
	}

	int squadID = agent->getSquadID();
	if (squadID != -1)
	{
		Squad* sq = Commander::getInstance()->getSquad(squadID);
		if (sq != NULL)
		{
			if (sq->isAttacking())
			{
				//Squad is attacking. Don't stop
				//at checkpoints.
				toReach = Position(goal.x*32+16,goal.y*32+16);
			}
		}
	}
	//Move
	if (!unit->isMoving()) return unit->attack(toReach);
	else return true;
}

float NavigationAgent::getAttackingUnitP(BaseAgent* agent, WalkPosition wp)
{
	float p = 0;
	
	//Enemy Units
	for (auto &u : Broodwar->enemy()->getUnits())
	{
		//Enemy seen

		UnitType t = u->getType();
		bool retreat = false;
		if (!agent->getUnitType().canAttack() && agent->getUnitType().isFlyer()) retreat = true;
		if (!agent->getUnitType().canAttack() && !agent->getUnitType().isFlyer()) retreat = true;
		if (agent->getUnit()->getGroundWeaponCooldown() >= 20 || agent->getUnit()->getAirWeaponCooldown() >= 20) retreat = true;

		float dist = PFFunctions::getDistance(wp, u);
		if (!retreat) p += PFFunctions::calcOffensiveUnitP(dist, agent->getUnit(), u);
		if (retreat) p += PFFunctions::calcDefensiveUnitP(dist, agent->getUnit(), u);
	}
	
	//Own Units
	Agentset agents = AgentManager::getInstance()->getAgents();
	for (auto &a : agents)
	{
		if (a->isAlive())
		{
			float dist = PFFunctions::getDistance(wp, a->getUnit());
			p += PFFunctions::calcOwnUnitP(dist, wp, agent->getUnit(), a->getUnit());
		}
	}

	//Neutral Units
	for (auto &u : Broodwar->getNeutralUnits())
	{
		if (u->getType().getID() == UnitTypes::Terran_Vulture_Spider_Mine.getID())
		{
			WalkPosition w2 = WalkPosition(u->getPosition());
			float dist = PFFunctions::getDistance(wp, u);
			if (dist <= 2)
			{
				p -= 50.0;
			}
		}
	}

	return p;
}

float NavigationAgent::getDefendingUnitP(BaseAgent* agent, WalkPosition wp)
{
	float p = 0;
	
	p += PFFunctions::getGoalP(agent, wp);
	p += PFFunctions::getTerrainP(agent, wp);

	//Own Units
	Agentset agents = AgentManager::getInstance()->getAgents();
	for (auto &a : agents)
	{
		if (a->isAlive())
		{
			float dist = PFFunctions::getDistance(wp, a->getUnit());
			float ptmp = PFFunctions::calcOwnUnitP(dist, wp, agent->getUnit(), a->getUnit());
			p += ptmp;
		}
	}

	return p;
}
