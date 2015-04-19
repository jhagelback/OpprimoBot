#include "CarrierAgent.h"

bool CarrierAgent::useAbilities()
{
	if (Broodwar->canMake(UnitTypes::Protoss_Interceptor, unit))
	{
		if (unit->train(UnitTypes::Protoss_Interceptor))
		{
			return true;
		}
	}

	return false;
}
