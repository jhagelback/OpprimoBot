#ifndef __TARGETINGAGENT_H__
#define __TARGETINGAGENT_H__

#include <BWAPI.h>
#include "BaseAgent.h"

using namespace BWAPI;
using namespace std;

/** This agent is used to find the best target to attack for a unit. 
 * It is possible to add several rules, for example always target workers first.
 *
 * Currently the most expensive target (highest destroyscore) in range is targeted.
 * Units and buildings that cannot attack back get reduced score.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class TargetingAgent {

private:
	static double getTargetModifier(UnitType attacker, UnitType target);
	static bool isCloakingUnit(UnitType type);
	static void handleCloakedUnit(Unit unit);
	static bool isHighprioTarget(UnitType type);

public:
	/** Returns the best target within seekrange for a unit agent, or NULL if no target
	 * was found. */
	static Unit findTarget(BaseAgent* agent);

	/** Returns a high prio target, if any, for strong attacks such as Yamato Gun. */
	static Unit findHighprioTarget(BaseAgent* agent, int maxDist, bool targetsAir, bool targetsGround);

	/** Returns the number of enemy units within range to attack an agent. */
	static int getNoAttackers(BaseAgent* agent);

	/** Checks if the specified type is an attacking unit or building. */
	static bool canAttack(UnitType type);

	/** Checks the current target for an agent. If the agent has a bad target (for example
	 * attacking a building and ignoring attacking units) a target switch is made. */
	static bool checkTarget(BaseAgent* agent);
};

#endif
