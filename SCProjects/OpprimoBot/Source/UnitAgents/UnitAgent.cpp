#include "UnitAgent.h"
#include "../Pathfinding/NavigationAgent.h"
#include "../MainAgents/TargetingAgent.h"

UnitAgent::UnitAgent()
{

}

UnitAgent::~UnitAgent()
{
	
}

UnitAgent::UnitAgent(Unit mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "UnitAgent";
	goal = TilePosition(-1, -1);
	infoUpdateFrame = 0;
}

void UnitAgent::computeActions()
{
	//Prio 1: Use abilities
	if (useAbilities())
	{
		lastOrderFrame = Broodwar->getFrameCount();
		return;
	}

	//Prio 2: Attack enemy
	if (TargetingAgent::checkTarget(this))
	{
		lastOrderFrame = Broodwar->getFrameCount();
		return;
	}

	//Prio 3: Move
	if (!unit->isLoaded() && !unit->isSieged() && !unit->isBurrowed())
	{
		if (NavigationAgent::getInstance()->computeMove(this, goal))
		{
			lastOrderFrame = Broodwar->getFrameCount();
			return;
		}
	}
}

void UnitAgent::printInfo()
{
	int e = Broodwar->getFrameCount() - infoUpdateFrame;
	if (e >= infoUpdateTime || (sx == 0 && sy == 0))
	{
		infoUpdateFrame = Broodwar->getFrameCount();
		sx = unit->getPosition().x;
		sy = unit->getPosition().y;
	}	

	Broodwar->drawBoxMap(sx-2,sy,sx+242,sy+105,Colors::Black,true);
	Broodwar->drawTextMap(sx+4,sy,"\x03%s", format(unit->getType().getName()).c_str());
	Broodwar->drawLineMap(sx,sy+14,sx+240,sy+14,Colors::Green);

	Broodwar->drawTextMap(sx+2,sy+15,"Id: \x11%d", unitID);
	Broodwar->drawTextMap(sx+2,sy+30,"Position: \x11(%d,%d)", unit->getTilePosition().x, unit->getTilePosition().y);
	Broodwar->drawTextMap(sx+2,sy+45,"Goal: \x11(%d,%d)", goal.x, goal.y);
	Broodwar->drawTextMap(sx+2,sy+60,"Squad: \x11%d", squadID);
	
	int range = unit->getType().seekRange();
	if (unit->getType().sightRange() > range)
	{
		range = unit->getType().sightRange();
	}
	int enemyInRange = 0;
	for (auto &u : Broodwar->enemy()->getUnits())
	{
		double dist = unit->getPosition().getDistance(u->getPosition());
		if (dist <= range)
		{
			enemyInRange++;
			break;
		}
	}

	Broodwar->drawTextMap(sx+2,sy+75,"Range: \x11%d", range);
	if (enemyInRange == 0) Broodwar->drawTextMap(sx+2,sy+90,"Enemies seen: \x11%d", enemyInRange);
	else Broodwar->drawTextMap(sx+2,sy+90,"Enemies seen: \x08%d", enemyInRange);

	string str = "\x07No";
	if (unit->isAttacking() || unit->isStartingAttack()) str = "\x08Yes";

	//Column two
	Broodwar->drawTextMap(sx+100,sy+15,"Attacking: %s", str.c_str());
	int nsy = sy+30;
	if (type.groundWeapon().targetsGround()) 
	{
		stringstream ss;
		if (unit->getGroundWeaponCooldown() == 0) ss << "\x07Ready";
		else
		{
			ss << "\x08";
			ss << unit->getGroundWeaponCooldown();
		}

		Broodwar->drawTextMap(sx+100,nsy,"Ground CD: %s", ss.str().c_str());
		nsy += 15;
	}

	if (type.airWeapon().targetsAir()) 
	{
		stringstream ss;
		if (unit->getAirWeaponCooldown() == 0) ss << "\x07Ready";
		else
		{
			ss << "\x08";
			ss << unit->getAirWeaponCooldown();
		}

		Broodwar->drawTextMap(sx+100,nsy,"Air CD: %s", ss.str().c_str());
		nsy += 15;
	}

	Unit target = unit->getTarget();
	if (target == NULL) target = unit->getOrderTarget();
	str = "";
	if (target != NULL)
	{
		str = format(target->getType().getName());
	}
	Broodwar->drawTextMap(sx+100,nsy,"Target: \x11%s", str.c_str());
	nsy += 15;
	
	Broodwar->drawLineMap(sx,sy+104,sx+240,sy+104,Colors::Green);
}
