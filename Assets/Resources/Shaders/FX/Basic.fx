//Basic HLSL Shader
//Ewan Burnett - 2022

RasterizerState DisableCulling
{
    CullMode = NONE;
};

cbuffer CBufferPerFrame
{
    float4x4 WorldViewProj : WORLDVIEWPROJECTION;
    float4 Colour : COLOUR;
};

struct VS_IN
{
    float3 position : POSITION;
};

struct VS_OUT
{
    float4 position : SV_Position; 
};

VS_OUT vs_main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;
    output.position = mul(float4(input.position, 1.0f), WorldViewProj);
    return output;
}

float4 ps_main(VS_OUT input) : SV_Target
{
    return (Colour);
}

technique11 main
{
    pass p0
    {
        SetVertexShader(CompileShader(vs_5_0, vs_main()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, ps_main()));

        SetRasterizerState(DisableCulling);

    }
}