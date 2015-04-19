#include "StructureAgent.h"
#include "../Managers/AgentManager.h"
#include "../Managers/Constructor.h"
#include "../Managers/Upgrader.h"
#include "../Commander/Commander.h"
#include "../MainAgents/WorkerAgent.h"
#include "../Managers/ResourceManager.h"

StructureAgent::StructureAgent()
{

}

StructureAgent::~StructureAgent()
{

}

StructureAgent::StructureAgent(Unit mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "StructureAgent";
}

void StructureAgent::debug_showGoal()
{
	if (!isAlive()) return;

	//Draw "is working" box
	int total = 0;
	int done = 0;
	string txt = "";
	Color cColor = Colors::Blue;
	int bar_h = 18;

	if (unit->isBeingConstructed())
	{
		total = unit->getType().buildTime();
		done = total - unit->getRemainingBuildTime();
		txt = "";
		bar_h = 8;
	}
	if (!unit->isBeingConstructed() && unit->getType().isResourceContainer())
	{
		total = unit->getInitialResources();
		done = unit->getResources();
		txt = "";
		cColor = Colors::Orange;
		bar_h = 8;
	}
	if (unit->isResearching())
	{
		total = unit->getTech().researchTime();
		done = total - unit->getRemainingResearchTime();
		txt = unit->getTech().getName();
	}
	if (unit->isUpgrading())
	{
		total = unit->getUpgrade().upgradeTime();
		done = total - unit->getRemainingUpgradeTime();
		txt = unit->getUpgrade().getName();
	}
	if (unit->isTraining())
	{
		if (unit->getTrainingQueue().size() > 0)
		{
			UnitType t = *(unit->getTrainingQueue().begin());
			total = t.buildTime();
			txt = t.getName();
			done = total - unit->getRemainingTrainTime();
		}
	}

	if (total > 0)
	{
		int w = unit->getType().tileWidth() * 32;
		int h = unit->getType().tileHeight() * 32;

		//Start 
		Position s = Position(unit->getPosition().x - w/2, unit->getPosition().y - 30);
		//End
		Position e = Position(s.x + w, s.y + bar_h);
		//Progress
		int prg = (int)((double)done / (double)total * w);
		Position p = Position(s.x + prg, s.y +  bar_h);

		Broodwar->drawBoxMap(s.x,s.y,e.x,e.y,cColor,false);
		Broodwar->drawBoxMap(s.x,s.y,p.x,p.y,cColor,true);

		Broodwar->drawTextMap(s.x + 5, s.y + 2, txt.c_str());
	}

	if (!unit->isBeingConstructed() && unit->getType().getID() == UnitTypes::Terran_Bunker.getID())
	{
		int w = unit->getType().tileWidth() * 32;
		int h = unit->getType().tileHeight() * 32;

		Broodwar->drawTextMap(unit->getPosition().x - w / 2, unit->getPosition().y - 10, unit->getType().getName().c_str());

		//Draw Loaded box
		Position a = Position(unit->getPosition().x - w/2, unit->getPosition().y - h/2);
		Position b = Position(a.x + 94, a.y + 6);
		Broodwar->drawBoxMap(a.x,a.y,b.x,b.y,Colors::Green,false);

		if ((int)unit->getLoadedUnits().size() > 0)
		{
			Position a = Position(unit->getPosition().x - w/2, unit->getPosition().y - h/2);
			Position b = Position(a.x + unit->getLoadedUnits().size() * 24, a.y + 6);

			Broodwar->drawBoxMap(a.x,a.y,b.x,b.y,Colors::Green,true);
		}
	}
}

void StructureAgent::computeActions()
{
	if (isAlive())
	{
		if (!unit->isIdle()) return;

		if (Upgrader::getInstance()->checkUpgrade(this)) return;
		
		if (Constructor::isTerran())
		{
			//Check addons here
			if (isOfType(UnitTypes::Terran_Science_Facility))
			{
				if (unit->getAddon() == NULL)
				{
					unit->buildAddon(UnitTypes::Terran_Physics_Lab);
				}
			}
			if (isOfType(UnitTypes::Terran_Starport))
			{
				if (unit->getAddon() == NULL)
				{
					unit->buildAddon(UnitTypes::Terran_Control_Tower);
				}
			}
			if (isOfType(UnitTypes::Terran_Factory))
			{
				if (unit->getAddon() == NULL)
				{
					unit->buildAddon(UnitTypes::Terran_Machine_Shop);
				}
			}
		}

		if (!unit->isBeingConstructed() && unit->isIdle() && getUnit()->getType().canProduce())
		{
			//Iterate through all unit types
			for (auto &u : UnitTypes::allUnitTypes())
			{
				//Check if we can (and need) to build the unit
				if (canBuildUnit(u))
				{
					//Build it!
					unit->train(u);
				}
			}
		}

		//Check for Spire upgrade
		if (isOfType(UnitTypes::Zerg_Spire))
		{
			if (Broodwar->canMake(UnitTypes::Zerg_Greater_Spire, unit) && AgentManager::getInstance()->countNoFinishedUnits(UnitTypes::Zerg_Hive) > 0)
			{
				if (ResourceManager::getInstance()->hasResources(UnitTypes::Zerg_Greater_Spire))
				{
					ResourceManager::getInstance()->lockResources(UnitTypes::Zerg_Greater_Spire);
					unit->morph(UnitTypes::Zerg_Greater_Spire);
					return;
				}
			}
		}

		//Check for Creep Colony upgrade
		if (isOfType(UnitTypes::Zerg_Creep_Colony))
		{
			if (ResourceManager::getInstance()->hasResources(UnitTypes::Zerg_Sunken_Colony))
			{
				ResourceManager::getInstance()->lockResources(UnitTypes::Zerg_Sunken_Colony);
				unit->morph(UnitTypes::Zerg_Sunken_Colony);
				return;
			}
		}
	}
}

