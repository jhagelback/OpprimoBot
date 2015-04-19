#include <BWTA.h>

#include "AIloop.h"
#include "Managers/BuildingPlacer.h"
#include "Utils/Profiler.h"
#include "Managers/Upgrader.h"
#include "Pathfinding/NavigationAgent.h"
#include "Managers/AgentManager.h"
#include "Influencemap/MapManager.h"
#include "Managers/ExplorationManager.h"
#include "Managers/Constructor.h"
#include "Commander/StrategySelector.h"

AIloop::AIloop()
{
	debugUnit = false;
	debugPF = false;
	debugBP = false;
	debugSQ = -1;
	debug = true;

	for (auto &u : Broodwar->self()->getUnits())
	{
		AgentManager::getInstance()->addAgent(u);
	}
}

AIloop::~AIloop()
{
	
}

void AIloop::toggleDebug()
{
	debug = !debug;
}

void AIloop::toggleUnitDebug()
{
	debugUnit = !debugUnit;
}

void AIloop::togglePFDebug()
{
	debugPF = !debugPF;
}

void AIloop::toggleBPDebug()
{
	debugBP = !debugBP;
}

void AIloop::setDebugSQ(int squadID)
{
	debugSQ = squadID;
}

void AIloop::computeActions()
{
	Profiler::getInstance()->start("OnFrame_MapManager");
	MapManager::getInstance()->update();
	Profiler::getInstance()->end("OnFrame_MapManager");
	Profiler::getInstance()->start("OnFrame_Constructor");
	Constructor::getInstance()->computeActions();
	Profiler::getInstance()->end("OnFrame_Constructor");
	Profiler::getInstance()->start("OnFrame_Commander");
	Commander::getInstance()->computeActions();
	Profiler::getInstance()->end("OnFrame_Commander");
	Profiler::getInstance()->start("OnFrame_ExplorationManager");
	ExplorationManager::getInstance()->computeActions();
	Profiler::getInstance()->end("OnFrame_ExplorationManager");
	Profiler::getInstance()->start("OnFrame_AgentManager");
	AgentManager::getInstance()->computeActions();
	Profiler::getInstance()->end("OnFrame_AgentManager");
}

void AIloop::addUnit(Unit unit)
{
	AgentManager::getInstance()->addAgent(unit);

	//Remove from buildorder if this is a building
	if (unit->getType().isBuilding())
	{
		Constructor::getInstance()->unlock(unit->getType());
	}
}

void AIloop::morphUnit(Unit unit)
{
	AgentManager::getInstance()->morphDrone(unit);
	Constructor::getInstance()->unlock(unit->getType());
}

void AIloop::unitDestroyed(Unit unit)
{
	if (unit->getPlayer()->getID() == Broodwar->self()->getID())
	{
		//Remove bunker squads if the destroyed unit
		//is a bunker
		if (unit->getType().getID() == UnitTypes::Terran_Bunker.getID())
		{
			Commander::getInstance()->removeBunkerSquad(unit->getID());
		}

		AgentManager::getInstance()->removeAgent(unit);
		if (unit->getType().isBuilding())
		{
			Constructor::getInstance()->buildingDestroyed(unit);
		}

		AgentManager::getInstance()->cleanup();
	}
	if (unit->getPlayer()->getID() != Broodwar->self()->getID() && !unit->getPlayer()->isNeutral())
	{
		//Update spotted buildings
		ExplorationManager::getInstance()->unitDestroyed(unit);
	}
}

