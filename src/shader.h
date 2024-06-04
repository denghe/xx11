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
    int CompileShader4(std::string_view const& src);		// "vs_main", "vs_4_0"  "ps_main", "ps_4_0"
};
