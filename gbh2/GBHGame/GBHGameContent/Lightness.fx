#include "Common.fxh"

texture frameTex;
sampler frameSampler = sampler_state
{
    Texture = <frameTex>;
    MagFilter = Point;
    MinFilter = Point;
    MipFilter = Point;
    AddressU = Wrap;
    AddressV = Wrap;
};

void VertexShaderFunction(in float3 in_pos : SV_Position,
						  in float2 in_texcoord : TEXCOORD0,
						  out float4 out_pos : SV_Position,
						  out float2 out_texcoord : TEXCOORD0)
{
    out_pos = float4(in_pos, 1.0f);
	out_texcoord = in_texcoord;
}

static const float3 COLOR_TO_LIGHTNESS = float3(0.299f, 0.587f, 0.114f);

float4 LightnessPS(in float2 in_texcoord : TEXCOORD0) : COLOR
{
	float3 color = tex2D(frameSampler, in_texcoord);
	float lightness = dot(color, COLOR_TO_LIGHTNESS);

	return float4(log(0.00001 + lightness), 1.0f, 1.0f, 1.0f);
}

static const float downsampleOffsets[] = { -1.5f, -0.5f, 0.5f, 1.5f };
float2 sourceSize;

float4 DownsamplePS(in float2 in_texcoord : TEXCOORD0) : COLOR
{
	float avg = 0.0f;

	for (int x = 0; x < 4; x++)
	{
		for (int y = 0; y < 4; y++)
		{
			float2 offset = float2(downsampleOffsets[x], downsampleOffsets[y]);
			offset /= sourceSize;
			avg += tex2D(frameSampler, in_texcoord + offset).r;
		}
	}

	avg /= 16.0f;

	return float4(avg, 1.0f, 1.0f, 1.0f);
}

float4 DownsamplePSFinal(in float2 in_texcoord : TEXCOORD0) : COLOR
{
	float avg = 0.0f;

	for (int x = 0; x < 4; x++)
	{
		for (int y = 0; y < 4; y++)
		{
			float2 offset = float2(downsampleOffsets[x], downsampleOffsets[y]);
			offset /= sourceSize;
			avg += tex2D(frameSampler, in_texcoord + offset).r;
		}
	}

	avg /= 16.0f;

	avg = float4(exp(avg), 1.0f, 1.0f, 1.0f);

	return float4(avg, 1.0f, 1.0f, 1.0f);
}

technique CalculateLightness
{
	pass Draw
	{
		VertexShader = compile vs_4_0_level_9_3 VertexShaderFunction();
        PixelShader = compile ps_4_0_level_9_3 LightnessPS();
	}
}

technique DownsampleLightness
{
	pass Draw
	{
		VertexShader = compile vs_4_0_level_9_3 VertexShaderFunction();
        PixelShader = compile ps_4_0_level_9_3 DownsamplePS();
	}
}

technique DownsampleLightnessFinalize
{
	pass Draw
	{
		VertexShader = compile vs_4_0_level_9_3 VertexShaderFunction();
        PixelShader = compile ps_4_0_level_9_3 DownsamplePSFinal();
	}
}