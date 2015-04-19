#ifndef __EXPLORATIONMANAGER_H__
#define __EXPLORATIONMANAGER_H__

#include "SpottedObject.h"
#include "../Commander/Squad.h"
#include <BWTA.h>

using namespace BWAPI;
using namespace BWTA;
using namespace std;

class SpottedObjectSet : public SetContainer<SpottedObject*, std::hash<void*>>
{
public:

};

class RegionItem
{
public:
	RegionItem(BWTA::Region* region)
	{
		location = TilePosition(region->getCenter());
		frameVisited = Broodwar->getFrameCount();
	};

	TilePosition location;
	int frameVisited;
};

class RegionSet : public SetContainer<RegionItem*, std::hash<void*>>
{
public:

};

/** The ExplorationManager handles all tasks involving exploration of the game world. It issue orders to a number of units
 * that is used as explorers, keep track of areas recently explored, and keep track of spotted resources or enemy buildings.
 *
 * The ExplorationManager is implemented as a singleton class. Each class that needs to access ExplorationManager can request an instance,
 * and all classes shares the same ExplorationManager instance.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class ExplorationManager {

private:
	SpottedObjectSet enemy;
	RegionSet explore;
	
	ExplorationManager();
	static ExplorationManager* instance;
	
	void cleanup();

	int lastCallFrame;

	int siteSetFrame;
	TilePosition expansionSite;

public:
	/** Destructor */
	~ExplorationManager();

	/** Returns the instance of the class. */
	static ExplorationManager* getInstance();

	/** Called each update to issue orders. */
	void computeActions();

	/** Returns the next position to explore for this squad. */
	TilePosition getNextToExplore(Squad* squad);

	/** Searches for the next position to expand the base to. */
	TilePosition searchExpansionSite();

	/** Returns the next position to expand the base to. */
	TilePosition getExpansionSite();

	/** Sets the next position to expand the base to. */
	void setExpansionSite(TilePosition pos);

	/** Shows all spotted objects as squares on the SC map. Use for debug purpose. */
	void printInfo();

	/** Notifies about an enemy unit that has been spotted. */
	void addSpottedUnit(Unit unit);

	/** Notifies that an enemy unit has been destroyed. If the destroyed unit was among
	 * the spotted units, it is removed from the list. */
	void unitDestroyed(Unit unit);

	/** Returns the closest enemy spotted building from a start position, or TilePosition(-1,-1) if 
	 * none was found. */
	TilePosition getClosestSpottedBuilding(TilePosition start);

	/** Calculates the influence of spotted enemy buildings within a specified region. */
	int getSpottedInfluenceInRegion(const BWTA::Region* region);

	/** Returns true if a ground unit can reach position b from position a.
	 * Uses BWTA. */
	static bool canReach(TilePosition a, TilePosition b);

	/** Returns true if an agent can reach position b. */
	static bool canReach(BaseAgent* agent, TilePosition b);

	/** Sets that a region is explored. The position must be the TilePosition for the center of the
	 * region. */
	void setExplored(TilePosition goal);

	/** Returns true if an enemy is Protoss. */
	static bool enemyIsProtoss();

	/** Returns true if an enemy is Zerg. */
	static bool enemyIsZerg();

	/** Returns true if an enemy is Terran. */
	static bool enemyIsTerran();

	/** All enemy races are currently unknown. */
	static bool enemyIsUnknown();
};

#endif
