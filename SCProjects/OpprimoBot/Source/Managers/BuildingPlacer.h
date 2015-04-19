#ifndef __BUILDINGPLACER_H__
#define __BUILDINGPLACER_H__

#include "../MainAgents/BaseAgent.h"

struct Corners {
	int x1;
	int y1;
	int x2;
	int y2;
};

/** The BuildingPlacer class is used to find positions to place new buildings at. It keeps track
 * of which map tiles are occupied (by own buildings) or unavailable (blocked by terrain, minerals patches etc.).
 *
 * Internally a matrix of the same size as the map is used. If a Tile is occupied or cant be reached by ground
 * units, the value if the tile is 0. If the Tile can be built on, the value is 1. 
 * Buildings typically use up more space in the matrix than their actual size since we want some free space
 * around each building. Different types of buildings have different space requirements.
 *
 * The BuildingPlacer is implemented as a singleton class. Each class that needs to access BuildingPlacer can request an instance,
 * and all classes shares the same BuildingPlacer instance.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class BuildingPlacer {

private:
	BuildingPlacer();
	static BuildingPlacer* instance;
	
	int range;
	int w;
	int h;
	int** cover_map;

	Corners getCorners(Unit unit);
	Corners getCorners(UnitType type, TilePosition center);

	TilePosition findSpotAtSide(UnitType toBuild, TilePosition start, TilePosition end);
	bool canBuildAt(UnitType toBuild, TilePosition pos);

	void fill(Corners c);
	void clear(Corners c);

	Unit findWorker(TilePosition spot);

	bool suitableForDetector(TilePosition pos);

	bool baseUnderConstruction(BaseAgent* base);
	bool isDefenseBuilding(UnitType toBuild);
	TilePosition findBuildSpot(UnitType toBuild, TilePosition start);

	Unit hasMineralNear(TilePosition pos);

public:
	/** Destructor */
	~BuildingPlacer();

	/** Returns the instance of the class. */
	static BuildingPlacer* getInstance();

	/** Adds a newly constructed building to the cover map. */
	void addConstructedBuilding(Unit unit);

	/** Used by WorkerAgent when constructing builds. */
	void fillTemp(UnitType toBuild, TilePosition buildSpot);

	/** Used by WorkerAgent when constructing builds. */
	void clearTemp(UnitType toBuild, TilePosition buildSpot);

	/** Called when a building is destroyed, to free up the space. */
	void buildingDestroyed(Unit unit);

	/** Checks if the specified building type can be built at the buildSpot. True if it can,
	 * false otherwise. */
	bool canBuild(UnitType toBuild, TilePosition buildSpot);

	/** Checks if a position is free. */
	bool positionFree(TilePosition pos);

	/** Blocks a position from being used as a valid buildSpot. Used when a worker is timedout when
	 * moving towards the buildSpot. */
	void blockPosition(TilePosition buildSpot);

	/** Finds and returns a buildSpot for the specified building type.
	 * If no buildspot is found, a TilePosition(-1,-1) is returned. */
	TilePosition findBuildSpot(UnitType toBuild);

	/** Searches for the closest vespene gas that is not in use. If no gas is sighted,
	 * the ExplorationManager is queried. */
	TilePosition findRefineryBuildSpot(UnitType toBuild, TilePosition start);

	/** Finds and returns the position of the closest free vespene gas around the specified start position.
	 * If no gas vein is found, a TilePosition(-1, -1) is returned. */
	TilePosition findClosestGasWithoutRefinery(UnitType toBuild, TilePosition start);

	/** Searches for a spot to build a refinery at. Returns TilePosition(-1, -1) if no spot was found.*/
	TilePosition searchRefinerySpot();

	/** Returns a position of a suitable site for expansion, i.e. new bases. */
	TilePosition findExpansionSite();

	/** Finds a mineral to gather from. */
	Unit findClosestMineral(TilePosition workerPos);

	/** Shows debug info on screen. */
	void debug();

	/** Tile is buildable. */
	static const int BUILDABLE = 1;
	/** Tile is blocked and cannot be built on. */
	static const int BLOCKED = 0;
	/** Tile is temporary blocked and cannot be built on. */
	static const int TEMPBLOCKED = 4;
	/** Tile contains a mineral vein. */
	static const int MINERAL = 2;
	/** Tile contains a gas vein. */
	static const int GAS = 3;
};

#endif