void AIloop::show_debug()
{
	if (debug)
	{
		//Show timer
		stringstream ss;
		ss << "\x0FTime: ";
		ss << Broodwar->elapsedTime() / 60;
		ss << ":";
		int sec = Broodwar->elapsedTime() % 60;
		if (sec < 10) ss << "0";
		ss << sec;

		Broodwar->drawTextScreen(110,5, ss.str().c_str());
		//

		//Show pathfinder version
		stringstream st;
		st << "\x0FPathfinder: ";
		if (NavigationAgent::pathfinding_version == 0)
		{
			st << "Built-in";
		}
		if (NavigationAgent::pathfinding_version == 1)
		{
			st << "Hybrid Boids";
		}
		if (NavigationAgent::pathfinding_version == 2)
		{
			st << "Hybrid PF";
		}
		
		Broodwar->drawTextScreen(500,310, st.str().c_str());
		//

		StrategySelector::getInstance()->printInfo();

		if (debugBP)
		{
			BuildingPlacer::getInstance()->debug();
		}
		drawTerrainData();

		Commander::getInstance()->debug_showGoal();

		Agentset agents = AgentManager::getInstance()->getAgents();
		for (auto &a : agents)
		{
			if (a->isBuilding()) a->debug_showGoal();
		}
		
		//Show goal info for selected units
		if (Broodwar->getSelectedUnits().size() > 0)
		{
			for (auto &u : Broodwar->getSelectedUnits())
			{
				int unitID = (u)->getID();
				BaseAgent* agent = AgentManager::getInstance()->getAgent(unitID);
				if (agent != NULL && agent->isAlive())
				{
					agent->debug_showGoal();
				}
			}
		}

		if (debugBP) 
		{
			//If we have any unit selected, use that to show PFs.
			if (Broodwar->getSelectedUnits().size() > 0)
			{
				for (auto &u : Broodwar->getSelectedUnits())
				{
					int unitID = u->getID();
					BaseAgent* agent = AgentManager::getInstance()->getAgent(unitID);
					if (agent != NULL)
					{
						NavigationAgent::getInstance()->displayPF(agent);
					}
					break;
				}
			}
		}

		if (debugUnit) 
		{
			//If we have any unit selected, show unit info.
			if (Broodwar->getSelectedUnits().size() > 0)
			{
				for (auto &u : Broodwar->getSelectedUnits())
				{
					int unitID = u->getID();
					BaseAgent* agent = AgentManager::getInstance()->getAgent(unitID);
					if (agent != NULL)
					{
						agent->printInfo();
						break;
					}
				}
			}
		}

		if (debugSQ >= 0)
		{
			Squad* squad = Commander::getInstance()->getSquad(debugSQ);
			if (squad != NULL) 
			{
				squad->printInfo();
			}
		}

		Upgrader::getInstance()->printInfo();
		Commander::getInstance()->printInfo();
	}
}

void AIloop::drawTerrainData()
{
	//we will iterate through all the base locations, and draw their outlines.
	for(BaseLocation* base : getBaseLocations())
	{
		TilePosition p = base->getTilePosition();
		Position c = base->getPosition();

		//Draw a progress bar at each resource
		for (auto &u : Broodwar->getStaticMinerals())
		{
			if (u->getResources() > 0)
			{
				
				int total = u->getInitialResources();
				int done = u->getResources();

				int w = 60;
				int h = 64;

				//Start 
				Position s = Position(u->getPosition().x - w/2 + 2, u->getPosition().y - 4);
				//End
				Position e = Position(s.x + w, s.y + 8);
				//Progress
				int prg = (int)((double)done / (double)total * w);
				Position p = Position(s.x + prg, s.y +  8);

				Broodwar->drawBoxMap(s.x, s.y, e.x, e.y, Colors::Orange,false);
				Broodwar->drawBoxMap(s.x, s.y, p.x, p.y, Colors::Orange,true);
			}
		}
	}

	if (debugBP)
	{
		//we will iterate through all the regions and draw the polygon outline of it in white.
		for(BWTA::Region* r : BWTA::getRegions())
		{
			BWTA::Polygon p = r->getPolygon();
			for(int j = 0; j < (int)p.size(); j++)
			{
				Position point1=p[j];
				Position point2=p[(j+1) % p.size()];
				Broodwar->drawLineMap(point1.x, point1.y, point2.x, point2.y, Colors::Orange);
			}
		}

		//we will visualize the chokepoints with yellow lines
		for(BWTA::Region* r : BWTA::getRegions())
		{
			for(BWTA::Chokepoint* c : r->getChokepoints())
			{
				Position point1 = c->getSides().first;
				Position point2 = c->getSides().second;
				Broodwar->drawLineMap(point1.x, point1.y, point2.x, point2.y, Colors::Yellow);
			}
		}
	}

	//locate zerg eggs and draw progress bars
	if (Constructor::isZerg())
	{
		for (auto &u : Broodwar->getAllUnits())
		{
			if (u->getType().getID() == UnitTypes::Zerg_Egg.getID() || u->getType().getID() == UnitTypes::Zerg_Lurker_Egg.getID() || u->getType().getID() == UnitTypes::Zerg_Cocoon.getID())
			{
				int total = u->getBuildType().buildTime();
				int done = total - u->getRemainingBuildTime();
				
				int w = u->getType().tileWidth() * 32;
				int h = u->getType().tileHeight() * 32;

				//Start 
				Position s = Position(u->getPosition().x - w/2, u->getPosition().y - 4);
				//End
				Position e = Position(s.x + w, s.y + 8);
				//Progress
				int prg = (int)((double)done / (double)total * w);
				Position p = Position(s.x + prg, s.y + 8);

				Broodwar->drawBoxMap(s.x, s.y, e.x, e.y, Colors::Blue,false);
				Broodwar->drawBoxMap(s.x, s.y, p.x, p.y, Colors::Blue,true);
			}
		}
	}
}
