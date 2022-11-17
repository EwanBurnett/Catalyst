/**
* \file Blinn.fx
* Blinn Shader, written in HLSL using FX.
*
* Currently supports:
*   - 
*   - Fog
*
* Ewan Burnett - 2022
*/

#include "Common.fxh"
#include "LightHelper.fxh"

// ---------------------------------------------------------------------
// ATTRIBUTES-----------------------------------------------------------

// CONSTANTS

/**
* Diffuse Texture Map
* \param T_DIFFUSE Semantic bound to this material's Diffuse Map texture
*/
Texture2D DiffuseMap : T_DIFFUSE;

/**
* Height Texture Map
* \param T_HEIGHT Semantic bound to this material's Height Map texture
*/
Texture2D HeightMap : T_HEIGHT;


/**
* Normal Texture Map
* \param T_SPECULAR Semantic bound to this material's Specular Map texture
*/
Texture2D SpecularMap : T_SPECULAR;

/**
* Specular Texture Map
* \param T_NORMAL Semantic bound to this material's Normal map texture
*/
Texture2D NormalMap : T_NORMAL;


// CONSTANT BUFFERS

/**
* Constant buffer for attributes updated once per frame.
* \param CAMERAPOSITION float3 Semantic for the world position of the camera. 
*/
cbuffer CBufferPerFrame{
    DirectionalLight l_DirectionalLight : DIRECTIONALLIGHT;
    PointLight l_PointLights[NUM_POINT_LIGHTS] : POINTLIGHTS;
    SpotLight l_SpotLights[NUM_SPOT_LIGHTS] : SPOTLIGHTS;

    float3 cameraPosition : CAMERAPOSITION;
}

/**
* Constant Buffer for general material attributes 
* \param AMBIENT float4 Semantic for Ambient colour.
* \param DIFFUSE float4 Semantic for Diffuse colour.
* \param SPECULAR float4 Semantic for Specular colour.
* \param SPECULARPOWER float Semantic for Specular Power.
* \image html BlinnAttributes.png
* \param FOGCOLOUR float3 Semantic for the colour of fog for this material.
* \param FOGSTART float Semantic for the distance at which fog begins to show.
* \param FOGRANGE float Semantic for the range at which fog fully envelops the material.
* \param DISPLACMENT float Semantic for the scale of the object's displacement, detemined by the heightmap.
*/
cbuffer CBufferPerAttribute{
    float4 ambientColour : AMBIENT;
    float4 diffuseColour : DIFFUSE;
    float4 specularColour : SPECULAR;
    float specularPower : SPECULARPOWER;

    float3 fogColour : FOGCOLOUR;
    float fogStart : FOGSTART;
    float fogRange : FOGRANGE;

    float displacement : DISPLACEMENT;
}

/**
* Constant Buffer for each object instance
* \param WORLDVIEWPROJECTION float4x4 Semantic for the object's WorldViewProjection matrix.
* \param WORLD float4x4 Semantic for the object's World Matrix.
*/
cbuffer CBufferPerObject{
    float4x4 WorldViewProjection : WORLDVIEWPROJECTION;
    float4x4 World : WORLD;
}


// INPUT LAYOUT

/**
* Vertex Shader Input
* ATTRIBUTE | BYTEWIDTH
* ----------|----------
* POSITION | 12
* TEXCOORD | 8
* NORMAL | 12
* TANGENT | 12
*/
struct VS_IN
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL; //temp
};

// HELPER 

struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float3 viewDirection : TEXCOORD1;
    float3 worldPosition : TEXCOORD2;
    float fog : TEXCOORD3;
};

// ---------------------------------------------------------------------
// VERTEX SHADER--------------------------------------------------------

