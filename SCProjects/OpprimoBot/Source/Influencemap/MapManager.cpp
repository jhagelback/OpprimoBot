#include "MapManager.h"
#include "../Managers/AgentManager.h"
#include "../Managers/ExplorationManager.h"
#include "../MainAgents/BaseAgent.h"

MapManager* MapManager::instance = NULL;

MapManager::MapManager()
{
	//Add the regions for this map
	for(BWTA::Region* r : BWTA::getRegions())
	{
		MRegion* mr = new MRegion();
		mr->region = r;
		mr->resetInfluence();
		map.insert(mr);
	}
	lastCallFrame = 0;

	//Add base locations for this map
	for (BaseLocation* base : BWTA::getBaseLocations())
	{
		bases.insert(new BaseLocationItem(base->getTilePosition()));
	}
}

const MRegion* MapManager::getMapRegion(const BWTA::Region* r)
{
	for (MRegion* mr : map)
	{
		if (mr->region->getCenter().x == r->getCenter().x && mr->region->getCenter().y == r->getCenter().y)
		{
			return mr;
		}
	}
	return NULL;
}

bool MapManager::isValidChokepoint(const Chokepoint* cp)
{
	//Use this code to hard-code chokepoints that shall not be used.
	if (Broodwar->mapFileName() == "(4)Andromeda.scx")
	{
		Position c = cp->getCenter();
		if (c.x == 2780 && c.y == 3604) return false;
		if (c.x == 2776 && c.y == 448) return false;
		if (c.x == 1292 && c.y == 436) return false;
		if (c.x == 1300 && c.y == 3584) return false;
	}
	if (Broodwar->mapFileName() == "(2)Benzene.scx")
	{
		Position c = cp->getCenter();
		if (c.x == 4044 && c.y == 1088) return false;
		if (c.x == 44 && c.y == 1064) return false;
	}
	if (Broodwar->mapFileName() == "(2)Destination.scx")
	{
		Position c = cp->getCenter();
		if (c.x == 1309 && c.y == 3851) return false;
		if (c.x == 1730 && c.y == 226) return false;
	}
	if (Broodwar->mapFileName() == "(4)Fortress.scx")
	{
		Position c = cp->getCenter();
		if (c.x == 3132 && c.y == 912) return false;
		if (c.x == 764 && c.y == 3312) return false;
	}

	return true;
}

const BWTA::Chokepoint* MapManager::findGuardChokepoint(const MRegion* mr)
{
	for(Chokepoint* c : mr->region->getChokepoints())
	{
		if (isValidChokepoint(c))
		{
			pair<BWTA::Region*,BWTA::Region*> regions = c->getRegions();
			if (regions.first->getCenter().x == mr->region->getCenter().x && regions.first->getCenter().y == mr->region->getCenter().y)
			{
				const MRegion* adj = getMapRegion(regions.second);
				if (adj->inf_own_buildings == 0)
				{
					return c;
				}
			}

			if (regions.second->getCenter().x == mr->region->getCenter().x && regions.second->getCenter().y == mr->region->getCenter().y)
			{
				const MRegion* adj = getMapRegion(regions.first);
				if (adj->inf_own_buildings == 0)
				{
					return c;
				}
			}
		}
	}
	return NULL;
}

const BWTA::Chokepoint* MapManager::getDefenseLocation()
{
	int bestInfluence = 0;
	const BWTA::Chokepoint* best = NULL;

	for (MRegion* base : map)
	{
		if (base->inf_own_buildings > bestInfluence)
		{
			const BWTA::Chokepoint* cp = findGuardChokepoint(base);
			if (cp != NULL)
			{
				if (base->inf_own_buildings > bestInfluence)
				{
					bestInfluence = base->inf_own_buildings;
					best = cp;
				}
			}
		}
	}

	return best;
}

MRegion* MapManager::getMapFor(Position p)
{
	for (auto &mr : map)
	{
		if (mr->region->getPolygon().isInside(p))
		{
			return mr;
		}
	}
	return NULL;
}

bool MapManager::hasEnemyInfluence()
{
	for (auto &mr : map)
	{
		if (mr->inf_en_buildings > 0)
		{
			return true;
		}
	}
	return false;
}

