// vertexShader����l���󂯎��p�̍\����
struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
};

Texture2D objTexture : TEXTURE : register(t0);  // shaderResource����󂯎���Ă���l
SamplerState objSamplerState : SAMPLER : register(s0);  // sampler����󂯎���Ă���l

// �����̈����� vertexShader ����󂯎���Ă���?
float4 main(PS_INPUT input) : SV_TARGET
{
    float3 pixelColor = objTexture.Sample(objSamplerState, input.inTexCoord);
    return float4(pixelColor, 1.0f);
}