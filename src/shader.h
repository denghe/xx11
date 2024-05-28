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


//struct QuadInstanceData {
//    XY pos{}, anchor{ 0.5, 0.5 };       // float * 4
//
//    XY scale{ 1, 1 };
//    float radians{}, colorplus{ 1 };    // float * 4
//
//    RGBA8 color{ 255, 255, 255, 255 };  // u8n * 4
//
//    UVRect texRect{};    // u16 * 4
//};

struct Shader_QuadInstanceData : Shader {
    //static constexpr int32_t maxQuadNums{ 100000 };
    //GLuint lastTextureId{};
    //std::unique_ptr<QuadInstanceData[]> quadInstanceDatas = std::make_unique<QuadInstanceData[]>(maxQuadNums);
    //int32_t quadCount{};

    int Init() override;
    void Begin() override;
    void End() override;
    void Commit();
    // todo: Draw?
};
