#ifndef __CORSAIRAGENT_H__
#define __CORSAIRAGENT_H__

#include "../UnitAgent.h"

/** 
 * The CorsairAgent handles abilities for Protoss Corsair flying units.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class CorsairAgent : public UnitAgent {

private:
	int lastUseFrame;
	Unit getClosestEnemyAirDefense(int maxRange);

public:
	CorsairAgent(Unit mUnit) : UnitAgent(mUnit)
	{
		agentType = "CorsairAgent";
	}

	bool useAbilities();
};

#endif
