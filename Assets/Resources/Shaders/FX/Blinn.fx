//Blinn FX Shader
//Ewan Burnett - 2022

#include "LightHelper.fxh"

cbuffer CBufferPerFrame
{
    DirectionalLight directionalLight;
    PointLight pointLights[NUM_POINT_LIGHTS];
    SpotLight spotLights[NUM_SPOT_LIGHTS];

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
    float4x4 ProjectiveTextureMatrix;
};

Texture2D DiffuseMap : T_DIFFUSE;

Texture2D NormalMap : T_NORMAL;

Texture2D SpecularMap : T_SPECULAR;

Texture2D ShadowMap;

static const float4 White = {1, 1, 1, 1};
static const float3 Black = {0, 0, 0};
static const float DepthBias = 0.005;

SamplerState ShadowMapSampler{
    Filter = ANISOTROPIC;
    AddressU = BORDER;
    AddressV = BORDER;
    BorderColor = White;
};

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
    float3 viewDir : TEXCOORD1;
    float Attenuation : TEXCOORD2;
    float4 ShadowTexCoords : TEXCOORD3;
    float3 worldPos : POSITION;
};

VS_OUT vs_main(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;
    
    output.pos = mul(float4(input.objPos, 1.0f), WorldViewProj); //Transform the positions into Homogeneous Clip Space
    output.texCoord = input.texCoord;
    output.normal = normalize(mul(float4(input.normal, 0), World).xyz);
    output.tangent = normalize(mul(float4(input.tangent, 0), World).xyz);
    output.binormal = normalize(mul(float4(input.binormal, 0), World).xyz);
    output.worldPos = mul(input.objPos, World).xyz;
    //Compute the view direction of the camera
    float3 worldPos = mul(float4(input.objPos, 1), World).xyz;
    output.viewDir = normalize(cameraPosition - worldPos);
    

    [unroll]
    for(int i = 0; i < NUM_POINT_LIGHTS; i++)
    {
        float3 lightDir = pointLights[i].position - output.worldPos;
        output.Attenuation += saturate(1.0f - (length(lightDir) / pointLights[i].radius));
    }
    output.Attenuation = normalize(output.Attenuation); //?

    output.ShadowTexCoords = mul(input.objPos, ProjectiveTextureMatrix);

    return output;
}

float4 ps_main(VS_OUT input, uniform int numPointLights, uniform int numSpotLights) : SV_Target
{
    float4 output = (float4) 0;
    
    float3 normal = normalize(input.normal);
    float3 viewDir = normalize(input.viewDir);
    
    float4 colour = diffuseColour;
   
    float3 ambient = GetVectorColourContribution(ambientColour, colour.rgb);
    
    float3 lightDir = normalize(directionalLight.direction);
    
    //Lambert's cosine law
    float n_dot_l = dot(lightDir, normal);
    float3 halfVector = normalize(lightDir + viewDir);
    float n_dot_h = dot(normal, halfVector);   

    float3 coefficients = lit(n_dot_l, n_dot_h, specularPower);

    LightContributionData lightData;
    lightData.colour = colour;
    lightData.normal = normal;
    lightData.viewDirection = viewDir;
    lightData.specularColour = specularColour;
    lightData.specularPower = specularPower;
    
    float3 totalContribution = (float3) 0;
    
    [unroll]
    for (int i = 0; i < numPointLights; i++)
    {
        lightData.lightDirection = GetLightData(pointLights[i].position, input.worldPos, pointLights[i].radius);
        lightData.lightColour = pointLights[i].colour;
        totalContribution += GetLightContribution(lightData);
    }
    
   [unroll]
    for (int i = 0; i < numSpotLights; i++)
    {
        lightData.lightDirection = GetLightData(spotLights[i].position, input.worldPos,spotLights[i].radius);
        lightData.lightColour = spotLights[i].colour;
        totalContribution += GetLightContribution(lightData);
    }

    float3 diffuse = (totalContribution * (coefficients.y * colour.rgb));
    if(input.ShadowTexCoords.w >= 0.0f){
        input.ShadowTexCoords.xyz /= input.ShadowTexCoords.w;
        float pixelDepth = input.ShadowTexCoords.z;
        float sampledDepth = ShadowMap.Sample(ShadowMapSampler, input.ShadowTexCoords.xy).x + DepthBias;

        //boolean shadows
        float3 shadow = (pixelDepth > sampledDepth ? Black : White.rgb);
        
        diffuse *= shadow;
        //specular *= shadow;
    }

    output.rgb = ambient + diffuse;
    output.a = colour.a;
    
        return output;
}

//Pixel shader for objects with a texture bound.
float4 ps_diffuse(VS_OUT input, uniform int numPointLights, uniform int numSpotLights) : SV_Target
{
     float4 output = (float4) 0;
    
    float3 normal = normalize(input.normal);
    float3 viewDir = normalize(input.viewDir);
    
    float4 colour =  DiffuseMap.Sample(ColourSampler, input.texCoord);
   
    float3 ambient = GetVectorColourContribution(ambientColour, colour.rgb);
    
     float3 lightDir = normalize(directionalLight.direction);
    
    //Lambert's cosine law
    float n_dot_l = dot(lightDir, normal);
    float3 halfVector = normalize(lightDir + viewDir);
    float n_dot_h = dot(normal, halfVector);   

    float3 coefficients = lit(n_dot_l, n_dot_h, specularPower);

    LightContributionData lightData;
    lightData.colour = colour;
    lightData.normal = normal;
    lightData.viewDirection = viewDir;
    lightData.specularColour = specularColour;
    lightData.specularPower = specularPower;
    
    float3 totalContribution = (float3) 0;
    
    [unroll]
    for (int i = 0; i < numPointLights; i++)
    {
        lightData.lightDirection = GetLightData(pointLights[i].position, input.worldPos, pointLights[i].radius);
        lightData.lightColour = pointLights[i].colour;
        totalContribution += GetLightContribution(lightData);
    }
    
   [unroll]
    for (int i = 0; i < numSpotLights; i++)
    {
        lightData.lightDirection = GetLightData(spotLights[i].position, input.worldPos,spotLights[i].radius);
        lightData.lightColour = spotLights[i].colour;
        totalContribution += GetLightContribution(lightData);
    }


    output.rgb = ambient + (totalContribution * (coefficients.y * colour.rgb));
    output.a = colour.a;
    
        return output;

}

