#include "Constructor.h"
//#include "../MainAgents/WorkerAgent.h"
#include "../StructureAgents/StructureAgent.h"
#include "../MainAgents/BaseAgent.h"
#include "../MainAgents/WorkerAgent.h"
#include "AgentManager.h"
#include "BuildingPlacer.h"
#include "ResourceManager.h"

Constructor* Constructor::instance = NULL;

Constructor::Constructor()
{
	lastCallFrame = Broodwar->getFrameCount();
}

Constructor::~Constructor()
{
	instance = NULL;
}

Constructor* Constructor::getInstance()
{
	if (instance == NULL)
	{
		instance = new Constructor();
	}
	return instance;
}

int Constructor::buildPlanLength()
{
	return (int)buildPlan.size();
}

void Constructor::buildingDestroyed(Unit building)
{
	if (building->getType().getID() == UnitTypes::Protoss_Pylon.getID())
	{
		return;
	}
	if (building->getType().getID() == UnitTypes::Terran_Supply_Depot.getID())
	{
		return;
	}
	if (building->getType().isAddon())
	{
		return;
	}
	if (building->getType().getID() == UnitTypes::Zerg_Sunken_Colony.getID())
	{
		buildPlan.insert(buildPlan.begin(),UnitTypes::Zerg_Spore_Colony);
		return;
	}
	buildPlan.insert(buildPlan.begin(), building->getType());
}

void Constructor::computeActions()
{
	//Check if we need more supply buildings
	if (isTerran() || isProtoss())
	{
		if (shallBuildSupply())
		{
			buildPlan.insert(buildPlan.begin(), Broodwar->self()->getRace().getSupplyProvider());
		}
	}

	//Check if we need to expand
	if (!hasResourcesLeft())
	{
		expand(Broodwar->self()->getRace().getCenter());
	}

	if (buildPlan.size() == 0 && buildQueue.size() == 0)
	{
		//Nothing to do
		return;
	}

	//Dont call too often
	int cFrame = Broodwar->getFrameCount();
	if (cFrame - lastCallFrame < 10)
	{
		return;
	}
	lastCallFrame = cFrame;

	if (AgentManager::getInstance()->getNoWorkers() == 0)
	{
		//No workers so cant do anything
		return;
	}

	//Check if we have possible "locked" items in the buildqueue
	for (int i = 0; i < (int)buildQueue.size(); i++)
	{
		int elapsed = cFrame - buildQueue.at(i).assignedFrame;
		if (elapsed >= 2000)
		{
			//Reset the build request
			WorkerAgent* worker = (WorkerAgent*)AgentManager::getInstance()->getAgent(buildQueue.at(i).assignedWorkerId);
			if (worker != NULL)
			{
				worker->reset();
			}
			buildPlan.insert(buildPlan.begin(), buildQueue.at(i).toBuild);
			ResourceManager::getInstance()->unlockResources(buildQueue.at(i).toBuild);
			buildQueue.erase(buildQueue.begin() + i);
			return;
		}
	}

	//Check if we can build next building in the buildplan
	if ((int)buildPlan.size() > 0)
	{
		executeOrder(buildPlan.at(0));
	}
}

bool Constructor::hasResourcesLeft()
{
	int totalMineralsLeft = 0;

	Agentset agents = AgentManager::getInstance()->getAgents();
	for (auto &a : agents)
	{
		if (a->getUnitType().isResourceDepot())
		{
			totalMineralsLeft += mineralsNearby(a->getUnit()->getTilePosition());
		}
	}

	if (totalMineralsLeft <= 5000)
	{
		return false;
	}
	return true;
}

int Constructor::mineralsNearby(TilePosition center)
{
	int mineralCnt = 0;

	for (auto &u : Broodwar->getMinerals())
	{
		if (u->exists())
		{
			double dist = center.getDistance(u->getTilePosition());
			if (dist <= 10)
			{
				mineralCnt += u->getResources();			
			}
		}
	}

	return mineralCnt;
}

