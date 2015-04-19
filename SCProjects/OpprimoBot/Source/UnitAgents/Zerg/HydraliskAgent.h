#ifndef __HYDRALISKAGENT_H__
#define __HYDRALISKAGENT_H__

#include "../UnitAgent.h"

/** 
 * The HydraliskAgent handles abilities for Zerg Hydralisk units.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class HydraliskAgent : public UnitAgent {

private:

public:
	HydraliskAgent(Unit mUnit) : UnitAgent(mUnit)
	{
		agentType = "HydraliskAgent";
	}

	bool useAbilities();
};

#endif
