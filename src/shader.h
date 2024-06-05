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

protected:
    ID3D11Device1* d3dDevice(); // return gLooper.d3dDevice1.Get();

    int CompileShader(std::string_view vs_src, std::string_view ps_src = {}
        , char const* vs_main = "vs_main", char const* vs_ver = "vs_4_0"
        , char const* ps_main = "ps_main", char const* ps_ver = "ps_4_0");

    // ...
};
