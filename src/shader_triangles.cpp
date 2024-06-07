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

    if (auto r = CompileShader(src, layout, std::size(layout))) 
        return r;

    buf = std::make_unique<Buf[]>(cap);

    if (auto r = CreateBuf(sizeof(Buf) * cap))
        return r;

    return 0;
}


int Shader_Triangles::InitBuf(void* ptr, UINT siz) {
    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_IMMUTABLE;
    bd.ByteWidth = siz;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA sd{};
    sd.pSysMem = ptr;

    auto hr = d3dDevice()->CreateBuffer(&bd, &sd, &vb);
    if (FAILED(hr)) {
        xx::CoutN("d3dDevice()->CreateBuffer error. hr = ", hr);
        return __LINE__;
    }

    return 0;
}


int Shader_Triangles::CreateBuf(UINT len) {
    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = len;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    auto hr = d3dDevice()->CreateBuffer(&bd, nullptr, &vb);
    if (FAILED(hr)) {
        xx::CoutN("d3dDevice()->CreateBuffer error. hr = ", hr);
        return __LINE__;
    }

    return 0;
}


void Shader_Triangles::FillBuf(void* buf, UINT len) {
    auto ctx = immediateContext();
    D3D11_MAPPED_SUBRESOURCE mres;
    auto hr = ctx->Map(vb.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mres);
    assert(hr == S_OK);
    memcpy(mres.pData, buf, len);
    ctx->Unmap(vb.Get(), 0);
}


int Shader_Triangles::Commit() {
    assert(gLooper.shader == this);
    if (len) {
        //InitBuf(buf.get(), sizeof(Buf) * len);    // slowly than 1 create n fill
        FillBuf(buf.get(), sizeof(Buf) * len);

        auto ic = immediateContext();

        UINT offset{}, stride{ sizeof(Buf) };
        ic->IASetVertexBuffers(0, 1, vb.GetAddressOf(), &stride, &offset);

        ic->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        ic->IASetInputLayout(vil.Get());

        ic->VSSetShader(vs.Get(), nullptr, 0);
        ic->PSSetShader(ps.Get(), nullptr, 0);

        ic->Draw(len, 0);

        drawVerts += len;
        drawCall += 1;
        len = {};
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
