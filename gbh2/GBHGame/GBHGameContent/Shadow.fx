//========================================================================
//
//	DeferredShadowMaps
//
//		by MJP  (mpettineo@gmail.com)
//		12/14/08      
//
//========================================================================
//
//	File:		ShadowMap.fx
//
//	Desc:		Contains shaders used for generating and applying deferred
//				shadow maps.  
//
//========================================================================

#include "Common.fxh"

float4x4	World;
float4x4	g_matWorldIT;
float4x4	g_matViewProj;

float4x4	g_matInvView;
float		g_fFarClip;

float4x4	InvertViewProjection;

float4x4	g_matLightViewProj;
float2		g_vShadowMapSize;
float2		g_vOcclusionTextureSize;

float3		g_vFrustumCornersVS [4];

static const float BIAS = 0.006f;

texture DepthTexture;
sampler2D DepthTextureSampler = sampler_state
{
    Texture = <DepthTexture>;
    MinFilter = point;
    MagFilter = point;
    MipFilter = point;
};

texture ShadowMap;
sampler2D ShadowMapSampler = sampler_state
{
    Texture = <ShadowMap>;
    MinFilter = point; 
    MagFilter = point; 
    MipFilter = point; 
};

texture ColorMap;
sampler2D ColorMapSampler = sampler_state
{
	Texture = <ColorMap>;
	MinFilter = point;
	MagFilter = point;
	MipFilter = point;
};

// Vertex shader for outputting light-space depth to the shadow map
void GenerateShadowMapVS(	in float4 in_vPositionOS	: SV_Position,
							in float2 in_vTexCoord		: TEXCOORD0,
							out float4 out_vPositionCS	: SV_Position,
							out float2 out_vTexCoord	: TEXCOORD0,
							out float2 out_vDepthCS		: TEXCOORD1	)
{
	// Figure out the position of the vertex in view space and clip space
	float4x4 matWorldViewProj = mul(World, g_matViewProj);
    out_vPositionCS = mul(in_vPositionOS, matWorldViewProj);
	out_vDepthCS = out_vPositionCS.zw;
	out_vTexCoord = in_vTexCoord;
}

// Pixel shader for outputting light-space depth to the shadow map
float4 GenerateShadowMapPS(in float2 in_vTexCoord : TEXCOORD0, in float2 in_vDepthCS : TEXCOORD1, out float4 out_cColor : COLOR1) : COLOR0
{
	// Negate and divide by distance to far clip (so that depth is in range [0,1])
	float fDepth = in_vDepthCS.x / in_vDepthCS.y;	

	float4 color = tex2D(ColorMapSampler, in_vTexCoord);
	
	clip(color.a - 0.78125);
		
	out_cColor = color;

    return float4(fDepth, 1, 1, 1); 
	//return color;
}



// Vertex shader for rendering the full-screen quad used for calculating
// the shadow occlusion factor.
void ShadowTermVS (	in float3 in_vPositionOS				: SV_Position,
					in float3 in_vTexCoordAndCornerIndex	: TEXCOORD0,		
					out float4 out_vPositionCS				: SV_Position,
					out float2 out_vTexCoord				: TEXCOORD0,
					out float3 out_vFrustumCornerVS			: TEXCOORD1	)
{
	// Offset the position by half a pixel to correctly align texels to pixels
	out_vPositionCS.x = in_vPositionOS.x - (1.0f / g_vOcclusionTextureSize.x);
	out_vPositionCS.y = in_vPositionOS.y + (1.0f / g_vOcclusionTextureSize.y);
	out_vPositionCS.z = in_vPositionOS.z;
	out_vPositionCS.w = 1.0f;
	
	// Pass along the texture coordiante and the position of the frustum corner
	out_vTexCoord = in_vTexCoordAndCornerIndex.xy;
	out_vFrustumCornerVS = g_vFrustumCornersVS[in_vTexCoordAndCornerIndex.z];
}	

// Calculates the shadow occlusion using bilinear PCF
float CalcShadowTermPCF(float fLightDepth, float2 vShadowTexCoord)
{
	float fShadowTerm = 0.0f;

	// transform to texel space
	float2 vShadowMapCoord = g_vShadowMapSize * vShadowTexCoord;
    
	// Determine the lerp amounts           
	float2 vLerps = frac(vShadowMapCoord);

	// Read in the 4 samples, doing a depth check for each
	float fSamples[4];	
	fSamples[0] = (tex2D(ShadowMapSampler, vShadowTexCoord).x + BIAS < fLightDepth) ? 0.0f: 1.0f;  
	fSamples[1] = (tex2D(ShadowMapSampler, vShadowTexCoord + float2(1.0/g_vShadowMapSize.x, 0)).x + BIAS < fLightDepth) ? 0.0f: 1.0f;  
	fSamples[2] = (tex2D(ShadowMapSampler, vShadowTexCoord + float2(0, 1.0/g_vShadowMapSize.y)).x + BIAS < fLightDepth) ? 0.0f: 1.0f;  
	fSamples[3] = (tex2D(ShadowMapSampler, vShadowTexCoord + float2(1.0/g_vShadowMapSize.x, 1.0/g_vShadowMapSize.y)).x + BIAS < fLightDepth) ? 0.0f: 1.0f;  
    
	// lerp between the shadow values to calculate our light amount
	fShadowTerm = lerp(lerp(fSamples[0], fSamples[1], vLerps.x), lerp( fSamples[2], fSamples[3], vLerps.x), vLerps.y);							  
								
	return fShadowTerm;								 
}

