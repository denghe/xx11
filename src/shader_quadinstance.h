#pragma once
#include "pch.h"
#include "shader.h"

//struct ShaderData_QuadInstance {
//    XY pos{}, anchor{ 0.5, 0.5 };       // float * 4
//
//    XY scale{ 1, 1 };
//    float radians{}, colorplus{ 1 };    // float * 4
//
//    RGBA8 color{ 255, 255, 255, 255 };  // u8n * 4
//
//    UVRect texRect{};    // u16 * 4
//};

struct Shader_QuadInstance : Shader {
    //static constexpr int32_t maxQuadNums{ 100000 };
    //GLuint lastTextureId{};
    //std::unique_ptr<QuadInstanceData[]> quadInstanceDatas = std::make_unique<QuadInstanceData[]>(maxQuadNums);
    //int32_t quadCount{};

    int Init() override;
    int Commit() override;
    // todo: Draw?
};
