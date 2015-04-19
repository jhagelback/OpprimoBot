#include "Commander.h"
#include "StrategySelector.h"
#include "../Managers/AgentManager.h"
#include "../Managers/ExplorationManager.h"
#include "../Influencemap/MapManager.h"
#include "../Managers/Constructor.h"
#include "../Managers/Upgrader.h"
#include "../Pathfinding/Pathfinder.h"
#include "../MainAgents/WorkerAgent.h"
#include "../Utils/Profiler.h"
#include <algorithm>

Commander* Commander::instance = NULL;

Commander::Commander()
{
	currentState = DEFEND;
	stage = 0;

	debug_bp = false;
	debug_sq = false;

	removalDone = false;

	lastCallFrame = Broodwar->getFrameCount();

	noWorkersPerRefinery = 2;
	noWorkers = 5;
}

Commander::~Commander()
{
	for (Squad* s : squads)
	{
		delete s;
	}
	instance = NULL;
}

Commander* Commander::getInstance()
{
	if (instance == NULL)
	{
		instance = StrategySelector::getInstance()->getStrategy();
	}
	return instance;
}

void Commander::checkBuildplan()
{
	int cSupply = Broodwar->self()->supplyUsed() / 2;

	for (int i = 0; i < (int)buildplan.size(); i++)
	{
		if (cSupply >= buildplan.at(i).supply && Constructor::getInstance()->buildPlanLength() == 0)
		{
			if (buildplan.at(i).type == BuildplanEntry::BUILDING)
			{
				Constructor::getInstance()->addBuilding(buildplan.at(i).unittype);
				buildplan.erase(buildplan.begin() + i);
				i--;
			}
			else if (buildplan.at(i).type == BuildplanEntry::UPGRADE)
			{
				Upgrader::getInstance()->addUpgrade(buildplan.at(i).upgradetype);
				buildplan.erase(buildplan.begin() + i);
				i--;
			}
			else if (buildplan.at(i).type == BuildplanEntry::TECH)
			{
				Upgrader::getInstance()->addTech(buildplan.at(i).techtype);
				buildplan.erase(buildplan.begin() + i);
				i--;
			}
		}
	}
}

void Commander::cutWorkers()
{
	noWorkers = AgentManager::getInstance()->getNoWorkers();
	Broodwar << "Worker production halted" << endl;
}

int Commander::getNoWorkers()
{
	return noWorkers;
}

int Commander::getWorkersPerRefinery()
{
	return noWorkersPerRefinery;
}

bool Commander::shallEngage()
{
	TilePosition toAttack = findAttackPosition();
	if (toAttack.x == -1)
	{
		//No enemy sighted. Dont launch attack.
		return false;
	}

	for (Squad* s : squads)
	{
		if (s->isRequired() && !s->isActive())
		{
			return false;
		}
	}
	return true;
}

void Commander::updateGoals()
{
	TilePosition defSpot = findChokePoint();
	
	if (defSpot.x != -1)
	{
		for (Squad* s : squads)
		{
			s->defend(defSpot);
		}
	}
}

void Commander::debug_showGoal()
{
	for (Squad* s : squads)
	{
		s->debug_showGoal();
	}
}

