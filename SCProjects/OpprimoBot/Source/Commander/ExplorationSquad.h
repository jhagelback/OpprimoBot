#ifndef __EXPLORATIONSQUAD_H__
#define __EXPLORATIONSQUAD_H__

#include "Squad.h"

/** This class handle squads used to explore the game world. Any unit type
 * can be used as explorer.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class ExplorationSquad : public Squad {

private:
	
	
public:
	/** Constructor. See Squad.h for more details. */
	ExplorationSquad(int mId, string mName, int mPriority);

	/** Returns true if this Squad is active, or false if not.
	 * A Squad is active when it first has been filled with agents.
	 * A Squad with destroyed units are still considered Active. */
	bool isActive();

	/** Called each update to issue orders. */
	void computeActions();

	/** Orders this squad to defend a position. */
	void defend(TilePosition mGoal);

	/** Orders this squad to launch an attack at a position. */
	void attack(TilePosition mGoal);

	/** Orders this squad to assist units at a position. */
	void assist(TilePosition mGoal);

	/** Clears the goal for this Squad, i.e. sets the goal
	 * to TilePosition(-1,-1). */
	void clearGoal();

	/** Returns the current goal of this Squad. */
	TilePosition getGoal();

	/** Returns true if this squad has an assigned goal. */
	bool hasGoal();
};

#endif
