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
	const float density = 0.001f;
	const float gradient = 1.5f;
    
    VSOut vso;
    vso.worldPos = (float3) mul(float4(pos, 1.0f), modelView);
	vso.normal = mul(n, (float3x3) modelView);
	vso.pos = mul(float4(pos, 1.0f), modelViewProj);
	vso.tc = tc;

	float4 posRelToCam = mul(projMatrix, vso.pos);
    float distance = length(posRelToCam.xyz);
	vso.visibility = exp(-pow(mul(distance, density), gradient));
	vso.visibility = clamp(vso.visibility, 0.0f, 1.0f);  
    
    
    return vso;
}