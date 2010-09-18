/*
	----------------
	| Effect (Base)|
	----------------
		   |
    -----------------------------------------------------
	| Parallax Occlusion Mapping Effect Class (Derived) |
	-----------------------------------------------------

	Loads Values for Parallax Occlusion Shader

	Shaun Mitchell
	
*/

#ifndef PARALLAX_OCCLUSION_MAPPING_EFFECT_H
#define PARALLAX_OCCLUSION_MAPPING_EFFECT_H

#include "../Headers/Effect.h"
#include "../Headers/ConfigFile.h"

class ParallaxOcclusionMappingEffect : public Effect
{

public:

	virtual const char * GetClassName() { return "ParallaxOcclusionMappingEffect"; }
	// Load ParallaxOcclusionMapping.fx
	void Load(LPCSTR filename); 
	// Read in values if needed
	virtual void LoadValuesFromConfig(std::string configFile);

	void Clean();

private:

	ConfigFile*  m_pFile;

};

#endif