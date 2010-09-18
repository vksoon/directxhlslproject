/*
    ----------------
	| Effect (Base)|
	----------------

	Derive specific effects

	Shaun Mitchell
*/
#ifndef EFFECT_H_
#define EFFECT_H_

#include <vector>
#include <string>
#include "D3DObject.h"

class Effect
{
public:
	static std::vector<ID3DXEffect*>   Effects;

	virtual ~Effect() { m_pEffect->Release(); }
	virtual const char * GetClassName() { return "Effect";}

	virtual void Load(LPCSTR filename);
	virtual void LoadValuesFromConfig(std::string configFile);

	ID3DXEffect* GetEffect() { return m_pEffect; }

	virtual void Clean();

protected:

	ID3DXEffect* m_pEffect;

};
typedef std::vector<ID3DXEffect*>  Effects;

#endif