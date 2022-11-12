//Blinn FX Shader
//Ewan Burnett - 2022

cbuffer CBufferPerFrame
{
    float3 lightDirection : DIRECTION;
    float3 cameraPosition : CAMERAPOSITION;   
};

cbuffer CBufferPerAttrib
{
    float4 ambientColour : AMBIENT;
    float4 diffuseColour : DIFFUSE;
    float4 specularColour : SPECULAR;
    float specularPower : SPECULARPOWER;
};

cbuffer CBufferPerObject
{
    float4x4 WorldViewProj : WORLDVIEWPROJECTION;
    float4x4 World : WORLD;
};

Texture2D DiffuseMap : T_DIFFUSE;

Texture2D NormalMap : T_NORMAL;

Texture2D SpecularMap : T_SPECULAR;

BlendState EnableAlphaBlending
{
    BlendEnable[0] = True;
    SrcBlend[0] = SRC_ALPHA;
    DestBlend[0] = INV_SRC_ALPHA;
    BlendOp[0] = ADD;
    SrcBlendAlpha[0] = ZERO;
    DestBlendAlpha[0] = ZERO;
    BlendOpAlpha[0] = ADD;
    RenderTargetWriteMask[0] = 0x0F;
};

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

struct VS_IN
{
    float3 objPos : POSITION;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float3 lightDir : TEXCOORD1;
    float3 viewDir : TEXCOORD2;
};

VS_OUT vs_main(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;
    
    output.pos = mul(float4(input.objPos, 1.0f), WorldViewProj); //Transform the positions into Homogeneous Clip Space
    output.texCoord = input.texCoord;
    output.normal = normalize(mul(float4(input.normal, 0), World).xyz);
    output.tangent = normalize(mul(float4(input.tangent, 0), World).xyz);
    output.binormal = normalize(mul(float4(input.binormal, 0), World).xyz);
    output.lightDir = normalize(lightDirection);
    
    //Compute the view direction of the camera
    float3 worldPos = mul(float4(input.objPos, 1), World).xyz;
    output.viewDir = normalize(cameraPosition - worldPos);
    
    return output;
}

float4 ps_main(VS_OUT input) : SV_Target
{
    float4 output = (float4) 0;
    
    float3 normal = normalize(input.normal);
    float3 lightDir = normalize(input.lightDir);
    float3 viewDir = normalize(input.viewDir);
    
    //Lambert's cosine law
    float n_dot_l = dot(lightDir, normal);
    float3 halfVector = normalize(lightDir + viewDir);
    float n_dot_h = dot(normal, halfVector);
    
    float4 colour = diffuseColour;
   

    float3 coefficients = lit(n_dot_l, n_dot_h, specularPower);

    //Apply lighting    
    float3 ambient = ambientColour.rgb * ambientColour.a * colour.rgb;
    float3 specular = specularColour.rgb * specularColour.a * min(coefficients.z, colour.w);
    float3 diffuse = diffuseColour.rgb * diffuseColour.a * (coefficients.y * colour.rgb);
    
    output.rgb = ambient + diffuse + specular;
    output.a = diffuseColour.a;
    
    return output;
}

//Pixel shader for objects with a texture bound.
float4 ps_diffuse(VS_OUT input) : SV_Target
{
    float4 output = (float4) 0;
    
    float3 normal = normalize(input.normal);
    float3 lightDir = normalize(input.lightDir);
    float3 viewDir = normalize(input.viewDir);
    
    //Lambert's cosine law
    float n_dot_l = dot(lightDir, normal);
    float3 halfVector = normalize(lightDir + viewDir);
    float n_dot_h = dot(normal, halfVector);
    
    float4 colour = DiffuseMap.Sample(ColourSampler, input.texCoord);
   

    float3 coefficients = lit(n_dot_l, n_dot_h, specularPower);

    //Apply lighting    
    float3 ambient = ambientColour.rgb * ambientColour.a * colour.rgb;
    float3 specular = specularColour.rgb * specularColour.a * min(coefficients.z, colour.w);
    float3 diffuse = diffuseColour.rgb * diffuseColour.a * (coefficients.y * colour.rgb);
    
    output.rgb = ambient + diffuse + specular;
    output.a = diffuseColour.a;
    
    return output;
}

