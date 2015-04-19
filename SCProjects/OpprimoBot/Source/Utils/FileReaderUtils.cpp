#include "FileReaderUtils.h"
#include "../Managers/Constructor.h"
#include "../Managers/ExplorationManager.h"
#include "Config.h"
#include <fstream>

FileReaderUtils::FileReaderUtils()
{
	nameHash.push_back(Tokens("(2)Destination", "4e24f217d2fe4dbfa6799bc57f74d8dc939d425b"));
	nameHash.push_back(Tokens("(2)Benzene", "af618ea3ed8a8926ca7b17619eebcb9126f0d8b1"));
	nameHash.push_back(Tokens("(2)Heartbreak Ridge", "6f8da3c3cc8d08d9cf882700efa049280aedca8c"));
	nameHash.push_back(Tokens("(3)Aztec", "ba2fc0ed637e4ec91cc70424335b3c13e131b75a"));
	nameHash.push_back(Tokens("(3)Tau Cross", "9bfc271360fa5bab3707a29e1326b84d0ff58911"));
	nameHash.push_back(Tokens("(4)Andromeda", "1e983eb6bcfa02ef7d75bd572cb59ad3aab49285"));
	nameHash.push_back(Tokens("(4)Circuit Breaker", "450a792de0e544b51af5de578061cb8a2f020f32"));
	nameHash.push_back(Tokens("(4)Empire of the Sun", "a220d93efdf05a439b83546a579953c63c863ca7"));
	nameHash.push_back(Tokens("(4)Fortress", "83320e505f35c65324e93510ce2eafbaa71c9aa1"));
	nameHash.push_back(Tokens("(4)Python", "de2ada75fbc741cfa261ee467bf6416b10f9e301"));
}

string FileReaderUtils::getFilename(string subpath)
{
	string filename = "N/A";

	if (Constructor::isProtoss())
	{
		if (ExplorationManager::enemyIsProtoss())
		{
			filename = "PvP.txt";
			if (!fileExists(subpath, filename)) filename = "PvX.txt";
		}
		else if (ExplorationManager::enemyIsTerran())
		{
			filename = "PvT.txt";
			if (!fileExists(subpath, filename)) filename = "PvX.txt";
		}
		else if (ExplorationManager::enemyIsZerg())
		{
			filename = "PvZ.txt";
			if (!fileExists(subpath, filename)) filename = "PvX.txt";
		}
		else
		{
			filename = "PvX.txt";
		}
	}
	else if (Constructor::isTerran())
	{
		if (ExplorationManager::enemyIsProtoss())
		{
			filename = "TvP.txt";
			if (!fileExists(subpath, filename)) filename = "TvX.txt";
		}
		else if (ExplorationManager::enemyIsTerran())
		{
			filename = "TvT.txt";
			if (!fileExists(subpath, filename)) filename = "TvX.txt";
		}
		else if (ExplorationManager::enemyIsZerg())
		{
			filename = "TvZ.txt";
			if (!fileExists(subpath, filename)) filename = "TvX.txt";
		}
		else
		{
			filename = "TvX.txt";
		}
	}
	else if (Constructor::isZerg())
	{
		if (ExplorationManager::enemyIsProtoss())
		{
			filename = "ZvP.txt";
			if (!fileExists(subpath, filename)) filename = "ZvX.txt";
		}
		else if (ExplorationManager::enemyIsTerran())
		{
			filename = "ZvT.txt";
			if (!fileExists(subpath, filename)) filename = "ZvX.txt";
		}
		else if (ExplorationManager::enemyIsZerg())
		{
			filename = "ZvZ.txt";
			if (!fileExists(subpath, filename)) filename = "ZvX.txt";
		}
		else
		{
			filename = "ZvX.txt";
		}
	}
	else
	{
		filename = "N/A";
	}

	//Check if file really exists
	if (!fileExists(subpath, filename))
	{
		filename = "N/A";
	}

	return filename;
}

string FileReaderUtils::getScriptPath()
{
	return "";
}

bool FileReaderUtils::fileExists(string subpath, string filename)
{
	ifstream inFile;

	stringstream ss;
	ss << getScriptPath();
	ss << subpath;
	ss << "\\";
	ss << filename;
	string filePath = ss.str();

	inFile.open(filePath.c_str());

	if (!inFile)
	{
		return false;
	}
	else
	{
		inFile.close();
		return true;
	}
}

UnitType FileReaderUtils::getUnitType(string line)
{
	if (line == "") return UnitTypes::Unknown;

	//Replace all _ with whitespaces, or they wont match
	replace(line);
	
	for (auto &u : UnitTypes::allUnitTypes())
	{
		if (u.getName() == line)
		{
			return u;
		}
	}

	//No UnitType match found
	Broodwar << "Error: No matching UnitType found for " << line << endl;
	return UnitTypes::Unknown;
}

UpgradeType FileReaderUtils::getUpgradeType(string line)
{
	if (line == "") return UpgradeTypes::Unknown;

	//Replace all _ with whitespaces, or they wont match
	replace(line);
	
	for (auto &u : UpgradeTypes::allUpgradeTypes())
	{
		if (u.getName() == line)
		{
			return u;
		}
	}

	//No UnitType match found
	return UpgradeTypes::Unknown;
}

TechType FileReaderUtils::getTechType(string line)
{
	if (line == "") return TechTypes::Unknown;

	//Replace all _ with whitespaces, or they wont match
	replace(line);
	
	for (auto &u : TechTypes::allTechTypes())
	{
		if (u.getName() == line)
		{
			return u;
		}
	}

	//No UnitType match found
	Broodwar << "Error: No matching TechType found for " << line << endl;
	return TechTypes::Unknown;
}

void FileReaderUtils::replace(string &line)
{
	int usIndex = line.find("_");
	while (usIndex != string::npos)
	{
		line.replace(usIndex, 1, " ");
		usIndex = line.find("_");
	}
}

int FileReaderUtils::toInt(string &str)
{
	stringstream ss(str);
	int n;
	ss >> n;
	return n;
}

Tokens FileReaderUtils::split(string line, string delimiter)
{
	Tokens tokens;
	tokens.key = "";
	tokens.value = "";

	int eqIndex = line.find(delimiter);
	if (eqIndex != string::npos)
	{
		tokens.key = line.substr(0, eqIndex);
		tokens.value = line.substr(eqIndex + 1, line.length());
	}
	return tokens;
}

string FileReaderUtils::nameToHash(string name)
{
	for (int i = 0; i < (int)nameHash.size(); i++)
	{
		if (nameHash.at(i).key == name)
		{
			return nameHash.at(i).value;
		}
	}
	return "N/A";
}

string FileReaderUtils::hashToName(string hash)
{
	for (int i = 0; i < (int)nameHash.size(); i++)
	{
		if (nameHash.at(i).value == hash)
		{
			return nameHash.at(i).key;
		}
	}
	return "N/A";
}

string FileReaderUtils::getMapName()
{
	return hashToName(Broodwar->mapHash());
}