float4 main(float4 pos : Position, float2 tc : TexCoord) : SV_POSITION
{
    return mul(pos, 0.0005f);
}