bool StructureAgent::canBuild(UnitType type)
{
	//1. Check if this unit can construct the unit
	pair<UnitType, int> builder = type.whatBuilds();
	if (builder.first.getID() != unit->getType().getID())
	{
		return false;
	}

	//2. Check canBuild
	if (!Broodwar->canMake(type, unit))
	{
		return false;
	}

	//3. Check if we have enough resources
	if (!ResourceManager::getInstance()->hasResources(type))
	{
		return false;
	}

	//All clear. Build the unit.
	return true;
}

bool StructureAgent::canBuildUnit(UnitType type)
{
	//1. Check if race matches
	if (type.getRace().getID() != Broodwar->self()->getRace().getID())
	{
		return false;
	}

	//2. Check if this unit can construct the unit
	pair<UnitType, int> builder = type.whatBuilds();
	if (builder.first.getID() != unit->getType().getID())
	{
		return false;
	}

	//3. Check if we need the unit in a squad
	if (!Commander::getInstance()->needUnit(type))
	{
		return false;
	}
	
	//4. Check canBuild
	if (!Broodwar->canMake(type, unit))
	{
		return false;
	}

	//5. Check if we have enough resources
	if (!ResourceManager::getInstance()->hasResources(type))
	{
		return false;
	}

	//All clear. Build the unit.
	return true;
}

void StructureAgent::printInfo()
{
	int sx = unit->getPosition().x;
	int sy = unit->getPosition().y;

	int h = sy+46;
	if (squadID != -1) h += 15;
	if (isOfType(UnitTypes::Terran_Bunker)) h += 60;

	Broodwar->drawBoxMap(sx-2,sy,sx+102,h,Colors::Black,true);
	Broodwar->drawTextMap(sx,sy,"\x03%s", format(unit->getType().getName()).c_str());
	Broodwar->drawLineMap(sx,sy+14,sx+100,sy+14,Colors::Purple);

	Broodwar->drawTextMap(sx+2,sy+15,"Id: \x11%d", unitID);
	Broodwar->drawTextMap(sx+2,sy+30,"Position: \x11(%d,%d)", unit->getTilePosition().x, unit->getTilePosition().y);
	if (squadID != -1) Broodwar->drawTextMap(sx+2,sy+45,"Squad: \x11%d", squadID);
	
	Broodwar->drawLineMap(sx,h-61,sx+100,h-61,Colors::Purple);

	if (isOfType(UnitTypes::Terran_Bunker))
	{
		Squad* sq = Commander::getInstance()->getSquad(squadID);
		if (sq != NULL)
		{
			int no = 0;
			Agentset agents = sq->getMembers();
			for (auto &a : agents)
			{
				Broodwar->drawTextMap(sx+2,h-60+15*no,"%s \x11(%d)", format(a->getUnitType().getName()).c_str(), a->getUnitID());
				no++;
			}
		}
		Broodwar->drawLineMap(sx,h-1,sx+100,h-1,Colors::Purple);
	}
}

void StructureAgent::sendWorkers()
{
	//We have constructed a new base. Make some workers move here.
	int noWorkers = AgentManager::getInstance()->getNoWorkers();
	int toSend = noWorkers / AgentManager::getInstance()->countNoBases();
	int hasSent = 0;

	Agentset agents = AgentManager::getInstance()->getAgents();
	for (auto &a : agents)
	{
		if (a->isAlive() && a->isFreeWorker())
		{
			Unit worker = a->getUnit();
			WorkerAgent* wa = (WorkerAgent*)a;
			worker->rightClick(unit->getPosition());
			hasSent++;
		}

		if (hasSent >= toSend)
		{
			return;
		}
	}
}

bool StructureAgent::canMorphInto(UnitType type)
{
	//1. Check canBuild
	if (!Broodwar->canMake(type, unit))
	{
		return false;
	}

	//Zerg morph units sometimes bugs in canMake, so we need to do an extra check
	if (type.getID() == UnitTypes::Zerg_Defiler_Mound.getID() && AgentManager::getInstance()->countNoFinishedUnits(UnitTypes::Zerg_Greater_Spire) == 0)
	{
		return false;
	}
	
	//2. Check if we have enough resources
	if (!ResourceManager::getInstance()->hasResources(type))
	{
		return false;
	}

	//3. Check if unit is already morphing
	if (unit->isMorphing())
	{
		return false;
	}

	//All clear. Build the unit.
	return true;
}

bool StructureAgent::canEvolveUnit(UnitType type)
{
	//1. Check if we need the unit in a squad
	if (!Commander::getInstance()->needUnit(type))
	{
		return false;
	}

	//2. Check canBuild
	if (!Broodwar->canMake(type, unit))
	{
		return false;
	}

	//3. Check if we have enough resources
	if (!ResourceManager::getInstance()->hasResources(type))
	{
		return false;
	}

	//All clear. Build the unit.
	return true;
}
