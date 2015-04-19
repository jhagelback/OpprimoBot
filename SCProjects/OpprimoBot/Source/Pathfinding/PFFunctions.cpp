#include "PFFunctions.h"

float PFFunctions::getDistance(WalkPosition w1, WalkPosition w2)
{
	return (float)w1.getDistance(w2);
}

float PFFunctions::getDistance(WalkPosition wt, Unit unit)
{
	WalkPosition w2 = WalkPosition(unit->getPosition());
	return (float)wt.getDistance(w2);
}

float PFFunctions::calcOwnUnitP(float d, WalkPosition wt, Unit unit, Unit otherOwnUnit)
{
	if (unit->getID() == otherOwnUnit->getID())
	{
		//Dont count collision with yourself...
		return 0;
	}

	if (otherOwnUnit->getType().isFlyer())
	{
		//Cannot collide with flying units.
		return 0;
	}
	if (unit->getType().isFlyer())
	{
		bool follow = false;
		if (unit->getType().getID() == UnitTypes::Terran_Science_Vessel.getID() && otherOwnUnit->getType().isMechanical()) follow = true;
		if (unit->getType().getID() == UnitTypes::Terran_SCV.getID() && otherOwnUnit->getType().isMechanical()) follow = true;
		if (unit->getType().getID() == UnitTypes::Protoss_Arbiter.getID()) follow = true;
		if (unit->getType().getID() == UnitTypes::Zerg_Overlord.getID()) follow = true;

		if (follow)
		{
			//Flying support unit. Make ground units slightly
			//attractive to group up squads.
			float p = (float)(100 - d * 0.2);
			if (p < 0) p = 0;
			return p;
		}
		else
		{
			//Offensive flying unit. No collisions.
			return 0;
		}
	}

	float p = 0;

	if ((unit->isCloaked() && !otherOwnUnit->isCloaked()) || (unit->isBurrowed() && !otherOwnUnit->isBurrowed()))
	{
		//Let cloaked or burrowed units stay away from visible
		//units to avoid getting killed by splash damage.
		if (d <= 4)
		{
			p = -50.0;
		}
	}

	if (otherOwnUnit->isIrradiated())
	{
		//Other unit under Irradite. Keep distance.
		if (d <= 2)
		{
			p = -50.0;
		}
	}
	if (otherOwnUnit->isUnderStorm())
	{
		//Other unit under Psionic Storm. Keep distance.
		if (d <= 3)
		{
			p = -50.0;
		}
	}

	if (!unit->getType().isBuilding())
	{
		if (d <= 2)
		{
			p = -25.0;
		}
	}

	if (otherOwnUnit->getType().isBuilding())
	{
		UnitType t = otherOwnUnit->getType();
		TilePosition ut = TilePosition(wt);
		TilePosition ot = otherOwnUnit->getTilePosition();
		for (int cx = ot.x; cx < ot.x + t.tileWidth(); cx++)
		{
			for (int cy = ot.y; cy < ot.y + t.tileHeight(); cy++)
			{
				if (ut.x == cx && ut.y == cy) p = -50;
			}
		}
	}
    return p;
}

float PFFunctions::getTrailP(BaseAgent* agent, WalkPosition wt)
{
	if (agent->getUnit()->isBeingConstructed()) return 0;

	float p = 0;
	
	//Add current position to trail
	agent->addTrailPosition(WalkPosition(agent->getUnit()->getPosition()));

	//Get trail
	vector<WalkPosition> trail = agent->getTrail();
	for (int i = 0; i < (int)trail.size(); i++)
	{
		WalkPosition twt = trail.at(i);
		float d = (float)twt.getDistance(wt);
		if (d <= 1.5) p = -10.0;
	}

	return p;
}

float PFFunctions::getTerrainP(BaseAgent* agent, WalkPosition wt)
{
	if (agent->getUnitType().isFlyer()) return 0;
	if (!Broodwar->isWalkable(wt)) return -1000.0;

	return 0;
}

float PFFunctions::getGoalP(BaseAgent* agent, WalkPosition wt)
{
	TilePosition goal = agent->getGoal();
	if (goal.x == -1) return 0;

	//Calc max wep range
    int range = 0;
	if (agent->getUnitType().isFlyer())
	{
		range = agent->getUnitType().airWeapon().maxRange();
	}
	else
	{
		range = agent->getUnitType().groundWeapon().maxRange();
	}
	if (range == 0)
	{
		//Non-attacking unit. Use sight range
		range = agent->getUnitType().sightRange();
	}

	//Set good defensive range
	range = (int)(range * 0.5);
	//if (range < 64) range = 64;
	//Convert range to walktiles
	range = range / 32;

    float p = 0;

	WalkPosition ut = WalkPosition(agent->getUnit()->getPosition());
	double d = ut.getDistance(wt);

	if (d < range)
	{
		p = (float)(100 - range * 2);
		if (p < 0)
		{
			p = 0;
		}
	}
	else if (d >= range && d < range + 1)
	{
		p = (float)80;
	}
	else
	{
		float d1 = (float)(d - range);

		p = (float)80 - (float)d1;
		if (p < 0)
		{
			p = 0;
		}
	}

	return p;	
}

