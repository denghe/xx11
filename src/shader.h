#pragma once
#include "pch.h"

std::string_view BlobToStringView(ComPtr<ID3DBlob> const& b);

struct Shader {
    Shader() = default;
    Shader(Shader const&) = delete;
    Shader& operator=(Shader const&) = delete;
    virtual ~Shader() {}
    virtual int Init() = 0;
    virtual void Begin() = 0;
    virtual void End() = 0;

    inline static int drawVerts{}, drawCall{};
    static void ClearCounter();

    ComPtr<ID3D11VertexShader> vs;
    ComPtr<ID3D11PixelShader> ps;
    ComPtr<ID3D11InputLayout> vil;
    ComPtr<ID3D11Buffer> buf;

protected:
    ID3D11Device1* d3dDevice();                             // return gLooper.d3dDevice1.Get();
    ID3D11DeviceContext1* immediateContext();               // return gLooper.immediateContext1.Get();

    int CompileShader(std::string_view vsSrc, D3D11_INPUT_ELEMENT_DESC const* layoutDescs, UINT layoutDescLen
        , std::string_view psSrc = {}
        , char const* vsMain = "vs_main", char const* vsVer = "vs_4_0"
        , char const* psMain = "ps_main", char const* psVer = "ps_4_0");

    int InitBuf(void* ptr, UINT siz, UINT stride);          // d3dDevice()->CreateBuffer( ..., &buf); + immediateContext()->IASetVertexBuffers

    // ...
};
