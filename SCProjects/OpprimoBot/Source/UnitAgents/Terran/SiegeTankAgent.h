#ifndef __SIEGETANKAGENT_H__
#define __SIEGETANKAGENT_H__

#include "../UnitAgent.h"

/** 
 * The SiegeTankAgent handles abilities for Terran Siege Tank units.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class SiegeTankAgent : public UnitAgent {
	
public:
	SiegeTankAgent(Unit mUnit) : UnitAgent(mUnit)
	{
		agentType = "SiegeTankAgent";
	}

	bool useAbilities();
};

#endif