float PFFunctions::calcOffensiveUnitP(float d, Unit attacker, Unit enemy)
{
	//Check if enemy unit exists and is visible.
	if (!enemy->exists())
	{
		return 0;
	}
	if (!enemy->isVisible())
	{
		return 0;
	}
	if (enemy->isCloaked()) 
	{
		return 0;
	}

	//SCV:s shall not attack
	if (attacker->getType().isWorker())
	{
		return 0;
	}

	//Check for flying buildings
	if (enemy->getType().isFlyingBuilding() && enemy->isLifted())
	{
		return 0;
	}

	//Check if we can attack the type
	if (enemy->getType().isFlyer() && !attacker->getType().airWeapon().targetsAir())
	{
		return 0;
	}
	if (!enemy->getType().isFlyer() && !attacker->getType().groundWeapon().targetsGround())
	{
		return 0;
	}

	//Calc max wep range
    int myMSD = 0;
	if (enemy->getType().isFlyer())
	{
		myMSD = getAirRange(attacker) - 1;
	}
	else
	{
		myMSD = getGroundRange(attacker) - 1;
	}

	if (!attacker->getType().canAttack())
	{
		//Unit cannot attack, use sightrange instead
		myMSD = 4;//attacker->getType().sightRange() / 8;
	}

	if (attacker->getType().getID() == UnitTypes::Terran_Medic.getID())
	{
		myMSD = 6;
	}
	if (attacker->getType().getID() == UnitTypes::Terran_SCV.getID())
	{
		myMSD = 8;
	}
	if (attacker->getType().getID() == UnitTypes::Protoss_High_Templar.getID())
	{
		myMSD = 6;
	}
	if (attacker->getType().getID() == UnitTypes::Zerg_Overlord.getID())
	{
		myMSD = 12;
	}

	//Calc attacker wep range
	int enemyMSD = 0;
	if (attacker->getType().isFlyer())
	{
		enemyMSD = getAirRange(enemy);
	}
	else
	{
		enemyMSD = getGroundRange(enemy);
	}
    
    float p = 0;

	//Enemy cannot attack back. It is safe to move
	//closer than MSD.
	if (enemyMSD == 0)
	{
		enemyMSD = (int)(enemyMSD * 0.5);
	}

	if (canAttack(attacker, enemy))
	{
		if (d < myMSD - 1)
		{
			float fact = (float)100 / myMSD;
			p = d * fact;
			if (p < 0)
			{
				p = 0;
			}
		}
		else if (d >= myMSD - 1 && d <= myMSD)
		{
			p = (float)100;
		}
		else
		{
			float d1 = d - myMSD;

			p = (float)80 - (float)d1;
			if (p < 0)
			{
				p = 0;
			}
		}
	}
	
    return p;
}

float PFFunctions::calcDefensiveUnitP(float d, Unit attacker, Unit enemy)
{
	//Check if enemy unit exists and is visible.
	if (!enemy->exists())
	{
		return 0;
	}
	
	//Check for flying buildings
	if (enemy->getType().isFlyingBuilding() && enemy->isLifted())
	{
		return 0;
	}

	//Calc attacker wep range
	int enemyMSD = 0;
	if (attacker->getType().isFlyer())
	{
		enemyMSD = getAirRange(enemy) + 2;
	}
	else
	{
		enemyMSD = getGroundRange(enemy) + 2;
	}
	//Cloaked unit: Watch out for detectors.
	if (attacker->isCloaked() && enemy->getType().isDetector())
	{
		enemyMSD = (int)(enemy->getType().sightRange() / 8 + 4);
	}
    
    double p = 0;
	
	//Defensive mode -> retreat
	p = (-80 + d) / 2;
	if (p > 0)
	{
		p = 0;
	}

    return (float)p;
}

int PFFunctions::getGroundRange(Unit cUnit)
{
	int range = 0;
	if (cUnit->getType().groundWeapon().targetsGround()) 
	{
		int gwR = cUnit->getType().groundWeapon().maxRange();
		if (gwR > range)
		{
			range = gwR;
		}
	}
	if (cUnit->getType().airWeapon().targetsGround()) 
	{
		int gwR = cUnit->getType().airWeapon().maxRange();
		if (gwR > range)
		{
			range = gwR;
		}
	}
	if (cUnit->getType().getID() == UnitTypes::Terran_Bunker.getID())
	{
		range = UnitTypes::Terran_Marine.groundWeapon().maxRange();
	}

	return range / 8;
}

int PFFunctions::getAirRange(Unit cUnit)
{
	int range = 0;
	if (cUnit->getType().groundWeapon().targetsAir())
	{
		int gwR = cUnit->getType().groundWeapon().maxRange();
		if (gwR > range)
		{
			range = gwR;
		}
	}
	if (cUnit->getType().airWeapon().targetsAir())
	{
		int gwR = cUnit->getType().airWeapon().maxRange();
		if (gwR > range)
		{
			range = gwR;
		}
	}
	if (cUnit->getType().getID() == UnitTypes::Terran_Bunker.getID())
	{
		range = UnitTypes::Terran_Marine.groundWeapon().maxRange();
	}

	return range / 8;
}

bool PFFunctions::canAttack(Unit ownUnit, Unit target)
{
	UnitType oType = ownUnit->getType();
	UnitType tType = target->getType();

	if (tType.isFlyer())
	{
		//Own unit is air
		if (oType.groundWeapon().targetsAir())
		{
			return true;
		}
		if (oType.airWeapon().targetsAir())
		{
			return true;
		}
	}
	else
	{
		//Own unit is ground
		if (oType.groundWeapon().targetsGround())
		{
			return true;
		}
		if (oType.airWeapon().targetsGround())
		{
			return true;
		}
	}

	return false;
}
