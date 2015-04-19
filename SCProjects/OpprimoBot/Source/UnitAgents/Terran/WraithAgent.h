#ifndef __WRAITHAGENT_H__
#define __WRAITHAGENT_H__

#include "../UnitAgent.h"

/** 
 * The WraithAgent handles abilities for Terran Wraith flying units.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class WraithAgent : public UnitAgent {

private:
	
public:
	WraithAgent(Unit mUnit) : UnitAgent(mUnit)
	{
		agentType = "WraithAgent";
	}

	bool useAbilities();
};

#endif