//Pixel shader for objects with a texture bound.
float4 ps_diffuse_normal(VS_OUT input, uniform int numPointLights, uniform int numSpotLights) : SV_Target
{
     float4 output = (float4) 0;
    
     //shift the sampled vector to the range [0, 1]
    float3 normal = (2 * NormalMap.Sample(ColourSampler, input.texCoord).xyz) - 1.0;
    
    float3x3 tbn = float3x3(input.tangent, input.binormal, input.normal);
    normal = mul(normal, tbn);
    float3 viewDir = normalize(input.viewDir);
    
    float4 colour =  DiffuseMap.Sample(ColourSampler, input.texCoord);
   
    float3 ambient = GetVectorColourContribution(ambientColour, colour.rgb);
    
     float3 lightDir = normalize(directionalLight.direction);
    
    //Lambert's cosine law
    float n_dot_l = dot(lightDir, normal);
    float3 halfVector = normalize(lightDir + viewDir);
    float n_dot_h = dot(normal, halfVector);   

    float3 coefficients = lit(n_dot_l, n_dot_h, specularPower);


    LightContributionData lightData;
    lightData.colour = colour;
    lightData.normal = normal;
    lightData.viewDirection = viewDir;
    lightData.specularColour = specularColour;
    lightData.specularPower = specularPower;
    
    float3 totalContribution = (float3) 0;
    
   [unroll]
    for (int i = 0; i < numPointLights; i++)
    {
        lightData.lightDirection = GetLightData(pointLights[i].position, input.worldPos, pointLights[i].radius);
        lightData.lightColour = pointLights[i].colour;
        totalContribution += GetLightContribution(lightData);
    }
    
   [unroll]
    for (int i = 0; i < numSpotLights; i++)
    {
        lightData.lightDirection = GetLightData(spotLights[i].position, input.worldPos,spotLights[i].radius);
        lightData.lightColour = spotLights[i].colour;
        totalContribution += GetLightContribution(lightData);
    }

    output.rgb = ambient + (totalContribution * (coefficients.y * colour.rgb));
    output.a = colour.a;
    
        return output;

}

//Pixel shader for objects with a texture bound.
float4 ps_diffuse_normal_spec(VS_OUT input, uniform int numPointLights, uniform int numSpotLights) : SV_Target
{
    float4 output = (float4) 0;
     float4 spec = SpecularMap.Sample(ColourSampler, input.texCoord);
    
       


 float3 specular = spec.rgb * spec.a ;

     //shift the sampled vector to the range [0, 1]
    float3 normal = (2 * NormalMap.Sample(ColourSampler, input.texCoord).xyz) - 1.0;
    
    float3x3 tbn = float3x3(input.tangent, input.binormal, input.normal);
    normal = mul(normal, tbn);
    float3 viewDir = normalize(input.viewDir);
    
    float4 colour =  DiffuseMap.Sample(ColourSampler, input.texCoord);
   
    float3 ambient = GetVectorColourContribution(ambientColour, colour.rgb);
    

    LightContributionData lightData;
    lightData.colour = colour;
    lightData.normal = normal;
    lightData.viewDirection = viewDir;
    lightData.specularColour = specularColour;
    lightData.specularPower = specularPower;
    
    float3 totalContribution = (float3) 0;
    
    //Directional Light
    lightData.lightDirection = (float4)(directionalLight.direction, 1);
    lightData.lightColour = directionalLight.colour;
    totalContribution += GetLightContribution(lightData);

    [unroll]
    for (int i = 0; i < numPointLights; i++)
    {
        lightData.lightDirection = GetLightData(pointLights[i].position, input.worldPos, pointLights[i].radius);
        lightData.lightColour = pointLights[i].colour;
        totalContribution += GetLightContribution(lightData);
    }
    
   [unroll]
    for (int i = 0; i < numSpotLights; i++)
    {
        lightData.lightDirection = GetLightData(spotLights[i].position, input.worldPos,spotLights[i].radius);
        lightData.lightColour = spotLights[i].colour;
        totalContribution += GetLightContribution(lightData);
    }



    output.rgb = ambient + totalContribution + specular;
    output.a = colour.a;
    
        return output;

}


technique11 main
{
    pass p0
    {
        SetVertexShader(CompileShader(vs_5_0, vs_main()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, ps_main(8, 8)));
		
        SetRasterizerState(DisableCulling);
    }

}


technique11 Textured
{
    pass p0
    {
        SetVertexShader(CompileShader(vs_5_0, vs_main()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, ps_diffuse(8, 8)));
		
        SetRasterizerState(DisableCulling);
    }
}

technique11 Alpha
{
    pass p0
    {
        SetVertexShader(CompileShader(vs_5_0, vs_main()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, ps_diffuse_normal(8, 8)));
		
        SetRasterizerState(DisableCulling);
    }

}

technique11 Specular
{
    pass p0
    {
        SetVertexShader(CompileShader(vs_5_0, vs_main()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, ps_diffuse_normal_spec(8, 8)));
		
        SetRasterizerState(DisableCulling);
        SetBlendState(EnableAlphaBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    }
}

