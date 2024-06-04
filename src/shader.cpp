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

int Shader::CompileShader4(std::string_view const& src) {
    auto& d = gLooper.d3dDevice1;
    ComPtr<ID3DBlob> b, bm;

    auto hr = D3DCompile(src.data(), src.size(), "src", 0, 0, "vs_main", "vs_4_0", 0, 0, &b, &bm);
    if (FAILED(hr)) {
        xx::CoutN("D3DCompile error: ", BlobToStringView(bm));
        return __LINE__;
    }

    hr = d->CreateVertexShader(b->GetBufferPointer(), b->GetBufferSize(), 0, &vs);
    if (FAILED(hr)) {
        xx::CoutN("d3dDevice1->CreateVertexShader error. hr = ", hr);
        return __LINE__;
    }

    hr = D3DCompile(src.data(), src.size(), "src", 0, 0, "ps_main", "ps_4_0", 0, 0, &b, &bm);
    if (FAILED(hr)) {
        xx::CoutN("D3DCompile error: ", BlobToStringView(bm));
        return __LINE__;
    }

    hr = d->CreatePixelShader(b->GetBufferPointer(), b->GetBufferSize(), 0, &ps);
    if (FAILED(hr)) {
        xx::CoutN("d3dDevice1->CreatePixelShader error. hr = ", hr);
        return __LINE__;
    }

    return 0;
}
