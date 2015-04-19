#include "ScienceVesselAgent.h"
#include "../../Managers/AgentManager.h"
#include "../../MainAgents/TargetingAgent.h"

bool ScienceVesselAgent::useAbilities()
{
	//Shielding
	if (unit->getEnergy() >= 100 && Broodwar->getFrameCount() - lastShieldFrame > 100)
	{
		BaseAgent* agent = findImportantUnit();
		if (agent != NULL)
		{
			if (agent->isAlive() && TargetingAgent::getNoAttackers(agent) > 0)
			{
				if (unit->useTech(TechTypes::Defensive_Matrix, agent->getUnit()))
				{
					Broodwar << "Used Defense Matrix on " << agent->getUnitType().getName() << endl;
					lastShieldFrame = Broodwar->getFrameCount();
					return true;
				}
			}
		}
	}

	//Irradiate
	TechType irradiate = TechTypes::Irradiate;
	if (unit->getEnergy() >= 75 && Broodwar->getFrameCount() - lastIrradiateFrame > 200 && Broodwar->self()->hasResearched(irradiate))
	{
		//Count enemy units and find an enemy organic unit
		int cntEnemy = 0;
		Unit enemyOrganic = NULL;
		for (auto &u : Broodwar->enemy()->getUnits())
		{
			if (u->exists())
			{
				if (unit->getDistance(u) <= 6 * 32 && u->getIrradiateTimer() == 0)
				{
					cntEnemy++;
					if (u->getType().isOrganic())
					{
						enemyOrganic = u;
					}
				}
			}
		}

		if (cntEnemy >= 5 && enemyOrganic != NULL)
		{
			if (unit->useTech(irradiate, enemyOrganic))
			{
				Broodwar << "Irradiate used on " << enemyOrganic->getType().getName() << endl;
				lastIrradiateFrame = Broodwar->getFrameCount();
				return true;
			}
		}
	}

	//EMP shockwave
	TechType emp = TechTypes::EMP_Shockwave;
	if (Broodwar->self()->hasResearched(emp) && unit->getEnergy() >= emp.energyCost())
	{
		int range = emp.getWeapon().maxRange();
		for (auto &a : Broodwar->enemy()->getUnits())
		{
			if (isEMPtarget(a) && unit->getDistance(a) <= range)
			{
				if (unit->useTech(emp, a->getPosition()))
				{
					Broodwar << "EMP Shockwave used on " << a->getType().getName() << endl;
					return true;
				}
			}
		}
	}

	return false;
}

bool ScienceVesselAgent::isEMPtarget(Unit e)
{
	if (e->getShields() < 60) return false;
	if (e->getType().getID() == UnitTypes::Protoss_Carrier.getID()) return true;
	if (e->getType().getID() == UnitTypes::Protoss_Arbiter.getID()) return true;
	if (e->getType().getID() == UnitTypes::Protoss_Photon_Cannon.getID()) return true;
	if (e->getType().getID() == UnitTypes::Protoss_Archon.getID()) return true;
	if (e->getType().getID() == UnitTypes::Protoss_Scout.getID()) return true;
	if (e->getType().getID() == UnitTypes::Protoss_Reaver.getID()) return true;
	return false;
}

BaseAgent* ScienceVesselAgent::findImportantUnit()
{
	Agentset agents = AgentManager::getInstance()->getAgents();
	for (auto &a : agents)
	{
		if (isImportantUnit(a))
		{
			double dist = unit->getDistance(a->getUnit());
			if (dist <= 320)
			{
				return a;
			}
		}
	}
	return NULL;
}

bool ScienceVesselAgent::isImportantUnit(BaseAgent* agent)
{
	UnitType type = agent->getUnitType();

	if (agent->isOfType(UnitTypes::Terran_Siege_Tank_Tank_Mode)) return true;
	if (agent->isOfType(UnitTypes::Terran_Siege_Tank_Siege_Mode)) return true;
	if (agent->isOfType(UnitTypes::Terran_Science_Vessel)) return true;
	if (agent->isOfType(UnitTypes::Terran_Battlecruiser)) return true;

	return false;
}
