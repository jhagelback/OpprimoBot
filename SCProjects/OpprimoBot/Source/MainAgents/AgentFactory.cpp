#include "AgentFactory.h"
#include "WorkerAgent.h"
#include "../StructureAgents/StructureAgent.h"
#include "../UnitAgents/UnitAgent.h"
#include "../Commander/Commander.h"
#include "../UnitAgents/TransportAgent.h"

/** Terran agents */
#include "../StructureAgents/Terran/CommandCenterAgent.h"
#include "../StructureAgents/Terran/ComsatAgent.h"
#include "../UnitAgents/Terran/SiegeTankAgent.h"
#include "../UnitAgents/Terran/MarineAgent.h"
#include "../UnitAgents/Terran/MedicAgent.h"
#include "../UnitAgents/Terran/VultureAgent.h"
#include "../UnitAgents/Terran/FirebatAgent.h"
#include "../StructureAgents/RefineryAgent.h"
#include "../UnitAgents/Terran/ScienceVesselAgent.h"
#include "../UnitAgents/Terran/BattlecruiserAgent.h"
#include "../UnitAgents/Terran/WraithAgent.h"
#include "../UnitAgents/Terran/GhostAgent.h"

/** Protoss agents */
#include "../StructureAgents/Protoss/NexusAgent.h"
#include "../UnitAgents/Protoss/ReaverAgent.h"
#include "../UnitAgents/Protoss/CorsairAgent.h"
#include "../UnitAgents/Protoss/CarrierAgent.h"
#include "../UnitAgents/Protoss/HighTemplarAgent.h"

/** Zerg agents */
#include "../StructureAgents/Zerg/HatcheryAgent.h"
#include "../UnitAgents/Zerg/HydraliskAgent.h"
#include "../UnitAgents/Zerg/LurkerAgent.h"
#include "../UnitAgents/Zerg/MutaliskAgent.h"
#include "../UnitAgents/Zerg/QueenAgent.h"
#include "../UnitAgents/Zerg/DefilerAgent.h"

bool AgentFactory::instanceFlag = false;
AgentFactory* AgentFactory::instance = NULL;

AgentFactory::AgentFactory()
{
	
}

AgentFactory::~AgentFactory()
{
	instanceFlag = false;
	delete instance;
}

AgentFactory* AgentFactory::getInstance()
{
	if (!instanceFlag)
	{
		instance = new AgentFactory();
		instanceFlag = true;
	}
	return instance;
}

BaseAgent* AgentFactory::createAgent(Unit unit)
{
	if (Broodwar->self()->getRace().getID() == Races::Terran.getID())
	{
		return createTerranAgent(unit);
	}
	if (Broodwar->self()->getRace().getID() == Races::Protoss.getID())
	{
		return createProtossAgent(unit);
	}
	if (Broodwar->self()->getRace().getID() == Races::Zerg.getID())
	{
		return createZergAgent(unit);
	}

	//Default agents
	if (unit->getType().isWorker())
	{
		return new WorkerAgent(unit);
	}
	else if (unit->getType().isBuilding())
	{
		return new StructureAgent(unit);
	}
	else
	{
		return new UnitAgent(unit);
	}
}

BaseAgent* AgentFactory::createZergAgent(Unit unit)
{
	UnitType type = unit->getType();
	
	if (type.isWorker())
	{
		return new WorkerAgent(unit);
	}
	else if (type.isBuilding())
	{
		//Add agents for special buildings here
		if (type.getID() == UnitTypes::Zerg_Hatchery.getID())
		{
			return new HatcheryAgent(unit);
		}
		else if (type.getID() == UnitTypes::Zerg_Lair.getID())
		{
			return new HatcheryAgent(unit);
		}
		else if (type.getID() == UnitTypes::Zerg_Hive.getID())
		{
			return new HatcheryAgent(unit);
		}
		else if (type.getID() == UnitTypes::Zerg_Extractor.getID())
		{
			return new RefineryAgent(unit);
		}
		else
		{
			//Default structure agent
			return new StructureAgent(unit);
		}
	}
	else
	{
		if (type.getID() == UnitTypes::Zerg_Overlord.getID())
		{
			return new UnitAgent(unit);
		}
		else if (type.getID() == UnitTypes::Zerg_Zergling.getID())
		{
			return new UnitAgent(unit);
		}
		else if (type.getID() == UnitTypes::Zerg_Hydralisk.getID())
		{
			return new HydraliskAgent(unit);
		}
		else if (type.getID() == UnitTypes::Zerg_Mutalisk.getID())
		{
			return new MutaliskAgent(unit);
		}
		else if (type.getID() == UnitTypes::Zerg_Lurker.getID())
		{
			return new LurkerAgent(unit);
		}
		else if (type.getID() == UnitTypes::Zerg_Queen.getID())
		{
			return new QueenAgent(unit);
		}
		else if (type.getID() == UnitTypes::Zerg_Ultralisk.getID())
		{
			return new UnitAgent(unit);
		}
		else if (type.getID() == UnitTypes::Zerg_Guardian.getID())
		{
			return new UnitAgent(unit);
		}
		else if (type.getID() == UnitTypes::Zerg_Devourer.getID())
		{
			return new UnitAgent(unit);
		}
		else if (type.getID() == UnitTypes::Zerg_Defiler.getID())
		{
			return new DefilerAgent(unit);
		}
		else if (type.getID() == UnitTypes::Zerg_Scourge.getID())
		{
			return new UnitAgent(unit);
		}
		else if (type.getID() == UnitTypes::Zerg_Infested_Terran.getID())
		{
			return new UnitAgent(unit);
		}
		else
		{
			//Default unit agent
			return new UnitAgent(unit);
		}
	}
	return NULL;
}

