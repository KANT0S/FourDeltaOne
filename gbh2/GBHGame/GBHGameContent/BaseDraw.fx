#include "Common.fxh"

float4x4 World;
float4x4 View;
float4x4 Projection;

texture ColorMap;
sampler colorSampler = sampler_state
{
    Texture = <ColorMap>;
    MagFilter = Linear;
    MinFilter = Linear;
    MipFilter = Point;
    AddressU = Wrap;
    AddressV = Wrap;
};

struct VertexShaderInput
{
    float4 Position : SV_Position;
	float2 TexCoord : TEXCOORD0;
	float4 Normal : NORMAL;
};

struct VertexShaderOutput
{
    float4 Position : SV_Position;
	float2 TexCoord : TEXCOORD0;
	float2 Depth : TEXCOORD1;
	float3 Normal : TEXCOORD2;
};

VertexShaderOutput VertexShaderFunction(VertexShaderInput input)
{
    VertexShaderOutput output;

    float4 worldPosition = mul(input.Position, World);
    float4 viewPosition = mul(worldPosition, View);
    output.Position = mul(viewPosition, Projection);

    output.TexCoord = input.TexCoord;

	output.Depth.x = output.Position.z;
	output.Depth.y = output.Position.w;

	output.Normal.xyz = (0.5f * normalize(input.Normal)) + 0.5f;

    return output;
}

struct PixelShaderOutput
{
	float4 Color : COLOR0;
	float4 Depth : COLOR1;
	float4 Normal : COLOR2;
};

PixelShaderOutput PixelShaderFunction(VertexShaderOutput input)
{
	PixelShaderOutput output;
	output.Color = tex2D(colorSampler, input.TexCoord);
	output.Depth = input.Depth.x / input.Depth.y;
	output.Normal = float4(input.Normal, 1.0);

	clip(output.Color.a - 0.78125);

    return output;
}

technique BaseDraw
{
    pass Draw
    {
        VertexShader = compile vs_4_0_level_9_3 VertexShaderFunction();
        PixelShader = compile ps_4_0_level_9_3 PixelShaderFunction();
    }
}