void MapManager::update()
{
	//Dont call too often
	int cFrame = Broodwar->getFrameCount();
	if (cFrame - lastCallFrame < 10)
	{
		return;
	}
	lastCallFrame = cFrame;

	//Reset previous influence scores
	for (MRegion* mr : map)
	{
		mr->resetInfluence();
	}

	//Update visited base locations
	for (auto &a : bases)
	{
		if (Broodwar->isVisible(a->baseLocation))
		{
			a->frameVisited = Broodwar->getFrameCount();
		}
	}

	//Update own influence
	Agentset agents = AgentManager::getInstance()->getAgents();
	for (auto &a : agents)
	{
		if (a->isAlive())
		{
			MRegion* mr = getMapFor(a->getUnit()->getPosition());
			if (mr != NULL)
			{
				if (a->getUnitType().isBuilding())
				{
					mr->inf_own_buildings += a->getUnitType().buildScore();
					if (a->getUnitType().canAttack())
					{
						if (a->getUnitType().groundWeapon().targetsGround() || a->getUnitType().airWeapon().targetsGround())
						{
							mr->inf_own_ground += a->getUnitType().buildScore();
						}
						if (a->getUnitType().groundWeapon().targetsAir() || a->getUnitType().airWeapon().targetsAir())
						{
							mr->inf_own_air += a->getUnitType().buildScore();
						}
					}
				}
				else if (a->getUnitType().isAddon())
				{
					//No influence from addons
				}
				else if (a->getUnitType().isWorker())
				{
					//No influence for workers
				}
				else
				{
					//Regular units
					if (a->getUnitType().canAttack())
					{
						if (a->getUnitType().groundWeapon().targetsGround() || a->getUnitType().airWeapon().targetsGround())
						{
							mr->inf_own_ground += a->getUnitType().buildScore();
						}
						if (a->getUnitType().groundWeapon().targetsAir() || a->getUnitType().airWeapon().targetsAir())
						{
							mr->inf_own_air += a->getUnitType().buildScore();
						}
					}
				}
			}
		}
	}

	//Update enemy buildings influence
	for (MRegion* mr : map)
	{
		mr->inf_en_buildings = ExplorationManager::getInstance()->getSpottedInfluenceInRegion(mr->region);
	}

	//Update enemy units influence
	for (auto &u : Broodwar->enemy()->getUnits())
	{
		//Enemy seen
		if (u->exists())
		{
			MRegion* mr = getMapFor(u->getPosition());
			if (mr != NULL)
			{
				UnitType type = u->getType();
				if (!type.isWorker() && type.canAttack())
				{
					if (type.groundWeapon().targetsGround() || type.airWeapon().targetsGround())
					{
						mr->inf_en_ground += type.buildScore();
					}
					if (type.groundWeapon().targetsAir() || type.airWeapon().targetsAir())
					{
						mr->inf_en_air += type.buildScore();
					}
				}
			}
		}
	}
}

int MapManager::getOwnGroundInfluenceIn(TilePosition pos)
{
	for (MRegion* cm : map)
	{
		if (cm->region->getPolygon().isInside(Position(pos)))
		{
			return cm->inf_own_ground;
		}
	}
	return 0;
}

int MapManager::getEnemyGroundInfluenceIn(TilePosition pos)
{
	for (MRegion* cm : map)
	{
		if (cm->region->getPolygon().isInside(Position(pos)))
		{
			return cm->inf_en_ground;
		}
	}
	return 0;
}

bool MapManager::hasOwnInfluenceIn(TilePosition pos)
{
	for (MRegion* cm : map)
	{
		if (cm->inf_own_buildings > 0 && cm->region->getPolygon().isInside(Position(pos)))
		{
			return true;
		}
	}
	return false;
}

bool MapManager::hasEnemyInfluenceIn(TilePosition pos)
{
	for (MRegion* cm : map)
	{
		if (cm->inf_en_buildings > 0 && cm->region->getPolygon().isInside(Position(pos)))
		{
			return true;
		}
	}
	return false;
}

TilePosition MapManager::findAttackPosition()
{
	MRegion* best = NULL;
	for (MRegion* cm : map)
	{
		if (cm->inf_en_buildings > 0)
		{
			if (best == NULL)
			{
				best = cm;
			}
			else
			{
				//Launch an attack at the enemy controlled region with the
				//lowest influence.
				if (cm->inf_en_buildings < best->inf_en_buildings)
				{
					best = cm;
				}
			}
		}
	}
	
	if (best != NULL)
	{
		return TilePosition(best->region->getCenter());
	}
	else
	{
		//No enemy building found. Move to starting positions.
		int longestVisitFrame = Broodwar->getFrameCount();
		TilePosition base = TilePosition(-1, -1);
		for (auto &a : bases)
		{
			if (a->frameVisited < longestVisitFrame)
			{
				longestVisitFrame = a->frameVisited;
				base = a->baseLocation;
			}
		}

		return base;
	}
}

MapManager::~MapManager()
{
	for (MRegion* mr : map)
	{
		delete mr;
	}
	for (auto &a : bases)
	{
		delete a;
	}
	
	instance = NULL;
}

MapManager* MapManager::getInstance()
{
	if (instance == NULL)
	{
		instance = new MapManager();
	}
	return instance;
}


void MapManager::printInfo()
{
	for (MRegion* mr : map)
	{
		int x1 = mr->region->getCenter().x;
		int y1 = mr->region->getCenter().y;
		int x2 = x1 + 110;
		int y2 = y1 + 90;

		Broodwar->drawBoxMap(x1,y1,x2,y2,Colors::Brown,true);
		Broodwar->drawTextMap(x1 + 5, y1, "Buildings own: %d", mr->inf_own_buildings);
		Broodwar->drawTextMap(x1 + 5, y1 + 15, "Ground own: %d", mr->inf_own_ground);
		Broodwar->drawTextMap(x1 + 5, y1 + 30, "Air own: %d", mr->inf_own_air);
		Broodwar->drawTextMap(x1 + 5, y1 + 45, "Buildings en: %d", mr->inf_en_buildings);
		Broodwar->drawTextMap(x1 + 5, y1 + 60, "Ground en: %d", mr->inf_en_ground);
		Broodwar->drawTextMap(x1 + 5, y1 + 75, "Air en: %d", mr->inf_en_air);

		//Print location of each chokepoint, and also if it is blocked
		//as defense position.
		for(Chokepoint* c : mr->region->getChokepoints())
		{
			x1 = c->getCenter().x;
			y1 = c->getCenter().y;
			Broodwar->drawTextMap(x1, y1, "(%d,%d)", x1, y1);
			if (!isValidChokepoint(c)) Broodwar->drawTextMap(x1, y1+12, "Blocked");
		}
		Broodwar->drawTextScreen(10, 120, "'%s'", Broodwar->mapFileName().c_str());
	}
}
