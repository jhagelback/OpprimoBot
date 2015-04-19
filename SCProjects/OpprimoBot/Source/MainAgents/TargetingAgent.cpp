#include "TargetingAgent.h"

bool TargetingAgent::canAttack(UnitType type)
{
	if (type.isBuilding())
	{
		if (type.canAttack()) return true;
		return false;
	}
	if (type.isAddon())
	{
		return false;
	}
	if (type.isWorker())
	{
		return false;
	}
	return true;
}

int TargetingAgent::getNoAttackers(BaseAgent* agent)
{
	int cnt = 0;

	for (auto &u : Broodwar->enemy()->getUnits())
	{
		if (canAttack(u->getType()))
		{
			int enemyMSD = 0;
			if (agent->getUnitType().isFlyer())
			{
				enemyMSD = u->getType().airWeapon().maxRange();	
			}
			else
			{
				enemyMSD = u->getType().groundWeapon().maxRange();	
			}

			double d = agent->getUnit()->getPosition().getDistance(u->getPosition());
			if (d <= enemyMSD)
			{
				cnt++;
			}
		}
	}

	return cnt;
}

bool TargetingAgent::checkTarget(BaseAgent* agent)
{
	if (!agent->getUnit()->isIdle() && !agent->getUnit()->isMoving()) return false;

	Unit pTarget = findTarget(agent);
	if (pTarget != NULL && pTarget->getPlayer()->isEnemy(Broodwar->self()))
	{
		bool ok = agent->getUnit()->attack(pTarget, true);
		if (!ok)
		{
			//Broodwar << "Switch target failed: " << Broodwar->getLastError() << endl;
		}
		return ok;
	}
	return false;
}

bool TargetingAgent::isHighprioTarget(UnitType type)
{
	if (type.getID() == UnitTypes::Terran_Bunker.getID()) return true;
	if (type.getID() == UnitTypes::Terran_Battlecruiser.getID()) return true;
	if (type.getID() == UnitTypes::Terran_Missile_Turret.getID()) return true;
	
	if (type.getID() == UnitTypes::Protoss_Carrier.getID()) return true;
	if (type.getID() == UnitTypes::Protoss_Photon_Cannon.getID()) return true;
	if (type.getID() == UnitTypes::Protoss_Archon.getID()) return true;

	if (type.getID() == UnitTypes::Zerg_Sunken_Colony.getID()) return true;
	if (type.getID() == UnitTypes::Zerg_Spore_Colony.getID()) return true;
	if (type.getID() == UnitTypes::Zerg_Ultralisk.getID()) return true;

	return false;
}

Unit TargetingAgent::findHighprioTarget(BaseAgent* agent, int maxDist, bool targetsAir, bool targetsGround)
{
	Unit target = NULL;
	Position cPos = agent->getUnit()->getPosition();
	int bestTargetScore = -10000;

	for (auto &u : Broodwar->enemy()->getUnits())
	{
		if (u->exists())
		{
			UnitType t = u->getType();
			bool targets = isHighprioTarget(t);
			if (t.isFlyer() && !targetsAir) targets = false;
			if (!t.isFlyer() && !targetsGround) targets = false;
			
			if (targets)
			{
				double dist = cPos.getDistance(u->getPosition());
				if (dist <= (double)maxDist)
				{
					if (t.destroyScore() > bestTargetScore)
					{
						target = u;
						bestTargetScore = t.destroyScore();
					}
				}
			}
		}
	}

	return target;
}

Unit TargetingAgent::findTarget(BaseAgent* agent)
{
	//Check if the agent targets ground and/or air
	bool targetsGround = false;
	if (agent->getUnitType().groundWeapon().targetsGround()) targetsGround = true;
	if (agent->getUnitType().airWeapon().targetsGround()) targetsGround = true;

	bool targetsAir = false;
	if (agent->getUnitType().groundWeapon().targetsAir()) targetsAir = true;
	if (agent->getUnitType().airWeapon().targetsAir()) targetsAir = true;

	//Iterate through enemies to select a target
	int bestTargetScore = -10000;
	Unit target = NULL;
	for (auto &u : Broodwar->enemy()->getUnits())
	{
		UnitType t = u->getType();

		bool canAttack = false;
		if (!t.isFlyer() && targetsGround) canAttack = true;
		if ((t.isFlyer() || u->isLifted()) && targetsAir) canAttack = true;
		if (u->isCloaked() && !u->isDetected())
		{
			canAttack = false;
			handleCloakedUnit(u);
		}
		if (u->isBurrowed() && !u->isDetected())
		{
			canAttack = false;
			handleCloakedUnit(u);
		}

		int maxRange = 600;
		if (agent->getUnit()->isSieged() || agent->getUnit()->isBurrowed() || agent->getUnit()->isLoaded()) maxRange = agent->getUnitType().groundWeapon().maxRange();

		if (canAttack && agent->getUnit()->getPosition().getDistance(u->getPosition()) <= maxRange)
		{
			double mod = getTargetModifier(agent->getUnit()->getType(), t);
			int cScore = (int)((double)t.destroyScore() * mod);
			if (u->getHitPoints() < u->getInitialHitPoints())
			{
				//Prioritize damaged targets
				cScore++;
			}

			if (cScore > bestTargetScore)
			{
				bestTargetScore = cScore;
				target = u;
			}
		}
	}

	return target;
}

double TargetingAgent::getTargetModifier(UnitType attacker, UnitType target)
{
	//Non-attacking buildings
	if (target.isBuilding() && !target.canAttack() && !target.getID() == UnitTypes::Terran_Bunker.getID())
	{
		return 0.05;
	}

	//Terran Goliath prefer air targets
	if (attacker.getID() == UnitTypes::Terran_Goliath.getID())
	{
		if (target.isFlyer()) return 2;
	}

	//Siege Tanks prefer to take out enemy defense buildings
	if (attacker.getID() == UnitTypes::Terran_Siege_Tank_Siege_Mode.getID())
	{
		if (target.isBuilding() && target.canAttack()) return 1.5;
		if (target.getID() == UnitTypes::Terran_Bunker.getID()) return 1.5;
	}

	//Siege Tanks are nasty and have high prio to be killed.
	if (target.getID() == UnitTypes::Terran_Siege_Tank_Siege_Mode.getID())
	{
		return 1.5;
	}

	//Prio to take out detectors when having cloaking units
	if (isCloakingUnit(attacker) && target.isDetector())
	{
		return 2;
	}

	if (attacker.isFlyer() && !target.airWeapon().targetsAir())
	{
		//Target cannot attack back. Set to low prio
		return 0.1;
	}

	if (!attacker.isFlyer() && !target.groundWeapon().targetsGround())
	{
		//Target cannot attack back. Set to low prio
		return 0.1;
	}

	if (target.isWorker())
	{
		//Workers are important but very weak units
		return 3;
	}
	
	return 1; //Default: No modifier
}

void TargetingAgent::handleCloakedUnit(Unit unit)
{
	//Terran: Cloaked units are handled by ComSat agent
	
	//Add code for handling cloaked units here.
}

bool TargetingAgent::isCloakingUnit(UnitType type)
{
	if (type.isCloakable()) return true;
	return false;
}
