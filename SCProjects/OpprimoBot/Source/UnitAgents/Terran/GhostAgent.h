#ifndef __GHOSTAGENT_H__
#define __GHOSTAGENT_H__

#include "../UnitAgent.h"

/** 
 * The GhostAgent handles abilities for Terran Ghost units.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class GhostAgent : public UnitAgent {

private:
	Unit findLockdownTarget();
	/** Returns the number of own units that are within maxRange of the agent. */
	int friendlyUnitsWithinRange(int maxRange);

public:
	GhostAgent(Unit mUnit) : UnitAgent(mUnit)
	{
		agentType = "GhostAgent";
	}

	bool useAbilities();
};

#endif
