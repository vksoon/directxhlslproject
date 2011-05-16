/*  
	Parallax Occlusion Mapping Shader
	Shaun Mitchell
	Adapted from DirectX SDK Samples
	and Jason Zink @ GameDev article
	http://www.gamedev.net/reference/articles/article2325.asp
*/

// The colour texture  
texture BaseTexture;

// The Parallax occlusion texture, Normal map with height map in alpha channel
texture HeightMapTexture;

float WaterMovement;

// Material Properties
// Loaded in from shader config file
float4 MaterialAmbientColour;
float4 MaterialDiffuseColour;
float4 MaterialSpecularColour;

float SpecularExponent = 10000.0; // Specular exponent of material 
// Lower colour means larger highlight

// Light Values //
float3 LightDirection; // Direction of light in world space
float4 DiffuseColour = float4(1,1,1,1); // Lights diffuse colour // Could be loaded from config file
float4 AmbientColour = float4(1,1,1,1); // Lights ambient colour // Could be loaded from config file

float4 EyeLocation; // Location of camera
float BaseTextureRepeat; // Amount to repeat texture // Loaded from config file
float HeightMapScale; // Loaded from config file
 
// Matrices //
float4x4 World;
float4x4 WorldViewProjection;
float4x4 View;

float2 TextureDimensions; // Used for accurate calculation of partial derivatives

int LevelOfDetailThreshold = 100;  // Used for performance enhancement // Distance from camera location before using normal mapping instead of POM

float ShadowSoftening = 0.5; // Blurriness of realtime shadows

float BlendAlpha = 1; // Alpha value

int MinSamples = 8; // Minimum amount of height map samples
int MaxSamples = 50; // Maximum amount of height map samples

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

	// The directional light vector in world space
	float3 LightWorldSpace = LightDirection;
	
	// Create a rotation matrix to transform from world space to tangent space 
	// using the tangent, binormal and normal vectors:
	float3x3 WorldToTangentMatrix = float3x3(TangentWorldSpace, BinormalWorldSpace, NormalWorldSpace);
	
	// Transform View and light vectors into tangent space
	output.LightTangentSpace = mul(LightWorldSpace, WorldToTangentMatrix);
	output.ViewTangentSpace = mul(WorldToTangentMatrix, ViewWorldSpace);
	
	output.ParallaxOffsetTangentSpace = output.ViewTangentSpace.xy / output.ViewTangentSpace.z;
	
	// Multiply this vector by an editable parameter that can be adjusted for 
	// different types of surfaces which may require less height contrast:
	output.ParallaxOffsetTangentSpace *= HeightMapScale;
	
	return output;
}

float4 PixelShaderFunction(VertexShaderOutput input) : COLOR0
{
	 // First normalise all the vectors that are input from the vertex shader, this is because 
	 // in all calculations from now on they will need to be normalised:
   float3 ViewTangentSpace   = normalize( input.ViewTangentSpace  );
   float3 ViewWorldSpace   = normalize( input.ViewWorldSpace  );
   float3 LightTangentSpace  = normalize( input.LightTangentSpace );
   float3 NormalWorldSpace = normalize( input.NormalWorldSpace );
	 
   float4 ResultColour = float4( 0, 0, 0, 1 );
   
   /* 
      According to Microsoft's MSDN no gradient computations can be made inside a control flow. 
      This algorithm uses a call to tex2Dgrad so the gradient computations need to be carried out before the loop itself. 
      This can be computed using the ddx and ddy functions which compute partial derivatives:
   */
   float2 TextureCoordinatesPerSize = input.TextureCoordinate * TextureDimensions;

   float2 dxSize, dySize;
   float2 dx, dy;

   // Computing partial derivatives uses the ddx and ddy functions.
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
	  
	  /* 
	     The number of samples to take is calculated using the dot product of the View and Normal vectors in world space
	     this value is used to determine how direct the eye is looking at the surface, the number of samples is calculated
	     by lerping between the passed in Max and Min samples values.
	  */
	  int NumSteps = (int) lerp( MaxSamples, MinSamples, dot( ViewWorldSpace, NormalWorldSpace ) );
	  
	  float CurrHeight = 0.0;
	  
	  // Use this value to create a step size which equates to how far to move along the parallax offset vector for each sample. 
	  // It is calculated using the maximum value in the height map (1.0) divided by NumSteps:
	  float StepSize   = 1.0 / (float) NumSteps;
	  
	  // To start off the algorithm the previous height value which is what is compared for each sample is set to 1.0 
	  // which is the highest point.
	  float PrevHeight = 1.0;
	  float NextHeight = 0.0;

	  int    StepIndex = 0;

	  float2 TexOffsetPerStep = StepSize * input.ParallaxOffsetTangentSpace;
	  float2 TexCurrentOffset = input.TextureCoordinate;
	  float  CurrentBound     = 1.0;
	  float  ParallaxAmount   = 0.0;

	  // Create the 2D vectors that will be used as the sampled points 
	  float2 pt1 = 0;
	  float2 pt2 = 0;
	   
	  float2 texOffset2 = 0;

	  /* 
	     While looping sample the texture coordinate along the parallax offset vector and then compare it to the height map which is stored in the
	     alpha channel. If the eye vector is higher in height than the height maps value continue. If eye vector has a lower height than the height map
	     then an intersection has been found and it will reside between currHeight and prevHeight.
	   */
	  
	  // Begin the loop
	  while ( StepIndex < NumSteps ) 
	  {
	     // Backtrace along the parallax offset vector by the StepSize value
		 TexCurrentOffset -= TexOffsetPerStep;

		 // Sample height map which in this case is stored in the alpha channel of the normal map:
		 CurrHeight = tex2Dgrad( HeightMapSampler, TexCurrentOffset, dx, dy ).a;

		 // Decrease the current height of the eye vector by the StepSize value
		 CurrentBound -= StepSize;

		 /* 
		    Check if the eye vector has a larger height value than the CurrHeight sampled from the 
		    height map which would mean there has been no intersection yet so increment the StepIndex. 
		    If there has been an intersection (eye vector has a smaller value than CurrHeight) it will lie 
		    between pt1 and pt2 which are the current sample point and the previous sampled point.
		 */
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

	  // Find the intersection of the eye vector with the line between pt1 and pt2 to get the current offset texture coordinates:
	  float Delta2 = pt2.x - pt2.y;
	  float Delta1 = pt1.x - pt1.y;
	  
	  float Denominator = Delta2 - Delta1;
	  
	  // Shader model 3.0 needs a check for divide by 0
	  if ( Denominator == 0.0f )
	  {
		 ParallaxAmount = 0.0f;
	  }
	  else
	  {
		 ParallaxAmount = (pt1.x * Delta2 - pt2.x * Delta1 ) / Denominator;
	  }
	  
	  // Set the newly found texture coordinates
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
	   
   float RdotL = saturate(  dot( ReflectionTangentSpace, LightTangentSpaceAdj ));
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


