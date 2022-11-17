#ifndef _LIGHTHELPER_FXH
#define _LIGHTHELPER_FXH

#define NUM_POINT_LIGHTS 4
#define NUM_SPOT_LIGHTS 4

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
    
    //Compute the angle between the normal and the light's direction 
    //float nDotL = dot(n, lightDir);
    //Compute a half-vector, as we're implementing the Blinn model
    
    //Compute the angle between the normal and the half vector
    //float nDotH = dot(n, halfVector);

    //Compute the lambertian diffuse and blinn-phong specular components using intrinsic functions
    //lit() returns the lighting coefficient vector:
    //(ambient, diffuse, specular, 1)
    //ambient = 1
    //diffuse = (n.l > 0) ? 0 : n.l
    //specular = ((n.l < 0) || (n.h < 0)) ? 0 : (n.h) ^ m
    float4 lightCoefficients = lit(nDotL, nDotH, IN.specularPower);
    
    float3 diffuse = GetVectorColourContribution(IN.lightColour, lightCoefficients.y * IN.colour.rgb) * IN.lightDirection.w;
    float3 specular = GetScalarColourContribution(IN.specularColour, min(lightCoefficients.z, IN.colour.w)) * IN.lightDirection.w * IN.lightColour.w;
    
    return (diffuse + specular);

}

float3 ComputeSpecular(float4 specularColour, float3 lightDirection, float3 viewDirection, float3 normal, float specularPower, float specularIntensity, float lightIntensity)
{
    float3 lightDir = normalize(lightDirection);
    float3 viewDir = normalize(viewDirection);
    float nDotL = dot(normal, lightDir);
    float3 halfVector = normalize(lightDir + viewDir);  
    float nDotH = dot(normal, halfVector);

    //Compute the angle between the normal and the light's direction 
    //float nDotL = dot(n, lightDir);
    //Compute a half-vector, as we're implementing the Blinn model
    
    //Compute the angle between the normal and the half vector
    //float nDotH = dot(n, halfVector);

    //Compute the lambertian diffuse and blinn-phong specular components using intrinsic functions
    //lit() returns the lighting coefficient vector:
    //(ambient, diffuse, specular, 1)
    //ambient = 1
    //diffuse = (n.l > 0) ? 0 : n.l
    //specular = ((n.l < 0) || (n.h < 0)) ? 0 : (n.h) ^ m
    float3 lightCoefficients = lit(nDotL, nDotH, specularPower);

    return (specularColour.rgb * specularColour.a * min(lightCoefficients.z, specularIntensity) * lightIntensity);
}

float3 ComputeDiffuse(float4 diffuseColour, float3 lightDirection, float3 viewDirection, float3 normal, float4 lightColour, float specularPower)
{
    float3 lightDir = normalize(lightDirection);
    float3 viewDir = normalize(viewDirection);
    float nDotL = dot(normal, lightDir);
    float3 halfVector = normalize(lightDir + viewDir);
    float nDotH = dot(normal, halfVector);

    float3 lightCoefficients = lit(nDotL, nDotH, specularPower);
    
    return(lightColour.rgb * lightColour.a * lightCoefficients.y * diffuseColour.rgb);
}

float3 ComputeAmbient(float4 ambientColour, float3 diffuseColour)
{
    return(ambientColour.rgb * ambientColour.a * diffuseColour);
}
#endif
