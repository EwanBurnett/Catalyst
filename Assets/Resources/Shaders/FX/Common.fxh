#ifndef _COMMON_H
#define _COMMON_H

// CONSTANTS
float4 Black = (float4)0;
float4 White = (float4)1;

// BLEND STATES

/**
* Enables Alpha Blending on the render target.
*/
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

// SAMPLER STATES

SamplerState LinearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressV = WRAP;
    AddressU = WRAP;
};

SamplerState PointSampler
{
    Filter = MIN_MAG_MIP_POINT;
    AddressV = WRAP;
    AddressU = WRAP;
};

SamplerState AnisotropicSampler
{
    Filter = ANISOTROPIC;
    AddressV = WRAP;
    AddressU = WRAP;
};

// RASTERIZER STATES
RasterizerState DisableCulling
{
    FrontCounterClockwise = FALSE;
    CullMode = NONE;
};

RasterizerState EnableCulling
{
    FrontCounterClockwise = FALSE;
    CullMode = BACK;
};

// HELPERS

/**
* Computes the amount of fog to apply to an object. 
* \param viewDir The direction the object is being viewed from. 
* \param start The fog's start range
* \param range The fog's distance
*/
float ComputeFog(float3 viewDir, float start, float range)
{
    //Fog is modeled as a colour, determined by the 
    //function of the distance from the camera and the start range, and the end range.
    return saturate((length(viewDir) - start) / (range));
}

#endif