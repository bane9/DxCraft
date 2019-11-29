Texture2D tex;

SamplerState splr;

float4 main(float3 worldPos : Position, float3 n : Normal, float2 tc : Texcoord) : SV_Target
{
    return tex.Sample(splr, tc);

}
