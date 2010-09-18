/*  
	Parallax Occlusion Mapping Shader
	Shaun Mitchell
	Adapted from DirectX SDK Samples
	and Jason Zink @ GameDev article
	http://www.gamedev.net/reference/articles/article2325.asp
*/

// The base texture 
texture BaseTexture;

// The height map texture
texture HeightMapTexture;

// Material Properties //

float WaterMovement;

float4 MaterialAmbientColour;
float4 MaterialDiffuseColour;
float4 MaterialSpecularColour;

float SpecularExponent = 10000.0; // Specular exponent of material 
// Lower colour means larger highlight

// Light Values //

float3 LightDirection;// = float3(0,1,0); // Direction of light in world space
float4 DiffuseColour = float4(1,1,1,1); // Lights diffuse colour
float4 AmbientColour = float4(1,1,1,1); // Lights ambient colour

float4 EyeLocation;// = float4(1,0,0,0); // Location of camera
float BaseTextureRepeat;// = 5.0; // Amount to repeat texture
float HeightMapScale;// = 0.02;

// Matrices //
float4x4 World;
float4x4 WorldViewProjection;
float4x4 View;

float2 TextureDimensions;// = float2(512,512);

int LevelOfDetailThreshold = 100;  // Used for performance enhancement

float ShadowSoftening = 0.5;

float BlendAlpha = 1;

int MinSamples = 8;
int MaxSamples = 50;

// Texture Samplers //

