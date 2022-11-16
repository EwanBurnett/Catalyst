#ifndef _LIGHTHELPER_FXH
#define _LIGHTHELPER_FXH

#define NUM_POINT_LIGHTS 16
#define NUM_SPOT_LIGHTS 16

struct DirectionalLight
{
    float4 colour;
    float3 direction;
};

struct PointLight
{
    float4 colour;
    float3 position;
    float radius;
};

struct SpotLight
{
    float4 colour;
    float3 position;
    float radius;
    float3 direction;
    float innerRadius;
    float outerRadius;
};


struct LightContributionData
{
    float4 colour;
    float3 normal;
    float3 viewDirection;
    float4 lightColour;
    float4 lightDirection;
    float4 specularColour;
    float specularPower;
};


float3 GetVectorColourContribution(float4 light, float3 colour)
{
    return light.rgb * light.a * colour;
}

float3 GetScalarColourContribution(float4 light, float colour)
{
    return light.rgb * light.a * colour;
}

float4 GetPointLightData(float3 lightPos, float3 worldPos, float lightRadius)
{
    float4 data;
    float3 lightDir = lightPos - worldPos;
    
    data.xyz = normalize(lightDir);
    data.w = saturate(1.0f - length(lightDir) / lightRadius);
    
    return data;
}

//float4 GetSpotLightData(float3 lightPos, float3 worldPos, )
float3 GetLightContribution(LightContributionData IN)
{
    float3 lightDir = IN.lightDirection.xyz;
    float nDotL = dot(IN.normal, lightDir);
    float3 halfVector = normalize(lightDir + IN.viewDirection);
    float nDotH = dot(IN.normal, halfVector);
    
    float4 lightCoefficients = lit(nDotL, nDotH, IN.specularPower);
    
    float3 diffuse = GetVectorColourContribution(IN.lightColour, lightCoefficients.y * IN.colour.rgb) * IN.lightDirection.w;
    float3 specular = GetScalarColourContribution(IN.specularColour, min(lightCoefficients.z, IN.colour.w)) * IN.lightDirection.w * IN.lightColour.w;
    
    return (diffuse + specular);

}

#endif
