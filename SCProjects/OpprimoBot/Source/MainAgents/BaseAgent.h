#ifndef __BASEAGENT_H__
#define __BASEAGENT_H__

#include <windows.h>
#include <BWAPI.h>
#include <vector>
using namespace BWAPI;
using namespace std;

/** The BaseAgent is the base agent class all agent classes directly or indirectly must extend. It contains some
 * common logic that is needed for several other agent implementations.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class BaseAgent {

private:

protected:
	Unit unit;
	UnitType type;
	TilePosition goal;
	int unitID;
	int squadID;
	bool alive;
	bool bBlock;
	string agentType;

	int infoUpdateFrame;
	int infoUpdateTime;
	int sx;
	int sy;

	int lastOrderFrame;

	vector<WalkPosition> trail;

	/** Removes race name from a string (Terran Marine -> Marine) */
	string format(string str);

public:
	/** Default constructor. Not used. */
	BaseAgent();
	/** Constructor. */
	BaseAgent(Unit mUnit);
	/** Destructor. */
	virtual ~BaseAgent();

	/** Returns the frame the last order for the unit was issued. */
	int getLastOrderFrame();

	/** Called each update to issue orders. */
	virtual void computeActions()
	{
	}

	/** Used in debug modes to show a line to the agents' goal. */
	virtual void debug_showGoal()
	{
	}

	/** Checks if there are any enemy units within sight range. */
	bool enemyUnitsVisible();

	/** Sets the goal for this unit. Goals are set from either the SquadCommander for attacking
	 * or defending units, or from ExplorationManager for explore units. */
	void setGoal(TilePosition goal);

	/** Clears the goal for this unit. */
	void clearGoal();

	/** Returns the current goal for this unit. */
	TilePosition getGoal();

	/** Returns the unique type name for the agent type. */
	string getTypeName();

	/** Used to print info about this agent to the screen. */
	virtual void printInfo();

	/** Returns the unique id for this agent. Agent id is the same as the id of the unit
	 * assigned to the agent. */
	int getUnitID();

	/** Returns the type for the unit handled by this agent. */
	UnitType getUnitType();

	/** Returns a reference to the unit assigned to this agent. */
	Unit getUnit();

	/** Called when the unit assigned to this agent is destroyed. */
	void destroyed();

	/** Returns true if this agent is active, i.e. the unit is not destroyed. */
	bool isAlive();

	/** Returns true if the specified unit is the same unit assigned to this agent. */
	bool matches(Unit mUnit);

	/** Returns true if the agent is of the specified type. */
	bool isOfType(UnitType type);

	/** Returns true if mType is the same UnitType as toCheckType. */
	static bool isOfType(UnitType mType, UnitType toCheckType);

	/** Checks if there are any enemy detector units withing range of the
	 * specified position. True if there is, false if not. */
	bool isDetectorWithinRange(TilePosition pos, int range);

	/** Returns true if this agent is a building. */
	bool isBuilding();

	/** Returns true if this agent is a worker. */
	bool isWorker();

	/** Returns true if this agent is a free worker, i.e. is idle or is gathering minerals. */
	virtual bool isFreeWorker()
	{
		return false;
	}

	/** Returns true if this agent is a combat unit. */
	bool isUnit();

	/** Returns true if this agent is under attack, i.e. lost hitpoints since last check. */
	bool isUnderAttack();

	/** Returns true if this agent is damaged. */
	bool isDamaged();

	/** Assigns this agent to the squad with the specified id. */
	void setSquadID(int id);

	/** Returns the squad this agent belongs to, or -1 if it doesnt
	 * belong to any squad. */
	int getSquadID();

	/** Adds a new trail entry to a PF pheromone trail. */
	void addTrailPosition(WalkPosition wt);

	/** Returns the PF pheromone trail. */
	vector<WalkPosition> getTrail();

};

#endif