void Commander::computeActionsBase()
{
	checkBuildplan();

	//Dont call too often
	int cFrame = Broodwar->getFrameCount();
	if (cFrame - lastCallFrame < 5)
	{
		return;
	}
	lastCallFrame = cFrame;

	//See if we need to assist a base or worker that is under attack
	if (assistBuilding()) return;
	if (assistWorker()) return;

	//Check if we shall launch an attack
	if (currentState == DEFEND)
	{
		if (shallEngage())
		{
			forceAttack();
		}
	}

	//Check if we shall go back to defend
	if (currentState == ATTACK)
	{
		bool activeFound = false;
		for (Squad* s : squads)
		{
			if (s->isRequired() && s->isActive())
			{
				activeFound = true;
			}
		}

		//No active required squads found.
		//Go back to defend.
		if (!activeFound)
		{
			currentState = DEFEND;
			TilePosition defSpot = findChokePoint();
			for (Squad* s : squads)
			{
				s->setGoal(defSpot);
			}
		}
	}

	if (currentState == DEFEND)
	{
		//Check if we need to attack/kite enemy workers in the base
		checkWorkersAttack(AgentManager::getInstance()->getClosestBase(Broodwar->self()->getStartLocation()));

		TilePosition defSpot = findChokePoint();
		for (Squad* s : squads)
		{
			if (!s->hasGoal())
			{
				if (defSpot.x != -1)
				{
					s->defend(defSpot);
				}
			}
		}
	}

	if (currentState == ATTACK)
	{
		for (Squad* s : squads)
		{
			if (s->isOffensive())
			{
				if (!s->hasGoal() && s->isActive())
				{
					TilePosition toAttack = findAttackPosition();
					if (toAttack.x >= 0)
					{
						s->attack(toAttack);
					}
				}
			}
			else
			{
				TilePosition defSpot = findChokePoint();
				if (defSpot.x != -1)
				{
					s->defend(defSpot);
				}
			}
		}
	}

	//Compute Squad actions.
	for(auto &sq : squads)
	{
		sq->computeActions();
	}

	//Attack if we have filled all supply spots
	if (currentState == DEFEND)
	{
		int supplyUsed = Broodwar->self()->supplyUsed() / 2;
		if (supplyUsed >= 198)
		{
			forceAttack();
		}
	}

	//Check if there are obstacles we can remove. Needed for some maps.
	checkRemovableObstacles();

	//Terran only: Check for repairs and finish unfinished buildings
	if (Constructor::isTerran())
	{
		//Check if there are unfinished buildings we need
		//to complete.
		checkDamagedBuildings();
	}

	//Check for units not belonging to a squad
	checkNoSquadUnits();
}

void Commander::checkNoSquadUnits()
{
	Agentset agents = AgentManager::getInstance()->getAgents();
	for (auto &a : agents)
	{
		bool notAssigned = true;
		if (!a->isAlive()) notAssigned = false;
		if (a->getUnitType().isWorker()) notAssigned = false;
		if (a->isOfType(UnitTypes::Zerg_Overlord)) notAssigned = false;
		if (a->getUnitType().isBuilding()) notAssigned = false;
		if (a->getUnitType().isAddon()) notAssigned = false;
		if (a->getSquadID() != -1) notAssigned = false;

		if (notAssigned)
		{
			assignUnit(a);
		}
	}
}

void Commander::assignUnit(BaseAgent* agent)
{
	//Broodwar << agent->getUnitID() << " (" << agent->getTypeName().c_str() << ") is not assigned to a squad" << endl;
	for (Squad* s : squads)
	{
		if (s->needUnit(agent->getUnitType()))
		{
			s->addMember(agent);
			Broodwar << agent->getUnitID() << " (" << agent->getTypeName().c_str() << ") is assigned to SQ " << s->getID() << endl;;
			return;
		}
	}
}

TilePosition Commander::findAttackPosition()
{
	TilePosition regionPos = MapManager::getInstance()->findAttackPosition();
	if (regionPos.x != -1)
	{
		TilePosition toAttack = ExplorationManager::getInstance()->getClosestSpottedBuilding(regionPos);
		if (toAttack.x != -1)
		{
			return toAttack;
		}
		else
		{
			return regionPos;
		}
	}

	return TilePosition(-1,-1);
}

void Commander::removeSquad(int id)
{
	for (int i = 0; i < (int)squads.size(); i++)
	{
		Squad* sq = squads.at(i);
		if (sq->getID() == id)
		{
			sq->disband();
			squads.erase(squads.begin() + i);
			return;
		}
	}
}

Squad* Commander::getSquad(int id)
{
	for (Squad* s : squads)
	{
		if (s->getID() == id)
		{
			return s;
		}
	}
	return NULL;
}

void Commander::addSquad(Squad* sq)
{
	squads.push_back(sq);
}

void Commander::unitDestroyed(BaseAgent* agent)
{
	int squadID = agent->getSquadID();
	if (squadID != -1)
	{
		Squad* squad = getSquad(squadID);
		if (squad != NULL)
		{
			squad->removeMember(agent);
		}
	}
}

void Commander::sortSquadList()
{
	sort(squads.begin(), squads.end(), SortSquadList());
}

void Commander::unitCreated(BaseAgent* agent)
{
	//Sort the squad list
	sortSquadList();

	for (Squad* s : squads)
	{
		if (s->addMember(agent))
		{
			break;
		}
	}
}

