#pragma once
#include "pch.h"
#include "shader.h"

struct Shader_IndexTriangles : Shader {

    static constexpr D3D11_INPUT_ELEMENT_DESC layout[2] {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    struct Vert {
        DirectX::XMFLOAT3 pos;
        DirectX::XMFLOAT4 color;
    };

    static constexpr UINT vcap{ 100000 }, icap{ vcap * 5 };
    std::unique_ptr<Vert[]> verts;
    std::unique_ptr<UINT[]> idxs;
    int32_t vlen{}, ilen{};

    int Init() override;
    int Commit() override;
    std::pair<Vert*, UINT*> Alloc(int32_t vnum, int32_t inum);
};
