#include "../Headers/Effect.h"

std::vector<ID3DXEffect*>   Effect::Effects;

void Effect::Load(LPCWSTR filename)
{
	D3DXCreateEffectFromFile(D3DObj::Instance()->GetDeviceClass(),filename,NULL,NULL,0,NULL,&m_pEffect,NULL);
	Effect::Effects.push_back(m_pEffect);
}

ID3DXEffect* Effect::GetEffect()
{
	return m_pEffect;
}

void Effect::Clean()
{
}

