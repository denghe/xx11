#include "pch.h"
#include "looper.h"
#include "shader_quadinstance.h"

int Shader_QuadInstance::Init() {

    // todo: optimize

    std::string_view src{ R"#(
cbuffer Uniforms : register(b0)
{
    float2 _91_uCxy : packoffset(c0);
};


static float4 gl_Position;
static float4 aPosAnchor;
static float4 aScaleRadiansColorplus;
static float4 aTexRect;
static float2 aVert;
static float4 vColor;
static float4 aColor;
static float vColorplus;
static float2 vTexCoord;

struct SPIRV_Cross_Input1
{
    float2 aVert : POSITION;
    float4 aPosAnchor : TEXCOORD2;
    float4 aScaleRadiansColorplus : TEXCOORD6;
    float4 aColor : COLOR2;
    float4 aTexRect : BLENDINDICES;
};

struct SPIRV_Cross_Output1
{
    float2 vTexCoord : TEXCOORD0;
    float vColorplus : TEXCOORD2;
    float4 vColor : TEXCOORD4;
    float4 gl_Position : SV_Position;
};

void vert_main()
{
    float2 pos = aPosAnchor.xy;
    float2 anchor = aPosAnchor.zw;
    float2 scale = float2(aScaleRadiansColorplus.x * aTexRect.z, aScaleRadiansColorplus.y * aTexRect.w);
    float _radians = aScaleRadiansColorplus.z;
    float2 offset = float2((aVert.x - anchor.x) * scale.x, (aVert.y - anchor.y) * scale.y);
    float c = cos(_radians);
    float s = sin(_radians);
    float2 v = pos + float2(dot(offset, float2(c, s)), dot(offset, float2(-s, c)));
    gl_Position = float4(v * _91_uCxy, 0.0f, 1.0f);
    vColor = aColor;
    vColorplus = aScaleRadiansColorplus.w;
    vTexCoord = float2(aTexRect.x + (aVert.x * aTexRect.z), (aTexRect.y + aTexRect.w) - (aVert.y * aTexRect.w));
}

SPIRV_Cross_Output1 vs_main(SPIRV_Cross_Input1 stage_input)
{
    aPosAnchor = stage_input.aPosAnchor;
    aScaleRadiansColorplus = stage_input.aScaleRadiansColorplus;
    aTexRect = stage_input.aTexRect;
    aVert = stage_input.aVert;
    aColor = stage_input.aColor;
    vert_main();
    SPIRV_Cross_Output1 stage_output;
    stage_output.gl_Position = gl_Position;
    stage_output.vColor = vColor;
    stage_output.vColorplus = vColorplus;
    stage_output.vTexCoord = vTexCoord;
    return stage_output;
}



Texture2D<float4> uTex0 : register(t0);
SamplerState _uTex0_sampler : register(s0);

static float4 oColor;

struct SPIRV_Cross_Input2
{
    float2 vTexCoord : TEXCOORD0;
    float vColorplus : TEXCOORD2;
    float4 vColor : TEXCOORD4;
};

struct SPIRV_Cross_Output2
{
    float4 oColor : SV_Target0;
};

uint2 spvTextureSize(Texture2D<float4> Tex, uint Level, out uint Param)
{
    uint2 ret;
    Tex.GetDimensions(Level, ret.x, ret.y, Param);
    return ret;
}

void frag_main()
{
    uint _27_dummy_parameter;
    float4 c = vColor * uTex0.Sample(_uTex0_sampler, vTexCoord / float2(int2(spvTextureSize(uTex0, uint(0), _27_dummy_parameter))));
    oColor = float4((c.x + 9.9999997473787516355514526367188e-06f) * vColorplus, (c.y + 9.9999997473787516355514526367188e-06f) * vColorplus, (c.z + 9.9999997473787516355514526367188e-06f) * vColorplus, c.w);
}

SPIRV_Cross_Output2 ps_main(SPIRV_Cross_Input2 stage_input)
{
    vColor = stage_input.vColor;
    vTexCoord = stage_input.vTexCoord;
    vColorplus = stage_input.vColorplus;
    frag_main();
    SPIRV_Cross_Output2 stage_output;
    stage_output.oColor = oColor;
    return stage_output;
}

)#" };

    // todo
    return 0;
}


int Shader_QuadInstance::Commit() {
    // todo
    return 0;
}
