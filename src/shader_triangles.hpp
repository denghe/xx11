#pragma once

inline int Shader_Triangles::Init() {

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

    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(Vert) * vcap;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    auto hr = d3dDevice()->CreateBuffer(&bd, nullptr, &vb);
    if (FAILED(hr)) {
        xx::CoutN("d3dDevice()->CreateBuffer error. hr = ", hr);
        return __LINE__;
    }

    return 0;
}


inline int Shader_Triangles::Commit() {
    assert(gLooper.shader == this);
    if (vlen) {
        auto ic = immediateContext();
        {
            D3D11_MAPPED_SUBRESOURCE mres;
            auto hr = ic->Map(vb.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mres);
            assert(hr == S_OK);
            memcpy(mres.pData, verts.get(), sizeof(Vert) * vlen);
            ic->Unmap(vb.Get(), 0);
        }

        {
            UINT offset{}, stride{ sizeof(Vert) };
            ic->IASetVertexBuffers(0, 1, vb.GetAddressOf(), &stride, &offset);
        }

        ic->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        ic->IASetInputLayout(vil.Get());

        ic->VSSetShader(vs.Get(), nullptr, 0);
        ic->PSSetShader(ps.Get(), nullptr, 0);

        ic->Draw(vlen, 0);

        drawVerts += vlen;
        drawCall += 1;
        vlen = {};
    }
    return 0;
}


inline Shader_Triangles::Vert* Shader_Triangles::Alloc(int32_t vnum) {
    assert(gLooper.shader == this);
    assert(vlen <= vcap);
    if (vlen + vnum > vcap) {
        Commit();
    }
    auto r = &verts[vlen];
    vlen += vnum;
    return r;
}
