#ifndef __COMMANDER_H__
#define __COMMANDER_H__

#include "Squad.h"
#include "../MainAgents/BaseAgent.h"
#include "../Managers/BuildplanEntry.h"
#include <BWTA.h>

using namespace BWAPI;
using namespace BWTA;
using namespace std;

struct SortSquadList {
	bool operator()(Squad*& sq1, Squad*& sq2)
	{
		if (sq1->getPriority() != sq2->getPriority())
		{
			return sq1->getPriority() < sq2->getPriority();
		}
		else
		{
			if (sq1->isRequired() && !sq2->isRequired()) return true;
			else return false;
		}
	}
};

/** The Commander class is the base class for commanders. The Commander classes are responsible for
 * which and when buildings to construct, when to do upgrades/techs, and which squads to build.
 * It is also responsible for finding defensive positions, launch attacks and where to launch an
 * attack.
 *
 * The Commander is implemented as a singleton class. Each class that needs to access Commander can
 * request an instance, and the correct commander (Terran/Protoss/Zerg) will be returned.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class Commander {

private:
	bool chokePointFortified(TilePosition center);
	void sortSquadList();
	TilePosition findDefensePos(const Chokepoint* choke);
	
	void checkNoSquadUnits();
	void assignUnit(BaseAgent* agent);

	int lastCallFrame;
	bool removalDone;

protected:
	static Commander* instance;
	int stage;

	int currentState;
	bool debug_bp;
	bool debug_sq;
	static const int DEFEND = 0;
	static const int ATTACK = 1;
	
	vector<Squad*> squads;
	vector<BuildplanEntry> buildplan;
	int noWorkers;
	int noWorkersPerRefinery;

	Commander();	

	/** Checks the Commander buildplan, and add buildings,
	 * techs and upgrades to the planners. */
	void checkBuildplan();

	/** Stops the production of workers. */
	void cutWorkers();

public:
	/** Executes basic code for a commander. */
	void computeActionsBase();

	/** Destructor. */
	~Commander();

	/** Returns the instance of the class. */
	static Commander* getInstance();

	/** Switch on/off buildplan debug info printing to screen. */
	void toggleBuildplanDebug();

	/** Switch on/off squads debug info printing to screen. */
	void toggleSquadsDebug();

	/** Called each update to issue orders. */
	virtual void computeActions() {}

	/** Returns the number of preferred workers, i.e. the
	 * number of workers should be built. */
	int getNoWorkers();

	/** Returns the preferred number of workers for a refinery. */
	int getWorkersPerRefinery();

	/** Used in debug modes to show goal of squads. */
	virtual void debug_showGoal();

	/** Checks if it is time to engage the enemy. This happens when all Required squads
	 * are active. */
	bool shallEngage();

	/** Updates the goals for all squads. */
	void updateGoals();

	/** Called each time a unit is created. The unit is then
	 * placed in a Squad. */
	void unitCreated(BaseAgent* agent);

	/** Called each time a unit is destroyed. The unit is then
	 * removed from its Squad. */
	void unitDestroyed(BaseAgent* agent);

	/* Checks if the specified unittype needs to be built. */
	bool needUnit(UnitType type);

	/** Removes a squad. */
	void removeSquad(int id);

	/** Adds a new squad. */
	void addSquad(Squad* sq);

	/** Returns the Squad with the specified id, or NULL if not found. */
	Squad* getSquad(int id);

	/** Returns the position where to launch an attack at. */
	TilePosition findAttackPosition();
	
	/** Checks if workers needs to attack. Happens if base is under attack and no offensive units
	 * are available. */
	bool checkWorkersAttack(BaseAgent* base);

	/** Checks if we need to assist a building. */
	bool assistBuilding();

	/** Checks if we need to assist a worker that is under attack. */
	bool assistWorker();

	/** Checks if there are any removable obstacles nearby, i.e. minerals with less than 20 resources
	 * left. */
	void checkRemovableObstacles();

	/** Forces an attack, even if some squads are not full. */
	void forceAttack();

	/** Shows some info on the screen. */
	void printInfo();

	/** Searches for and returns a good chokepoint position to defend the territory. */
	TilePosition findChokePoint();

	/** Checks if there are any unfinished buildings that does not have an SCV working on them. Terran only. */
	bool checkDamagedBuildings();

	/** Assigns a worker to finish constructing an interrupted building. Terran only. */
	void finishBuild(BaseAgent* agent);

	/** Adds a bunker squad when a Terran Bunker has been created. Returns
	 * the squadID of the bunker squad. */
	int addBunkerSquad();

	/** Removes a bunker squad. Used when the bunker is destroyed
	 * with units inside. */
	bool removeBunkerSquad(int unitID);

	/** Removes the race from a string, Terran Marine = Marine. */
	static string format(string str);
};

#endif