/**
* Vertex Shader for the Blinn effect.
* \param fogEnabled a boolean value, determining whether fog is enabled or disabled.
*/
VS_OUT vs_main(VS_IN input, uniform bool fogEnabled)
{
    VS_OUT output = (VS_OUT) 0;

    //Due to lights being represented in world space, in order for them to interact with the normals of the object properly, the normal has to be transformed into world space.
    //Transform the normal, tangent and binormal into world space,for use in the pixel shader.
    output.normal = normalize(mul(float4(input.normal, 0), World).xyz); 
    output.tangent = normalize(mul(float4(input.tangent, 0), World).xyz);
    output.binormal = cross(output.normal, output.tangent); 

    //Apply the height map
    if(displacement > 0.0f)
    {
        float height = HeightMap.SampleLevel(AnisotropicSampler, input.texCoord, 0);
        input.position.xyz += output.normal * height * (displacement - 1);
    }

    // Transform the input position into Homogenous Clip Space, by multiplying by the WorldViewProjection matrix.
    output.position = mul(float4(input.position, 1.0f), WorldViewProjection);
    output.texCoord = input.texCoord;   //Forwards the texture coordinates

    //Compute the world position of the vertex, for use in the pixel shader.
    output.worldPosition = mul(float4(input.position, 1), World).xyz;

    //Process fog if applicable 
    if(fogEnabled)
    {
        output.fog = ComputeFog(output.viewDirection, fogStart, fogRange);
    }

    return output;
}


// ---------------------------------------------------------------------
// PIXEL SHADER---------------------------------------------------------

/**
* Pixel Shader for the Blinn effect.
*/
float4 ps_main(VS_OUT input, uniform bool fogEnabled) : SV_Target
{
    float4 output = (float4) 0;

    //Compute the view direction
    float3 viewDir = normalize(cameraPosition - input.worldPosition);


    //DIFFUSE MAPPING --------------------
    float4 diffuseColour = DiffuseMap.Sample(AnisotropicSampler, input.texCoord);

    //NORMAL MAPPING ------------------------
    //Sample the normal map, shifting the normal from the range [0..1] to [-1..1]
    float3 n = ((2 * NormalMap.Sample(AnisotropicSampler, input.texCoord).xyz) - 1.0f);

    //Construct the tangent space matrix TBN
    float3x3 tbn = float3x3(input.tangent, input.binormal, input.normal);

    //Transform the normal into world space
    n = mul(n, tbn);



    //SPECULAR MAPPING ----------------------------
    //Specular = n.h ^ specularPower * specularIntensity
    float specularIntensity = SpecularMap.Sample(AnisotropicSampler, input.texCoord).r;

    float3 specular = (float3) 0;
    
    float3 lightDir = (float3)0;

    lightDir = normalize(-l_DirectionalLight.direction);   

    specular += ComputeSpecular(specularColour, lightDir, viewDir, n, specularPower, specularIntensity, l_DirectionalLight.colour.a);

    float3 diffuse = (float3) 0;
    diffuse += ComputeDiffuse(diffuseColour, lightDir, viewDir, n, l_DirectionalLight.colour, specularPower);

    float3 ambient = ComputeAmbient(ambientColour, diffuseColour);

    //Point Lights
    [unroll]
    for (int i = 0; i < NUM_POINT_LIGHTS; i++)
    {
        lightDir += l_PointLights[i].position - input.worldPosition;
        float attenuation = saturate(1.0f - (length(lightDir) / l_PointLights[i].radius));
        diffuse += (attenuation * ComputeDiffuse(diffuseColour, lightDir, viewDir, n, l_PointLights[i].colour, specularPower));
        specular += (attenuation * ComputeSpecular(specularColour, lightDir, viewDir, n, specularPower, specularIntensity, l_PointLights[i].colour.a));
   }

    //PRE-OUTPUT----------------------------
    output.rgb = ambient + diffuse + specular;
    output.a = diffuseColour.a;
    
    //Apply fog with Linear Interpolation.
    if (fogEnabled)
    {
        output.rgb = lerp(output.rgb, fogColour, input.fog);
    }
    
    return output;
}

// ---------------------------------------------------------------------
// FX DECLARATIONS------------------------------------------------------

technique11 main
{
    pass p0
    {
        SetVertexShader(CompileShader(vs_5_0, vs_main(false)));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, ps_main(false)));
		
        SetRasterizerState(EnableCulling);
        SetBlendState(EnableAlphaBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);

    }

}