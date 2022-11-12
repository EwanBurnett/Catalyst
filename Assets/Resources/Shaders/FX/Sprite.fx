
SamplerState ColourSampler
{
    Filter = ANISOTROPIC;
    AddressV = WRAP;
    AddressU = WRAP;
};

RasterizerState DisableCulling
{
    FrontCounterClockwise = FALSE;
    CullMode = NONE;
};

BlendState EnableAlphaBlending
{
    BlendEnable[0] = True;
    SrcBlend[0] = SRC_ALPHA;
    DestBlend[0] = INV_SRC_ALPHA;
};


cbuffer CBufferPerView
{
    float4x4 WorldViewProj : WORLDVIEWPROJECTION;
};

Texture2D SpriteAtlas : T_ATLAS;

struct VS_IN
{
    float3 position : POSITION;
    float2 rect : TEXCOORD;
   
};

struct VS_OUT
{
    float4 screenPos : SV_Position;
    float2 texRect : TEXCOORD0;
};


VS_OUT vs_main(in VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;
    
    output.screenPos = mul(float4(input.position, 1.0f), WorldViewProj);
    output.texRect = input.rect;  //float2(input.rect.x * 1.0f, input.rect.y * 1.0f);
    
    return output;
}

float4 ps_main(in VS_OUT input) : SV_Target
{
    float4 Albedo = SpriteAtlas.Sample(ColourSampler, input.texRect);
    return Albedo;
}

float4 ps_text(in VS_OUT input) : SV_Target
{
    float3 Albedo = SpriteAtlas.Sample(ColourSampler, input.texRect).rrr;
    float Alpha = SpriteAtlas.Sample(ColourSampler, input.texRect).r;
    
    return float4(Albedo * Alpha, Alpha);
}

technique11 Sprite
{
    pass p0
    {
        SetVertexShader(CompileShader(vs_5_0, vs_main()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, ps_main()));
		
        SetRasterizerState(DisableCulling);
        SetBlendState(EnableAlphaBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    }
}

technique11 Text
{
    pass p0
    {
        SetVertexShader(CompileShader(vs_5_0, vs_main()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, ps_text()));
		
        SetRasterizerState(DisableCulling);
        SetBlendState(EnableAlphaBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    }
}