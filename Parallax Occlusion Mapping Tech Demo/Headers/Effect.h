/*
Game Object Base Class
Shaun Mitchell
*/
#ifndef EFFECT_H_
#define EFFECT_H_

#include <vector>
#include <string>
#include "D3DObject.h"

// Effect Base Class, Derive specific effects
class Effect
{
public:
	static std::vector<ID3DXEffect*>   Effects;

	virtual ~Effect() { m_pEffect->Release(); }
	virtual const char * GetClassName() { return "Effect";}

	virtual void Load(LPCWSTR filename);
	ID3DXEffect* GetEffect();

	virtual void Clean();

protected:

	ID3DXEffect* m_pEffect;

};
typedef std::vector<ID3DXEffect*>  Effects;

#endif