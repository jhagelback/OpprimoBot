#ifndef __HIGHTEMPLARAGENT_H__
#define __HIGHTEMPLARAGENT_H__

#include "../UnitAgent.h"

/** 
 * The HighTemplarAgent handles abilities for Protoss High Templar units.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class HighTemplarAgent : public UnitAgent {

private:
	Unit findPsiStormTarget();
	BaseAgent* findHallucinationTarget();
	BaseAgent* findArchonTarget();
	bool hasCastTransform;
	/** Returns the number of own units that are within maxRange of the specified tile. */
	int friendlyUnitsWithinRange(TilePosition tilePos, int maxRange);

public:
	HighTemplarAgent(Unit mUnit) : UnitAgent(mUnit)
	{
		agentType = "HighTemplarAgent";
	}

	bool useAbilities();
};

#endif
