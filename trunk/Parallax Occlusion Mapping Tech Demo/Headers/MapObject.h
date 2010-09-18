/*
    --------------------
	| GameObject (Base)|
	--------------------
			|
	----------------------
	| MapObject (Derived)|
	----------------------

	Map Object Class
	
	Use to create map objects that use parallax occlusion mapping
	
	Contains ParallaxOcclusionMappingEffect * 

	Shaun Mitchell
*/
#ifndef MAP_OBJECT_H
#define MAP_OBJECT_H

#include "../Headers/GameObject.h"
#include "../Headers/ParallaxOcclusionMappingEffect.h"

#include <vector>

class MapObject : public GameObject
{
public:

	virtual const char * GetClassName() { return "MapObject";}

	bool Load(File * pFile);
	void Draw();
	void Update(float dt);
	void Clean();

	Effect* GetEffect();

private:

	Effect* m_pParallaxEffect;
};

#endif