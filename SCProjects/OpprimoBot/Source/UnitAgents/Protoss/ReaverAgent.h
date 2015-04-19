#ifndef __REAVERAGENT_H__
#define __REAVERAGENT_H__

#include "../UnitAgent.h"

/** 
 * The ReaverAgent handles abilities for Protoss Reaver units.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class ReaverAgent : public UnitAgent {

private:

public:
	ReaverAgent(Unit mUnit) : UnitAgent(mUnit)
	{
		agentType = "ReaverAgent";
	}

	bool useAbilities();
};

#endif
