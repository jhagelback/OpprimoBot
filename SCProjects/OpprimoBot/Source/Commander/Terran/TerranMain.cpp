#include "TerranMain.h"
#include "../../Managers/BuildplanEntry.h"
#include "../../Managers/AgentManager.h"
#include "../ExplorationSquad.h"
#include "../RushSquad.h"

TerranMain::TerranMain()
{
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Supply_Depot, 9));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Barracks, 9));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Refinery, 12));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Bunker, 14));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Supply_Depot, 16));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Bunker, 16));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Factory, 18));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Academy, 20));
	buildplan.push_back(BuildplanEntry(TechTypes::Tank_Siege_Mode, 22));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Supply_Depot, 23));
	buildplan.push_back(BuildplanEntry(TechTypes::Stim_Packs, 29));
	buildplan.push_back(BuildplanEntry(UpgradeTypes::U_238_Shells, 30));
	buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Supply_Depot, 31));

	mainSquad = new Squad(1, Squad::OFFENSIVE, "MainSquad", 10);
	mainSquad->addSetup(UnitTypes::Terran_Marine, 10);
	mainSquad->setRequired(true);
	mainSquad->setBuildup(true);
	squads.push_back(mainSquad);

	secondarySquad = new Squad(2, Squad::OFFENSIVE, "SecondarySquad", 10);
	secondarySquad->setRequired(false);
	secondarySquad->setBuildup(true);
	squads.push_back(secondarySquad);

	sc1 = new RushSquad(10, "ScoutingSquad1", 11);
	sc1->setRequired(false);
	
	sc2 = new ExplorationSquad(11, "ScoutingSquad2", 11);
	sc2->setRequired(false);
	squads.push_back(sc2);

	backupSquad1 = new Squad(5, Squad::OFFENSIVE, "BackupSquad1", 11);
	backupSquad1->setRequired(false);
	backupSquad1->setBuildup(true);
	squads.push_back(backupSquad1);

	backupSquad2 = new Squad(6, Squad::OFFENSIVE, "BackupSquad2", 12);
	backupSquad2->setRequired(false);
	backupSquad2->setBuildup(true);
	squads.push_back(backupSquad2);

	noWorkers = 16;
	noWorkersPerRefinery = 2;
}

TerranMain::~TerranMain()
{
	for (Squad* s : squads)
	{
		delete s;
	}
	instance = NULL;
}

void TerranMain::computeActions()
{
	computeActionsBase();

	noWorkers = 12 * AgentManager::getInstance()->countNoFinishedUnits(UnitTypes::Terran_Command_Center) + 2 * AgentManager::getInstance()->countNoFinishedUnits(UnitTypes::Terran_Refinery);
	if (noWorkers > 30) noWorkers = 30;

	int cSupply = Broodwar->self()->supplyUsed() / 2;
	int min = Broodwar->self()->minerals();
	int gas = Broodwar->self()->gas();

	if (cSupply >= 20 && stage == 0)
	{
		mainSquad->addSetup(UnitTypes::Terran_Siege_Tank_Tank_Mode, 4);
		mainSquad->addSetup(UnitTypes::Terran_SCV, 1);
		mainSquad->addSetup(UnitTypes::Terran_Marine, 6);
		mainSquad->addSetup(UnitTypes::Terran_Medic, 4);

		stage++;
	}
	if (cSupply >= 30 && min > 200 && stage == 1)
	{
		buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Factory, cSupply));
		buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Armory, cSupply));
		buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Engineering_Bay, cSupply));

		mainSquad->addSetup(UnitTypes::Terran_Goliath, 3);
		mainSquad->setBuildup(false);

		stage++;
	}
	if (cSupply >= 45 && min > 200 && stage == 2)
	{
		buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Barracks, cSupply));
		buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Missile_Turret, cSupply));

		sc1->addSetup(UnitTypes::Terran_Vulture, 1);
		sc1->setPriority(1);
		sc1->setActivePriority(1000);
		squads.push_back(sc1);

		secondarySquad->addSetup(UnitTypes::Terran_Siege_Tank_Tank_Mode, 2);
		secondarySquad->addSetup(UnitTypes::Terran_Marine, 8);
		secondarySquad->addSetup(UnitTypes::Terran_Goliath, 2);
		secondarySquad->setBuildup(false);

		stage++;
	}
	if (Commander::getInstance()->getSquad(1)->isActive() && stage == 3)
	{
		buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Command_Center, cSupply));

		sc2->addSetup(UnitTypes::Terran_Vulture, 2);
		sc2->setBuildup(false);

		stage++;
	}
	
	if (stage == 4 && AgentManager::getInstance()->countNoUnits(UnitTypes::Terran_Command_Center) >= 2)
	{
		buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Missile_Turret, cSupply));
		buildplan.push_back(BuildplanEntry(UpgradeTypes::Terran_Vehicle_Weapons, cSupply));
		buildplan.push_back(BuildplanEntry(UpgradeTypes::Terran_Infantry_Weapons, cSupply));
		buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Starport, cSupply));
		buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Science_Facility, cSupply));

		stage++;
	}
	if (stage == 5 && AgentManager::getInstance()->countNoFinishedUnits(UnitTypes::Terran_Science_Facility) > 0)
	{
		buildplan.push_back(BuildplanEntry(UnitTypes::Terran_Missile_Turret, cSupply));
		
		mainSquad->addSetup(UnitTypes::Terran_Science_Vessel, 1);

		buildplan.push_back(BuildplanEntry(TechTypes::Cloaking_Field, cSupply));

		backupSquad1->addSetup(UnitTypes::Terran_Wraith, 5);
		backupSquad1->setBuildup(false);

		stage++;
	}
	if (stage == 6 && AgentManager::getInstance()->countNoFinishedUnits(UnitTypes::Terran_Science_Vessel) > 0)
	{
		buildplan.push_back(BuildplanEntry(TechTypes::EMP_Shockwave, cSupply));
		buildplan.push_back(BuildplanEntry(UpgradeTypes::Terran_Vehicle_Weapons, cSupply));

		stage++;
	}
	if (stage == 7 && AgentManager::getInstance()->countNoFinishedUnits(UnitTypes::Terran_Physics_Lab) > 0)
	{
		backupSquad2->addSetup(UnitTypes::Terran_Battlecruiser, 2);
		backupSquad2->setBuildup(false);
		buildplan.push_back(BuildplanEntry(TechTypes::Yamato_Gun, cSupply));
		
		stage++;
	}
}
