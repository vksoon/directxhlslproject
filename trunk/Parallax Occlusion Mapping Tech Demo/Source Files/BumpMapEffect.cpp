#include "../Headers/BumpMapEffect.h"

void BumpMapEffect::Load(LPCSTR filename)
{
	Effect::Load(filename);
}

void BumpMapEffect::LoadValuesFromConfig(std::string configFile)
{
	m_pFile = new ConfigFile();
	m_pFile->Load(configFile);

	float AmbientRed,AmbientGreen,AmbientBlue,AmbientAlpha;

	AmbientRed = m_pFile->GetFloat("AmbientRed");
	AmbientGreen = m_pFile->GetFloat("AmbientGreen");
	AmbientBlue = m_pFile->GetFloat("AmbientBlue");
	AmbientAlpha = m_pFile->GetFloat("AmbientAlpha");

	//AmbientIntensity = m_pFile->GetFloat("AmbientIntensity");

	D3DXCOLOR AmbientColour(AmbientRed,AmbientGreen,AmbientBlue,AmbientAlpha);
	m_pEffect->SetValue("AmbientColour", &AmbientColour, sizeof(AmbientColour)); // set in shader
}

void BumpMapEffect::Clean() {}