sampler BaseSampler =
sampler_state
{
	Texture = <BaseTexture>;
	MipFilter = LINEAR;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

sampler HeightMapSampler =
sampler_state
{
	Texture = <HeightMapTexture>;
	MipFilter = LINEAR;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

// Vertex Input Structure

struct VertexShaderInput
{
	float4 Position : POSITION;
	float2 TextureCoordinate : TEXCOORD0;
	float3 Normal : NORMAL;
	float3 Binormal : BINORMAL;
	float3 Tangent : TANGENT;
};

// Vertex Output Structure //

struct VertexShaderOutput
{
	float4 Position		: POSITION;
	float2 TextureCoordinate	: TEXCOORD0;
	float3 LightTangentSpace	: TEXCOORD1;
	float3 ViewTangentSpace		: TEXCOORD2;
	float2 ParallaxOffsetTangentSpace	: TEXCOORD3;
	float3 NormalWorldSpace : TEXCOORD4;
	float3 ViewWorldSpace	: TEXCOORD5;
};

VertexShaderOutput VertexShaderFunction(VertexShaderInput input)
{
	VertexShaderOutput output;
	
	// Transform position to world space
	output.Position = mul(input.Position, WorldViewProjection);
	
	// Send texture coordinates through // and WaterMovement for moving water
	output.TextureCoordinate = (input.TextureCoordinate + WaterMovement) * BaseTextureRepeat;
	
	// Transform Normal, Tangent and Binormal to world space
	float3 NormalWorldSpace = mul(input.Normal, (float3x3) World);
	float3 TangentWorldSpace = mul(input.Tangent, (float3x3) World);
	float3 BinormalWorldSpace = mul(input.Binormal, (float3x3) World);
	
	// Propogate Normal in world space, denormalized
	output.NormalWorldSpace = NormalWorldSpace;
	
	// Normalize vectors
	NormalWorldSpace = normalize(NormalWorldSpace);
	TangentWorldSpace = normalize(TangentWorldSpace);
	BinormalWorldSpace = normalize(BinormalWorldSpace);
	
	// Get position in world space
	float4 PositionWorldSpace = mul(input.Position, World);
	
	// Compute view vector in world space
	float3 ViewWorldSpace = EyeLocation - PositionWorldSpace;
	output.ViewWorldSpace = ViewWorldSpace;

	float3 LightWorldSpace = LightDirection;
	
	// Create a world to tangent matrix
	float3x3 WorldToTangentMatrix = float3x3(TangentWorldSpace, BinormalWorldSpace, NormalWorldSpace);
	
	// Transform View and light vectors into tangent space
	output.LightTangentSpace = mul(LightWorldSpace, WorldToTangentMatrix);
	output.ViewTangentSpace = mul(WorldToTangentMatrix, ViewWorldSpace);
	
	// Initial parallax displacement vector 
	float2 ParallaxDirection = normalize(output.ViewTangentSpace.xy);
	
	// Compute length of parallax displacement vector, this equates to the amount of displacement
	float Length = length(output.ViewTangentSpace);
	float ParallaxLength = sqrt(Length * Length - output.ViewTangentSpace.z * output.ViewTangentSpace.z) / output.ViewTangentSpace.z;
	
	// output initial parallax offset in tangent space 
	output.ParallaxOffsetTangentSpace = ParallaxDirection * ParallaxLength;
	
	// output parallax vector multiplied by editable value
	output.ParallaxOffsetTangentSpace *= HeightMapScale;
	
	return output;
}

float4 PixelShaderFunction(VertexShaderOutput input) : COLOR0
{
	 //  Normalize the interpolated vectors:
   float3 ViewTangentSpace   = normalize( input.ViewTangentSpace  );
   float3 ViewWorldSpace   = normalize( input.ViewWorldSpace  );
   float3 LightTangentSpace  = normalize( input.LightTangentSpace );
   float3 NormalWorldSpace = normalize( input.NormalWorldSpace );
	 
   float4 ResultColour = float4( 0, 0, 0, 1 );
   
   // Compute the current gradients:
   float2 TextureCoordinatesPerSize = input.TextureCoordinate * TextureDimensions;

   // Compute all 4 derivatives in x and y in a single instruction to optimize:
   float2 dxSize, dySize;
   float2 dx, dy;

   float4( dxSize, dx ) = ddx( float4( TextureCoordinatesPerSize, input.TextureCoordinate ) );
   float4( dySize, dy ) = ddy( float4( TextureCoordinatesPerSize, input.TextureCoordinate ) );
				  
   float  MipLevel;      
   float  MipLevelInt;    // mip level integer portion
   float  MipLevelFrac;   // mip level fractional amount for blending in between levels

   float  MinTexCoordDelta;
   float2 TexCoords;

   // Find min of change in u and v across quad: compute du and dv magnitude across quad
   TexCoords = dxSize * dxSize + dySize * dySize;

   // Standard mipmapping uses max here
   MinTexCoordDelta = max( TexCoords.x, TexCoords.y );

   // Compute the current mip level  (* 0.5 is effectively computing a square root before )
   MipLevel = max( 0.5 * log2( MinTexCoordDelta ), 0 );
	
   // Start the current sample located at the input texture coordinate, which would correspond
   // to computing a bump mapping result:
   float2 TextureSample = input.TextureCoordinate;

   float OcclusionShadow = 1.0;

   if ( MipLevel <= (float) LevelOfDetailThreshold )
   {
	  
	  int NumSteps = (int) lerp( MaxSamples, MinSamples, dot( ViewWorldSpace, NormalWorldSpace ) );
	  
	  float CurrHeight = 0.0;
	  float StepSize   = 1.0 / (float) NumSteps;
	  float PrevHeight = 1.0;
	  float NextHeight = 0.0;

	  int    StepIndex = 0;
	  bool   Condition = true;

	  float2 TexOffsetPerStep = StepSize * input.ParallaxOffsetTangentSpace;
	  float2 TexCurrentOffset = input.TextureCoordinate;
	  float  CurrentBound     = 1.0;
	  float  ParallaxAmount   = 0.0;

	  float2 pt1 = 0;
	  float2 pt2 = 0;
	   
	  float2 texOffset2 = 0;

	  while ( StepIndex < NumSteps ) 
	  {
		 TexCurrentOffset -= TexOffsetPerStep;

		 // Sample height map which in this case is stored in the alpha channel of the normal map:
		 CurrHeight = tex2Dgrad( HeightMapSampler, TexCurrentOffset, dx, dy ).a;

		 CurrentBound -= StepSize;

		 if ( CurrHeight > CurrentBound ) 
		 {   
			pt1 = float2( CurrentBound, CurrHeight );
			pt2 = float2( CurrentBound + StepSize, PrevHeight );

			texOffset2 = TexCurrentOffset - TexOffsetPerStep;

			StepIndex = NumSteps + 1;
			PrevHeight = CurrHeight;
		 }
		 else
		 {
			StepIndex++;
			PrevHeight = CurrHeight;
		 }
	  }   

	  float Delta2 = pt2.x - pt2.y;
	  float Delta1 = pt1.x - pt1.y;
	  
	  float Denominator = Delta2 - Delta1;
	  
	  // SM 3.0 requires a check for divide by zero, since that operation will generate
	  // an 'Inf' number instead of 0, as previous models (conveniently) did:
	  if ( Denominator == 0.0f )
	  {
		 ParallaxAmount = 0.0f;
	  }
	  else
	  {
		 ParallaxAmount = (pt1.x * Delta2 - pt2.x * Delta1 ) / Denominator;
	  }
	  
	  float2 ParallaxOffset = input.ParallaxOffsetTangentSpace * (1 - ParallaxAmount );

	  // The computed texture offset for the displaced point on the pseudo-extruded surface:
	  float2 TextureSampleBase = input.TextureCoordinate - ParallaxOffset;
	  TextureSample = TextureSampleBase;

	  if ( MipLevel > (float)(LevelOfDetailThreshold - 1) )
	  {
		 // Lerp based on the fractional part:
		 MipLevelFrac = modf( MipLevel, MipLevelInt );
	 
		 // Lerp the texture coordinate from parallax occlusion mapped coordinate to bump mapping
		 // smoothly based on the current mip level:
		 TextureSample = lerp( TextureSampleBase, input.TextureCoordinate, MipLevelFrac );
	  }  
	  
		float2 LightRayTangentSpace = LightTangentSpace.xy * HeightMapScale;
	  
		// Compute the soft blurry shadows taking into account self-occlusion for 
		// features of the height field:
   
		float sh0 =  tex2Dgrad( HeightMapSampler, TextureSampleBase, dx, dy ).a;
		float shA = (tex2Dgrad( HeightMapSampler, TextureSampleBase + LightRayTangentSpace * 0.88, dx, dy ).a - sh0 - 0.88 ) *  1 * ShadowSoftening;
		float sh9 = (tex2Dgrad( HeightMapSampler, TextureSampleBase + LightRayTangentSpace * 0.77, dx, dy ).a - sh0 - 0.77 ) *  2 * ShadowSoftening;
		float sh8 = (tex2Dgrad( HeightMapSampler, TextureSampleBase + LightRayTangentSpace * 0.66, dx, dy ).a - sh0 - 0.66 ) *  4 * ShadowSoftening;
		float sh7 = (tex2Dgrad( HeightMapSampler, TextureSampleBase + LightRayTangentSpace * 0.55, dx, dy ).a - sh0 - 0.55 ) *  6 * ShadowSoftening;
		float sh6 = (tex2Dgrad( HeightMapSampler, TextureSampleBase + LightRayTangentSpace * 0.44, dx, dy ).a - sh0 - 0.44 ) *  8 * ShadowSoftening;
		float sh5 = (tex2Dgrad( HeightMapSampler, TextureSampleBase + LightRayTangentSpace * 0.33, dx, dy ).a - sh0 - 0.33 ) * 10 * ShadowSoftening;
		float sh4 = (tex2Dgrad( HeightMapSampler, TextureSampleBase + LightRayTangentSpace * 0.22, dx, dy ).a - sh0 - 0.22 ) * 12 * ShadowSoftening;
   
		// Compute the actual shadow strength:
		OcclusionShadow = 1 - max( max( max( max( max( max( shA, sh9 ), sh8 ), sh7 ), sh6 ), sh5 ), sh4 );
	  
		// The previous computation overbrightens the image, let's adjust for that:
		OcclusionShadow = OcclusionShadow * 0.6 + 0.4;         
			
   } 

   // Sample the normal from the normal map for the given texture sample:
   float3 NormalTangentSpace = normalize( tex2D( HeightMapSampler, TextureSample ) * 2 - 1 );
   
   // Sample base map
   float4 BaseColour = tex2D( BaseSampler, TextureSample );
   BaseColour.a = 0.1;
   
   // Compute diffuse color component:
   float3 LightTangentSpaceAdj = float3( LightTangentSpace.x, -LightTangentSpace.y, LightTangentSpace.z );
   
   float4 Diffuse = saturate( dot( NormalTangentSpace, LightTangentSpaceAdj )) * MaterialDiffuseColour;
   
   // Compute the specular component 
   float4 Specular = 0;
   float3 ReflectionTangentSpace = normalize( 2 * dot( ViewTangentSpace, NormalTangentSpace ) * NormalTangentSpace - ViewTangentSpace );
	   
   
   float RdotL = saturate( dot( ReflectionTangentSpace, LightTangentSpaceAdj ));
   Specular = saturate( pow( RdotL, SpecularExponent )) * MaterialSpecularColour;
   
   // Composite the final color:
   float4 FinalColour = (( MaterialAmbientColour + Diffuse ) * BaseColour + Specular ) * OcclusionShadow;
  
   ResultColour.rgb = FinalColour;
   ResultColour.a = BlendAlpha;
	
   return ResultColour;
		
}

technique ParallaxOcclusionMapping
{
	pass P0
	{
		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;  
		VertexShader = compile vs_3_0 VertexShaderFunction();
		PixelShader  = compile ps_3_0 PixelShaderFunction(); 
	}
}