// Calculates the shadow term using PCF soft-shadowing
float CalcShadowTermSoftPCF(float fLightDepth, float2 vShadowTexCoord, int iSqrtSamples)
{
	float fShadowTerm = 0.0f;  
		
	float fRadius = (iSqrtSamples - 1.0f) / 2;
	float fWeightAccum = 0.0f;
	
	for (float y = -fRadius; y <= fRadius; y++)
	{
		for (float x = -fRadius; x <= fRadius; x++)
		{
			float2 vOffset = 0;
			vOffset = float2(x, y);				
			vOffset /= g_vShadowMapSize;
			float2 vSamplePoint = vShadowTexCoord + vOffset;			
			float fDepth = tex2D(ShadowMapSampler, vSamplePoint).x;
			float fSample = (fLightDepth <= fDepth + BIAS);
			
			// Edge tap smoothing
			float xWeight = 1;
			float yWeight = 1;
			
			if (x == -fRadius)
				xWeight = 1 - frac(vShadowTexCoord.x * g_vShadowMapSize.x);
			else if (x == fRadius)
				xWeight = frac(vShadowTexCoord.x * g_vShadowMapSize.x);
				
			if (y == -fRadius)
				yWeight = 1 - frac(vShadowTexCoord.y * g_vShadowMapSize.y);
			else if (y == fRadius)
				yWeight = frac(vShadowTexCoord.y * g_vShadowMapSize.y);
				
			fShadowTerm += fSample * xWeight * yWeight;
			fWeightAccum = xWeight * yWeight;
		}											
	}		
	
	fShadowTerm /= (iSqrtSamples * iSqrtSamples);
	fShadowTerm *= 1.55f;	
	
	return fShadowTerm;
}

// Pixel shader for computing the shadow occlusion factor
float4 ShadowTermPS(	in float2 in_vTexCoord			: TEXCOORD0,
						in float3 in_vFrustumCornerVS	: TEXCOORD1)	: COLOR0
{
	int iFilterSize = 2;

	// Reconstruct view-space position from the depth buffer
	//float fPixelDepth = tex2D(DepthTextureSampler, in_vTexCoord).r;
	//float4 vPositionVS = float4(fPixelDepth * in_vFrustumCornerVS, 1.0f);	
	float fPixelDepth = tex2D(DepthTextureSampler, in_vTexCoord).r;
	float2 texCoord = (float2(in_vTexCoord.x - 0.5f, -in_vTexCoord.y + 0.5f)) * 2.0f;
    
	float4 position;
    position.xy = texCoord.xy;
    position.z = fPixelDepth;
    position.w = 1.0f;
    position = mul(position, InvertViewProjection);
    position /= position.w;

	// Determine the depth of the pixel with respect to the light
	//float4x4 matViewToLightViewProj = mul(g_matInvView, g_matLightViewProj);
	//float4 vPositionLightCS = mul(vPositionVS, matViewToLightViewProj);
	float4 vPositionLightCS = mul(position, g_matLightViewProj);
	
	float fLightDepth = vPositionLightCS.z / vPositionLightCS.w;	

	// Transform from light space to shadow map texture space.
    float2 vShadowTexCoord = 0.5 * vPositionLightCS.xy / vPositionLightCS.w + float2(0.5f, 0.5f);
    vShadowTexCoord.y = 1.0f - vShadowTexCoord.y;
        
    // Offset the coordinate by half a texel so we sample it correctly
    vShadowTexCoord += (0.5f / g_vShadowMapSize);

	// Get the shadow occlusion factor and output it
	float fShadowTerm = 0;
	if (iFilterSize == 2)
		fShadowTerm = CalcShadowTermPCF(fLightDepth, vShadowTexCoord);
	else
		fShadowTerm = CalcShadowTermSoftPCF(fLightDepth, vShadowTexCoord, iFilterSize);

	return float4(fShadowTerm, fShadowTerm, fShadowTerm, 1);
}


technique GenerateShadowMap
{
	pass p0
	{
		ZWriteEnable = true;
		ZEnable = true;		
		AlphaBlendEnable = false;
		FillMode = Solid;
		CullMode = CCW;
		
		VertexShader = compile vs_4_0_level_9_3 GenerateShadowMapVS();
        PixelShader = compile ps_4_0_level_9_3 GenerateShadowMapPS();
	}
}

technique CreateShadowTerm2x2PCF
{
    pass p0
    {
		ZWriteEnable = false;
		ZEnable = false;
		AlphaBlendEnable = false;
		CullMode = NONE;

        VertexShader = compile vs_4_0_level_9_3 ShadowTermVS();
        PixelShader = compile ps_4_0_level_9_3 ShadowTermPS();	
    }
}

/*technique CreateShadowTerm3x3PCF
{
    pass p0
    {
		ZWriteEnable = false;
		ZEnable = false;
		AlphaBlendEnable = false;
		CullMode = NONE;

        VertexShader = compile vs_4_0_level_9_3 ShadowTermVS();
        PixelShader = compile ps_4_0_level_9_3 ShadowTermPS(3);	
    }
}

technique CreateShadowTerm5x5PCF
{
    pass p0
    {
		ZWriteEnable = false;
		ZEnable = false;
		AlphaBlendEnable = false;
		CullMode = NONE;

        VertexShader = compile vs_4_0_level_9_3 ShadowTermVS();
        PixelShader = compile ps_4_0_level_9_3 ShadowTermPS(5);	
    }
}

technique CreateShadowTerm7x7PCF
{
    pass p0
    {
		ZWriteEnable = false;
		ZEnable = false;
		AlphaBlendEnable = false;
		CullMode = NONE;

        VertexShader = compile vs_4_0_level_9_3 ShadowTermVS();
        PixelShader = compile ps_4_0_level_9_3 ShadowTermPS(7);	
    }
}*/


