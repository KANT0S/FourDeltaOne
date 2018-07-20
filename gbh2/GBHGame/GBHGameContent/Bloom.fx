#include "Common.fxh"

texture frameTex;
sampler frameSampler = sampler_state
{
    Texture = <frameTex>;
    MagFilter = Linear;
    MinFilter = Linear;
    MipFilter = Point;
    AddressU = Wrap;
    AddressV = Wrap;
};

texture avgLightTex;
sampler avgLightSampler = sampler_state
{
    Texture = <avgLightTex>;
    MagFilter = Point;
    MinFilter = Point;
    MipFilter = Point;
    AddressU = Wrap;
    AddressV = Wrap;
};

texture bloomTex;
sampler bloomSampler = sampler_state
{
    Texture = <bloomTex>;
    MagFilter = Linear;
    MinFilter = Linear;
    MipFilter = Point;
    AddressU = Wrap;
    AddressV = Wrap;
};

void VertexShaderFunction(in float3 in_pos : SV_Position,
						  in float2 in_texcoord : TEXCOORD0,
						  out float4 out_pos : SV_Position,
						  out float2 out_texcoord : TEXCOORD0)
{
	out_texcoord = in_texcoord;
    out_pos = float4(in_pos, 1.0f);
}

// only consider brightest colors for blur
float3 filter_for_blur(float3 color)
{
        return max(float3(0,0,0), color - float3(1,1,1));
        //return color;
}

static const float3 COLOR_TO_LIGHTNESS = float3(0.299f, 0.587f, 0.114f); 

float3 gamma_correct (float3 x) { return pow(abs(x),2.2f); }

float3 tone_map (float3 x)
{
	x = pow(abs(x),1.0/2.2f);
	float averageLightness = tex2D(avgLightSampler, float2(0.5f, 0.5f));
	float thisLightness = dot(x, COLOR_TO_LIGHTNESS);

	float l1 = (thisLightness * 0.6f) / averageLightness;     
	float l2 = (l1 * (1 + (l1 / (16.0f * 16.0f)))) / (1 + l1); 
	return l2 * x;
}

float4 texture_size;

float4 BloomHorizontal(in float2 texcoord : TEXCOORD0) : COLOR
{
	return 4.0f * tex2D(frameSampler, texcoord);

	/*float3 col = float3(0,0,0);

    float off = texture_size.z;

    col += 0.061 * filter_for_blur(gamma_correct(4.0f * tex2D(frameSampler, texcoord + off*float2(-2, 0)).rgb));
    col += 0.242 * filter_for_blur(gamma_correct(4.0f * tex2D(frameSampler, texcoord + off*float2(-1, 0)).rgb));
    col += 0.383 * filter_for_blur(gamma_correct(4.0f * tex2D(frameSampler, texcoord + off*float2( 0, 0)).rgb));
    col += 0.242 * filter_for_blur(gamma_correct(4.0f * tex2D(frameSampler, texcoord + off*float2( 1, 0)).rgb));
    col += 0.061 * filter_for_blur(gamma_correct(4.0f * tex2D(frameSampler, texcoord + off*float2( 2, 0)).rgb));

    return float4(tone_map(col), 1.0f);*/
}

float4 BloomVertical(in float2 texcoord : TEXCOORD0) : COLOR
{
	return tex2D(frameSampler, texcoord);

	/*float3 col = float3(0,0,0);

    float off = texture_size.w;

    col += 0.061 * gamma_correct(4.0f * tex2D(bloomSampler, texcoord + off*float2(0, -2)).rgb);
    col += 0.242 * gamma_correct(4.0f * tex2D(bloomSampler, texcoord + off*float2(0, -1)).rgb);
    col += 0.383 * gamma_correct(4.0f * tex2D(bloomSampler, texcoord + off*float2(0,  0)).rgb);
    col += 0.242 * gamma_correct(4.0f * tex2D(bloomSampler, texcoord + off*float2(0,  1)).rgb);
    col += 0.061 * gamma_correct(4.0f * tex2D(bloomSampler, texcoord + off*float2(0,  2)).rgb);

    return float4(tone_map(col + gamma_correct(tex2D(frameSampler, texcoord).rgb * 4.0f)), 1.0);*/
}

technique BloomHorz
{
    pass Draw
    {
        VertexShader = compile vs_4_0_level_9_3 VertexShaderFunction();
        PixelShader = compile ps_4_0_level_9_3 BloomHorizontal();
    }
}

technique BloomVert
{
    pass Draw
    {
        VertexShader = compile vs_4_0_level_9_3 VertexShaderFunction();
        PixelShader = compile ps_4_0_level_9_3 BloomVertical();
    }
}
