float2 HalfPixel;

texture ColorMap;
sampler colorSampler = sampler_state
{
    Texture = <ColorMap>;
    MagFilter = Point;
    MinFilter = Point;
    MipFilter = Point;
    AddressU = Clamp;
    AddressV = Clamp;
};

texture LightMap;
sampler lightSampler = sampler_state
{
    Texture = <LightMap>;
    MagFilter = Point;
    MinFilter = Point;
    MipFilter = Point;
    AddressU = Clamp;
    AddressV = Clamp;
};

texture ShadowOcclusionMap;
sampler shadowSampler = sampler_state
{
    Texture = <ShadowOcclusionMap>;
    MagFilter = Point;
    MinFilter = Point;
    MipFilter = Point;
    AddressU = Clamp;
    AddressV = Clamp;
};

struct VertexShaderInput
{
    float3 Position : SV_Position;
	float2 TexCoord : TEXCOORD0;
};

struct VertexShaderOutput
{
    float4 Position : SV_Position;
	float2 TexCoord : TEXCOORD0;
};

VertexShaderOutput VertexShaderFunction(VertexShaderInput input)
{
    VertexShaderOutput output;

    output.Position = float4(input.Position,1);
    output.TexCoord = input.TexCoord - HalfPixel;

    return output;
}

float4 PixelShaderFunction(VertexShaderOutput input) : COLOR0
{
    float3 color = tex2D(colorSampler, input.TexCoord).rgb;
	float3 light = (tex2D(lightSampler, input.TexCoord).rgb);
	float4 returnColor = float4(color * light, 1.0f);
	//float shadowTerm = tex2D(shadowSampler, input.TexCoord).r;
	//float3 ambient = float3(0.5, 0.5, 0.5);
	//ambient *= shadowTerm;
	//light *= shadowTerm;

	/*if (returnColor.r > 1.0 || returnColor.g > 1.0 || returnColor.b > 1.0)
	{
		returnColor = float4(color * float3(1.0f, 0.0f, 0.0f), 1.0f);
	}*/

	//light = float3(0.5, 0.5, 0.5) * shadowTerm;

	return returnColor;
	//return float4(color * shadowTerm, 1);
	//return float4(shadowTerm, shadowTerm, shadowTerm, 1);
}

technique BaseDraw
{
    pass Draw
    {
        VertexShader = compile vs_4_0_level_9_3 VertexShaderFunction();
        PixelShader = compile ps_4_0_level_9_3 PixelShaderFunction();
    }
}
