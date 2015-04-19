#include "BattlecruiserAgent.h"
#include "../../MainAgents/TargetingAgent.h"

bool BattlecruiserAgent::useAbilities()
{
	//To prevent order spamming
	lastOrderFrame = Broodwar->getFrameCount();

	if (Broodwar->getFrameCount() - lastUseFrame >= 80 && (unit->isIdle() || unit->isMoving()))
	{
		TechType gun = TechTypes::Yamato_Gun;
		if (Broodwar->self()->hasResearched(gun))
		{
			if (unit->getEnergy() >= gun.energyCost())
			{
				int range = gun.getWeapon().maxRange();
				
				Unit target = TargetingAgent::findHighprioTarget(this, range, true, true);
				if (target != NULL)
				{
					Broodwar << "Yamato Gun used on " << target->getType().getName() << endl;
					unit->useTech(gun, target);
					lastUseFrame = Broodwar->getFrameCount();
					return true;
				}
			}
		}
	}

	return false;
}
