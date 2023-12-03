// vertexShaderから値を受け取る用の構造体
struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
};

Texture2D objTexture : TEXTURE : register(t0);  // shaderResourceから受け取っている値
SamplerState objSamplerState : SAMPLER : register(s0);  // samplerから受け取っている値

// ここの引数は vertexShader から受け取っている?
float4 main(PS_INPUT input) : SV_TARGET
{
    float3 pixelColor = objTexture.Sample(objSamplerState, input.inTexCoord);
    return float4(pixelColor, 1.0f);
}