bool Commander::checkWorkersAttack(BaseAgent *base)
{
	int noAttack = 0;

	for (auto &u : Broodwar->enemy()->getUnits())
	{
		if (u->exists() && u->getType().isWorker())
		{
			double dist = u->getTilePosition().getDistance(base->getUnit()->getTilePosition());
			if (dist <= 12)
			{
				//Enemy unit discovered. Attack with some workers.
				Agentset agents = AgentManager::getInstance()->getAgents();
				for (auto &a : agents)
				{
					if (a->isAlive() && a->isWorker() && noAttack < 1)
					{
						WorkerAgent* wAgent = (WorkerAgent*)a;
						wAgent->setState(WorkerAgent::ATTACKING);
						a->getUnit()->attack(u);
						noAttack++;
					}
				}
			}
		}
	}

	if (noAttack > 0)
	{
		return true;
	}
	return false;
}

void Commander::checkRemovableObstacles()
{
	if (removalDone) return;

	//This method is used to handle the removal of obstacles
	//that is needed on some maps.

	if (Broodwar->mapFileName() == "(2)Destination.scx")
	{
		Unit mineral = NULL;
		if (Broodwar->self()->getStartLocation().x == 64)
		{
			for (auto &u : Broodwar->getAllUnits())
			{
				if (u->getType().isResourceContainer() && u->getTilePosition().x == 40 && u->getTilePosition().y == 120)
				{
					mineral = u;
				}
			}
		}
		if (Broodwar->self()->getStartLocation().x == 31)
		{
			for (auto &u : Broodwar->getAllUnits())
			{
				if (u->getType().isResourceContainer() && u->getTilePosition().x == 54 && u->getTilePosition().y == 6)
				{
					mineral = u;
				}
			}
		}
		if (mineral != NULL)
		{
			if (!AgentManager::getInstance()->workerIsTargeting(mineral))
			{
				BaseAgent* worker = AgentManager::getInstance()->findClosestFreeWorker(Broodwar->self()->getStartLocation());
				if (worker != NULL)
				{
					worker->getUnit()->rightClick(mineral);
					removalDone = true;
				}
			}
		}
	}
}


TilePosition Commander::findChokePoint()
{
	const Chokepoint* bestChoke = MapManager::getInstance()->getDefenseLocation();

	TilePosition guardPos = Broodwar->self()->getStartLocation();
	if (bestChoke != NULL)
	{
		guardPos = findDefensePos(bestChoke);
	}

	return guardPos;
}

TilePosition Commander::findDefensePos(const Chokepoint* choke)
{
	TilePosition defPos = TilePosition(choke->getCenter());
	TilePosition chokePos = defPos;

	double size = choke->getWidth();
	if (size <= 32 * 3)
	{
		//Very narrow chokepoint, dont crowd it
		double bestDist = 10000;
		TilePosition basePos = Broodwar->self()->getStartLocation();

		int maxD = 3;
		int minD = 2;

		//We found a chokepoint. Now we need to find a good place to defend it.
		for (int cX = chokePos.x - maxD; cX <= chokePos.x + maxD; cX++)
		{
			for (int cY = chokePos.y - maxD; cY <= chokePos.y + maxD; cY++)
			{
				TilePosition cPos = TilePosition(cX, cY);
				if (ExplorationManager::canReach(basePos, cPos))
				{
					double chokeDist = chokePos.getDistance(cPos);
					double baseDist = basePos.getDistance(cPos);

					if (chokeDist >= minD && chokeDist <= maxD)
					{
						if (baseDist < bestDist)
						{
							bestDist = baseDist;
							defPos = cPos;
						}
					}
				}
			}
		}
	}

	//Make defenders crowd around defensive structures.
	if (Broodwar->self()->getRace().getID() == Races::Zerg.getID())
	{
		UnitType defType;
		if (Constructor::isZerg()) defType = UnitTypes::Zerg_Sunken_Colony;
		if (Constructor::isProtoss()) defType = UnitTypes::Protoss_Photon_Cannon;
		if (Constructor::isTerran()) defType = UnitTypes::Terran_Bunker;
		
		BaseAgent* turret = AgentManager::getInstance()->getClosestAgent(defPos, defType);
		if (turret != NULL)
		{
			TilePosition tPos = turret->getUnit()->getTilePosition();
			double dist = tPos.getDistance(defPos);
			if (dist <= 22)
			{
				defPos = tPos;
			}
		}
	}

	return defPos;
}

