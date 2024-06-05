﻿#include "pch.h"
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

ID3D11DeviceContext1* Shader::immediateContext() {
    return gLooper.immediateContext1.Get();
}

int Shader::CompileShader(std::string_view vsSrc, D3D11_INPUT_ELEMENT_DESC const* layoutDescs, UINT layoutDescLen, std::string_view psSrc, char const* vsMain, char const* vsVer, char const* psMain, char const* psVer) {
    if (psSrc.empty()) {
        psSrc = vsSrc;
    }

    ComPtr<ID3DBlob> b, bm;
    auto hr = D3DCompile(vsSrc.data(), vsSrc.size(), "vsSrc", 0, 0, vsMain, vsVer, 0, 0, &b, &bm);
    if (FAILED(hr)) {
        xx::CoutN("D3DCompile error: ", BlobToStringView(bm));
        return __LINE__;
    }

    hr = d3dDevice()->CreateVertexShader(b->GetBufferPointer(), b->GetBufferSize(), 0, &vs);
    if (FAILED(hr)) {
        xx::CoutN("d3dDevice()->CreateVertexShader error. hr = ", hr);
        return __LINE__;
    }

    hr = d3dDevice()->CreateInputLayout(layoutDescs, layoutDescLen, b->GetBufferPointer(), b->GetBufferSize(), &vil);
    if (FAILED(hr)) {
        xx::CoutN("d3dDevice()->CreateInputLayout error. hr = ", hr);
        return __LINE__;
    }

    hr = D3DCompile(psSrc.data(), psSrc.size(), "psSrc", 0, 0, psMain, psVer, 0, 0, &b, &bm);
    if (FAILED(hr)) {
        xx::CoutN("D3DCompile error: ", BlobToStringView(bm));
        return __LINE__;
    }

    hr = d3dDevice()->CreatePixelShader(b->GetBufferPointer(), b->GetBufferSize(), 0, &ps);
    if (FAILED(hr)) {
        xx::CoutN("d3dDevice()->CreatePixelShader error. hr = ", hr);
        return __LINE__;
    }

    return 0;
}

int Shader::InitBuf(void* ptr, UINT siz, UINT stride) {
    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_IMMUTABLE;
    bd.ByteWidth = siz;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA sd{};
    sd.pSysMem = ptr;

    auto hr = d3dDevice()->CreateBuffer(&bd, &sd, &buf);
    if (FAILED(hr)) {
        xx::CoutN("d3dDevice()->CreateBuffer error. hr = ", hr);
        return __LINE__;
    }

    UINT offset = 0;
    immediateContext()->IASetVertexBuffers(0, 1, buf.GetAddressOf(), &stride, &offset);

    return 0;
}
