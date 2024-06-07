#include "pch.h"
#include "looper.h"
#include "shader_indextriangles.h"

int Shader_IndexTriangles::Init() {

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

    verts = std::make_unique<Vert[]>(vcap);
    idxs = std::make_unique<UINT[]>(icap);

    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = vcap * sizeof(Vert);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    auto hr = d3dDevice()->CreateBuffer(&bd, nullptr, &vb);
    if (FAILED(hr)) {
        xx::CoutN("d3dDevice()->CreateBuffer( vb ) error. hr = ", hr);
        return __LINE__;
    }

    bd.ByteWidth = icap * sizeof(UINT);
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

    hr = d3dDevice()->CreateBuffer(&bd, nullptr, &ib);
    if (FAILED(hr)) {
        xx::CoutN("d3dDevice()->CreateBuffer( ib ) error. hr = ", hr);
        return __LINE__;
    }

    return 0;
}


int Shader_IndexTriangles::Commit() {
    assert(gLooper.shader == this);
    if (vlen) {
        assert(ilen);
        auto ic = immediateContext();

        D3D11_MAPPED_SUBRESOURCE mv, mi;
        auto hr = ic->Map(vb.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mv);
        assert(hr == S_OK);
        hr = ic->Map(ib.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mi);
        assert(hr == S_OK);
        memcpy(mv.pData, verts.get(), vlen * sizeof(Vert));
        memcpy(mv.pData, idxs.get(), ilen * sizeof(UINT));
        ic->Unmap(ib.Get(), 0);
        ic->Unmap(vb.Get(), 0);

        UINT offset{}, stride{ sizeof(Vert) };
        ic->IASetVertexBuffers(0, 1, vb.GetAddressOf(), &stride, &offset);
        ic->IASetIndexBuffer(ib.Get(), DXGI_FORMAT_R32_UINT, 0);

        ic->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        ic->IASetInputLayout(vil.Get());

        ic->VSSetShader(vs.Get(), nullptr, 0);
        ic->PSSetShader(ps.Get(), nullptr, 0);

        ic->DrawIndexed(ilen, 0, 0);

        drawVerts += ilen;
        drawCall += 1;
        vlen = ilen = {};
    }
    return 0;
}


std::pair<Shader_IndexTriangles::Vert*, UINT*> Shader_IndexTriangles::Alloc(int32_t vnum, int32_t inum) {
    assert(gLooper.shader == this);
    assert(vnum <= vcap);
    assert(inum <= icap);
    assert(vlen <= vcap);
    assert(ilen <= icap);
    if (vlen + vnum > vcap || ilen + inum > icap) {
        Commit();
    }
    std::pair<Shader_IndexTriangles::Vert*, UINT*> r{ &verts[vlen], &idxs[ilen] };
    vlen += vnum;
    ilen += inum;
    return r;
}
