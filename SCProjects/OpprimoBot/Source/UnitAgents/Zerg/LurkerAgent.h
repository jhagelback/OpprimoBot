#ifndef __LURKERAGENT_H__
#define __LURKERAGENT_H__

#include "../UnitAgent.h"

/** 
 * The LurkerAgent handles abilities for Zerg Lurker units.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class LurkerAgent : public UnitAgent {

private:
	
public:
	LurkerAgent(Unit mUnit) : UnitAgent(mUnit)
	{
		agentType = "LurkerAgent";
	}

	bool useAbilities();
};

#endif
