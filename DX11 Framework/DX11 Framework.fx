//--------------------------------------------------------------------------------------
// File: DX11 Framework.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

Texture2D txDiffuse : register(t0); // access texture register 0
SamplerState samLinear : register(s0); // access sampler register 0

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register( b0 )
{
	matrix World;
	matrix View;
	matrix Projection;

    float4 DiffuseMtrl; // colour vector that describes the diffuse material
    float4 DiffuseLight; // colour vector that describes the diffuse light colour     

    float4 AmbientMtrl;
    float4 AmbientLight;

    float4 SpecularMtrl;
    float4 SpecularLight;

    float3 EyePosW;
    float SpecularPower;

    float3 LightVecW; // vector that points in the direction of the light source
    float pad1;
}

//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float3 Norm : NORMAL;
    float3 PosW : POSITION;
    float2 Tex : TEXCOORD0;
};

struct VS_INPUT_TEXCOORDS
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS( float4 Pos : POSITION, float3 NormalL : NORMAL, float2 Tex : TEXCOORD0)
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    output.Pos = mul(Pos, World);
    output.PosW = output.Pos;
    float3 normalW = mul(float4(NormalL, 0.0f), World).xyz;
    
    output.Tex = Tex;
    output.Norm = normalW;
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);    

    

    return output;
}


//float4 PS(VS_OUTPUT_TEXCOORDS input) : SV_Target
//{
//    float4 textureColor = txDiffuse.Sample(samLinear, input.Tex);
//    return textureColor;
//}
//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT input ) : SV_Target
{
    float3 toEye = normalize(EyePosW - input.PosW.xyz);
    
    //float4 textureColor = { 1, 1, 1, 1 }
    float4 textureColor = txDiffuse.Sample(samLinear, input.Tex);

    // convert from local space to world space (w component is 0 as vectors can't be translated)

    input.Norm = normalize(input.Norm);

    // compute colour
    // compute the reflection vector
    float3 r = reflect(-LightVecW, input.Norm);
    float4 Color;

    // determine how much specular light makes it into the eye
    float specularAmount = pow(max(dot(r, toEye), 0.0f), SpecularPower);
    float3 specularColor = specularAmount * (SpecularMtrl * SpecularLight).rgb;

    // compute colour using diffuse lighting only
    float diffuseAmount = max(dot(LightVecW, input.Norm), 0.0f);
    float3 diffuseColor = diffuseAmount * (DiffuseMtrl * DiffuseLight).rgb;

    float3 ambientColor = AmbientMtrl * AmbientLight;


    // sum all the terms together and cioy over the diffuse alpha
    Color.rgb = (diffuseColor + ambientColor + specularColor) * textureColor;
    Color.a = DiffuseMtrl.a;

    // NEED CODE FOR USING TEXCOORDS

    return Color;
    //return textureColor;
}
