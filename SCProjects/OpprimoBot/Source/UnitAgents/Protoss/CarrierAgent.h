#ifndef __CARRIERAGENT_H__
#define __CARRIERAGENT_H__

#include "../UnitAgent.h"

/** 
 * The CarrierAgent handles abilities for Protoss Carrier flying units.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class CarrierAgent : public UnitAgent {

private:

public:
	CarrierAgent(Unit mUnit) : UnitAgent(mUnit)
	{
		agentType = "CarrierAgent";
	}

	bool useAbilities();
};

#endif
