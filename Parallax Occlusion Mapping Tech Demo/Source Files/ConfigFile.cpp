#include "../Headers/ConfigFile.h"

bool ConfigFile::Load(const std::string& filename)
{
	if(!m_file.Open(filename))
	{
		return false;
	}
	std::string s;
	while (m_file.GetString(&s))
	{
		unsigned int pos = s.find('=');
		if (pos != std::string::npos)
		{
			std::string key = s.substr(0, pos);
			std::string value = s.substr(pos + 1);
			m_map[key] = value;

		}
	}
	return true;
}

int ConfigFile::GetInt(const std::string& key)
{
	std::string intString = GetString(key);
	return atoi(intString.c_str());
}

float ConfigFile::GetFloat(const std::string& key)
{
	std::string floatString = GetString(key);
	return (float)atof(floatString.c_str());
}

std::string ConfigFile::GetString(const std::string& key)
{
	ConfigMap::iterator it = m_map.find(key);
	return it->second;
}

bool ConfigFile::EndOfFile()
{
	return true;
}