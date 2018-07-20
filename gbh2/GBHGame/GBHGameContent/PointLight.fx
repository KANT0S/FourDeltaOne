#include "Common.fxh"

float4x4 World;
float4x4 View;
float4x4 Projection;
float4x4 InvertViewProjection; 
float2 HalfPixel;

texture DepthMap;
sampler depthSampler = sampler_state
{
    Texture = <DepthMap>;
    MagFilter = Point;
    MinFilter = Point;
    MipFilter = Point;
    AddressU = Wrap;
    AddressV = Wrap;
};

struct VertexShaderInput
{
    float4 Position : SV_Position;
	float4 Color : COLOR0;
	float4 Normal : NORMAL0;
	float2 ExtraData : TEXCOORD0;
};

struct VertexShaderOutput
{
    float4 Position : SV_Position;
	float4 Color : TEXCOORD0;
	float3 Normal : TEXCOORD1;
	float2 Intensity : TEXCOORD2;
	float4 ScreenPosition : TEXCOORD3;
};

VertexShaderOutput VertexShaderFunction(VertexShaderInput input)
{
    VertexShaderOutput output;

    //float4 worldPosition = mul(input.Position, World);
    float4 viewPosition = mul(input.Position, View);
    output.Position = mul(viewPosition, Projection);

    output.Color = input.Color;

	output.Normal = input.Normal;
	output.Intensity = input.ExtraData;

	output.Color.a = 1;

	output.ScreenPosition = output.Position;

    return output;
}

float4 PixelShaderFunction(VertexShaderOutput input) : COLOR0
{
    input.ScreenPosition.xy /= input.ScreenPosition.w;
    float2 texCoord = 0.5f * (float2(input.ScreenPosition.x, -input.ScreenPosition.y) + 1);
    texCoord -= HalfPixel;

    float depthVal = tex2D(depthSampler, texCoord).r;
    float4 position;
    position.xy = input.ScreenPosition.xy;
    position.z = depthVal;
    position.w = 1.0f;
    position = mul(position, InvertViewProjection);
    position /= position.w;

    float3 lightVector = input.Normal - position;
    float attenuation = saturate(1.0f - length(lightVector) / input.Intensity.x); 

	return float4((input.Intensity.y * attenuation * input.Color.rgb) / 4.0f, 1);
}

technique BaseDraw
{
    pass Draw
    {
        VertexShader = compile vs_4_0_level_9_3 VertexShaderFunction();
        PixelShader = compile ps_4_0_level_9_3 PixelShaderFunction();
    }
}
