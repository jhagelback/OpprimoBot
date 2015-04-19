#include "ResourceManager.h"

ResourceManager* ResourceManager::instance = NULL;

ResourceManager::ResourceManager()
{
	locks.push_back(ResourceLock(Broodwar->self()->getRace().getCenter()));
}

ResourceManager::~ResourceManager()
{
	instance = NULL;
}

ResourceManager* ResourceManager::getInstance()
{
	if (instance == NULL)
	{
		instance = new ResourceManager();
	}
	return instance;
}

bool ResourceManager::hasResources(UnitType type)
{
	int nMinerals = type.mineralPrice();
	int nGas = type.gasPrice();

	return hasResources(nMinerals, nGas);
}

bool ResourceManager::hasResources(UpgradeType type)
{
	int nMinerals = type.mineralPrice();
	int nGas = type.gasPrice();

	return hasResources(nMinerals, nGas);
}

bool ResourceManager::hasResources(TechType type)
{
	int nMinerals = type.mineralPrice();
	int nGas = type.gasPrice();

	return hasResources(nMinerals, nGas);
}

bool ResourceManager::hasResources(int neededMinerals, int neededGas)
{
	if (Broodwar->self()->minerals() - calcLockedMinerals() >= neededMinerals)
	{
		if (Broodwar->self()->gas() - calcLockedGas() >= neededGas)
		{
			return true;
		}
	}

	return false;
}

void ResourceManager::lockResources(UnitType type)
{
	locks.push_back(type);
}

void ResourceManager::unlockResources(UnitType type)
{
	for (int i = 0; i < (int)locks.size(); i++)
	{
		if (locks.at(i).unit.getID() == type.getID())
		{
			locks.erase(locks.begin() + i);
			return;
		}
	}
}

int ResourceManager::calcLockedMinerals()
{
	int nMinerals = 0;

	for (int i = 0; i < (int)locks.size(); i++)
	{
		nMinerals += locks.at(i).mineralCost;
	}

	return nMinerals;
}

int ResourceManager::calcLockedGas()
{
	int nGas = 0;

	for (int i = 0; i < (int)locks.size(); i++)
	{
		nGas += locks.at(i).gasCost;
	}

	return nGas;
}

void ResourceManager::printInfo()
{
	Broodwar->drawTextScreen(5,96, "Locked minerals: %d", calcLockedMinerals());
	Broodwar->drawTextScreen(5,112, "Locked gas: %d", calcLockedGas());
}
