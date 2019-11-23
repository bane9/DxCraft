Texture2D tex;

SamplerState splr;

float4 main(float3 worldPos : Position, float3 n : Normal, float2 tc : Texcoord) : SV_Target
{
    float4 dtex = tex.Sample(splr, tc);
    clip(dtex.a < 0.1f ? -1 : 1);
    return dtex;

}