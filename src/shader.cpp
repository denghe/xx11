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

ID3D11DeviceContext1* Shader::immediateContext() {
    return gLooper.immediateContext1.Get();
}

IDXGISwapChain1* Shader::swapChain() {
    return gLooper.swapChain1.Get();
}

ID3D11RenderTargetView* Shader::renderTargetView() {
    return gLooper.renderTargetView.Get();
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
