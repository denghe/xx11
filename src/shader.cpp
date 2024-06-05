#include "pch.h"
#include "looper.h"
#include "shader.h"

std::string_view BlobToStringView(ComPtr<ID3DBlob> const& b) {
    if (!b) return {};
    return { (char*)b->GetBufferPointer(), b->GetBufferSize() };
}

void Shader::ClearCounter() {
    drawVerts = {};
    drawCall = {};
}

ID3D11Device1* Shader::d3dDevice() {
    return gLooper.d3dDevice1.Get();
}

int Shader::CompileShader(std::string_view vs_src, std::string_view ps_src, char const* vs_main, char const* vs_ver, char const* ps_main, char const* ps_ver) {
    ComPtr<ID3DBlob> b, bm;
    HRESULT hr{};

    if (ps_src.empty()) {
        ps_src = vs_src;
    }

    hr = D3DCompile(vs_src.data(), vs_src.size(), "vs_src", 0, 0, vs_main, vs_ver, 0, 0, &b, &bm);
    if (FAILED(hr)) {
        xx::CoutN("D3DCompile error: ", BlobToStringView(bm));
        return __LINE__;
    }

    hr = d3dDevice()->CreateVertexShader(b->GetBufferPointer(), b->GetBufferSize(), 0, &vs);
    if (FAILED(hr)) {
        xx::CoutN("d3dDevice1->CreateVertexShader error. hr = ", hr);
        return __LINE__;
    }

    hr = D3DCompile(ps_src.data(), ps_src.size(), "ps_src", 0, 0, ps_main, ps_ver, 0, 0, &b, &bm);
    if (FAILED(hr)) {
        xx::CoutN("D3DCompile error: ", BlobToStringView(bm));
        return __LINE__;
    }

    hr = d3dDevice()->CreatePixelShader(b->GetBufferPointer(), b->GetBufferSize(), 0, &ps);
    if (FAILED(hr)) {
        xx::CoutN("d3dDevice1->CreatePixelShader error. hr = ", hr);
        return __LINE__;
    }

    return 0;
}
