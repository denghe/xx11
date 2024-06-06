#pragma once
#include "pch.h"
#include "shader.h"

struct Shader_Triangles : Shader {

    static constexpr D3D11_INPUT_ELEMENT_DESC layout[2] {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    struct Buf {
        DirectX::XMFLOAT3 pos;
        DirectX::XMFLOAT4 color;
    };

    static constexpr UINT cap{ 100000 };
    std::unique_ptr<Buf[]> buf;
    int32_t len{};

    int Init() override;
    int Commit() override;
    Buf* Alloc(int32_t num);
};
