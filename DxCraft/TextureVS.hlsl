cbuffer CBuf
{
    matrix modelViewProj;
    matrix transformMatrix;
    matrix modelView;
	matrix projMatrix;
};

struct VSOut
{
    float3 worldPos : Position;
    float3 normal : Normal;
    float2 tc : Texcoord;
	float visibility : Visibility;
    float4 pos : SV_Position;
};

VSOut main(float3 pos : Position, float3 n : Normal, float2 tc : Texcoord)
{
    VSOut vso;
    vso.worldPos = mul(float4(pos, 1.0f), modelView);
	vso.normal = mul(n, (float3x3) modelView);
	vso.pos = mul(float4(pos, 1.0f), modelViewProj);
	vso.tc = tc;
    vso.visibility = exp(-pow(mul(length(mul(projMatrix, vso.pos).xyz), 0.005f), 1.0f));
    
    return vso;
}
