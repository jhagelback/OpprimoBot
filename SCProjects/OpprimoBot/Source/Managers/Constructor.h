#ifndef __CONSTRUCTOR_H__
#define __CONSTRUCTOR_H__

#include <BWAPI.h>
using namespace BWAPI;
using namespace std;

struct BuildQueueItem {
	UnitType toBuild;
	int assignedFrame;
	int assignedWorkerId;
};

/** The Constructor class is resonsible for constructing the buildings added to the
 * build queue.
 *
 * The Constructor is implemented as a singleton class. Each class that needs to access Constructor can request an instance,
 * and all classes shares the same Constructor instance.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class Constructor {

private:
	static Constructor* instance;

protected:
	Constructor();
	vector<UnitType> buildPlan;
	vector<BuildQueueItem> buildQueue;
	void lock(int buildOrderIndex, int unitId);
	bool executeOrder(UnitType type);
	bool shallBuildSupplyDepot();
	string format(UnitType type);
	/** Checks if we currently is constructing a building */
	bool isBeingBuilt(UnitType type);
	
	bool hasResourcesLeft();
	int mineralsNearby(TilePosition center);

	int lastCommandCenter;

	int lastCallFrame;

public:
	/** Destructor. */
	~Constructor();

	/** Returns the instance to the Constructor that is currently used. */
	static Constructor* getInstance();

	/** Returns the number of entries in the build plan. */
	int buildPlanLength();

	/** Returns the number of units of the specified type currently being produced. */
	int noInProduction(UnitType type);

	/** Checks if we have, or is currently building, or planning to build a structure. */
	bool needBuilding(UnitType type);

	/** Called each update to issue orders. */
	void computeActions();

	/** Notifies that an own unit has been destroyed. */
	void buildingDestroyed(Unit building);

	/** When a request to construct a new building is issued, no construction are
	 * allowed until the worker has moved to the buildspot and started constructing
	 * the building. This is to avoid that the needed resources are not used up by
	 * other build orders. During this time the Constructor is locked, and new 
	 * construction can only be done when unlock has been called. */
	void unlock(UnitType type);

	/** Removes a building from the buildorder. */
	void remove(UnitType type);

	/** Called when a worker that is constructing a building is destroyed. */
	void handleWorkerDestroyed(UnitType type, int workerID);

	/** Sets that a new command center has been built. */
	void commandCenterBuilt();

	/** Shows some debug info on screen. */
	void printInfo();

	/** Is called when no buildspot has been found for the specified type. Gives each Constructor
	 * an opportunity to handle it. */
	void handleNoBuildspotFound(UnitType toBuild);

	/** Checks if more supply buildings are needed. */
	bool shallBuildSupply();

	/** Checks if a supply is under construction. */
	bool supplyBeingBuilt();

	/** Returns true if next in buildorder is of the specified type. Returns false if
	 * buildorder is empty. */
	bool nextIsOfType(UnitType type);

	/** Returns true if buildorder contains a unit of the specified type. */
	bool containsType(UnitType type);

	/** Adds a building to the buildorder queue. */
	void addBuilding(UnitType type);

	/** Adds a building first in the buildorder queue. */
	void addBuildingFirst(UnitType type);

	/** Requests to expand the base. */
	void expand(UnitType commandCenterUnit);

	/** Checks if next building in the buildqueue is an expansion. */
	bool nextIsExpand();

	/** Adds a refinery to the buildorder list. */
	void addRefinery();

	/** Checks if the specified TilePosition is covered by a detector buildings sight radius. */
	static bool coveredByDetector(TilePosition pos);

	/** Morphs a Zerg drone to a building. */
	bool executeMorph(UnitType target, UnitType evolved);

	/** Returns true if the player is Terran. */
	static bool isTerran();

	/** Returns true if the player is Protoss. */
	static bool isProtoss();

	/** Returns true if the player is Zerg. */
	static bool isZerg();
};

#endif