//Pixel shader for objects with a texture bound.
float4 ps_diffuse_normal(VS_OUT input) : SV_Target
{
    float4 output = (float4) 0;
    
    //shift the sampled vector to the range [0, 1]
    float3 normal = (2 * NormalMap.Sample(ColourSampler, input.texCoord).xyz) - 1.0;
    
    float3x3 tbn = float3x3(input.tangent, input.binormal, input.normal);
    normal = mul(normal, tbn);
    
    float3 lightDir = normalize(input.lightDir);
    float3 viewDir = normalize(input.viewDir);
    
    //Lambert's cosine law
    float n_dot_l = dot(lightDir, normal);
    float3 halfVector = normalize(lightDir + viewDir);
    float n_dot_h = dot(normal, halfVector);
    
    float4 colour = DiffuseMap.Sample(ColourSampler, input.texCoord);
   

    float3 coefficients = lit(n_dot_l, n_dot_h, specularPower);

    //Apply lighting    
    float3 ambient = ambientColour.rgb * ambientColour.a * colour.rgb;
    float3 specular = specularColour.rgb * specularColour.a * min(coefficients.z, colour.w);
    float3 diffuse = diffuseColour.rgb * diffuseColour.a * (coefficients.y * colour.rgb);
    
    output.rgb = ambient + diffuse + specular;
    output.a = diffuseColour.a;
    
    return output;
}

//Pixel shader for objects with a texture bound.
float4 ps_diffuse_normal_spec(VS_OUT input) : SV_Target
{
    float4 output = (float4) 0;
    
    //shift the sampled vector to the range [0, 1]
    float3 normal = (2 * NormalMap.Sample(ColourSampler, input.texCoord).xyz) - 1.0;
    
    float3x3 tbn = float3x3(input.tangent, input.binormal, input.normal);
    normal = mul(normal, tbn);
    
    float3 lightDir = normalize(input.lightDir);
    float3 viewDir = normalize(input.viewDir);
    
    //Lambert's cosine law
    float n_dot_l = dot(lightDir, normal);
    float3 halfVector = normalize(lightDir + viewDir);
    float n_dot_h = dot(normal, halfVector);
    
    float4 colour = DiffuseMap.Sample(ColourSampler, input.texCoord);
    float4 spec = SpecularMap.Sample(ColourSampler, input.texCoord);

    float3 coefficients = lit(n_dot_l, n_dot_h, specularPower);

    //Apply lighting    
    float3 ambient = ambientColour.rgb * ambientColour.a * colour.rgb;
    float3 specular = spec.rgb * spec.a * min(coefficients.z, colour.w);
    //float3 specular = (spec.rgb * spec.a * min(pow(saturate(dot(normal, halfVector)), specularPower), colour.w));
    float3 diffuse = diffuseColour.rgb * diffuseColour.a * (coefficients.y * colour.rgb);
    
    output.rgb = ambient + diffuse + specular;
    output.a = diffuseColour.a;
    
    return output;
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


technique11 Textured
{
    pass p0
    {
        SetVertexShader(CompileShader(vs_5_0, vs_main()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, ps_diffuse()));
		
        SetRasterizerState(DisableCulling);
    }
}

technique11 Alpha
{
    pass p0
    {
        SetVertexShader(CompileShader(vs_5_0, vs_main()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, ps_diffuse_normal()));
		
        SetRasterizerState(DisableCulling);
    }

}

technique11 Specular
{
    pass p0
    {
        SetVertexShader(CompileShader(vs_5_0, vs_main()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, ps_diffuse_normal_spec()));
		
        SetRasterizerState(DisableCulling);
        SetBlendState(EnableAlphaBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    }
}

