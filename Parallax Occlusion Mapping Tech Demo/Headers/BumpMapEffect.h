/*
    ----------------
	| Effect (Base)|
	----------------
		   |
    -----------------------------------
	| Bump Map Effect Class (Derived) |
	-----------------------------------

	Loads Values for BumpMap Shader

	Shaun Mitchell
	
*/

#ifndef BUMPMAPEFFECT_H
#define BUMPMAPEFFECT_H

#include "../Headers/Effect.h"
#include "../Headers/ConfigFile.h"

class BumpMapEffect : public Effect
{

public:

	virtual const char * GetClassName() { return "BumpMapEffect"; }

	// Load BumpMapping.fx and read values from BumpMappingConfig.txt file
	void Load(LPCSTR filename); 
	virtual void LoadValuesFromConfig(std::string configFile);

	void Clean();

private:

	ConfigFile*  m_pFile;

};


#endif