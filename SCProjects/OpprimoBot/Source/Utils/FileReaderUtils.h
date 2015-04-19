#ifndef __FILEREADERUTILS_H__
#define __FILEREADERUTILS_H__

#include <BWAPI.h>
#include <vector>

using namespace BWAPI;
using namespace std;

struct Tokens {
	string key;
	string value;

	Tokens()
	{
		key = "";
		value = "";
	}

	Tokens(string mKey, string mValue)
	{
		key = mKey;
		value = mValue;
	}
};

/** This class contains some common methods used by classes handling the
 * reading of buildorder/techs/upgrades/squad setup files.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class FileReaderUtils {

private:
	vector<Tokens> nameHash;

public:
	FileReaderUtils();

	/** Returns the filename to use in sub folder squads, buildorder or upgrades.
	 * The methods checks if for example PvZ is defined, and if not PvX is used. */
	string getFilename(string subpath);

	/** Returns the path to the folder where the scripfiles are placed. */
	string getScriptPath();

	/** Checks if a file in the specified subpath exists, for example PvZ.txt in
	 * subfolder buildorder. */
	bool fileExists(string subpath, string filename);

	/** Returns a unit type from a textline, or Unknown if no type was found. */
	UnitType getUnitType(string line);

	/** Returns an upgrade type from a textline, or Unknown if no type was found. */
	UpgradeType getUpgradeType(string line);

	/** Returns a tech type from a textline, or Unknown if no type was found. */
	TechType getTechType(string line);

	/** Replaces all underscores (_) with whitespaces in a string. */
	void replace(string &line);

	/** Splits a line into tokens. Delimiter is the characted to split at, for example = or :. */
	Tokens split(string line, string delimiter);

	/** Converts a string to an int. */
	int toInt(string &str);

	/** Gets the name for the current map. */
	string getMapName();

	/** Returns the hash id for a map name. */
	string nameToHash(string name);

	/** Returns the map name for a hash id. */
	string hashToName(string hash);

};

#endif
