#ifndef __DEFILERAGENT_H__
#define __DEFILERAGENT_H__

#include "../UnitAgent.h"

/** 
 * The DefilerAgent handles abilities for Zerg Defiler units.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class DefilerAgent : public UnitAgent {

private:
	int darkSwarmFrame;

public:
	DefilerAgent(Unit mUnit) : UnitAgent(mUnit)
	{
		agentType = "DefilerAgent";
		darkSwarmFrame = 0;
	}

	bool useAbilities();
};

#endif
