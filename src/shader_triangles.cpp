#include "pch.h"
#include "looper.h"
#include "shader_triangles.h"

int Shader_Triangles::Init() {

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

    if (auto r = CompileShader(src, layout, ARRAYSIZE(layout))) 
        return r;

    buf = std::make_unique<Buf[]>(cap);

    return 0;
}


int Shader_Triangles::Commit() {
    assert(gLooper.shader == this);
    assert(buf);
    if (len) {
        if (auto r = InitBuf(buf.get(),  sizeof(Buf) * len))
            return r;

        auto ic = immediateContext();

        UINT offset{}, stride{ sizeof(Buf) };
        ic->IASetVertexBuffers(0, 1, vb.GetAddressOf(), &stride, &offset);

        ic->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        ic->IASetInputLayout(vil.Get());

        ic->VSSetShader(vs.Get(), nullptr, 0);
        ic->PSSetShader(ps.Get(), nullptr, 0);

        ic->Draw(3, 0);

        len = {};
        drawVerts += len * 3;
        drawCall += 1;
    }
    return 0;
}


Shader_Triangles::Buf* Shader_Triangles::Alloc(int32_t num) {
    assert(gLooper.shader == this);
    assert(len <= cap);
    if (len + num > cap/* || (lastTextureId && lastTextureId != texId)*/) {
        Commit();
    }
    //lastTextureId = texId;
    auto r = &buf[len];
    len += num;
    return r;
}
