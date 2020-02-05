Texture2D tex;

SamplerState splr;

float4 main(/*float3 worldPos : Position,*/ float2 tc : Texcoord/*, float visibility : Visibility*/, float light : Light) : SV_Target
{
	//const float skyIntesity = 0.5f;
	//const float3 skyColor = float3(0.5f * skyIntesity, 0.91f * skyIntesity, 1.0f * skyIntesity);
	
    float4 tx = tex.Sample(splr, tc);
    clip(tx.a < 0.1f ? -1 : 1);
	//return lerp(float4(skyColor, 1.0f), tx, visibility);
    return float4(mul(tx.xyz, light), 1.0f);
}
