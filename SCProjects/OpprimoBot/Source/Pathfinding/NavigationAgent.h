#ifndef __NAVIGATIONAGENT_H__
#define __NAVIGATIONAGENT_H__

#include "../MainAgents/BaseAgent.h"

using namespace BWAPI;
using namespace std;

/** The bot uses two techniques for navigation: if no enemy units are close units navigate using the built in pathfinder in
 * Starcraft. If enemy units are close, own units uses potential fields to engage and surround the enemy.
 * The NavigationAgent class is the main class for the navigation system, and it switches between built in pathfinder and
 * potential fields when necessary.
 *
 * The NavigationAgent is implemented as a singleton class. Each class that needs to access NavigationAgent can request an instance,
 * and all classes shares the same NavigationAgent instance.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class NavigationAgent {

private:
	NavigationAgent();
	static NavigationAgent* instance;
	static bool instanceFlag;

	/** Moves a unit to the specified goal using the pathfinder, and stops at a distance where the
	* potential field navigation system should be used instead. */
	bool moveToGoal(BaseAgent* agent, TilePosition checkpoint, TilePosition goal);
	
	/** Calculates the potential field values for an attacking unit. */
	float getAttackingUnitP(BaseAgent* agent, WalkPosition wp);
	float getDefendingUnitP(BaseAgent* agent, WalkPosition wp);
	
	int getMaxUnitSize(UnitType type);

	int checkRange;
	int mapW;
	int mapH;

	Color getColor(float p);

public:
	/** 
	 * Used to switch between the three pathfinding systems:
	 * - Hybrid pathfinding with potential fields
	 * - Hybrid pathfinding with boids
	 * - Non-hybrid pathfinding
	*/
	static int pathfinding_version;

	/** Destructor */
	~NavigationAgent();

	/** Returns the instance to the class. */
	static NavigationAgent* getInstance();

	/** Is used to compute and execute movement commands for attacking units using the potential field
	 * navigation system. */
	bool computeMove(BaseAgent* agent, TilePosition goal);

	/** Computes a pathfinding move (no enemy units in range) */
	bool computePathfindingMove(BaseAgent* agent, TilePosition goal);
	
	/** Computes a PF move (enemy units within range) */
	bool computePotentialFieldMove(BaseAgent* agent);
	
	/** Computes a boids move (enemy units within range) */
	bool computeBoidsMove(BaseAgent* agent);

	/** Displays a debug view of the potential fields for an agent. */
	void displayPF(BaseAgent* agent);
};

#endif