bool Commander::needUnit(UnitType type)
{
	int prevPrio = 1000;

	for (Squad* s : squads)
	{
		if (!s->isFull())
		{
			if (s->getPriority() > prevPrio)
			{
				return false;
			}

			if (s->needUnit(type))
			{
				return true;
			}
			
			prevPrio = s->getPriority();
		}
	}
	return false;
}

bool Commander::assistBuilding()
{
	for (auto& a : AgentManager::getInstance()->getAgents())
	{
		if (a->isAlive() && a->isBuilding() && a->isUnderAttack())
		{
			for (Squad* s : squads)
			{
				bool ok = true;
				if (s->isExplorer()) ok = false;
				if (s->isBunkerDefend()) ok = false;
				if (s->isRush() && s->isActive()) ok = false;

				if (ok)
				{
					s->assist(a->getUnit()->getTilePosition());
					return true;
				}
			}
		}
	}

	return false;
}

bool Commander::assistWorker()
{
	for (auto& a : AgentManager::getInstance()->getAgents())
	{
		if (a->isAlive() && a->isWorker() && a->isUnderAttack())
		{
			for (Squad* s : squads)
			{
				bool ok = true;
				if (s->isExplorer()) ok = false;
				if (s->isBunkerDefend()) ok = false;
				if (s->isRush() && s->isActive()) ok = false;

				if (ok)
				{
					s->assist(a->getUnit()->getTilePosition());
					return true;
				}
			}
		}
	}

	return false;
}

void Commander::forceAttack()
{
	TilePosition cGoal = findAttackPosition();
	Broodwar << "Launch attack at (" <<cGoal.x << "," << cGoal.y << ")" << endl;
	if (cGoal.x == -1)
	{
		return;
	}

	for (Squad* s : squads)
	{
		if (s->isOffensive() || s->isSupport())
		{
			if (cGoal.x >= 0)
			{
				s->forceActive();
				s->attack(cGoal);
			}
		}
	}

	currentState = ATTACK;
}

void Commander::finishBuild(BaseAgent* baseAgent)
{
	//First we must check if someone is repairing this building
	if(AgentManager::getInstance()->isAnyAgentRepairingThisAgent(baseAgent)) return;

	BaseAgent* repUnit = AgentManager::getInstance()->findClosestFreeWorker(baseAgent->getUnit()->getTilePosition());
	if (repUnit != NULL)
	{
		WorkerAgent* w = (WorkerAgent*)repUnit;
		w->assignToRepair(baseAgent->getUnit());
	}
}

bool Commander::checkDamagedBuildings()
{
	Agentset agents = AgentManager::getInstance()->getAgents();
	for (auto &a : agents)
	{
		if (a->isAlive() && a->isBuilding() && a->isDamaged())
		{
			Unit builder = a->getUnit()->getBuildUnit();
			if (builder == NULL || !builder->isConstructing())
			{
				finishBuild(a);
			}
		}
	}
	return false;
}

void Commander::toggleBuildplanDebug()
{
	debug_bp = !debug_bp;
}

void Commander::toggleSquadsDebug()
{
	debug_sq = !debug_sq;
}

string Commander::format(string str)
{
	string res = str;

	string raceName = Broodwar->self()->getRace().getName();
	if (str.find(raceName) == 0)
	{
		int i = str.find("_");
		res = str.substr(i + 1, str.length());
	}

	if (res == "Siege Tank Tank Mode") res = "Siege Tank";

	return res;
}

