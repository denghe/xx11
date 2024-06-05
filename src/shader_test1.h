#pragma once
#include "pch.h"
#include "shader.h"


struct Shader_Test1 : Shader {

    struct Buf {
        DirectX::XMFLOAT3 pos;
        DirectX::XMFLOAT4 color;
    };
    static const D3D11_INPUT_ELEMENT_DESC inputLayout[2];

    int Init() override;
    void Begin() override;
    void End() override;
    void Commit();
    // todo: Draw?
};
