#include "Upgrader.h"
#include "AgentManager.h"
#include "ResourceManager.h"
#include <sstream>

Upgrader* Upgrader::instance = NULL;

Upgrader::Upgrader()
{
	debug = false;
}

Upgrader::~Upgrader()
{
	instance = NULL;
}

Upgrader* Upgrader::getInstance()
{
	if (instance == NULL)
	{
		instance = new Upgrader();
	}
	return instance;
}

void Upgrader::toggleDebug()
{
	debug = !debug;
}

string Upgrader::format(string str)
{
	string res = str;

	string raceName = Broodwar->self()->getRace().getName();
	if (str.find(raceName) == 0)
	{
		int i = str.find("_");
		res = str.substr(i + 1, str.length());
	}

	return res;
}

void Upgrader::printInfo()
{
	if (!debug) return;

	//Precalc total lines
	int totalLines = 1;
	for (auto &u : TechTypes::allTechTypes())
	{
		if (u.getRace().getID() == Broodwar->self()->getRace().getID() && Broodwar->self()->hasResearched(u) && u.mineralPrice() > 0)
		{
			totalLines++;
		}
	}
	for (auto &u : UpgradeTypes::allUpgradeTypes())
	{
		if (u.getRace().getID() == Broodwar->self()->getRace().getID() && u.mineralPrice() > 0)
		{
			int cLevel = Broodwar->self()->getUpgradeLevel(u);
			if (cLevel > 0)
			{
				totalLines++;
			}
		}
	}
	if (totalLines == 1) totalLines++;
	Broodwar->drawBoxScreen(3,25,162,26+totalLines*16,Colors::Black,true);
	//

	int line = 1;
	for (auto &u : TechTypes::allTechTypes())
	{
		if (u.getRace().getID() == Broodwar->self()->getRace().getID() && Broodwar->self()->hasResearched(u) && u.mineralPrice() > 0)
		{
			Broodwar->drawTextScreen(5,25+16*line, u.getName().c_str());
			line++;
		}
	}
	for (auto &u : UpgradeTypes::allUpgradeTypes())
	{
		if (u.getRace().getID() == Broodwar->self()->getRace().getID() && u.mineralPrice() > 0)
		{
			int cLevel = Broodwar->self()->getUpgradeLevel(u);
			if (cLevel > 0)
			{
				stringstream ss;
				ss << format(u.getName());
				ss << " \x0F(";
				ss << cLevel;
				ss << "/";
				ss << u.maxRepeats();
				ss << ")";
				
				Broodwar->drawTextScreen(5,25+16*line, ss.str().c_str());
				line++;
			}
		}
	}

	Broodwar->drawTextScreen(5,25,"\x03Techs/upgrades:");
	Broodwar->drawLineScreen(5,39,160,39,Colors::Orange);
	if (line == 1) line++;
	Broodwar->drawLineScreen(5,25+line*16,160,25+line*16,Colors::Orange);
}


bool Upgrader::checkUpgrade(BaseAgent* agent)
{
	if (agent->isAlive() && agent->getUnit()->isIdle())
	{
		Unit unit = agent->getUnit();

		//Check techs
		for (int i = 0; i < (int)techs.size(); i++)
		{
			TechType type = techs.at(i);
			if (Broodwar->self()->hasResearched(type))
			{
				techs.erase(techs.begin() + i);
				return true;
			}
			if (canResearch(type, unit))
			{
				unit->research(type);
				return true;
			}
		}

		//Check upgrades
		for (int i = 0; i < (int)upgrades.size(); i++)
		{
			UpgradeType type = upgrades.at(i);
			if (canUpgrade(type, unit))
			{
				if (unit->upgrade(type))
				{
					upgrades.erase(upgrades.begin() + i);
					return true;
				}
			}
		}
	}

	return false;
}

bool Upgrader::canUpgrade(UpgradeType type, Unit unit)
{
	//1. Check if unit is idle
	if (!unit->isIdle())
	{
		return false;
	}

	//2. Check if unit can do this upgrade
	if (!Broodwar->canUpgrade(type, unit))
	{
		return false;
	}

	//3. Check if we have enough resources
	if (!ResourceManager::getInstance()->hasResources(type))
	{
		return false;
	}
	
	//4. Check if unit is being constructed
	if (unit->isBeingConstructed())
	{
		return false;
	}

	//5. Check if some other building is already doing this upgrade
	Agentset agents = AgentManager::getInstance()->getAgents();
	for (auto &a : agents)
	{
		if (a->getUnit()->getUpgrade().getID() == type.getID())
		{
			return false;
		}
	}

	//6. Check if we are currently upgrading it
	if (Broodwar->self()->isUpgrading(type))
	{
		return false;
	}

	//All clear. Can do the upgrade.
	return true;
}

bool Upgrader::canResearch(TechType type, Unit unit)
{
	//Seems Broodwar->canResearch bugs when Lurker Aspect is requested without
	//having an upgraded Lair.
	if (type.getID() == TechTypes::Lurker_Aspect.getID())
	{
		if (AgentManager::getInstance()->countNoFinishedUnits(UnitTypes::Zerg_Lair) == 0) return false;
	}

	//1. Check if unit can do this upgrade
	if (!Broodwar->canResearch(type, unit))
	{
		return false;
	}
	
	//2. Check if we have enough resources
	if (!ResourceManager::getInstance()->hasResources(type))
	{
		return false;
	}
	
	//3. Check if unit is idle
	if (!unit->isIdle())
	{
		return false;
	}

	//4. Check if unit is being constructed
	if (unit->isBeingConstructed())
	{
		return false;
	}

	//5. Check if some other building is already doing this upgrade
	Agentset agents = AgentManager::getInstance()->getAgents();
	for (auto &a : agents)
	{
		if (a->getUnit()->getTech().getID() == type.getID())
		{
			return false;
		}
	}

	//6. Check if we are currently researching it
	if (Broodwar->self()->isResearching(type))
	{
		return false;
	}

	//All clear. Can do the research.
	return true;
}

void Upgrader::addUpgrade(UpgradeType type)
{
	upgrades.push_back(type);
}

void Upgrader::addTech(TechType type)
{
	techs.push_back(type);
}