float4x4 World;
float4x4 View;
float4x4 Projection;
float4x4 InvertViewProjection; 
float2 HalfPixel;



texture ShadowMap;
sampler shadowSampler = sampler_state
{
    Texture = <ShadowMap>;
    MagFilter = Point;
    MinFilter = Point;
    MipFilter = Point;
    AddressU = Wrap;
    AddressV = Wrap;
};

texture NormalMap;
sampler normalSampler = sampler_state
{
    Texture = <NormalMap>;
    MagFilter = Point;
    MinFilter = Point;
    MipFilter = Point;
    AddressU = Wrap;
    AddressV = Wrap;
};

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

    output.Position = input.Position;
    output.TexCoord = input.TexCoord;

    return output;
}

float g_directional;

float4 PixelShaderFunction(VertexShaderOutput input) : COLOR0
{
    float2 texCoord = input.TexCoord - HalfPixel;

    float depthVal = tex2D(depthSampler, texCoord).r;
    float4 position;
    position.xy = input.TexCoord.xy;
    position.z = depthVal;
    position.w = 1.0f;
    position = mul(position, InvertViewProjection);
    position /= position.w;

    float4 normalStuff = tex2D(normalSampler, texCoord);
	float3 normal = 2.0f * (normalStuff.xyz - 0.5f);

	float3 ambient = float3(0.5f, 0.5f, 0.5f);
	float dirFactor = saturate(dot(float3(-0.5f, -0.5f, 1.0f), normal));
	float3 directional = float3(g_directional, g_directional, g_directional) * dirFactor;

	float shadowTerm = tex2D(shadowSampler, texCoord).r;

	float4 lightColor = float4(ambient + (directional * shadowTerm), 1.0f);

	if (lightColor.r > 1.0f || lightColor.g > 1.0f || lightColor.b > 1.0f)
	{
		//return float4(1.0f, 0.0f, 0.0f, 1.0f);
	}

	return lightColor / 4.0f;
}

technique BaseDraw
{
    pass Draw
    {
        VertexShader = compile vs_4_0_level_9_3 VertexShaderFunction();
        PixelShader = compile ps_4_0_level_9_3 PixelShaderFunction();
    }
}