BaseAgent* AgentFactory::createTerranAgent(Unit unit)
{
	if (unit->getType().isWorker())
	{
		return new WorkerAgent(unit);
	}
	else if (unit->getType().isBuilding())
	{
		//Add agents for special buildings here
		if (isOfType(unit, UnitTypes::Terran_Command_Center))
		{
			return new CommandCenterAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Terran_Comsat_Station))
		{
			return new ComsatAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Terran_Refinery))
		{
			return new RefineryAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Terran_Bunker))
		{
			//Make sure we set the squad id to the bunker, so we
			//can remove the squad if the bunker is destroyed.
			int squadID = Commander::getInstance()->addBunkerSquad();
			BaseAgent* a = new StructureAgent(unit);
			a->setSquadID(squadID);
			return a;
		}
		else
		{
			//Default structure agent
			return new StructureAgent(unit);
		}
	}
	else
	{
		if (isOfType(unit, UnitTypes::Terran_Siege_Tank_Tank_Mode))
		{
			return new SiegeTankAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Terran_Siege_Tank_Siege_Mode))
		{
			return new SiegeTankAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Terran_Marine))
		{
			return new MarineAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Terran_Firebat))
		{
			return new FirebatAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Terran_Medic))
		{
			return new MedicAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Terran_Vulture))
		{
			return new VultureAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Terran_Science_Vessel))
		{
			return new ScienceVesselAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Terran_Battlecruiser))
		{
			return new BattlecruiserAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Terran_Wraith))
		{
			return new WraithAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Terran_Ghost))
		{
			return new GhostAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Terran_Goliath))
		{
			return new UnitAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Terran_Valkyrie))
		{
			return new UnitAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Terran_Dropship))
		{
			return new TransportAgent(unit);
		}
		else
		{
			//Default unit agent
			return new UnitAgent(unit);
		}
	}
	return NULL;
}

BaseAgent* AgentFactory::createProtossAgent(Unit unit)
{
	if (unit->getType().isWorker())
	{
		return new WorkerAgent(unit);
	}
	else if (unit->getType().isBuilding())
	{
		//Add agents for special buildings here
		if (isOfType(unit, UnitTypes::Protoss_Nexus))
		{
			return new NexusAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Protoss_Assimilator))
		{
			return new RefineryAgent(unit);
		}
		else
		{
			//Default structure agent
			return new StructureAgent(unit);
		}
	}
	else
	{
		if (isOfType(unit, UnitTypes::Protoss_Zealot))
		{
			return new UnitAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Protoss_Dragoon))
		{
			return new UnitAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Protoss_Reaver))
		{
			return new ReaverAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Protoss_Observer))
		{
			return new UnitAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Protoss_Dark_Templar))
		{
			return new UnitAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Protoss_Scout))
		{
			return new UnitAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Protoss_Shuttle))
		{
			return new TransportAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Protoss_Corsair))
		{
			return new CorsairAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Protoss_Carrier))
		{
			return new CarrierAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Protoss_High_Templar))
		{
			return new HighTemplarAgent(unit);
		}
		else if (isOfType(unit, UnitTypes::Protoss_Arbiter))
		{
			return new UnitAgent(unit);
		}
		else
		{
			//Default unit agent
			return new UnitAgent(unit);
		}
	}
	return NULL;
}

bool AgentFactory::isOfType(Unit unit, UnitType type)
{
	if (unit->getType().getID() == type.getID())
	{
		return true;
	}
	return false;
}

