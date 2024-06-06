#pragma once
#include "pch.h"

std::string_view BlobToStringView(ComPtr<ID3DBlob> const& b);

struct Shader {
    Shader() = default;
    Shader(Shader const&) = delete;
    Shader& operator=(Shader const&) = delete;
    virtual ~Shader() {}
    virtual int Init() = 0;
    virtual int Commit() = 0;

    inline static int drawVerts{}, drawCall{};
    static void ClearCounter();
protected:

    ComPtr<ID3D11VertexShader> vs;
    ComPtr<ID3D11PixelShader> ps;
    ComPtr<ID3D11InputLayout> vil;
    ComPtr<ID3D11Buffer> vb;

    // return gLooper.*.Get();
    ID3D11Device1* d3dDevice();
    ID3D11DeviceContext1* immediateContext();
    IDXGISwapChain1* swapChain();
    ID3D11RenderTargetView* renderTargetView();

    int CompileShader(std::string_view vsSrc, D3D11_INPUT_ELEMENT_DESC const* layoutDescs, UINT layoutDescLen
        , std::string_view psSrc = {}
        , char const* vsMain = "vs_main", char const* vsVer = "vs_4_0"
        , char const* psMain = "ps_main", char const* psVer = "ps_4_0");

    int InitBuf(void* ptr, UINT siz);   // create + memcpy  ( tested: slowly than 1 create + n fill )

    int CreateBuf(UINT len);            // create only

    void FillBuf(void* buf, UINT len);  // memcpy
    // ...
};
