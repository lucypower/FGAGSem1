//--------------------------------------------------------------------------------------
// File: DX11 Framework.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

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
    float SpecularPower;

    float3 EyePosW;
    float3 LightVecW; // vector that points in the direction of the light source
}

//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float3 Norm : NORMAL;
    float3 PosW : POSITION;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS( float4 Pos : POSITION, float3 NormalL : NORMAL, float3 PosW : POSITION )
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    output.Pos = mul(Pos, World);
    output.PosW = EyePosW;
    float3 normalW = mul(float4(NormalL, 0.0f), World).xyz;
    
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);    

    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT input ) : SV_Target
{
    float3 toEye = normalize(EyePosW - input.Pos.xyz);

    // convert from local space to world space (w component is 0 as vectors can't be translated)

    float3 normalW = normalize(normalW);

    // compute colour
    // compute the reflection vector
    float3 r = reflect(-LightVecW, normalW);

    // determine how much specular light makes it into the eye
    float specularAmount = pow(max(dot(r, toEye), 0.0f), SpecularPower);

    // compute colour using diffuse lighting only
    float diffuseAmount = max(dot(LightVecW, normalW), 0.0f);
    output.Color.rgb = diffuseAmount * (DiffuseMtrl * DiffuseLight).rgb;
    output.Color.a = DiffuseMtrl.a;

    //float3 ambientAmount = AmbientMtrl * AmbientLight;
    ////output.Color.rgb = diffuseAmount + ambientAmount;
    //output.Color.a = DiffuseMtrl.a;

    //// sum all the terms together and cioy over the diffuse alpha
    //output.Color.rgb = diffuseAmount + ambientAmount + specularAmount;
    //output.Color.a = DiffuseMtrl.a;

    return input.Color;
}
