#include "../Headers/Effect.h"

std::vector<ID3DXEffect*>   Effect::Effects;

void Effect::Load(LPCSTR filename)
{
	D3DXCreateEffectFromFile(D3DObj::Instance()->GetDeviceClass(),filename,NULL,NULL,0,NULL,&m_pEffect,NULL);
	Effect::Effects.push_back(m_pEffect);
}

void Effect::LoadValuesFromConfig(std::string configFile)
{
}

void Effect::Clean()
{
}

