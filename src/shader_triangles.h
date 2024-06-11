#pragma once
#include "pch.h"
#include "shader.h"

struct Shader_Triangles : Shader {

    static constexpr D3D11_INPUT_ELEMENT_DESC layout[2] {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    struct Vert {
        DirectX::XMFLOAT3 pos;
        DirectX::XMFLOAT4 color;
    };

    static constexpr UINT vcap{ 100000 };
    std::unique_ptr<Vert[]> verts;
    int32_t vlen{};

    int Init() override;
    int Commit() override;
    Vert* Alloc(int32_t vnum);
};
