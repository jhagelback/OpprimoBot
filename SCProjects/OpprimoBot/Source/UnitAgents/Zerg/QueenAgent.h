#ifndef __QUEENAGENT_H__
#define __QUEENAGENT_H__

#include "../UnitAgent.h"

/** 
 * The QueenAgent handles abilities for Zerg Queen units.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class QueenAgent : public UnitAgent {

private:
	
public:
	QueenAgent(Unit mUnit) : UnitAgent(mUnit)
	{
		agentType = "QueenAgent";
	}

	bool useAbilities();
};

#endif
