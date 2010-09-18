#include "../Headers/ParallaxOcclusionMappingEffect.h"

/* 
	Parallax Occlusion Mapping Values
	
	texture Basetexture - base texture of the mesh (not passed in here)
	texture HeightMapTexture - the parallax occlusion map (not passed in here)

	float WaterMovement - pass in for smooth movement (not passed in here)

	float4 MaterialAmbientColour - materials ambient colour
	float4 MaterialDiffuseColour - materials diffuse colour
	float4 MaterialSpecularColour - materials specular colour

	float SpecularExponent - lower value larger highlight

	float3 LightDirection - direction of light in world space - TODO update dynamically
	float4 DiffuseColour - diffuse colour of light
	float4 AmbientColour - ambient colour of light

	float4 EyeLocation - cameras location (not passed in here)
	float BaseTextureRepeat - amount to repeat a single texture
	float HeightMapScale - the depth of the mapping

	float4x4 World - the world matrix (not passed in here)
	float4x4 WorldViewProjection - the world * view * projection matrix (not passed in here)
	float4x4 View - the view matrix (not passed in here)

	float2 TextureDimensions - the dimensions of the base texture
	int LevelOfDetailThreshold - performance enhancement

	float ShadowSoftening - blur of self occlusion shadows

	float BlendAlpha - alpha value of texture

	int MinSamples - minimum amount of samples or cross sections
	int MaxSamples - maximum amount of samples or cross sections
*/

void ParallaxOcclusionMappingEffect::Load(LPCSTR filename)
{
	Effect::Load(filename);
}

void ParallaxOcclusionMappingEffect::LoadValuesFromConfig(std::string configFile)
{
	m_pFile = new ConfigFile();
	m_pFile->Load(configFile);

	// Get and set MaterialAmbientColour
	float MaterialAmbientRed, MaterialAmbientGreen, MaterialAmbientBlue, MaterialAmbientAlpha;
	MaterialAmbientRed = m_pFile->GetFloat("MaterialAmbientRed");
	MaterialAmbientGreen = m_pFile->GetFloat("MaterialAmbientGreen");
	MaterialAmbientBlue = m_pFile->GetFloat("MaterialAmbientBlue");
	MaterialAmbientAlpha = m_pFile->GetFloat("MaterialAmbientAlpha");
	
	D3DXCOLOR MaterialAmbientColour(MaterialAmbientRed, MaterialAmbientGreen, MaterialAmbientBlue, MaterialAmbientAlpha );
	m_pEffect->SetValue("MaterialAmbientColour", &MaterialAmbientColour, sizeof(MaterialAmbientColour));

	// Get and set MaterialDiffuseColour
	float MaterialDiffuseRed, MaterialDiffuseGreen, MaterialDiffuseBlue, MaterialDiffuseAlpha;
	MaterialDiffuseRed = m_pFile->GetFloat("MaterialDiffuseRed");
	MaterialDiffuseGreen = m_pFile->GetFloat("MaterialDiffuseGreen");
	MaterialDiffuseBlue = m_pFile->GetFloat("MaterialDiffuseBlue");
	MaterialDiffuseAlpha = m_pFile->GetFloat("MaterialDiffuseAlpha");

	D3DXCOLOR MaterialDiffuseColour(MaterialDiffuseRed, MaterialDiffuseGreen, MaterialDiffuseBlue, MaterialDiffuseAlpha );
	m_pEffect->SetValue("MaterialDiffuseColour", &MaterialDiffuseColour, sizeof(MaterialDiffuseColour));

	// Get and set MaterialSpecularColour
	float MaterialSpecularRed, MaterialSpecularGreen, MaterialSpecularBlue, MaterialSpecularAlpha;
	MaterialSpecularRed = m_pFile->GetFloat("MaterialSpecularRed");
	MaterialSpecularGreen = m_pFile->GetFloat("MaterialSpecularGreen");
	MaterialSpecularBlue = m_pFile->GetFloat("MaterialSpecularBlue");
	MaterialSpecularAlpha = m_pFile->GetFloat("MaterialSpecularAlpha");

	D3DXCOLOR MaterialSpecularColour(MaterialSpecularRed, MaterialSpecularGreen, MaterialSpecularBlue, MaterialSpecularAlpha );
	m_pEffect->SetValue("MaterialSpecularColour", &MaterialSpecularColour, sizeof(MaterialSpecularColour));

	// Get and set LightDirection vector
	float LightDirectionX, LightDirectionY, LightDirectionZ;
	LightDirectionX = m_pFile->GetFloat("LightDirectionX");
	LightDirectionY = m_pFile->GetFloat("LightDirectionY");
	LightDirectionZ = m_pFile->GetFloat("LightDirectionZ");
	D3DXVECTOR3 LightDirection(LightDirectionX, LightDirectionY, LightDirectionZ);
	m_pEffect->SetValue("LightDirection", &LightDirection, sizeof(LightDirection));
	
	// Get and set BlendAlpha
	float BlendAlpha;
	BlendAlpha = m_pFile->GetFloat("BlendAlpha");
	m_pEffect->SetValue("BlendAlpha", &BlendAlpha, sizeof(BlendAlpha)); // set in shader

	// Get and set TextureRepeat
	float TextureRepeat;
	TextureRepeat = m_pFile->GetFloat("TextureRepeat");
	m_pEffect->SetValue("BaseTextureRepeat", &TextureRepeat, sizeof(TextureRepeat)); // set in shader

	// Get and set dt
	float WaterMovement;
	WaterMovement = m_pFile->GetFloat("WaterMovement");
	m_pEffect->SetValue("WaterMovement", &WaterMovement, sizeof(WaterMovement)); // set in shader

	float SpecularExponent;
	SpecularExponent = m_pFile->GetFloat("SpecularExponent");
	m_pEffect->SetValue("SpecularExponent", &SpecularExponent, sizeof(SpecularExponent));

	float HeightMapScale;
	HeightMapScale = m_pFile->GetFloat("HeightMapScale");
	m_pEffect->SetValue("HeightMapScale", &HeightMapScale, sizeof(HeightMapScale));

	float TextureDimensionsX, TextureDimensionsY;
	TextureDimensionsX = m_pFile->GetFloat("TextureDimensionsX");
	TextureDimensionsY = m_pFile->GetFloat("TextureDimensionsY");
	D3DXVECTOR2 TextureDimensions(TextureDimensionsX, TextureDimensionsY);
	m_pEffect->SetValue("TextureDimensions", &TextureDimensions, sizeof(TextureDimensions));
}

void ParallaxOcclusionMappingEffect::Clean() {}