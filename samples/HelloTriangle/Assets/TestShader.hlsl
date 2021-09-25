cbuffer ModelViewConstants : register(b0)
{
    float4x4 u_ModelViewProj;
}

Texture2D u_Texture : register(t0);
SamplerState u_Sampler : register(s0);

struct PSInput
{
    float4 position : SV_POSITION;
    float4 color    : COLOR;
    float2 texCoord : TEXCOORD;
};

PSInput VSMain(float3 position : POSITION, float2 texCoord : TEXCOORD, float4 color : COLOR)
{
    PSInput result;
    result.position = mul(float4(position.x, position.y, position.z, 1.0f), u_ModelViewProj);
    result.texCoord = texCoord;
    result.color = color;
    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return u_Texture.Sample(u_Sampler, input.texCoord) * input.color;
}
