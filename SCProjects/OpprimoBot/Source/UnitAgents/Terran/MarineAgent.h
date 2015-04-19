#ifndef __MARINEAGENT_H__
#define __MARINEAGENT_H__

#include "../UnitAgent.h"

/** 
 * The MarineAgent handles abilities for Terran Marine units.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class MarineAgent : public UnitAgent
{

private:

public:
	MarineAgent(Unit mUnit) : UnitAgent(mUnit)
	{
		agentType = "MarineAgent";
	}

	bool useAbilities();
};

#endif
