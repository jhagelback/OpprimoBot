#ifndef __FIREBATAGENT_H__
#define __FIREBATAGENT_H__

#include "../UnitAgent.h"

/** 
 * The FirebatAgent handles abilities for Terran Firebat units.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class FirebatAgent : public UnitAgent {

private:

public:
	FirebatAgent(Unit mUnit) : UnitAgent(mUnit)
	{
		agentType = "FirebatAgent";
	}

	bool useAbilities();
};

#endif
