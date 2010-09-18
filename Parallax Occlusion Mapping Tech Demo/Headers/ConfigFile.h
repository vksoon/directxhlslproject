#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <string>
#include <map>
#include "File.h"

class ConfigFile
{
public:
	ConfigFile() {}
	bool Load(const std::string& filename);
	int GetInt(const std::string& key);
	float GetFloat(const std::string& key);
	std::string GetString(const std::string& key);
	bool EndOfFile();

private:
	typedef std::map<std::string, std::string> ConfigMap;
	ConfigMap m_map;
	File m_file;


};

#endif