bool Constructor::shallBuildSupply()
{
	UnitType supply = Broodwar->self()->getRace().getSupplyProvider();

	//Check if we need supplies
	int supplyTotal = Broodwar->self()->supplyTotal() / 2;
	int supplyUsed = Broodwar->self()->supplyUsed() / 2;

	int preDiff = 2;
	//Speed up supply production in middle/late game
	if (supplyUsed > 30) preDiff = 4;

	if (supplyUsed <= supplyTotal - preDiff)
	{
		return false;
	}
	//Don't use automatic supply adding in the early game
	//to make it a bit more controlled.
	if (supplyUsed <= 30)
	{
		return false;
	}

	//Check if we have reached max supply
	if (supplyTotal >= 200)
	{
		return false;
	}

	//Check if there aready is a supply in the list
	if (nextIsOfType(supply))
	{
		return false;
	}

	//Check if we are already building a supply
	if (supplyBeingBuilt())
	{
		return false;
	}

	return true;
}

bool Constructor::supplyBeingBuilt()
{
	//Zerg
	if (isZerg())
	{
		if (noInProduction(UnitTypes::Zerg_Overlord) > 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	
	//Terran and Protoss
	UnitType supply = Broodwar->self()->getRace().getSupplyProvider();

	//1. Check if we are already building a supply
	Agentset agents = AgentManager::getInstance()->getAgents();
	for (auto &a : agents)
	{
		if (a->isAlive())
		{
			if (a->getUnitType().getID() == supply.getID())
			{
				if (a->getUnit()->isBeingConstructed())
				{
					//Found one that is being constructed
					return true;
				}
			}
		}
	}

	//2. Check if we have a supply in build queue
	for (int i = 0; i < (int)buildQueue.size(); i++)
	{
		if (buildQueue.at(i).toBuild.getID() == supply.getID())
		{
			return true;
		}
	}

	return false;
}

void Constructor::lock(int buildPlanIndex, int unitId)
{
	UnitType type = buildPlan.at(buildPlanIndex);
	buildPlan.erase(buildPlan.begin() + buildPlanIndex);

	BuildQueueItem item;
	item.toBuild = type;
	item.assignedWorkerId = unitId;
	item.assignedFrame = Broodwar->getFrameCount();

	buildQueue.push_back(item);
}

void Constructor::remove(UnitType type)
{
	for (int i = 0; i < (int)buildPlan.size(); i++)
	{
		if (buildPlan.at(i).getID() == type.getID())
		{
			buildPlan.erase(buildPlan.begin() + i);
			return;
		}
	}
}

void Constructor::unlock(UnitType type)
{
	for (int i = 0; i < (int)buildQueue.size(); i++)
	{
		if (buildQueue.at(i).toBuild.getID() == type.getID())
		{
			buildQueue.erase(buildQueue.begin() + i);
			return;
		}
	}
}

void Constructor::handleWorkerDestroyed(UnitType type, int workerID)
{
	for (int i = 0; i < (int)buildQueue.size(); i++)
	{
		if (buildQueue.at(i).assignedWorkerId == workerID)
		{
			buildQueue.erase(buildQueue.begin() + i);
			buildPlan.insert(buildPlan.begin(), type);
			ResourceManager::getInstance()->unlockResources(type);
		}
	}
}

bool Constructor::executeMorph(UnitType target, UnitType evolved)
{
	BaseAgent* agent = AgentManager::getInstance()->getClosestAgent(Broodwar->self()->getStartLocation(), target);
	if (agent != NULL)
	{
		StructureAgent* sAgent = (StructureAgent*)agent;
		if (sAgent->canMorphInto(evolved))
		{
			sAgent->getUnit()->morph(evolved);
			lock(0, sAgent->getUnitID());
			return true;
		}
	}
	else
	{
		//No building available that can do this morph.
		remove(evolved);
	}
	return false;
}

bool Constructor::executeOrder(UnitType type)
{
	//Max 5 concurrent buildings allowed at the same time
	if ((int)buildQueue.size() >= 5)
	{
		return false;
	}

	//Check if we meet requirements for the building
	map<UnitType,int> reqs = type.requiredUnits();
	for(map<UnitType,int>::iterator j=reqs.begin();j!=reqs.end();j++)
	{
		if (!AgentManager::getInstance()->hasBuilding((*j).first))
		{
			return false;
		}
	}

	if (type.isResourceDepot())
	{
		TilePosition pos = BuildingPlacer::getInstance()->findExpansionSite();
		if (pos.x == -1)
		{
			//No expansion site found.
			if ((int)buildPlan.size() > 0) buildPlan.erase(buildPlan.begin());
			return true;
		}
	}
	if (type.isRefinery())
	{
		TilePosition rSpot = BuildingPlacer::getInstance()->searchRefinerySpot();
		if (rSpot.x < 0)
		{
			//No buildspot found
			if ((int)buildPlan.size() > 0) buildPlan.erase(buildPlan.begin());
			return true;
		}
	}
	if (isZerg())
	{
		pair<UnitType, int> builder = type.whatBuilds();
		if (builder.first.getID() != UnitTypes::Zerg_Drone.getID())
		{
			//Needs to be morphed
			if (executeMorph(builder.first, type))
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}

	//Check if we have resources
	if (!ResourceManager::getInstance()->hasResources(type))
	{
		return false;
	}
	
	//Check if we have a free worker
	bool found = false;
	BaseAgent* a = AgentManager::getInstance()->findClosestFreeWorker(Broodwar->self()->getStartLocation());
	if (a != NULL)
	{
		WorkerAgent* w = (WorkerAgent*)a;
		found = true;
		if (w->assignToBuild(type))
		{
			lock(0, a->getUnitID());
			return true;
		}
		else
		{
			//Unable to find a buildspot. Dont bother checking for all
			//other workers
			handleNoBuildspotFound(type);
			return false;
		}
	}

	return false;
}

bool Constructor::isTerran()
{
	if (Broodwar->self()->getRace().getID() == Races::Terran.getID())
	{
		return true;
	}
	return false;
}

bool Constructor::isProtoss()
{
	if (Broodwar->self()->getRace().getID() == Races::Protoss.getID())
	{
		return true;
	}
	return false;
}

bool Constructor::isZerg()
{
	if (Broodwar->self()->getRace().getID() == Races::Zerg.getID())
	{
		return true;
	}
	return false;
}

bool Constructor::nextIsExpand()
{
	if ((int)buildPlan.size() > 0)
	{
		if (buildPlan.at(0).isResourceDepot()) return true;
	}
	return false;
}

void Constructor::addRefinery()
{
	//Don't add if we already have enough.
	UnitType ref = Broodwar->self()->getRace().getRefinery();
	int no = AgentManager::getInstance()->countNoUnits(ref);
	if (no >= 4) return;

	UnitType refinery = Broodwar->self()->getRace().getRefinery();

	if (!this->nextIsOfType(refinery))
	{
		buildPlan.insert(buildPlan.begin(), refinery);
	}
}

void Constructor::commandCenterBuilt()
{
	lastCommandCenter = Broodwar->getFrameCount();
}

string Constructor::format(UnitType type)
{
	string name = type.getName();
	int i = name.find("_");
	string fname = name.substr(i + 1, name.length());
	return fname;
}

void Constructor::printInfo()
{
	int totLines = (int)buildPlan.size() + (int)buildQueue.size();
	if (buildPlan.size() == 0) totLines++;
	if (buildQueue.size() == 0) totLines++;

	if (totLines > 0)
	{
		Broodwar->drawBoxScreen(488, 25, 602, 62 + totLines * 16, Colors::Black, true);
		Broodwar->drawTextScreen(490, 25, "\x03Next to build");
		Broodwar->drawLineScreen(490, 39, 600, 39, Colors::Orange);

		int no = 0;
		for (int i = 0; i < (int)buildPlan.size(); i++)
		{
			Broodwar->drawTextScreen(490, 40 + no * 16, format(buildPlan.at(i)).c_str());
			no++;
		}
		if (no == 0) no++;
		Broodwar->drawLineScreen(490, 40 + no * 16, 600, 40 + no * 16, Colors::Orange);

		int s = 40 + no * 16;
		Broodwar->drawTextScreen(490, s + 2, "\x03In progress");
		Broodwar->drawLineScreen(490, s + 19, 600, s + 19, Colors::Orange);

		no = 0;
		for (int i = 0; i < (int)buildQueue.size(); i++)
		{
			Broodwar->drawTextScreen(490, s + 20 + no * 16, format(buildQueue.at(i).toBuild).c_str());
			no++;
		}
		if (no == 0) no++;
		Broodwar->drawLineScreen(490, s + 20 + no * 16, 600, s + 20 + no * 16, Colors::Orange);
	}
}

void Constructor::handleNoBuildspotFound(UnitType toBuild)
{
	bool removeOrder = false;
	if (toBuild.getID() == UnitTypes::Protoss_Photon_Cannon) removeOrder = true;
	if (toBuild.getID() == UnitTypes::Terran_Missile_Turret) removeOrder = true;
	if (toBuild.isAddon()) removeOrder = true;
	if (toBuild.getID() == UnitTypes::Zerg_Spore_Colony) removeOrder = true;
	if (toBuild.getID() == UnitTypes::Zerg_Sunken_Colony) removeOrder = true;
	if (toBuild.isResourceDepot()) removeOrder = true;
	if (toBuild.isRefinery()) removeOrder = true;

	if (removeOrder)
	{
		remove(toBuild);
	}

	if (!removeOrder)
	{
		if (isProtoss() && !supplyBeingBuilt())
		{
			//Insert a pylon to increase PSI coverage
			if (!nextIsOfType(UnitTypes::Protoss_Pylon))
			{
				buildPlan.insert(buildPlan.begin(), UnitTypes::Protoss_Pylon);
			}
		}
	}
}

bool Constructor::nextIsOfType(UnitType type)
{
	if ((int)buildPlan.size() == 0)
	{
		return false;
	}
	else
	{
		if (buildPlan.at(0).getID() == type.getID())
		{
			return true;
		}
	}
	return false;
}

bool Constructor::containsType(UnitType type)
{
	for (int i = 0; i < (int)buildPlan.size(); i++)
	{
		if (buildPlan.at(i).getID() == type.getID())
		{
			return true;
		}
	}
	for (int i = 0; i < (int)buildQueue.size(); i++)
	{
		if (buildQueue.at(i).toBuild.getID() == type.getID())
		{
			return true;
		}
	}
	return false;
}

bool Constructor::coveredByDetector(TilePosition pos)
{
	Agentset agents = AgentManager::getInstance()->getAgents();
	for (auto &a : agents)
	{
		if (a->isAlive())
		{
			UnitType type = a->getUnitType();
			if (type.isDetector() && type.isBuilding())
			{
				double range = type.sightRange() * 1.5;
				double dist = a->getUnit()->getPosition().getDistance(Position(pos));
				if (dist <= range)
				{
					return true;
				}
			}
		}
	}
	return false;
}

void Constructor::addBuilding(UnitType type)
{
	buildPlan.push_back(type);
}

void Constructor::addBuildingFirst(UnitType type)
{
	buildPlan.insert(buildPlan.begin(), type);
}

void Constructor::expand(UnitType commandCenterUnit)
{
	if (isBeingBuilt(commandCenterUnit))
	{
		return;
	}

	if (containsType(commandCenterUnit))
	{
		return;
	}

	TilePosition pos = BuildingPlacer::getInstance()->findExpansionSite();
	if (pos.x == -1)
	{
		//No expansion site found.
		return;
	}

	buildPlan.insert(buildPlan.begin(), commandCenterUnit);
}

bool Constructor::needBuilding(UnitType type)
{
	if (AgentManager::getInstance()->hasBuilding(type)) return false;
	if (isBeingBuilt(type)) return false;
	if (containsType(type)) return false;
	
	return true;
}

bool Constructor::isBeingBuilt(UnitType type)
{
	Agentset agents = AgentManager::getInstance()->getAgents();
	for (auto &a : agents)
	{
		if (a->isOfType(type) && a->getUnit()->isBeingConstructed())
		{
			return true;
		}
	}
	return false;
}

int Constructor::noInProduction(UnitType type)
{
	int no = 0;
	
	Agentset agents = AgentManager::getInstance()->getAgents();
	for (auto &a : agents)
	{
		if (a->isAlive())
		{
			if (a->getUnitType().canProduce() && !a->getUnit()->isBeingConstructed())
			{
				for (auto &u : a->getUnit()->getTrainingQueue())
				{
					if (u.getID() == type.getID())
					{
						no++;
					}
				}
			}
		}
	}

	if (isZerg())
	{
		for (auto &u : Broodwar->self()->getUnits())
		{
			if (u->exists())
			{
				if (u->getType().getID() == UnitTypes::Zerg_Egg.getID())
				{
					if (u->getBuildType().getID() == type.getID())
				{
						no++;
						if (type.isTwoUnitsInOneEgg()) no++;
					}
				}
			}
		}
	}

	return no;
}
