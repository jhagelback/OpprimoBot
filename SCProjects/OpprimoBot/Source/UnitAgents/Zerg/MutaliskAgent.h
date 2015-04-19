#ifndef __MUTALISKAGENT_H__
#define __MUTALISKAGENT_H__

#include "../UnitAgent.h"

/** 
 * The MutaliskAgent handles abilities for Zerg Mutalisk units.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class MutaliskAgent : public UnitAgent {

private:
	
public:
	MutaliskAgent(Unit mUnit) : UnitAgent(mUnit)
	{
		agentType = "MutaliskAgent";
	}

	bool useAbilities();
};

#endif
