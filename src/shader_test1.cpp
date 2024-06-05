#include "pch.h"
#include "looper.h"
#include "shader_test1.h"


const D3D11_INPUT_ELEMENT_DESC Shader_Test1::inputLayout[2] = {
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

int Shader_Test1::Init() {

    std::string_view src{ R"#(

struct VertexIn {
    float3 pos : POSITION;
    float4 color : COLOR;
};

struct VertexOut {
    float4 posH : SV_POSITION;
    float4 color : COLOR;
};

VertexOut vs_main(VertexIn vIn) {
    VertexOut vOut;
    vOut.posH = float4(vIn.pos, 1.0f);
    vOut.color = vIn.color;
    return vOut;
}

float4 ps_main(VertexOut pIn) : SV_Target {
    return pIn.color;   
}

)#" };
    if (auto r = CompileShader(src)) return r;

    // todo

    Buf vertices[] {
        { {0.0f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f, 1.0f} },
        { {0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f} },
        { {-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f} }
    };

    D3D11_BUFFER_DESC vbd{};
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(vertices);
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    

    return 0;
}

void Shader_Test1::Begin() {
    // todo
    //immediateContext1->VSSetShader(vs.Get(), 0, 0);
    //immediateContext1->PSSetShader(ps.Get(), 0, 0);
    // ...
}

void Shader_Test1::End() {
    //assert(gLooper.shader == this);
    //if (quadCount) {
    //    Commit();
    //}
}

void Shader_Test1::Commit() {
    // todo: send buffer + draw instanced

    //drawVerts += quadCount * 6;
    //drawCall += 1;

    //lastTextureId = 0;
    //quadCount = 0;
}
