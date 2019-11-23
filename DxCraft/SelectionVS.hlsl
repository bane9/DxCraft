cbuffer CBuf
{
    matrix modelViewProj;
    matrix modelView;
};

struct VSOut
{
    float3 worldPos : Position;
    float4 pos : SV_Position;
};

VSOut main(float3 pos : Position)
{
    VSOut vso;
    vso.worldPos = (float3) mul(float4(pos, 1.0f), modelView);
    vso.pos = mul(float4(pos, 1.0f), modelViewProj);
    return vso;
}