void Commander::printInfo()
{
	if (debug_sq)
	{
		int totLines = 0;
		for (Squad* s : squads)
		{
			bool vis = true;
			if (s->getTotalUnits() == 0) vis = false;
			if (s->isBunkerDefend()) vis = false;
			if (s->getPriority() == 1000 && !s->isActive()) vis = false;

			if (vis)
			{
				totLines++;
			}	
		}
		if (totLines == 0) totLines++;

		Broodwar->drawBoxScreen(168,25,292,41+totLines*16,Colors::Black,true);
		if (currentState == DEFEND) Broodwar->drawTextScreen(170,25,"\x03Squads \x07(Defending)");
		if (currentState == ATTACK) Broodwar->drawTextScreen(170,25,"\x03Squads \x08(Attacking)");
		Broodwar->drawLineScreen(170,39,290,39,Colors::Orange);
		int no = 0;
		for (Squad* s : squads)
		{
			bool vis = true;
			if (s->getTotalUnits() == 0) vis = false;
			if (s->isBunkerDefend()) vis = false;
			if (s->getPriority() == 1000 && !s->isActive()) vis = false;

			if (vis)
			{
				int cSize = s->getSize();
				int totSize = s->getTotalUnits();

				if (s->isRequired())
				{
					if (cSize < totSize) Broodwar->drawTextScreen(170,41+no*16, "*SQ %d: \x18(%d/%d)", s->getID(), s->getSize(), s->getTotalUnits());
					else Broodwar->drawTextScreen(170,41+no*16, "*SQ %d: \x07(%d/%d)", s->getID(), s->getSize(), s->getTotalUnits());
					no++;
				}
				else
				{
					if (cSize < totSize) Broodwar->drawTextScreen(170,41+no*16, "SQ %d: \x18(%d/%d)", s->getID(), s->getSize(), s->getTotalUnits());
					else Broodwar->drawTextScreen(170,41+no*16, "SQ %d: \x07(%d/%d)", s->getID(), s->getSize(), s->getTotalUnits());
					no++;
				}
			}	
		}
		if (no == 0) no++;
		Broodwar->drawLineScreen(170,40+no*16,290,40+no*16,Colors::Orange);
	}

	if (debug_bp && buildplan.size() > 0)
	{
		int totLines = (int)buildplan.size();
		if (totLines > 4) totLines = 4;
		if (totLines == 0) totLines = 1;
		
		Broodwar->drawBoxScreen(298,25,482,41+totLines*16,Colors::Black,true);
		Broodwar->drawTextScreen(300,25,"\x03Strategy Plan");
		Broodwar->drawLineScreen(300,39,480,39,Colors::Orange);
		int no = 0;

		int max = (int)buildplan.size();
		if (max > 4) max = 4;

		for (int i = 0; i < max; i++)
		{
			string name = "";
			if (buildplan.at(i).type == BuildplanEntry::BUILDING) name = buildplan.at(i).unittype.getName();
			if (buildplan.at(i).type == BuildplanEntry::UPGRADE) name = buildplan.at(i).upgradetype.getName();
			if (buildplan.at(i).type == BuildplanEntry::TECH) name = buildplan.at(i).techtype.getName();
			name = format(name);

			stringstream ss;
			ss << name;
			ss << " \x0F(@";
			ss << buildplan.at(i).supply;
			ss << ")";
			
			Broodwar->drawTextScreen(300,40+no*16, ss.str().c_str());
			no++;
		}
		if (no == 0) no++;
		Broodwar->drawLineScreen(300,40+no*16,480,40+no*16,Colors::Orange);
		Constructor::getInstance()->printInfo();
	}
}

int Commander::addBunkerSquad()
{
	Squad* bSquad = new Squad(100 + AgentManager::getInstance()->countNoUnits(UnitTypes::Terran_Bunker), Squad::BUNKER, "BunkerSquad", 5);
	bSquad->addSetup(UnitTypes::Terran_Marine, 4);
	squads.push_back(bSquad);

	//Try to fill from other squads.
	int added = 0;
	for (Squad* s : squads)
	{
		if (s->isOffensive() || s->isDefensive())
		{
			for (int i = 0; i < 4 - added; i++)
			{
				if (s->hasUnits(UnitTypes::Terran_Marine, 1))
				{
					if (added < 4)
					{
						BaseAgent* ma = s->removeMember(UnitTypes::Terran_Marine);
						if (ma != NULL)
						{
							added++;
							bSquad->addMember(ma);
							ma->clearGoal();
						}
					}
				}
			}
		}
	}

	return bSquad->getID();
}

bool Commander::removeBunkerSquad(int unitID)
{
	for (Squad* s : squads)
	{
		if (s->isBunkerDefend())
		{
			if (s->getBunkerID() == unitID)
			{
				int sID = s->getID();
				removeSquad(sID);
				return true;
			}
		}
	}
	return false;
}